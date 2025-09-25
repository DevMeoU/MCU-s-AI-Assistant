# ESP-Hi

## Introduction

<div align="center">
    <a href="https://oshwhub.com/esp-college/esp-hi"><b> LCSC Open Source Platform </b></a>
    |
    <a href="https://www.bilibili.com/video/BV1BHJtz6E2S"><b> Bilibili </b></a>
</div>

ESP-Hi is an ultra-**low-cost** AI conversational robot based on ESP32C3, open-sourced by ESP Friends. ESP-Hi integrates a 0.96-inch color screen for displaying expressions, and the **robot dog has implemented dozens of actions**. By fully utilizing the peripherals of ESP32-C3, only minimal board-level hardware is required to achieve sound pickup and vocalization. The software has been synchronously optimized to reduce memory and Flash usage, achieving **wake word detection** and various peripheral drives under resource-constrained conditions. For hardware details, please refer to the [LCSC Open Source Project](https://oshwhub.com/esp-college/esp-hi).

## WebUI

ESP-Hi x XiaoZhi has a built-in WebUI for controlling body movements. Please connect your mobile phone and ESP-Hi to the same Wi-Fi network, and access `http://esp-hi.local/` on your phone to use it.

To disable it, uncheck `ESP_HI_WEB_CONTROL_ENABLED`, i.e., uncheck `Component config` → `Servo Dog Configuration` → `Web Control` → `Enable ESP-HI Web Control`.

## Configuration and Compilation Commands

Since ESP-Hi requires configuring many sdkconfig options, it is recommended to use a compilation script.

**Compile**

```bash
python ./scripts/release.py esp-hi
```

If you need to compile manually, please refer to `esp-hi/config.json` to modify the corresponding options in menuconfig.

**Flash**

```bash
idf.py flash
```


> [!TIP]
>
> **Servo control occupies the USB Type-C interface of ESP-Hi**, making it impossible to connect to a computer (cannot flash/view running logs). If this happens, please follow the tips below:
>
> **Flashing**
>
> 1. Disconnect the power of ESP-Hi, leaving only the head, do not connect the body.
> 2. Press and hold the button on ESP-Hi and connect it to the computer.
> 
> At this point, ESP-Hi (ESP32C3) should be in flashing mode, and you can use a computer to flash the program. After flashing, you may need to re-plug the power.
>
> **Viewing logs**
>
> Please set `CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG=y`, i.e., `Component config` → `ESP System Settings` → `Channel for console output` select `USB Serial/JTAG Controller`. This will also disable the servo control function.

---

# ESP-Hi

## Giới thiệu

<div align="center">
    <a href="https://oshwhub.com/esp-college/esp-hi"><b> Nền tảng mã nguồn mở LCSC </b></a>
    |
    <a href="https://www.bilibili.com/video/BV1BHJtz6E2S"><b> Bilibili </b></a>
</div>

ESP-Hi là một robot đàm thoại AI siêu **chi phí thấp** dựa trên ESP32C3, được ESP Friends phát hành mã nguồn mở. ESP-Hi tích hợp màn hình màu 0.96 inch để hiển thị biểu cảm, và **chú chó robot đã thực hiện hàng chục hành động**. Bằng cách khai thác tối đa các thiết bị ngoại vi của ESP32-C3, chỉ cần phần cứng cấp bo mạch tối thiểu để thu âm và phát âm. Phần mềm đã được tối ưu hóa đồng bộ để giảm mức sử dụng bộ nhớ và Flash, đồng thời đạt được **phát hiện từ khóa đánh thức** và nhiều trình điều khiển thiết bị ngoại vi trong điều kiện tài nguyên hạn chế. Để biết chi tiết phần cứng, vui lòng tham khảo [Dự án mã nguồn mở LCSC](https://oshwhub.com/esp-college/esp-hi).

## WebUI

ESP-Hi x XiaoZhi có một WebUI tích hợp để điều khiển các chuyển động cơ thể. Vui lòng kết nối điện thoại di động của bạn và ESP-Hi vào cùng một mạng Wi-Fi, và truy cập `http://esp-hi.local/` trên điện thoại của bạn để sử dụng.

Để tắt nó, bỏ chọn `ESP_HI_WEB_CONTROL_ENABLED`, tức là bỏ chọn `Component config` → `Servo Dog Configuration` → `Web Control` → `Enable ESP-HI Web Control`.

## Cấu hình và lệnh biên dịch

Vì ESP-Hi yêu cầu cấu hình nhiều tùy chọn sdkconfig, nên khuyến nghị sử dụng tập lệnh biên dịch.

**Biên dịch**

```bash
python ./scripts/release.py esp-hi
```

Nếu bạn cần biên dịch thủ công, vui lòng tham khảo `esp-hi/config.json` để sửa đổi các tùy chọn tương ứng trong menuconfig.

**Nạp chương trình**

```bash
idf.py flash
```


> [!TIP]
>
> **Điều khiển servo chiếm giao diện USB Type-C của ESP-Hi**, khiến không thể kết nối với máy tính (không thể nạp chương trình/xem nhật ký chạy). Nếu điều này xảy ra, vui lòng làm theo các mẹo dưới đây:
>
> **Nạp chương trình**
>
> 1. Ngắt nguồn của ESP-Hi, chỉ để lại phần đầu, không kết nối thân.
> 2. Nhấn và giữ nút trên ESP-Hi và kết nối nó với máy tính.
> 
> Tại thời điểm này, ESP-Hi (ESP32C3) sẽ ở chế độ nạp chương trình, và bạn có thể sử dụng máy tính để nạp chương trình. Sau khi nạp chương trình, bạn có thể cần cắm lại nguồn.
>
>
> **Xem nhật ký**
>
> Vui lòng đặt `CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG=y`, tức là `Component config` → `ESP System Settings` → `Channel for console output` chọn `USB Serial/JTAG Controller`. Điều này cũng sẽ tắt chức năng điều khiển servo.
