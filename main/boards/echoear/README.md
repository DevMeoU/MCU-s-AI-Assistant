# EchoEar

EchoEar 支持多种不同的 UI 显示风格，通过 menuconfig 配置选择：

- `Xiaozhi Assistant` → `Select display style` → 选择显示风格

#### 可选风格

##### 表情动画风格 (Emote animation style) - 推荐
- **配置选项**: `USE_EMOTE_MESSAGE_STYLE`
- **特点**: 使用自定义的 `EmoteDisplay` 表情显示系统
- **功能**: 支持丰富的表情动画、眼睛动画、状态图标显示
- **适用**: 智能助手场景，提供更生动的人机交互体验
- **类**: `emote::EmoteDisplay`

**⚠️ 重要**: 选择此风格需要额外配置自定义资源文件：
1. `Xiaozhi Assistant` → `Flash Assets` → 选择 `Flash Custom Assets`
2. `Xiaozhi Assistant` → `Custom Assets File` → 填入资源文件地址：
   ```
   https://dl.espressif.com/AE/wn9_nihaoxiaozhi_tts-font_puhui_common_20_4-echoear.bin
   ```

##### 默认消息风格 (Enable default message style)
- **配置选项**: `USE_DEFAULT_MESSAGE_STYLE` (默认)
- **特点**: 使用标准的消息显示界面
- **功能**: 传统的文本和图标显示界面
- **适用**: 标准的对话场景
- **类**: `SpiLcdDisplay`

##### 微信消息风格 (Enable WeChat Message Style)
- **配置选项**: `USE_WECHAT_MESSAGE_STYLE`
- **特点**: 仿微信聊天界面风格
- **功能**: 类似微信的消息气泡显示
- **适用**: 喜欢微信风格的用户
- **类**: `SpiLcdDisplay`
>>>>>>> upstream/main

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

EchoEar 支持多种不同的 UI 显示风格，通过 menuconfig 配置选择：

- `Xiaozhi Assistant` → `Select display style` → 选择显示风格

#### 可选风格

##### 表情动画风格 (Emote animation style) - 推荐
- **配置选项**: `USE_EMOTE_MESSAGE_STYLE`
- **特点**: 使用自定义的 `EmoteDisplay` 表情显示系统
- **功能**: 支持丰富的表情动画、眼睛动画、状态图标显示
- **适用**: 智能助手场景，提供更生动的人机交互体验
- **类**: `emote::EmoteDisplay`

**⚠️ 重要**: 选择此风格需要额外配置自定义资源文件：
1. `Xiaozhi Assistant` → `Flash Assets` → 选择 `Flash Custom Assets`
2. `Xiaozhi Assistant` → `Custom Assets File` → 填入资源文件地址：
   ```
   https://dl.espressif.com/AE/wn9_nihaoxiaozhi_tts-font_puhui_common_20_4-echoear.bin
   ```

##### 默认消息风格 (Enable default message style)
- **配置选项**: `USE_DEFAULT_MESSAGE_STYLE` (默认)
- **特点**: 使用标准的消息显示界面
- **功能**: 传统的文本和图标显示界面
- **适用**: 标准的对话场景
- **类**: `SpiLcdDisplay`

##### 微信消息风格 (Enable WeChat Message Style)
- **配置选项**: `USE_WECHAT_MESSAGE_STYLE`
- **特点**: 仿微信聊天界面风格
- **功能**: 类似微信的消息气泡显示
- **适用**: 喜欢微信风格的用户
- **类**: `SpiLcdDisplay`

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
=======
EchoEar 支持多种不同的 UI 显示风格，通过 menuconfig 配置选择：

- `Xiaozhi Assistant` → `Select display style` → 选择显示风格

#### 可选风格

##### 表情动画风格 (Emote animation style) - 推荐
- **配置选项**: `USE_EMOTE_MESSAGE_STYLE`
- **特点**: 使用自定义的 `EmoteDisplay` 表情显示系统
- **功能**: 支持丰富的表情动画、眼睛动画、状态图标显示
- **适用**: 智能助手场景，提供更生动的人机交互体验
- **类**: `emote::EmoteDisplay`

**⚠️ 重要**: 选择此风格需要额外配置自定义资源文件：
1. `Xiaozhi Assistant` → `Flash Assets` → 选择 `Flash Custom Assets`
2. `Xiaozhi Assistant` → `Custom Assets File` → 填入资源文件地址：
   ```
   https://dl.espressif.com/AE/wn9_nihaoxiaozhi_tts-font_puhui_common_20_4-echoear.bin
   ```

##### 默认消息风格 (Enable default message style)
- **配置选项**: `USE_DEFAULT_MESSAGE_STYLE` (默认)
- **特点**: 使用标准的消息显示界面
- **功能**: 传统的文本和图标显示界面
- **适用**: 标准的对话场景
- **类**: `SpiLcdDisplay`

##### 微信消息风格 (Enable WeChat Message Style)
- **配置选项**: `USE_WECHAT_MESSAGE_STYLE`
- **特点**: 仿微信聊天界面风格
- **功能**: 类似微信的消息气泡显示
- **适用**: 喜欢微信风格的用户
- **类**: `SpiLcdDisplay`
>>>>>>> upstream/main

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