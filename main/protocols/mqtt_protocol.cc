#include "mqtt_protocol.h"
#include "board.h"
#include "application.h"
#include "settings.h"

#include <esp_log.h>
#include <cstring>
#include <arpa/inet.h>
#include "assets/lang_config.h"

#define TAG "MQTT"

MqttProtocol::MqttProtocol() {
    event_group_handle_ = xEventGroupCreate();

    // Initialize reconnect timer
    esp_timer_create_args_t reconnect_timer_args = {
        .callback = [](void* arg) {
            MqttProtocol* protocol = (MqttProtocol*)arg;
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateIdle) {
                    ESP_LOGI(TAG, "Reconnecting to MQTT server");
                app.Schedule([protocol]() {
                    protocol->StartMqttClient(false);
                });
            }
        },
        .arg = this,
    };
    esp_timer_create(&reconnect_timer_args, &reconnect_timer_);
}

MqttProtocol::~MqttProtocol() {
        ESP_LOGI(TAG, "MqttProtocol deinit");
    if (reconnect_timer_ != nullptr) {
        esp_timer_stop(reconnect_timer_);
        esp_timer_delete(reconnect_timer_);
    }
    
    udp_.reset();
    mqtt_.reset();
    
    if (event_group_handle_ != nullptr) {
        vEventGroupDelete(event_group_handle_);
    }
}

bool MqttProtocol::Start() {
    return StartMqttClient(false);
}

bool MqttProtocol::StartMqttClient(bool report_error) {
    if (mqtt_ != nullptr) {
        ESP_LOGW(TAG, "Mqtt client already started");
        mqtt_.reset();
    }

    Settings settings("mqtt", false);
    auto endpoint = settings.GetString("endpoint");
    auto client_id = settings.GetString("client_id");
    auto username = settings.GetString("username");
    auto password = settings.GetString("password");
    int keepalive_interval = settings.GetInt("keepalive", 240);
    publish_topic_ = settings.GetString("publish_topic");

    ESP_LOGI(TAG, "MQTT settings - endpoint: %s, client_id: %s, username: %s, keepalive: %d, publish_topic: %s",
             endpoint.c_str(), client_id.c_str(), username.c_str(), keepalive_interval, publish_topic_.c_str());

    if (endpoint.empty()) {
        ESP_LOGW(TAG, "MQTT endpoint is not specified");
        if (report_error) {
            SetError(Lang::Strings::SERVER_NOT_FOUND);
        }
        return false;
    }

    auto network = Board::GetInstance().GetNetwork();
    mqtt_ = network->CreateMqtt(0);
    mqtt_->SetKeepAlive(keepalive_interval);

    mqtt_->OnDisconnected([this]() {
        ESP_LOGI(TAG, "MQTT disconnected");
        if (on_disconnected_ != nullptr) {
            on_disconnected_();
        }
        ESP_LOGI(TAG, "MQTT disconnected, schedule reconnect in %d seconds", MQTT_RECONNECT_INTERVAL_MS / 1000);
        esp_timer_start_once(reconnect_timer_, MQTT_RECONNECT_INTERVAL_MS * 1000);
    });

    mqtt_->OnConnected([this]() {
        ESP_LOGI(TAG, "MQTT connected");
        if (on_connected_ != nullptr) {
            on_connected_();
        }
        esp_timer_stop(reconnect_timer_);
    });

    mqtt_->OnMessage([this](const std::string& topic, const std::string& payload) {
        ESP_LOGI(TAG, "Received MQTT message on topic: %s", topic.c_str());
        ESP_LOGI(TAG, "Message payload: %s", payload.c_str());
        
        cJSON* root = cJSON_Parse(payload.c_str());
        if (root == nullptr) {
            ESP_LOGE(TAG, "Failed to parse json message %s", payload.c_str());
            return;
        }
        cJSON* type = cJSON_GetObjectItem(root, "type");
        if (!cJSON_IsString(type)) {
            ESP_LOGE(TAG, "Message type is invalid");
            cJSON_Delete(root);
            return;
        }

        ESP_LOGI(TAG, "Message type: %s", type->valuestring);
        if (strcmp(type->valuestring, "hello") == 0) {
            ESP_LOGI(TAG, "Processing hello message");
            ParseServerHello(root);
        } else if (strcmp(type->valuestring, "goodbye") == 0) {
            auto session_id = cJSON_GetObjectItem(root, "session_id");
            ESP_LOGI(TAG, "Received goodbye message, session_id: %s", session_id ? session_id->valuestring : "null");
            ESP_LOGI(TAG, "Current session_id: %s", session_id_.c_str());
            ESP_LOGI(TAG, "Session ID comparison: %s", 
                     (session_id == nullptr || session_id_ == session_id->valuestring) ? "MATCH" : "MISMATCH");
            
            if (session_id == nullptr || session_id_ == session_id->valuestring) {
                ESP_LOGI(TAG, "Closing audio channel due to goodbye message");
                Application::GetInstance().Schedule([this]() {
                    CloseAudioChannel();
                });
            } else {
                ESP_LOGI(TAG, "Ignoring goodbye message for different session");
            }
        } else if (on_incoming_json_ != nullptr) {
            on_incoming_json_(root);
        } else {
            ESP_LOGI(TAG, "Unhandled message type: %s", type->valuestring);
        }
        cJSON_Delete(root);
        last_incoming_time_ = std::chrono::steady_clock::now();
    });

    ESP_LOGI(TAG, "Connecting to endpoint %s", endpoint.c_str());
    std::string broker_address;
    int broker_port = 8883;
    size_t pos = endpoint.find(':');
    if (pos != std::string::npos) {
        broker_address = endpoint.substr(0, pos);
        broker_port = std::stoi(endpoint.substr(pos + 1));
        ESP_LOGI(TAG, "Parsed broker address: %s, port: %d", broker_address.c_str(), broker_port);
    } else {
        broker_address = endpoint;
        ESP_LOGI(TAG, "Using default port 8883 for broker address: %s", broker_address.c_str());
    }
    
    ESP_LOGI(TAG, "Attempting to connect to MQTT broker...");
    if (!mqtt_->Connect(broker_address, broker_port, client_id, username, password)) {
        ESP_LOGE(TAG, "Failed to connect to endpoint %s:%d", broker_address.c_str(), broker_port);
        SetError(Lang::Strings::SERVER_NOT_CONNECTED);
        return false;
    }

    ESP_LOGI(TAG, "Successfully connected to endpoint %s:%d", broker_address.c_str(), broker_port);
    return true;
}

bool MqttProtocol::SendText(const std::string& text) {
    if (publish_topic_.empty()) {
        ESP_LOGW(TAG, "Publish topic is empty");
        return false;
    }
    
    ESP_LOGI(TAG, "Sending message to topic: %s", publish_topic_.c_str());
    ESP_LOGI(TAG, "Message content: %s", text.c_str());
    
    if (!mqtt_->Publish(publish_topic_, text)) {
        ESP_LOGE(TAG, "Failed to publish message: %s", text.c_str());
        SetError(Lang::Strings::SERVER_ERROR);
        return false;
    }
    
    ESP_LOGI(TAG, "Message published successfully");
    return true;
}

bool MqttProtocol::SendAudio(std::unique_ptr<AudioStreamPacket> packet) {
    std::lock_guard<std::mutex> lock(channel_mutex_);
    if (udp_ == nullptr) {
        return false;
    }

    std::string nonce(aes_nonce_);
    *(uint16_t*)&nonce[2] = htons(packet->payload.size());
    *(uint32_t*)&nonce[8] = htonl(packet->timestamp);
    *(uint32_t*)&nonce[12] = htonl(++local_sequence_);

    std::string encrypted;
    encrypted.resize(aes_nonce_.size() + packet->payload.size());
    memcpy(encrypted.data(), nonce.data(), nonce.size());

    size_t nc_off = 0;
    uint8_t stream_block[16] = {0};
    if (mbedtls_aes_crypt_ctr(&aes_ctx_, packet->payload.size(), &nc_off, (uint8_t*)nonce.c_str(), stream_block,
        (uint8_t*)packet->payload.data(), (uint8_t*)&encrypted[nonce.size()]) != 0) {
        ESP_LOGE(TAG, "Failed to encrypt audio data");
        return false;
    }

    return udp_->Send(encrypted) > 0;
}

void MqttProtocol::CloseAudioChannel() {
    ESP_LOGI(TAG, "CloseAudioChannel called");
    ESP_LOGI(TAG, "Current session_id: %s", session_id_.c_str());
    
    {
        std::lock_guard<std::mutex> lock(channel_mutex_);
        if (udp_ != nullptr) {
            ESP_LOGI(TAG, "Closing UDP connection");
            udp_.reset();
        } else {
            ESP_LOGI(TAG, "UDP connection already closed");
        }
    }

    std::string message = "{";
    message += "\"session_id\":\"" + session_id_ + "\",";
    message += "\"type\":\"goodbye\"";
    message += "}";
    
    ESP_LOGI(TAG, "Sending goodbye message: %s", message.c_str());
    SendText(message);

    if (on_audio_channel_closed_ != nullptr) {
        ESP_LOGI(TAG, "Calling on_audio_channel_closed callback");
        on_audio_channel_closed_();
    } else {
        ESP_LOGI(TAG, "No on_audio_channel_closed callback registered");
    }
    
    ESP_LOGI(TAG, "CloseAudioChannel completed");
}

bool MqttProtocol::OpenAudioChannel() {
    ESP_LOGI(TAG, "OpenAudioChannel called");
    ESP_LOGI(TAG, "MQTT connection status: %s", (mqtt_ != nullptr && mqtt_->IsConnected()) ? "Connected" : "Not connected");
    
    if (mqtt_ == nullptr || !mqtt_->IsConnected()) {
        ESP_LOGI(TAG, "MQTT is not connected, try to connect now");
        if (!StartMqttClient(true)) {
            ESP_LOGE(TAG, "Failed to establish MQTT connection");
            return false;
        }
    }

    error_occurred_ = false;
    session_id_ = "";
    xEventGroupClearBits(event_group_handle_, MQTT_PROTOCOL_SERVER_HELLO_EVENT);
        
    auto message = GetHelloMessage();
    ESP_LOGI(TAG, "Sending hello message: %s", message.c_str());
    if (!SendText(message)) {
        ESP_LOGE(TAG, "Failed to send hello message");
        return false;
    }

    // Chờ phản hồi từ máy chủ
    ESP_LOGI(TAG, "Waiting for server hello response (10 second timeout)...");
    EventBits_t bits = xEventGroupWaitBits(event_group_handle_, MQTT_PROTOCOL_SERVER_HELLO_EVENT, pdTRUE, pdFALSE, pdMS_TO_TICKS(10000));
    if (!(bits & MQTT_PROTOCOL_SERVER_HELLO_EVENT)) {
        ESP_LOGE(TAG, "Failed to receive server hello after 10 seconds");
        ESP_LOGI(TAG, "MQTT connection status: %s", mqtt_->IsConnected() ? "Connected" : "Disconnected");
        SetError(Lang::Strings::SERVER_TIMEOUT);
        return false;
    }

    // Set up UDP connection as specified in mqtt-udp.md
    std::lock_guard<std::mutex> lock(channel_mutex_);
    auto network = Board::GetInstance().GetNetwork();
    udp_ = network->CreateUdp(2);
    udp_->OnMessage([this](const std::string& data) {
        /*
         * UDP Encrypted OPUS Packet Format according to mqtt-udp.md:
         * |type 1u|flags 1u|payload_len 2u|ssrc 4u|timestamp 4u|sequence 4u|
         * |payload payload_len|
         */
        if (data.size() < sizeof(aes_nonce_)) {
            ESP_LOGE(TAG, "Invalid audio packet size: %u", data.size());
            return;
        }
        if (data[0] != 0x01) {
            ESP_LOGE(TAG, "Invalid audio packet type: %x", data[0]);
            return;
        }
        uint32_t timestamp = ntohl(*(uint32_t*)&data[8]);
        uint32_t sequence = ntohl(*(uint32_t*)&data[12]);
        if (sequence < remote_sequence_) {
            ESP_LOGW(TAG, "Received audio packet with old sequence: %lu, expected: %lu", sequence, remote_sequence_);
            return;
        }
        if (sequence != remote_sequence_ + 1) {
            ESP_LOGW(TAG, "Received audio packet with wrong sequence: %lu, expected: %lu", sequence, remote_sequence_ + 1);
        }

        size_t decrypted_size = data.size() - aes_nonce_.size();
        size_t nc_off = 0;
        uint8_t stream_block[16] = {0};
        auto nonce = (uint8_t*)data.data();
        auto encrypted = (uint8_t*)data.data() + aes_nonce_.size();
        auto packet = std::make_unique<AudioStreamPacket>();
        packet->sample_rate = server_sample_rate_;
        packet->frame_duration = server_frame_duration_;
        packet->timestamp = timestamp;
        packet->payload.resize(decrypted_size);
        int ret = mbedtls_aes_crypt_ctr(&aes_ctx_, decrypted_size, &nc_off, nonce, stream_block, encrypted, (uint8_t*)packet->payload.data());
        if (ret != 0) {
            ESP_LOGE(TAG, "Failed to decrypt audio data, ret: %d", ret);
            return;
        }
        if (on_incoming_audio_ != nullptr) {
            on_incoming_audio_(std::move(packet));
        }
        remote_sequence_ = sequence;
        last_incoming_time_ = std::chrono::steady_clock::now();
    });

    udp_->Connect(udp_server_, udp_port_);
    ESP_LOGI(TAG, "UDP connection established to %s:%d", udp_server_.c_str(), udp_port_);

    if (on_audio_channel_opened_ != nullptr) {
        on_audio_channel_opened_();
    }
    
    ESP_LOGI(TAG, "OpenAudioChannel completed successfully");
    return true;
}

std::string MqttProtocol::GetHelloMessage() {
    // Send hello message according to mqtt-udp.md specification
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "type", "hello");
    cJSON_AddNumberToObject(root, "version", 3);
    cJSON_AddStringToObject(root, "transport", "udp");
    cJSON* features = cJSON_CreateObject();
#if CONFIG_USE_SERVER_AEC
    cJSON_AddBoolToObject(features, "aec", true);
#endif
    cJSON_AddBoolToObject(features, "mcp", true);
    cJSON_AddItemToObject(root, "features", features);
    cJSON* audio_params = cJSON_CreateObject();
    cJSON_AddStringToObject(audio_params, "format", "opus");
    cJSON_AddNumberToObject(audio_params, "sample_rate", 16000);
    cJSON_AddNumberToObject(audio_params, "channels", 1);
    cJSON_AddNumberToObject(audio_params, "frame_duration", OPUS_FRAME_DURATION_MS);
    cJSON_AddItemToObject(root, "audio_params", audio_params);
    auto json_str = cJSON_PrintUnformatted(root);
    std::string message(json_str);
    ESP_LOGI(TAG, "Generated hello message: %s", message.c_str());
    cJSON_free(json_str);
    cJSON_Delete(root);
    return message;
}

void MqttProtocol::ParseServerHello(const cJSON* root) {
    ESP_LOGI(TAG, "Received server hello message");
    
    auto transport = cJSON_GetObjectItem(root, "transport");
    if (transport == nullptr) {
        ESP_LOGE(TAG, "Transport field is missing in server hello");
        return;
    }
    
    if (!cJSON_IsString(transport)) {
        ESP_LOGE(TAG, "Transport field is not a string");
        return;
    }
    
    ESP_LOGI(TAG, "Transport: %s", transport->valuestring);
    if (strcmp(transport->valuestring, "udp") != 0) {
        ESP_LOGE(TAG, "Unsupported transport: %s", transport->valuestring);
        return;
    }

    auto session_id = cJSON_GetObjectItem(root, "session_id");
    if (cJSON_IsString(session_id)) {
        session_id_ = session_id->valuestring;
        ESP_LOGI(TAG, "Session ID: %s", session_id_.c_str());
    } else {
        ESP_LOGW(TAG, "Session ID is missing or invalid");
    }

    // Get sample rate from hello message
    auto audio_params = cJSON_GetObjectItem(root, "audio_params");
    if (cJSON_IsObject(audio_params)) {
        auto sample_rate = cJSON_GetObjectItem(audio_params, "sample_rate");
        if (cJSON_IsNumber(sample_rate)) {
            server_sample_rate_ = sample_rate->valueint;
            ESP_LOGI(TAG, "Server sample rate: %d", server_sample_rate_);
        }
        auto frame_duration = cJSON_GetObjectItem(audio_params, "frame_duration");
        if (cJSON_IsNumber(frame_duration)) {
            server_frame_duration_ = frame_duration->valueint;
            ESP_LOGI(TAG, "Server frame duration: %d", server_frame_duration_);
        }
    } else {
        ESP_LOGW(TAG, "Audio params are missing or invalid");
    }

    // Get UDP configuration as specified in mqtt-udp.md
    auto udp = cJSON_GetObjectItem(root, "udp");
    if (!cJSON_IsObject(udp)) {
        ESP_LOGE(TAG, "UDP configuration is missing or invalid");
        return;
    }
    
    auto udp_server = cJSON_GetObjectItem(udp, "server");
    auto udp_port = cJSON_GetObjectItem(udp, "port");
    auto key = cJSON_GetObjectItem(udp, "key");
    auto nonce = cJSON_GetObjectItem(udp, "nonce");
    
    if (!cJSON_IsString(udp_server) || !cJSON_IsNumber(udp_port) || 
        !cJSON_IsString(key) || !cJSON_IsString(nonce)) {
        ESP_LOGE(TAG, "UDP configuration fields are missing or invalid");
        return;
    }
    
    udp_server_ = udp_server->valuestring;
    udp_port_ = udp_port->valueint;
    ESP_LOGI(TAG, "UDP server: %s, port: %d", udp_server_.c_str(), udp_port_);
    
    auto key_str = key->valuestring;
    auto nonce_str = nonce->valuestring;
    ESP_LOGI(TAG, "Key length: %d, Nonce length: %d", strlen(key_str), strlen(nonce_str));

    // auto encryption = cJSON_GetObjectItem(udp, "encryption")->valuestring;
    // ESP_LOGI(TAG, "UDP server: %s, port: %d, encryption: %s", udp_server_.c_str(), udp_port_, encryption);
    aes_nonce_ = DecodeHexString(nonce_str);
    mbedtls_aes_init(&aes_ctx_);
    mbedtls_aes_setkey_enc(&aes_ctx_, (const unsigned char*)DecodeHexString(key_str).c_str(), 128);
    local_sequence_ = 0;
    remote_sequence_ = 0;
    ESP_LOGI(TAG, "Server hello parsing completed successfully");
    xEventGroupSetBits(event_group_handle_, MQTT_PROTOCOL_SERVER_HELLO_EVENT);
}

static const char hex_chars[] = "0123456789ABCDEF";
// Hàm chuyển đổi một ký tự hexa thành một số nguyên
static inline uint8_t CharToHex(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;  // Đối không hợp lệ
}

std::string MqttProtocol::DecodeHexString(const std::string& hex_string) {
    std::string decoded;
    decoded.reserve(hex_string.size() / 2);
    for (size_t i = 0; i < hex_string.size(); i += 2) {
        char byte = (CharToHex(hex_string[i]) << 4) | CharToHex(hex_string[i + 1]);
        decoded.push_back(byte);
    }
    return decoded;
}

bool MqttProtocol::IsAudioChannelOpened() const {
    return udp_ != nullptr && !error_occurred_ && !IsTimeout();
}
