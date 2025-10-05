#include "movements.h"

#include <algorithm>
#include <cstring>

#include "oscillator.h"

Otto::Otto() {
    is_otto_resting_ = false;
    for (int i = 0; i < SERVO_COUNT; i++) {
        servo_pins_[i] = -1;
        servo_trim_[i] = 0;
    }
}

Otto::~Otto() {
    DetachServos();
}

unsigned long IRAM_ATTR millis() {
    return (unsigned long)(esp_timer_get_time() / 1000ULL);
}

void Otto::Init(int right_pitch, int right_roll, int left_pitch, int left_roll, int body,
                int head) {
    servo_pins_[RIGHT_PITCH] = right_pitch;
    servo_pins_[RIGHT_ROLL] = right_roll;
    servo_pins_[LEFT_PITCH] = left_pitch;
    servo_pins_[LEFT_ROLL] = left_roll;
    servo_pins_[BODY] = body;
    servo_pins_[HEAD] = head;

    AttachServos();
    is_otto_resting_ = false;
}

///////////////////////////////////////////////////////////////////
//-- ATTACH & DETACH FUNCTIONS ----------------------------------//
///////////////////////////////////////////////////////////////////
void Otto::AttachServos() {
    for (int i = 0; i < SERVO_COUNT; i++) {
        if (servo_pins_[i] != -1) {
            servo_[i].Attach(servo_pins_[i]);
        }
    }
}

void Otto::DetachServos() {
    for (int i = 0; i < SERVO_COUNT; i++) {
        if (servo_pins_[i] != -1) {
            servo_[i].Detach();
        }
    }
}

///////////////////////////////////////////////////////////////////
//-- OSCILLATORS TRIMS ------------------------------------------//
///////////////////////////////////////////////////////////////////
void Otto::SetTrims(int right_pitch, int right_roll, int left_pitch, int left_roll, int body,
                    int head) {
    servo_trim_[RIGHT_PITCH] = right_pitch;
    servo_trim_[RIGHT_ROLL] = right_roll;
    servo_trim_[LEFT_PITCH] = left_pitch;
    servo_trim_[LEFT_ROLL] = left_roll;
    servo_trim_[BODY] = body;
    servo_trim_[HEAD] = head;

    for (int i = 0; i < SERVO_COUNT; i++) {
        if (servo_pins_[i] != -1) {
            servo_[i].SetTrim(servo_trim_[i]);
        }
    }
}

///////////////////////////////////////////////////////////////////
//-- BASIC MOTION FUNCTIONS -------------------------------------//
///////////////////////////////////////////////////////////////////
void Otto::MoveServos(int time, int servo_target[]) {
    if (GetRestState() == true) {
        SetRestState(false);
    }

    final_time_ = millis() + time;
    if (time > 10) {
        for (int i = 0; i < SERVO_COUNT; i++) {
            if (servo_pins_[i] != -1) {
                increment_[i] = (servo_target[i] - servo_[i].GetPosition()) / (time / 10.0);
            }
        }

        for (int iteration = 1; millis() < final_time_; iteration++) {
            partial_time_ = millis() + 10;
            for (int i = 0; i < SERVO_COUNT; i++) {
                if (servo_pins_[i] != -1) {
                    servo_[i].SetPosition(servo_[i].GetPosition() + increment_[i]);
                }
            }
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    } else {
        for (int i = 0; i < SERVO_COUNT; i++) {
            if (servo_pins_[i] != -1) {
                servo_[i].SetPosition(servo_target[i]);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(time));
    }

    // final adjustment to the target.
    bool f = true;
    int adjustment_count = 0;
    while (f && adjustment_count < 10) {
        f = false;
        for (int i = 0; i < SERVO_COUNT; i++) {
            if (servo_pins_[i] != -1 && servo_target[i] != servo_[i].GetPosition()) {
                f = true;
                break;
            }
        }
        if (f) {
            for (int i = 0; i < SERVO_COUNT; i++) {
                if (servo_pins_[i] != -1) {
                    servo_[i].SetPosition(servo_target[i]);
                }
            }
            vTaskDelay(pdMS_TO_TICKS(10));
            adjustment_count++;
        }
    };
}

void Otto::MoveSingle(int position, int servo_number) {
    if (position > 180)
        position = 90;
    if (position < 0)
        position = 90;

    if (GetRestState() == true) {
        SetRestState(false);
    }

    if (servo_number >= 0 && servo_number < SERVO_COUNT && servo_pins_[servo_number] != -1) {
        servo_[servo_number].SetPosition(position);
    }
}

void Otto::OscillateServos(int amplitude[SERVO_COUNT], int offset[SERVO_COUNT], int period,
                           double phase_diff[SERVO_COUNT], float cycle = 1) {
    for (int i = 0; i < SERVO_COUNT; i++) {
        if (servo_pins_[i] != -1) {
            servo_[i].SetO(offset[i]);
            servo_[i].SetA(amplitude[i]);
            servo_[i].SetT(period);
            servo_[i].SetPh(phase_diff[i]);
        }
    }

    double ref = millis();
    double end_time = period * cycle + ref;

    while (millis() < end_time) {
        for (int i = 0; i < SERVO_COUNT; i++) {
            if (servo_pins_[i] != -1) {
                servo_[i].Refresh();
            }
        }
        vTaskDelay(5);
    }
    vTaskDelay(pdMS_TO_TICKS(10));
}

void Otto::Execute(int amplitude[SERVO_COUNT], int offset[SERVO_COUNT], int period,
                   double phase_diff[SERVO_COUNT], float steps = 1.0) {
    if (GetRestState() == true) {
        SetRestState(false);
    }

    int cycles = (int)steps;

    //-- Execute complete cycles
    if (cycles >= 1)
        for (int i = 0; i < cycles; i++)
            OscillateServos(amplitude, offset, period, phase_diff);

    //-- Execute the final not complete cycle
    OscillateServos(amplitude, offset, period, phase_diff, (float)steps - cycles);
    vTaskDelay(pdMS_TO_TICKS(10));
}

///////////////////////////////////////////////////////////////////
//-- HOME = Otto at rest position -------------------------------//
///////////////////////////////////////////////////////////////////
void Otto::Home(bool hands_down) {
    if (is_otto_resting_ == false) {  // Go to rest position only if necessary
        MoveServos(1000, servo_initial_);
        is_otto_resting_ = true;
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
}

bool Otto::GetRestState() {
    return is_otto_resting_;
}

void Otto::SetRestState(bool state) {
    is_otto_resting_ = state;
}

///////////////////////////////////////////////////////////////////
//-- PREDETERMINED MOTION SEQUENCES -----------------------------//
///////////////////////////////////////////////////////////////////

//---------------------------------------------------------
//-- Hàm hành động tay thống nhất
//--  Parameters:
//--    action: Loại hành động 1=nâng tay trái, 2=nâng tay phải, 3=nâng cả hai tay, 4=hạ tay trái, 5=hạ tay phải, 6=hạ cả hai tay,
//--            7=vẫy tay trái, 8=vẫy tay phải, 9=vẫy cả hai tay, 10=vỗ tay trái, 11=vỗ tay phải, 12=vỗ cả hai tay
//--    times: Số lần lặp lại
//--    amount: Biên độ hành động (10-50)
//--    period: Thời gian hành động
//---------------------------------------------------------
void Otto::HandAction(int action, int times, int amount, int period) {
    // Giới hạn phạm vi tham số
    times = 2 * std::max(3, std::min(100, times));
    amount = std::max(10, std::min(50, amount));
    period = std::max(100, std::min(1000, period));

    int current_positions[SERVO_COUNT];
    for (int i = 0; i < SERVO_COUNT; i++) {
        current_positions[i] = (servo_pins_[i] != -1) ? servo_[i].GetPosition() : servo_initial_[i];
    }

    switch (action) {
        case 1:  // Nâng tay trái
            current_positions[LEFT_PITCH] = 180;
            MoveServos(period, current_positions);
            break;

        case 2:  // Nâng tay phải
            current_positions[RIGHT_PITCH] = 0;
            MoveServos(period, current_positions);
            break;

        case 3:  // Nâng cả hai tay
            current_positions[LEFT_PITCH] = 180;
            current_positions[RIGHT_PITCH] = 0;
            MoveServos(period, current_positions);
            break;

        case 4:  // Hạ tay trái
        case 5:  // Hạ tay phải
        case 6:  // Hạ cả hai tay
            // Trở về vị trí ban đầu
            memcpy(current_positions, servo_initial_, sizeof(current_positions));
            MoveServos(period, current_positions);
            break;

        case 7:  // Vẫy tay trái
            current_positions[LEFT_PITCH] = 150;
            MoveServos(period, current_positions);
            for (int i = 0; i < times; i++) {
                current_positions[LEFT_PITCH] = 150 + (i % 2 == 0 ? -30 : 30);
                MoveServos(period / 10, current_positions);
                vTaskDelay(pdMS_TO_TICKS(period / 10));
            }
            memcpy(current_positions, servo_initial_, sizeof(current_positions));
            MoveServos(period, current_positions);
            break;

        case 8:  // Vẫy tay phải
            current_positions[RIGHT_PITCH] = 30;
            MoveServos(period, current_positions);
            for (int i = 0; i < times; i++) {
                current_positions[RIGHT_PITCH] = 30 + (i % 2 == 0 ? 30 : -30);
                MoveServos(period / 10, current_positions);
                vTaskDelay(pdMS_TO_TICKS(period / 10));
            }
            memcpy(current_positions, servo_initial_, sizeof(current_positions));
            MoveServos(period, current_positions);
            break;

        case 9:  // Vẫy cả hai tay
            current_positions[LEFT_PITCH] = 150;
            current_positions[RIGHT_PITCH] = 30;
            MoveServos(period, current_positions);
            for (int i = 0; i < times; i++) {
                current_positions[LEFT_PITCH] = 150 + (i % 2 == 0 ? -30 : 30);
                current_positions[RIGHT_PITCH] = 30 + (i % 2 == 0 ? 30 : -30);
                MoveServos(period / 10, current_positions);
                vTaskDelay(pdMS_TO_TICKS(period / 10));
            }
            memcpy(current_positions, servo_initial_, sizeof(current_positions));
            MoveServos(period, current_positions);
            break;

        case 10:  // Vỗ tay trái
            current_positions[LEFT_ROLL] = 20;
            MoveServos(period, current_positions);
            for (int i = 0; i < times; i++) {
                current_positions[LEFT_ROLL] = 20 - amount;
                MoveServos(period / 10, current_positions);
                current_positions[LEFT_ROLL] = 20 + amount;
                MoveServos(period / 10, current_positions);
            }
            current_positions[LEFT_ROLL] = 0;
            MoveServos(period, current_positions);
            break;

        case 11:  // Vỗ tay phải
            current_positions[RIGHT_ROLL] = 160;
            MoveServos(period, current_positions);
            for (int i = 0; i < times; i++) {
                current_positions[RIGHT_ROLL] = 160 + amount;
                MoveServos(period / 10, current_positions);
                current_positions[RIGHT_ROLL] = 160 - amount;
                MoveServos(period / 10, current_positions);
            }
            current_positions[RIGHT_ROLL] = 180;
            MoveServos(period, current_positions);
            break;

        case 12:  // Vỗ cả hai tay
            current_positions[LEFT_ROLL] = 20;
            current_positions[RIGHT_ROLL] = 160;
            MoveServos(period, current_positions);
            for (int i = 0; i < times; i++) {
                current_positions[LEFT_ROLL] = 20 - amount;
                current_positions[RIGHT_ROLL] = 160 + amount;
                MoveServos(period / 10, current_positions);
                current_positions[LEFT_ROLL] = 20 + amount;
                current_positions[RIGHT_ROLL] = 160 - amount;
                MoveServos(period / 10, current_positions);
            }
            current_positions[LEFT_ROLL] = 0;
            current_positions[RIGHT_ROLL] = 180;
            MoveServos(period, current_positions);
            break;
    }
}

//---------------------------------------------------------
//-- Hàm hành động thân thống nhất
//--  Parameters:
//--    action: Loại hành động 1=quay trái, 2=quay phải, 3=về tâm
//--    times: Số lần quay
//--    amount: Góc quay (0-90 độ, với 90 độ là tâm quay trái phải)
//--    period: Thời gian hành động
//---------------------------------------------------------
void Otto::BodyAction(int action, int times, int amount, int period) {
    // Giới hạn phạm vi tham số
    times = std::max(1, std::min(10, times));
    amount = std::max(0, std::min(90, amount));
    period = std::max(500, std::min(3000, period));

    int current_positions[SERVO_COUNT];
    for (int i = 0; i < SERVO_COUNT; i++) {
        if (servo_pins_[i] != -1) {
            current_positions[i] = servo_[i].GetPosition();
        } else {
            current_positions[i] = servo_initial_[i];
        }
    }

    int body_center = servo_initial_[BODY];
    int target_angle = body_center;

    switch (action) {
        case 1:  // Quay trái
            target_angle = body_center + amount;
            target_angle = std::min(180, target_angle);
            break;
        case 2:  // Quay phải
            target_angle = body_center - amount;
            target_angle = std::max(0, target_angle);
            break;
        case 3:  // Về tâm
            target_angle = body_center;
            break;
        default:
            return;  // Hành động không hợp lệ
    }

    current_positions[BODY] = target_angle;
    MoveServos(period, current_positions);
    vTaskDelay(pdMS_TO_TICKS(100));
}

//---------------------------------------------------------
//-- Hàm hành động đầu thống nhất
//--  Parameters:
//--    action: Loại hành động 1=ngẩng đầu, 2=cúi đầu, 3=gật đầu, 4=về tâm, 5=gật đầu liên tục
//--    times: Số lần lặp lại (chỉ có hiệu lực với gật đầu liên tục)
//--    amount: Độ lệch góc (trong phạm vi 1-15 độ)
//--    period: Thời gian hành động
//---------------------------------------------------------
void Otto::HeadAction(int action, int times, int amount, int period) {
    // Giới hạn phạm vi tham số
    times = std::max(1, std::min(10, times));
    amount = std::max(1, std::min(15, abs(amount)));
    period = std::max(300, std::min(3000, period));

    int current_positions[SERVO_COUNT];
    for (int i = 0; i < SERVO_COUNT; i++) {
        if (servo_pins_[i] != -1) {
            current_positions[i] = servo_[i].GetPosition();
        } else {
            current_positions[i] = servo_initial_[i];
        }
    }

    int head_center = 90;  // Vị trí tâm đầu

    switch (action) {
        case 1:                                              // Ngẩng đầu
            current_positions[HEAD] = head_center + amount;  // Ngẩng đầu là tăng góc
            MoveServos(period, current_positions);
            break;

        case 2:                                              // Cúi đầu
            current_positions[HEAD] = head_center - amount;  // Cúi đầu là giảm góc
            MoveServos(period, current_positions);
            break;

        case 3:  // Gật đầu (chuyển động lên xuống)
            // Trước tiên ngẩng đầu
            current_positions[HEAD] = head_center + amount;
            MoveServos(period / 3, current_positions);
            vTaskDelay(pdMS_TO_TICKS(period / 6));

            // Sau đó cúi đầu
            current_positions[HEAD] = head_center - amount;
            MoveServos(period / 3, current_positions);
            vTaskDelay(pdMS_TO_TICKS(period / 6));

            // Trở về tâm
            current_positions[HEAD] = head_center;
            MoveServos(period / 3, current_positions);
            break;

        case 4:  // Trở về vị trí tâm
            current_positions[HEAD] = head_center;
            MoveServos(period, current_positions);
            break;

        case 5:  // Gật đầu liên tục
            for (int i = 0; i < times; i++) {
                // Ngẩng đầu
                current_positions[HEAD] = head_center + amount;
                MoveServos(period / 2, current_positions);

                // Cúi đầu
                current_positions[HEAD] = head_center - amount;
                MoveServos(period / 2, current_positions);

                vTaskDelay(pdMS_TO_TICKS(50));  // Tạm dừng ngắn
            }

            // Trở về tâm
            current_positions[HEAD] = head_center;
            MoveServos(period / 2, current_positions);
            break;

        default:
            // Hành động không hợp lệ, trở về tâm
            current_positions[HEAD] = head_center;
            MoveServos(period, current_positions);
            break;
    }
}
