#include "audio_codec.h"
#include "board.h"
#include "settings.h"

#include <esp_log.h>
#include <cstring>
#include <driver/i2s_common.h>

#define TAG "AudioCodec"

AudioCodec::AudioCodec() {
}

AudioCodec::~AudioCodec() {
}

void AudioCodec::OutputData(std::vector<int16_t>& data) {
    Write(data.data(), data.size());
}

bool AudioCodec::InputData(std::vector<int16_t>& data) {
    int samples = Read(data.data(), data.size());
    if (samples > 0) {
        return true;
    }
    return false;
}

void AudioCodec::Start() {
    Settings settings("audio", false);
    output_volume_ = settings.GetInt("output_volume", output_volume_);
    if (output_volume_ <= 0) {
        ESP_LOGW(TAG, "Output volume value (%d) is too small, setting to default (10)", output_volume_);
        output_volume_ = 10;
    }

    if (tx_handle_ != nullptr) {
        ESP_ERROR_CHECK(i2s_channel_enable(tx_handle_));
    }

    if (rx_handle_ != nullptr) {
        ESP_ERROR_CHECK(i2s_channel_enable(rx_handle_));
    }

    EnableInput(true);
    EnableOutput(true);
    ESP_LOGI(TAG, "Audio codec started");
}

void AudioCodec::SetOutputVolume(int volume) {
    output_volume_ = volume;
    ESP_LOGI(TAG, "Set output volume to %d", output_volume_);
    
    Settings settings("audio", true);
    settings.SetInt("output_volume", output_volume_);
}

void AudioCodec::SetInputGain(float gain) {
    input_gain_ = gain;
    ESP_LOGI(TAG, "Set input gain to %.1f", input_gain_);
}

void AudioCodec::EnableInput(bool enable) {
    if (enable == input_enabled_) {
        return;
    }
    input_enabled_ = enable;
    ESP_LOGI(TAG, "Set input enable to %s", enable ? "true" : "false");
}

void AudioCodec::EnableOutput(bool enable) {
    if (enable == output_enabled_) {
        return;
    }
    output_enabled_ = enable;
    ESP_LOGI(TAG, "Set output enable to %s", enable ? "true" : "false");
}

bool AudioCodec::SetOutputSampleRate(int sample_rate) {
    // Xử lý đặc biệt: Nếu truyền vào -1, có nghĩa là đặt lại về tần số lấy mẫu gốc
    if (sample_rate == -1) {
        if (original_output_sample_rate_ > 0) {
            sample_rate = original_output_sample_rate_;
            ESP_LOGI(TAG, "Đang đặt lại về tần số lấy mẫu đầu ra gốc: %d Hz", sample_rate);
        } else {
            ESP_LOGW(TAG, "Tần số lấy mẫu gốc không khả dụng, không thể đặt lại");
            return false;
        }
    }
    
    if (sample_rate <= 0 || sample_rate > 192000) {
        ESP_LOGE(TAG, "Tần số lấy mẫu không hợp lệ: %d", sample_rate);
        return false;
    }
    
    if (output_sample_rate_ == sample_rate) {
        ESP_LOGI(TAG, "Tần số lấy mẫu đã được đặt thành %d Hz", sample_rate);
        return true;
    }
    
    if (tx_handle_ == nullptr) {
        ESP_LOGW(TAG, "TX handle là null, chỉ cập nhật biến tần số lấy mẫu");
        output_sample_rate_ = sample_rate;
        return true;
    }
    
    ESP_LOGI(TAG, "Thay đổi tần số lấy mẫu đầu ra từ %d thành %d Hz", output_sample_rate_, sample_rate);
    
    // Trước tiên thử vô hiệu hóa kênh I2S (nếu đã được kích hoạt)
    esp_err_t disable_ret = i2s_channel_disable(tx_handle_);
    if (disable_ret == ESP_OK) {
        ESP_LOGI(TAG, "Đã vô hiệu hóa kênh I2S TX để cấu hình lại");
    } else if (disable_ret == ESP_ERR_INVALID_STATE) {
        // Kênh có thể đã ở trạng thái vô hiệu hóa, điều này là bình thường
        ESP_LOGI(TAG, "Kênh I2S TX đã được vô hiệu hóa trước đó");
    } else {
        ESP_LOGW(TAG, "Không thể vô hiệu hóa kênh I2S TX: %s", esp_err_to_name(disable_ret));
    }
    
    // Cấu hình lại đồng hồ I2S
    i2s_std_clk_config_t clk_cfg = {
        .sample_rate_hz = (uint32_t)sample_rate,
        .clk_src = I2S_CLK_SRC_DEFAULT,
        .mclk_multiple = I2S_MCLK_MULTIPLE_256,
#ifdef I2S_HW_VERSION_2
        .ext_clk_freq_hz = 0,
#endif
    };
    
    esp_err_t ret = i2s_channel_reconfig_std_clock(tx_handle_, &clk_cfg);
    
    // Kích hoạt lại kênh (bất kể trạng thái trước đó là gì, bây giờ cần kích hoạt để phát âm thanh)
    esp_err_t enable_ret = i2s_channel_enable(tx_handle_);
    if (enable_ret != ESP_OK) {
        ESP_LOGE(TAG, "Không thể kích hoạt kênh I2S TX: %s", esp_err_to_name(enable_ret));
    } else {
        ESP_LOGI(TAG, "Đã kích hoạt kênh I2S TX");
    }
    
    if (ret == ESP_OK) {
        output_sample_rate_ = sample_rate;
        ESP_LOGI(TAG, "Đã thay đổi thành công tần số lấy mẫu đầu ra thành %d Hz", sample_rate);
        return true;
    } else {
        ESP_LOGE(TAG, "Không thể thay đổi tần số lấy mẫu thành %d Hz: %s", sample_rate, esp_err_to_name(ret));
        return false;
    }
}