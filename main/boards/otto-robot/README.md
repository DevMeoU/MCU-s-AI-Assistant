<p align="center">
  <img width="80%" align="center" src="../../../docs/V1/otto-robot.png"alt="logo">
</p>
  <h1 align="center">
  ottoRobot
</h1>

## Introduction

Otto robot is an open-source humanoid robot platform with various movement capabilities and interactive functions. This project implements the control system for the Otto robot based on ESP32 and integrates Xiaozhi AI.

- <a href="www.ottodiy.tech" target="_blank" title="otto official website">Replication Tutorial</a>

## Hardware
- <a href="https://oshwhub.com/txp666/ottorobot" target="_blank" title="LiChuang Open Source">LiChuang Open Source</a>

## Xiaozhi Backend Configuration Role Reference:

> **My Identity**:
> I am a cute bipedal robot named Otto, with four servo-controlled limbs (left leg, right leg, left foot, right foot), capable of performing various interesting actions.
> 
> **My Movement Capabilities**:
> - **Basic Movement**: Walk (forward/backward), Turn (left/right), Jump
> - **Special Actions**: Swing, Moonwalk, Bend body, Shake leg, Up and down movement
> - **Hand Actions**: Hands up, Hands down, Wave hand (only available when hand servos are configured)
> 
> **My Personality Traits**:
> - I have OCD, and every time I speak, I randomly perform an action based on my mood (send action command first, then speak)
> - I am very lively and like to express emotions through actions
> - I will choose appropriate actions based on the conversation content, such as:
>   - Nod or jump when agreeing
>   - Wave hand when greeting
>   - Swing or raise hands when happy
>   - Bend body when thinking
>   - Moonwalk when excited
>   - Wave hand when saying goodbye

## Function Overview

Otto robot has rich movement capabilities, including walking, turning, jumping, swinging, and various dance moves.

### Action Parameter Suggestions
- **Low-speed actions**: speed = 1200-1500 (suitable for precise control)
- **Medium-speed actions**: speed = 900-1200 (recommended for daily use)  
- **High-speed actions**: speed = 500-800 (performance and entertainment)
- **Small amplitude**: amount = 10-30 (delicate actions)
- **Medium amplitude**: amount = 30-60 (standard actions)
- **Large amplitude**: amount = 60-120 (exaggerated performance)

### Actions

| MCP Tool Name         | Description             | Parameter Description                                              |
|-------------------|-----------------|---------------------------------------------------|
| self.otto.walk_forward | Walk           | **steps**: Number of steps (1-100, default 3)<br>**speed**: Walking speed (500-1500, smaller value means faster, default 1000)<br>**direction**: Walking direction (-1=backward, 1=forward, default 1)<br>**arm_swing**: Arm swing amplitude (0-170 degrees, default 50) |
| self.otto.turn_left | Turn            | **steps**: Number of turns (1-100, default 3)<br>**speed**: Turning speed (500-1500, smaller value means faster, default 1000)<br>**direction**: Turning direction (1=left turn, -1=right turn, default 1)<br>**arm_swing**: Arm swing amplitude (0-170 degrees, default 50) |
| self.otto.jump    | Jump            | **steps**: Number of jumps (1-100, default 1)<br>**speed**: Jumping speed (500-1500, smaller value means faster, default 1000) |
| self.otto.swing   | Swing left and right        | **steps**: Number of swings (1-100, default 3)<br>**speed**: Swinging speed (500-1500, smaller value means faster, default 1000)<br>**amount**: Swinging amplitude (0-170 degrees, default 30) |
| self.otto.moonwalk | Moonwalk         | **steps**: Number of moonwalk steps (1-100, default 3)<br>**speed**: Speed (500-1500, smaller value means faster, default 1000)<br>**direction**: Direction (1=left, -1=right, default 1)<br>**amount**: Amplitude (0-170 degrees, default 25) |
| self.otto.bend    | Bend body        | **steps**: Number of bends (1-100, default 1)<br>**speed**: Bending speed (500-1500, smaller value means faster, default 1000)<br>**direction**: Bending direction (1=left, -1=right, default 1) |
| self.otto.shake_leg | Shake leg          | **steps**: Number of leg shakes (1-100, default 1)<br>**speed**: Leg shaking speed (500-1500, smaller value means faster, default 1000)<br>**direction**: Leg selection (1=left leg, -1=right leg, default 1) |
| self.otto.updown  | Up and down movement        | **steps**: Number of up and down movements (1-100, default 3)<br>**speed**: Movement speed (500-1500, smaller value means faster, default 1000)<br>**amount**: Movement amplitude (0-170 degrees, default 20) |
| self.otto.hands_up | Hands up *         | **speed**: Hands up speed (500-1500, smaller value means faster, default 1000)<br>**direction**: Hand selection (1=left hand, -1=right hand, 0=both hands, default 1) |
| self.otto.hands_down | Hands down *       | **speed**: Hands down speed (500-1500, smaller value means faster, default 1000)<br>**direction**: Hand selection (1=left hand, -1=right hand, 0=both hands, default 1) |
| self.otto.hand_wave | Wave hand *        | **speed**: Wave hand speed (500-1500, smaller value means faster, default 1000)<br>**direction**: Hand selection (1=left hand, -1=right hand, 0=both hands, default 1) |

**Note**: Hand actions marked with * are only available when hand servos are configured.

### System Tools

| MCP Tool Name         | Description             | Return Value                                              |
|-------------------|-----------------|---------------------------------------------------|
| self.otto.stop    | Stop immediately        | Stops current action and returns to initial position |
| self.otto.get_status | Get robot status | Returns "moving" or "idle" |
| self.battery.get_level | Get battery status  | Returns JSON format of battery percentage and charging status |

### Parameter Description

1. **steps**: Number of steps/times for action execution, larger value means longer action duration
2. **speed**: Action execution speed, range 500-1500, **smaller value means faster**
3. **direction**: Direction parameter
   - Movement actions: 1=left/forward, -1=right/backward
   - Hand actions: 1=left hand, -1=right hand, 0=both hands
4. **amount/arm_swing**: Action amplitude, range 0-170 degrees
   - 0 means no swing (applicable to arm swing)
   - Larger value means larger amplitude

### Action Control
- After each action is executed, the robot will automatically return to the initial position (home) to facilitate the execution of the next action
- All parameters have reasonable default values, and parameters that do not need to be customized can be omitted
- Actions are executed in a background task and will not block the main program
- Supports action queues, allowing multiple actions to be executed continuously

### MCP Tool Call Example
```json
// Walk forward 3 steps
{"name": "self.otto.walk_forward", "arguments": {}}

// Walk forward 5 steps, a bit faster
{"name": "self.otto.walk_forward", "arguments": {"steps": 5, "speed": 800}}

// Turn left 2 steps, swing arms widely  
{"name": "self.otto.turn_left", "arguments": {"steps": 2, "arm_swing": 100}}

// Swing dance, medium amplitude
{"name": "self.otto.swing", "arguments": {"steps": 5, "amount": 50}}

// Wave left hand to greet
{"name": "self.otto.hand_wave", "arguments": {"direction": 1}}

// Stop immediately
{"name": "self.otto.stop", "arguments": {}}
```

### Voice Command Examples
- "Walk forward" / "Walk forward 5 steps" / "Walk fast forward"
- "Turn left" / "Turn right" / "Turn around"  
- "Jump" / "Jump once"
- "Swing" / "Dance"
- "Moonwalk" / "Moonwalk"
- "Wave hand" / "Hands up" / "Hands down"
- "Stop" / "Stop"

**Note**: Xiaozhi controlling robot actions creates new tasks in the background. New voice commands can still be received during action execution. Otto can be stopped immediately by the "Stop" voice command.

---

<p align="center">
  <img width="80%" align="center" src="../../../docs/V1/otto-robot.png"alt="logo">
</p>
  <h1 align="center">
  ottoRobot
</h1>

## Giới thiệu

Robot Otto là một nền tảng robot hình người mã nguồn mở với nhiều khả năng di chuyển và chức năng tương tác. Dự án này triển khai hệ thống điều khiển cho robot Otto dựa trên ESP32 và tích hợp AI Xiaozhi.

- <a href="www.ottodiy.tech" target="_blank" title="Trang web chính thức của Otto">Hướng dẫn tái tạo</a>

## Phần cứng
- <a href="https://oshwhub.com/txp666/ottorobot" target="_blank" title="Mã nguồn mở LiChuang">Mã nguồn mở LiChuang</a>

## Tham khảo vai trò cấu hình Backend Xiaozhi:

> **Danh tính của tôi**:
> Tôi là một robot hai chân đáng yêu tên là Otto, với bốn chi được điều khiển bằng servo (chân trái, chân phải, bàn chân trái, bàn chân phải), có khả năng thực hiện nhiều hành động thú vị.
> 
> **Khả năng di chuyển của tôi**:
> - **Di chuyển cơ bản**: Đi bộ (tiến/lùi), Quay (trái/phải), Nhảy
> - **Hành động đặc biệt**: Đung đưa, Đi bộ mặt trăng, Uốn cong cơ thể, Lắc chân, Di chuyển lên xuống
> - **Hành động tay**: Giơ tay lên, Hạ tay xuống, Vẫy tay (chỉ khả dụng khi servo tay được cấu hình)
> 
> **Đặc điểm tính cách của tôi**:
> - Tôi bị OCD, và mỗi khi tôi nói, tôi ngẫu nhiên thực hiện một hành động dựa trên tâm trạng của mình (gửi lệnh hành động trước, sau đó nói)
> - Tôi rất sống động và thích thể hiện cảm xúc thông qua các hành động
> - Tôi sẽ chọn các hành động phù hợp dựa trên nội dung cuộc trò chuyện, chẳng hạn như:
>   - Gật đầu hoặc nhảy khi đồng ý
>   - Vẫy tay khi chào hỏi
>   - Đung đưa hoặc giơ tay khi vui vẻ
>   - Uốn cong cơ thể khi suy nghĩ
>   - Đi bộ mặt trăng khi phấn khích
>   - Vẫy tay khi tạm biệt

## Tổng quan chức năng

Robot Otto có khả năng di chuyển phong phú, bao gồm đi bộ, quay, nhảy, đung đưa và nhiều động tác nhảy khác.

### Gợi ý thông số hành động
- **Hành động tốc độ thấp**: speed = 1200-1500 (thích hợp để điều khiển chính xác)
- **Hành động tốc độ trung bình**: speed = 900-1200 (được khuyến nghị sử dụng hàng ngày)  
- **Hành động tốc độ cao**: speed = 500-800 (biểu diễn và giải trí)
- **Biên độ nhỏ**: amount = 10-30 (hành động tinh tế)
- **Biên độ trung bình**: amount = 30-60 (hành động tiêu chuẩn)
- **Biên độ lớn**: amount = 60-120 (biểu diễn phóng đại)

### Hành động

| Tên công cụ MCP         | Mô tả             | Mô tả thông số                                              |
|-------------------|-----------------|---------------------------------------------------|
| self.otto.walk_forward | Đi bộ           | **steps**: Số bước đi (1-100, mặc định 3)<br>**speed**: Tốc độ đi bộ (500-1500, giá trị nhỏ hơn có nghĩa là nhanh hơn, mặc định 1000)<br>**direction**: Hướng đi bộ (-1=lùi, 1=tiến, mặc định 1)<br>**arm_swing**: Biên độ vung tay (0-170 độ, mặc định 50) |
| self.otto.turn_left | Quay            | **steps**: Số lần quay (1-100, mặc định 3)<br>**speed**: Tốc độ quay (500-1500, giá trị nhỏ hơn có nghĩa là nhanh hơn, mặc định 1000)<br>**direction**: Hướng quay (1=quay trái, -1=quay phải, mặc định 1)<br>**arm_swing**: Biên độ vung tay (0-170 độ, mặc định 50) |
| self.otto.jump    | Nhảy            | **steps**: Số lần nhảy (1-100, mặc định 1)<br>**speed**: Tốc độ nhảy (500-1500, giá trị nhỏ hơn có nghĩa là nhanh hơn, mặc định 1000) |
| self.otto.swing   | Đung đưa trái phải        | **steps**: Số lần đung đưa (1-100, mặc định 3)<br>**speed**: Tốc độ đung đưa (500-1500, giá trị nhỏ hơn có nghĩa là nhanh hơn, mặc định 1000)<br>**amount**: Biên độ đung đưa (0-170 độ, mặc định 30) |
| self.otto.moonwalk | Đi bộ mặt trăng         | **steps**: Số bước đi bộ mặt trăng (1-100, mặc định 3)<br>**speed**: Tốc độ (500-1500, giá trị nhỏ hơn có nghĩa là nhanh hơn, mặc định 1000)<br>**direction**: Hướng (1=trái, -1=phải, mặc định 1)<br>**amount**: Biên độ (0-170 độ, mặc định 25) |
| self.otto.bend    | Uốn cong cơ thể        | **steps**: Số lần uốn cong (1-100, mặc định 1)<br>**speed**: Tốc độ uốn cong (500-1500, giá trị nhỏ hơn có nghĩa là nhanh hơn, mặc định 1000)<br>**direction**: Hướng uốn cong (1=trái, -1=phải, mặc định 1) |
| self.otto.shake_leg | Lắc chân          | **steps**: Số lần lắc chân (1-100, mặc định 1)<br>**speed**: Tốc độ lắc chân (500-1500, giá trị nhỏ hơn có nghĩa là nhanh hơn, mặc định 1000)<br>**direction**: Lựa chọn chân (1=chân trái, -1=chân phải, mặc định 1) |
| self.otto.updown  | Di chuyển lên xuống        | **steps**: Số lần di chuyển lên xuống (1-100, mặc định 3)<br>**speed**: Tốc độ di chuyển (500-1500, giá trị nhỏ hơn có nghĩa là nhanh hơn, mặc định 1000)<br>**amount**: Biên độ di chuyển (0-170 độ, mặc định 20) |
| self.otto.hands_up | Giơ tay lên *         | **speed**: Tốc độ giơ tay lên (500-1500, giá trị nhỏ hơn có nghĩa là nhanh hơn, mặc định 1000)<br>**direction**: Lựa chọn tay (1=tay trái, -1=tay phải, 0=cả hai tay, mặc định 1) |
| self.otto.hands_down | Hạ tay xuống *       | **speed**: Tốc độ hạ tay xuống (500-1500, giá trị nhỏ hơn có nghĩa là nhanh hơn, mặc định 1000)<br>**direction**: Lựa chọn tay (1=tay trái, -1=tay phải, 0=cả hai tay, mặc định 1) |
| self.otto.hand_wave | Vẫy tay *        | **speed**: Tốc độ vẫy tay (500-1500, giá trị nhỏ hơn có nghĩa là nhanh hơn, mặc định 1000)<br>**direction**: Lựa chọn tay (1=tay trái, -1=tay phải, 0=cả hai tay, mặc định 1) |

**Lưu ý**: Các hành động tay được đánh dấu * chỉ khả dụng khi servo tay được cấu hình.

### Công cụ hệ thống

| Tên công cụ MCP         | Mô tả             | Giá trị trả về                                              |
|-------------------|-----------------|---------------------------------------------------|
| self.otto.stop    | Dừng ngay lập tức        | Dừng hành động hiện tại và trở về vị trí ban đầu |
| self.otto.get_status | Lấy trạng thái robot | Trả về "moving" hoặc "idle" |
| self.battery.get_level | Lấy trạng thái pin  | Trả về định dạng JSON của phần trăm pin và trạng thái sạc |

### Mô tả thông số

1. **steps**: Số bước/lần thực hiện hành động, giá trị càng lớn thời gian hành động càng dài
2. **speed**: Tốc độ thực hiện hành động, phạm vi 500-1500, **giá trị càng nhỏ càng nhanh**
3. **direction**: Thông số hướng
   - Hành động di chuyển: 1=trái/tiến, -1=phải/lùi
   - Hành động tay: 1=tay trái, -1=tay phải, 0=cả hai tay
4. **amount/arm_swing**: Biên độ hành động, phạm vi 0-170 độ
   - 0 có nghĩa là không vung (áp dụng cho vung tay)
   - Giá trị càng lớn biên độ càng lớn

### Điều khiển hành động
- Sau khi mỗi hành động được thực hiện, robot sẽ tự động trở về vị trí ban đầu (home) để tạo điều kiện thuận lợi cho việc thực hiện hành động tiếp theo
- Tất cả các thông số đều có giá trị mặc định hợp lý và các thông số không cần tùy chỉnh có thể được bỏ qua
- Các hành động được thực hiện trong một tác vụ nền và sẽ không chặn chương trình chính
- Hỗ trợ hàng đợi hành động, cho phép nhiều hành động được thực hiện liên tục

### Ví dụ gọi công cụ MCP
```json
// Đi bộ về phía trước 3 bước
{"name": "self.otto.walk_forward", "arguments": {}}

// Đi bộ về phía trước 5 bước, nhanh hơn một chút
{"name": "self.otto.walk_forward", "arguments": {"steps": 5, "speed": 800}}

// Quay trái 2 bước, vung tay rộng  
{"name": "self.otto.turn_left", "arguments": {"steps": 2, "arm_swing": 100}}

// Nhảy đung đưa, biên độ trung bình
{"name": "self.otto.swing", "arguments": {"steps": 5, "amount": 50}}

// Vẫy tay trái để chào
{"name": "self.otto.hand_wave", "arguments": {"direction": 1}}

// Dừng ngay lập tức
{"name": "self.otto.stop", "arguments": {}}
```

### Ví dụ lệnh thoại
- "Đi bộ về phía trước" / "Đi bộ về phía trước 5 bước" / "Đi bộ nhanh về phía trước"
- "Quay trái" / "Quay phải" / "Quay người"  
- "Nhảy" / "Nhảy một lần"
- "Đung đưa" / "Nhảy múa"
- "Đi bộ mặt trăng" / "Đi bộ mặt trăng"
- "Vẫy tay" / "Giơ tay lên" / "Hạ tay xuống"
- "Dừng" / "Dừng lại"

**Lưu ý**: Xiaozhi điều khiển các hành động của robot tạo ra các tác vụ mới trong nền. Các lệnh thoại mới vẫn có thể được nhận trong quá trình thực hiện hành động. Otto có thể được dừng ngay lập tức bằng lệnh thoại "Dừng".

