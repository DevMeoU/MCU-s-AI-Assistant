# EchoEar

## Introduction

<div align="center">
    <a href="https://oshwhub.com/esp-college/echoear"><b> LiChuang Open Source Platform </b></a>
</div>

EchoEar is an intelligent AI development kit, equipped with an ESP32-S3-WROOM-1 module, a 1.85-inch QSPI circular touchscreen, and a dual microphone array, supporting offline voice wake-up and sound source localization algorithms. For hardware details, please refer to the [LiChuang Open Source Project](https://oshwhub.com/esp-college/echoear).

## Configuration and Compilation Commands

**Configure the compilation target to ESP32S3**

```bash
idf.py set-target esp32s3
```

**Open menuconfig and configure**

```bash
idf.py menuconfig
```

Configure the following options respectively:

### Basic Configuration
- `Xiaozhi Assistant` → `Board Type` → Select `EchoEar`

### UI Style Selection

EchoEar supports two different UI display styles, which can be selected by modifying the macro definition in the code:

#### Custom Emote Display System (Recommended)
```c
#define USE_LVGL_DEFAULT    0
```
- **Features**: Uses a custom `EmoteDisplay` emote display system
- **Functions**: Supports rich emote animations, eye animations, and status icon display
- **Applicable**: Smart assistant scenarios, providing a more vivid human-computer interaction experience
- **Classes**: `anim::EmoteDisplay` + `anim::EmoteEngine`

#### LVGL Default Display System
```c
#define USE_LVGL_DEFAULT    1
```
- **Features**: Uses the standard LVGL graphics library display system
- **Functions**: Traditional text and icon display interface
- **Applicable**: Application scenarios requiring standard GUI controls
- **Classes**: `SpiLcdDisplay`

#### How to Modify
1. Open the `main/boards/echoear/EchoEar.cc` file
2. Find the macro definition on line 29: `#define USE_LVGL_DEFAULT    0`
3. Change it to the desired value (0 or 1)
4. Recompile the project

> **Note**: EchoEar uses 16MB Flash and requires a dedicated partition table configuration to reasonably allocate storage space for applications, OTA updates, resource files, etc.

Press `S` to save, press `Q` to exit.

**Compile**

```bash
idf.py build
```

**Flash**

Connect EchoEar to the computer, **make sure the power is on**, and run:

```bash
idf.py flash
```

---

# EchoEar

## Giới thiệu

<div align="center">
    <a href="https://oshwhub.com/esp-college/echoear"><b> Nền tảng mã nguồn mở LiChuang </b></a>
</div>

EchoEar là một bộ công cụ phát triển AI thông minh, được trang bị mô-đun ESP32-S3-WROOM-1, màn hình cảm ứng tròn QSPI 1.85 inch và mảng micrô kép, hỗ trợ đánh thức bằng giọng nói ngoại tuyến và thuật toán định vị nguồn âm thanh. Để biết chi tiết phần cứng, vui lòng tham khảo [Dự án mã nguồn mở LiChuang](https://oshwhub.com/esp-college/echoear).

## Lệnh cấu hình và biên dịch

**Cấu hình mục tiêu biên dịch là ESP32S3**

```bash
idf.py set-target esp32s3
```

**Mở menuconfig và cấu hình**

```bash
idf.py menuconfig
```

Cấu hình các tùy chọn sau:

### Cấu hình cơ bản
- `Xiaozhi Assistant` → `Board Type` → Chọn `EchoEar`

### Lựa chọn kiểu giao diện người dùng (UI)

EchoEar hỗ trợ hai kiểu hiển thị UI khác nhau, có thể được chọn bằng cách sửa đổi định nghĩa macro trong mã:

#### Hệ thống hiển thị biểu tượng cảm xúc tùy chỉnh (Khuyên dùng)
```c
#define USE_LVGL_DEFAULT    0
```
- **Tính năng**: Sử dụng hệ thống hiển thị biểu tượng cảm xúc `EmoteDisplay` tùy chỉnh
- **Chức năng**: Hỗ trợ hoạt ảnh biểu tượng cảm xúc phong phú, hoạt ảnh mắt và hiển thị biểu tượng trạng thái
- **Áp dụng**: Các kịch bản trợ lý thông minh, cung cấp trải nghiệm tương tác người-máy sống động hơn
- **Lớp**: `anim::EmoteDisplay` + `anim::EmoteEngine`

#### Hệ thống hiển thị mặc định LVGL
```c
#define USE_LVGL_DEFAULT    1
```
- **Tính năng**: Sử dụng hệ thống hiển thị thư viện đồ họa LVGL tiêu chuẩn
- **Chức năng**: Giao diện hiển thị văn bản và biểu tượng truyền thống
- **Áp dụng**: Các kịch bản ứng dụng yêu cầu điều khiển GUI tiêu chuẩn
- **Lớp**: `SpiLcdDisplay`

#### Cách sửa đổi
1. Mở tệp `main/boards/echoear/EchoEar.cc`
2. Tìm định nghĩa macro trên dòng 29: `#define USE_LVGL_DEFAULT    0`
3. Thay đổi nó thành giá trị mong muốn (0 hoặc 1)
4. Biên dịch lại dự án

> **Lưu ý**: EchoEar sử dụng Flash 16MB và yêu cầu cấu hình bảng phân vùng chuyên dụng để phân bổ hợp lý không gian lưu trữ cho các ứng dụng, cập nhật OTA, tệp tài nguyên, v.v.

Nhấn `S` để lưu, nhấn `Q` để thoát.

**Biên dịch**

```bash
idf.py build
```

**Nạp chương trình**

Kết nối EchoEar với máy tính, **đảm bảo đã bật nguồn**, và chạy:

```bash
idf.py flash
```