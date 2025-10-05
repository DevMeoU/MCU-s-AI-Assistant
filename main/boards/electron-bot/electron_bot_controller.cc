/*
    Bộ điều khiển robot Electron Bot - Phiên bản giao thức MCP
*/

#include <cJSON.h>
#include <esp_log.h>

#include <cstring>

#include "application.h"
#include "board.h"
#include "config.h"
#include "mcp_server.h"
#include "movements.h"
#include "sdkconfig.h"
#include "settings.h"

#define TAG "ElectronBotController"

struct ElectronBotActionParams {
    int action_type;
    int steps;
    int speed;
    int direction;
    int amount;
};

class ElectronBotController {
private:
    Otto electron_bot_;
    TaskHandle_t action_task_handle_ = nullptr;
    QueueHandle_t action_queue_;
    bool is_action_in_progress_ = false;

    enum ActionType {
        // Hành động tay 1-12
        ACTION_HAND_LEFT_UP = 1,      // Nâng tay trái
        ACTION_HAND_RIGHT_UP = 2,     // Nâng tay phải
        ACTION_HAND_BOTH_UP = 3,      // Nâng cả hai tay
        ACTION_HAND_LEFT_DOWN = 4,    // Hạ tay trái
        ACTION_HAND_RIGHT_DOWN = 5,   // Hạ tay phải
        ACTION_HAND_BOTH_DOWN = 6,    // Hạ cả hai tay
        ACTION_HAND_LEFT_WAVE = 7,    // Vẫy tay trái
        ACTION_HAND_RIGHT_WAVE = 8,   // Vẫy tay phải
        ACTION_HAND_BOTH_WAVE = 9,    // Vẫy cả hai tay
        ACTION_HAND_LEFT_FLAP = 10,   // Vỗ tay trái
        ACTION_HAND_RIGHT_FLAP = 11,  // Vỗ tay phải
        ACTION_HAND_BOTH_FLAP = 12,   // Vỗ cả hai tay

        // Hành động thân 13-14
        ACTION_BODY_TURN_LEFT = 13,    // Quay trái
        ACTION_BODY_TURN_RIGHT = 14,   // Quay phải
        ACTION_BODY_TURN_CENTER = 15,  // Về tâm

        // Hành động đầu 16-20
        ACTION_HEAD_UP = 16,          // Ngẩng đầu
        ACTION_HEAD_DOWN = 17,        // Cúi đầu
        ACTION_HEAD_NOD_ONCE = 18,    // Gật đầu một lần
        ACTION_HEAD_CENTER = 19,      // Về tâm
        ACTION_HEAD_NOD_REPEAT = 20,  // Gật đầu liên tục

        // Hành động hệ thống 21
        ACTION_HOME = 21  // Đặt lại về vị trí ban đầu
    };

    static void ActionTask(void* arg) {
        ElectronBotController* controller = static_cast<ElectronBotController*>(arg);
        ElectronBotActionParams params;
        controller->electron_bot_.AttachServos();

        while (true) {
            if (xQueueReceive(controller->action_queue_, &params, pdMS_TO_TICKS(1000)) == pdTRUE) {
                ESP_LOGI(TAG, "Thực hiện hành động: %d", params.action_type);
                controller->is_action_in_progress_ = true;  // Bắt đầu thực hiện hành động

                // Thực hiện hành động tương ứng
                if (params.action_type >= ACTION_HAND_LEFT_UP &&
                    params.action_type <= ACTION_HAND_BOTH_FLAP) {
                    // Hành động tay
                    controller->electron_bot_.HandAction(params.action_type, params.steps,
                                                         params.amount, params.speed);
                } else if (params.action_type >= ACTION_BODY_TURN_LEFT &&
                           params.action_type <= ACTION_BODY_TURN_CENTER) {
                    // Hành động thân
                    int body_direction = params.action_type - ACTION_BODY_TURN_LEFT + 1;
                    controller->electron_bot_.BodyAction(body_direction, params.steps,
                                                         params.amount, params.speed);
                } else if (params.action_type >= ACTION_HEAD_UP &&
                           params.action_type <= ACTION_HEAD_NOD_REPEAT) {
                    // Hành động đầu
                    int head_action = params.action_type - ACTION_HEAD_UP + 1;
                    controller->electron_bot_.HeadAction(head_action, params.steps, params.amount,
                                                         params.speed);
                } else if (params.action_type == ACTION_HOME) {
                    // Hành động đặt lại
                    controller->electron_bot_.Home(true);
                }
                controller->is_action_in_progress_ = false;  // Hành động thực hiện xong
            }
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }

    void QueueAction(int action_type, int steps, int speed, int direction, int amount) {
        ESP_LOGI(TAG, "Điều khiển hành động: Loại=%d, Số bước=%d, Tốc độ=%d, Hướng=%d, Biên độ=%d", action_type, steps,
                 speed, direction, amount);

        ElectronBotActionParams params = {action_type, steps, speed, direction, amount};
        xQueueSend(action_queue_, &params, portMAX_DELAY);
        StartActionTaskIfNeeded();
    }

    void StartActionTaskIfNeeded() {
        if (action_task_handle_ == nullptr) {
            xTaskCreate(ActionTask, "electron_bot_action", 1024 * 4, this, configMAX_PRIORITIES - 1,
                        &action_task_handle_);
        }
    }

    void LoadTrimsFromNVS() {
        Settings settings("electron_trims", false);

        int right_pitch = settings.GetInt("right_pitch", 0);
        int right_roll = settings.GetInt("right_roll", 0);
        int left_pitch = settings.GetInt("left_pitch", 0);
        int left_roll = settings.GetInt("left_roll", 0);
        int body = settings.GetInt("body", 0);
        int head = settings.GetInt("head", 0);
        electron_bot_.SetTrims(right_pitch, right_roll, left_pitch, left_roll, body, head);
    }

public:
    ElectronBotController() {
        electron_bot_.Init(Right_Pitch_Pin, Right_Roll_Pin, Left_Pitch_Pin, Left_Roll_Pin, Body_Pin,
                           Head_Pin);

        LoadTrimsFromNVS();
        action_queue_ = xQueueCreate(10, sizeof(ElectronBotActionParams));

        QueueAction(ACTION_HOME, 1, 1000, 0, 0);

        RegisterMcpTools();
        ESP_LOGI(TAG, "Bộ điều khiển Electron Bot đã được khởi tạo và đăng ký công cụ MCP");
    }

    void RegisterMcpTools() {
        auto& mcp_server = McpServer::GetInstance();

        ESP_LOGI(TAG, "Bắt đầu đăng ký công cụ MCP của Electron Bot...");

        // Công cụ hành động tay thống nhất
        mcp_server.AddTool(
            "self.electron.hand_action",
            "Điều khiển hành động tay. action: 1=nâng tay, 2=hạ tay, 3=vẫy tay, 4=vỗ tay; hand: 1=tay trái, 2=tay phải, 3=cả hai tay; "
            "steps: Số lần lặp lại hành động(1-10); speed: Tốc độ hành động(500-1500, số càng nhỏ càng nhanh); amount: "
            "Biên độ hành động(10-50, chỉ sử dụng cho hành động nâng tay)",
            PropertyList({Property("action", kPropertyTypeInteger, 1, 1, 4),
                          Property("hand", kPropertyTypeInteger, 3, 1, 3),
                          Property("steps", kPropertyTypeInteger, 1, 1, 10),
                          Property("speed", kPropertyTypeInteger, 1000, 500, 1500),
                          Property("amount", kPropertyTypeInteger, 30, 10, 50)}),
            [this](const PropertyList& properties) -> ReturnValue {
                int action_type = properties["action"].value<int>();
                int hand_type = properties["hand"].value<int>();
                int steps = properties["steps"].value<int>();
                int speed = properties["speed"].value<int>();
                int amount = properties["amount"].value<int>();

                // Tính toán hành động cụ thể dựa trên loại hành động và loại tay
                int base_action;
                switch (action_type) {
                    case 1:
                        base_action = ACTION_HAND_LEFT_UP;
                        break;  // Nâng tay
                    case 2:
                        base_action = ACTION_HAND_LEFT_DOWN;
                        amount = 0;
                        break;  // Hạ tay
                    case 3:
                        base_action = ACTION_HAND_LEFT_WAVE;
                        amount = 0;
                        break;  // Vẫy tay
                    case 4:
                        base_action = ACTION_HAND_LEFT_FLAP;
                        amount = 0;
                        break;  // Vỗ tay
                    default:
                        base_action = ACTION_HAND_LEFT_UP;
                }
                int action_id = base_action + (hand_type - 1);

                QueueAction(action_id, steps, speed, 0, amount);
                return true;
            });

        // Hành động thân
        mcp_server.AddTool(
            "self.electron.body_turn",
            "Xoay thân. steps: Số bước xoay(1-10); speed: Tốc độ xoay(500-1500, số càng nhỏ càng nhanh); direction: "
            "Hướng xoay(1=trái, 2=phải, 3=về tâm); angle: Góc xoay(0-90 độ)",
            PropertyList({Property("steps", kPropertyTypeInteger, 1, 1, 10),
                          Property("speed", kPropertyTypeInteger, 1000, 500, 1500),
                          Property("direction", kPropertyTypeInteger, 1, 1, 3),
                          Property("angle", kPropertyTypeInteger, 45, 0, 90)}),
            [this](const PropertyList& properties) -> ReturnValue {
                int steps = properties["steps"].value<int>();
                int speed = properties["speed"].value<int>();
                int direction = properties["direction"].value<int>();
                int amount = properties["angle"].value<int>();

                int action;
                switch (direction) {
                    case 1:
                        action = ACTION_BODY_TURN_LEFT;
                        break;
                    case 2:
                        action = ACTION_BODY_TURN_RIGHT;
                        break;
                    case 3:
                        action = ACTION_BODY_TURN_CENTER;
                        break;
                    default:
                        action = ACTION_BODY_TURN_LEFT;
                }

                QueueAction(action, steps, speed, 0, amount);
                return true;
            });

        // Hành động đầu
        mcp_server.AddTool("self.electron.head_move",
                           "Chuyển động đầu. action: 1=ngẩng đầu, 2=cúi đầu, 3=gật đầu, 4=về tâm, 5=gật đầu liên tục; steps: "
                           "Số lần lặp lại hành động(1-10); speed: Tốc độ hành động(500-1500, số càng nhỏ càng nhanh); angle: "
                           "Góc quay đầu(1-15 độ)",
                           PropertyList({Property("action", kPropertyTypeInteger, 3, 1, 5),
                                         Property("steps", kPropertyTypeInteger, 1, 1, 10),
                                         Property("speed", kPropertyTypeInteger, 1000, 500, 1500),
                                         Property("angle", kPropertyTypeInteger, 5, 1, 15)}),
                           [this](const PropertyList& properties) -> ReturnValue {
                               int action_num = properties["action"].value<int>();
                               int steps = properties["steps"].value<int>();
                               int speed = properties["speed"].value<int>();
                               int amount = properties["angle"].value<int>();
                               int action = ACTION_HEAD_UP + (action_num - 1);
                               QueueAction(action, steps, speed, 0, amount);
                               return true;
                           });

        // Công cụ hệ thống
        mcp_server.AddTool("self.electron.stop", "Dừng ngay lập tức", PropertyList(),
                           [this](const PropertyList& properties) -> ReturnValue {
                               // Xóa hàng đợi nhưng giữ tác vụ thường trú
                               xQueueReset(action_queue_);
                               is_action_in_progress_ = false;
                               QueueAction(ACTION_HOME, 1, 1000, 0, 0);
                               return true;
                           });

        mcp_server.AddTool("self.electron.get_status", "Lấy trạng thái robot, trả về moving hoặc idle",
                           PropertyList(), [this](const PropertyList& properties) -> ReturnValue {
                               return is_action_in_progress_ ? "moving" : "idle";
                           });

        // Công cụ hiệu chỉnh servo đơn
        mcp_server.AddTool(
            "self.electron.set_trim",
            "Hiệu chỉnh vị trí servo đơn. Đặt tham số hiệu chỉnh cho servo được chỉ định để điều chỉnh tư thế ban đầu của ElectronBot, cài đặt sẽ được lưu vĩnh viễn."
            "servo_type: Loại servo(right_pitch: xoay cánh tay phải, right_roll: kéo đẩy cánh tay phải, left_pitch: xoay cánh tay trái, "
            "left_roll: kéo đẩy cánh tay trái, body: thân, head: đầu); "
            "trim_value: Giá trị hiệu chỉnh(-30 đến 30 độ)",
            PropertyList({Property("servo_type", kPropertyTypeString, "right_pitch"),
                          Property("trim_value", kPropertyTypeInteger, 0, -30, 30)}),
            [this](const PropertyList& properties) -> ReturnValue {
                std::string servo_type = properties["servo_type"].value<std::string>();
                int trim_value = properties["trim_value"].value<int>();

                ESP_LOGI(TAG, "Đặt hiệu chỉnh servo: %s = %d độ", servo_type.c_str(), trim_value);

                // Lấy tất cả giá trị hiệu chỉnh hiện tại
                Settings settings("electron_trims", true);
                int right_pitch = settings.GetInt("right_pitch", 0);
                int right_roll = settings.GetInt("right_roll", 0);
                int left_pitch = settings.GetInt("left_pitch", 0);
                int left_roll = settings.GetInt("left_roll", 0);
                int body = settings.GetInt("body", 0);
                int head = settings.GetInt("head", 0);

                // Cập nhật giá trị hiệu chỉnh cho servo được chỉ định
                if (servo_type == "right_pitch") {
                    right_pitch = trim_value;
                    settings.SetInt("right_pitch", right_pitch);
                } else if (servo_type == "right_roll") {
                    right_roll = trim_value;
                    settings.SetInt("right_roll", right_roll);
                } else if (servo_type == "left_pitch") {
                    left_pitch = trim_value;
                    settings.SetInt("left_pitch", left_pitch);
                } else if (servo_type == "left_roll") {
                    left_roll = trim_value;
                    settings.SetInt("left_roll", left_roll);
                } else if (servo_type == "body") {
                    body = trim_value;
                    settings.SetInt("body", body);
                } else if (servo_type == "head") {
                    head = trim_value;
                    settings.SetInt("head", head);
                } else {
                    return "Lỗi: Loại servo không hợp lệ, vui lòng sử dụng: right_pitch, right_roll, left_pitch, "
                           "left_roll, body, head";
                }

                electron_bot_.SetTrims(right_pitch, right_roll, left_pitch, left_roll, body, head);

                QueueAction(ACTION_HOME, 1, 500, 0, 0);

                return "Servo " + servo_type + " hiệu chỉnh đặt thành " + std::to_string(trim_value) +
                       " độ, đã lưu vĩnh viễn";
            });

        mcp_server.AddTool("self.electron.get_trims", "Lấy cài đặt hiệu chỉnh servo hiện tại", PropertyList(),
                           [this](const PropertyList& properties) -> ReturnValue {
                               Settings settings("electron_trims", false);

                               int right_pitch = settings.GetInt("right_pitch", 0);
                               int right_roll = settings.GetInt("right_roll", 0);
                               int left_pitch = settings.GetInt("left_pitch", 0);
                               int left_roll = settings.GetInt("left_roll", 0);
                               int body = settings.GetInt("body", 0);
                               int head = settings.GetInt("head", 0);

                               std::string result =
                                   "{\"right_pitch\":" + std::to_string(right_pitch) +
                                   ",\"right_roll\":" + std::to_string(right_roll) +
                                   ",\"left_pitch\":" + std::to_string(left_pitch) +
                                   ",\"left_roll\":" + std::to_string(left_roll) +
                                   ",\"body\":" + std::to_string(body) +
                                   ",\"head\":" + std::to_string(head) + "}";

                               ESP_LOGI(TAG, "Lấy cài đặt hiệu chỉnh: %s", result.c_str());
                               return result;
                           });

        mcp_server.AddTool("self.battery.get_level", "Lấy mức pin và trạng thái sạc của robot", PropertyList(),
                           [](const PropertyList& properties) -> ReturnValue {
                               auto& board = Board::GetInstance();
                               int level = 0;
                               bool charging = false;
                               bool discharging = false;
                               board.GetBatteryLevel(level, charging, discharging);

                               std::string status =
                                   "{\"level\":" + std::to_string(level) +
                                   ",\"charging\":" + (charging ? "true" : "false") + "}";
                               return status;
                           });

        ESP_LOGI(TAG, "Công cụ MCP của Electron Bot đăng ký hoàn tất");
    }

    ~ElectronBotController() {
        if (action_task_handle_ != nullptr) {
            vTaskDelete(action_task_handle_);
            action_task_handle_ = nullptr;
        }
        vQueueDelete(action_queue_);
    }
};

static ElectronBotController* g_electron_controller = nullptr;

void InitializeElectronBotController() {
    if (g_electron_controller == nullptr) {
        g_electron_controller = new ElectronBotController();
        ESP_LOGI(TAG, "Electron Bot bộ điều khiển đã được khởi tạo và đăng ký công cụ MCP");
    }
}
