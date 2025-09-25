# Xiaozhi Yunliao S3

## Introduction
Xiaozhi Yunliao S3 is a modified project of Xiaozhi AI, and it is the first mass-produced product with a 2.8-inch eye-protection large screen + large font + 2000mah large battery, with a lot of innovation and optimization.

## Merged Version
The merged version code is maintained in the main project of Xiaozhi AI, and it is updated with the main project version, which is convenient for users to expand by themselves and for third-party firmware expansion. It supports voice wake-up, voice interruption, OTA, 4G free switching and other functions.

>### Button Operations
>- **Power On**: In shutdown state, long press for 1 second and release the button to automatically power on.
>- **Power Off**: In power-on state, long press for 1 second and release the button, the title bar will display 'Please wait', and then wait for 2 seconds to automatically power off.
>- **Wake up/Interrupt**: In a normal call environment, single click the button.
>- **Switch 4G/Wifi**: During startup or network configuration interface, double click the button within 1 second (4G module needs to be installed).
>- **Reconnect to network**: In power-on state, triple click the button within 1 second, it will automatically restart and enter the network configuration interface.

## Modified Version
Due to significant underlying changes, the modified version code is maintained separately and regularly merged with the main project code.

>### Why is it modified?
>- First to implement WeChat QR code network configuration.
>- First to support single mobile phone network configuration.
>- First to support scanning QR code to access the console.
>- First to support Traditional Chinese, Japanese, and English interfaces.
>- First full voice control mode.
>- Exclusively provides one-click flashing scripts and other flashing methods.

## Version Differences
>| Feature | Merged Version | Modified Version |
>| --- | --- | --- |
>| Voice Interruption | ✓ | ✓ |
>| 4G Function | ✓ | ✓ |
>| Automatic Firmware Update | ✓ | X |
>| Third-party Firmware Support | ✓ | X |
>| Weather Standby Interface | X | ✓ |
>| Alarm Reminder | X | ✓ |
>| Network Music Playback | X | ✓ |
>| WeChat Scan Code Network Configuration | X | ✓ |
>| Single Mobile Phone Network Configuration | X | ✓ |
>| Scan Code Access Console | X | ✓ |
>| Traditional Chinese, Japanese, English Interface | X | ✓ |
>| Multi-language Support | X | ✓ |
>| External Bluetooth Speaker | X | ✓ |


# Compilation Configuration Commands

**Clone Project**

```bash
git clone https://github.com/78/xiaozhi-esp32.git
```

**Enter Project**

```bash
cd xiaozhi-esp32
```

**Configure target to ESP32S3**

```bash
idf.py set-target esp32s3
```

**Open menuconfig**

```bash
idf.py menuconfig
```

**Select Board**

```bash
- `Xiaozhi Assistant` → `Board Type` → Select `Xiaozhi Yunliao-S3` → Select `Enable Device-Side AEC`
```

**Compile**

```ba
idf.py build
```

**Download and open serial terminal**

```bash
idf.py build flash monitor
```

# Xiaozhi Yunliao S3

## Giới thiệu
Xiaozhi Yunliao S3 là một dự án được sửa đổi của Xiaozhi AI, và đây là sản phẩm sản xuất hàng loạt đầu tiên với màn hình lớn 2.8 inch bảo vệ mắt + phông chữ lớn + pin lớn 2000mah, với nhiều đổi mới và tối ưu hóa.

## Phiên bản hợp nhất
Mã phiên bản hợp nhất được duy trì trong dự án chính của Xiaozhi AI, và nó được cập nhật cùng với phiên bản dự án chính, thuận tiện cho người dùng tự mở rộng và mở rộng firmware của bên thứ ba. Nó hỗ trợ đánh thức bằng giọng nói, ngắt giọng nói, OTA, chuyển đổi 4G miễn phí và các chức năng khác.

>### Thao tác nút
>- **Bật nguồn**: Ở trạng thái tắt máy, nhấn giữ 1 giây rồi thả nút để tự động bật nguồn.
>- **Tắt nguồn**: Ở trạng thái bật nguồn, nhấn giữ 1 giây rồi thả nút, thanh tiêu đề sẽ hiển thị 'Vui lòng đợi', sau đó đợi 2 giây để tự động tắt nguồn.
>- **Đánh thức/Ngắt**: Trong môi trường gọi điện bình thường, nhấp một lần vào nút.
>- **Chuyển đổi 4G/Wifi**: Trong quá trình khởi động hoặc giao diện cấu hình mạng, nhấp đúp vào nút trong vòng 1 giây (cần cài đặt mô-đun 4G).
>- **Kết nối lại mạng**: Ở trạng thái bật nguồn, nhấp ba lần vào nút trong vòng 1 giây, nó sẽ tự động khởi động lại và vào giao diện cấu hình mạng.

## Phiên bản sửa đổi
Do những thay đổi cơ bản đáng kể, mã phiên bản sửa đổi được duy trì riêng và thường xuyên được hợp nhất với mã dự án chính.

>### Tại sao lại sửa đổi?
>- Đầu tiên triển khai cấu hình mạng mã QR WeChat.
>- Đầu tiên hỗ trợ cấu hình mạng điện thoại di động đơn.
>- Đầu tiên hỗ trợ quét mã QR để truy cập bảng điều khiển.
>- Đầu tiên hỗ trợ giao diện tiếng Trung phồn thể, tiếng Nhật và tiếng Anh.
>- Chế độ điều khiển giọng nói đầy đủ đầu tiên.
>- Cung cấp độc quyền các tập lệnh flash một cú nhấp chuột và các phương pháp flash khác.

## Sự khác biệt giữa các phiên bản
>| Tính năng | Phiên bản hợp nhất | Phiên bản sửa đổi |
>| --- | --- | --- |
>| Ngắt giọng nói | ✓ | ✓ |
>| Chức năng 4G | ✓ | ✓ |
>| Cập nhật firmware tự động | ✓ | X |
>| Hỗ trợ firmware của bên thứ ba | ✓ | X |
>| Giao diện chờ thời tiết | X | ✓ |
>| Nhắc nhở báo thức | X | ✓ |
>| Phát nhạc trực tuyến | X | ✓ |
>| Cấu hình mạng quét mã WeChat | X | ✓ |
>| Cấu hình mạng điện thoại di động đơn | X | ✓ |
>| Truy cập bảng điều khiển quét mã | X | ✓ |
>| Giao diện tiếng Trung phồn thể, tiếng Nhật, tiếng Anh | X | ✓ |
>| Hỗ trợ đa ngôn ngữ | X | ✓ |
>| Loa Bluetooth ngoài | X | ✓ |


# Lệnh cấu hình biên dịch

**Sao chép dự án**

```bash
git clone https://github.com/78/xiaozhi-esp32.git
```

**Vào dự án**

```bash
cd xiaozhi-esp32
```

**Cấu hình mục tiêu thành ESP32S3**

```bash
idf.py set-target esp32s3
```

**Mở menuconfig**

```bash
idf.py menuconfig
```

**Chọn bảng**

```bash
- `Xiaozhi Assistant` → `Board Type` → Chọn `Xiaozhi Yunliao-S3` → Chọn `Enable Device-Side AEC`
```

**Biên dịch**

```ba
idf.py build
```

**Tải xuống và mở thiết bị đầu cuối nối tiếp**

```bash
idf.py build flash monitor
```

