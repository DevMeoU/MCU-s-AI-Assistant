<<<<<<< HEAD
# An MCP-based Chatbot
>>>>>>> upstream/main

（中文 | [English](README_en.md) | [日本語](README_ja.md) | [Tiếng Việt](README_vi.md)）

<<<<<<< HEAD
## Video
## 介绍
>>>>>>> upstream/main

👉 [Con người: Lắp camera cho AI vs AI: Phát hiện chủ nhân ba ngày không gội đầu ngay tại chỗ【bilibili】](https://www.bilibili.com/video/BV1bpjgzKEhd/)

👉 [Tự tay chế tạo bạn gái AI của bạn, hướng dẫn cho người mới bắt đầu【bilibili】](https://www.bilibili.com/video/BV1XnmFYLEJN/)

<<<<<<< HEAD
## Giới thiệu

Đây là một dự án ESP32 mã nguồn mở của Xiaoge, được phát hành theo giấy phép MIT, cho phép bất kỳ ai sử dụng miễn phí hoặc cho mục đích thương mại.

Chúng tôi hy vọng thông qua dự án này, có thể giúp mọi người hiểu về phát triển phần cứng AI, áp dụng các mô hình ngôn ngữ lớn đang phát triển nhanh chóng vào các thiết bị phần cứng thực tế.

Nếu bạn có bất kỳ ý tưởng hoặc đề xuất nào, vui lòng gửi Issues hoặc tham gia nhóm QQ: 1011329060

### Điều khiển mọi thứ dựa trên MCP

Robot trò chuyện AI Xiaozhi, với tư cách là một cổng tương tác giọng nói, sử dụng khả năng AI của các mô hình lớn như Qwen / DeepSeek để thực hiện điều khiển đa thiết bị thông qua giao thức MCP.

![Điều khiển mọi thứ bằng MCP](docs/mcp-based-graph.jpg)
小智 AI 聊天机器人作为一个语音交互入口，利用 Qwen / DeepSeek 等大模型的 AI 能力，通过 MCP 协议实现多端控制。

<img src="docs/mcp-based-graph.jpg" alt="通过MCP控制万物" width="320">

### 版本说明

当前 v2 版本与 v1 版本分区表不兼容，所以无法从 v1 版本通过 OTA 升级到 v2 版本。分区表说明参见 [partitions/v2/README.md](partitions/v2/README.md)。

使用 v1 版本的所有硬件，可以通过手动烧录固件来升级到 v2 版本。

v1 的稳定版本为 1.9.2，可以通过 `git checkout v1` 来切换到 v1 版本，该分支会持续维护到 2026 年 2 月。
>>>>>>> upstream/main

### Các chức năng đã được triển khai

- Wi-Fi / ML307 Cat.1 4G
<<<<<<< HEAD
- Đánh thức giọng nói ngoại tuyến [ESP-SR](https://github.com/espressif/esp-sr)
- Hỗ trợ hai giao thức truyền thông ([Websocket](docs/websocket.md) hoặc MQTT+UDP)
- Sử dụng mã hóa và giải mã âm thanh OPUS
- Tương tác giọng nói dựa trên kiến trúc ASR + LLM + TTS theo luồng
- Nhận dạng giọng nói, xác định danh tính người nói hiện tại [3D Speaker](https://github.com/modelscope/3D-Speaker)
- Màn hình OLED / LCD, hỗ trợ hiển thị biểu cảm
- Hiển thị pin và quản lý nguồn
- Hỗ trợ đa ngôn ngữ (tiếng Trung, tiếng Anh, tiếng Nhật, tiếng Việt)
- Hỗ trợ nền tảng chip ESP32-C3, ESP32-S3, ESP32-P4
- Điều khiển thiết bị thông qua MCP trên thiết bị (âm lượng, đèn, động cơ, GPIO, v.v.)
- Mở rộng khả năng của mô hình lớn thông qua MCP đám mây (điều khiển nhà thông minh, thao tác máy tính, tìm kiếm kiến thức, gửi và nhận email, v.v.)
- 离线语音唤醒 [ESP-SR](https://github.com/espressif/esp-sr)
- 支持两种通信协议（[Websocket](docs/websocket.md) 或 MQTT+UDP）
- 采用 OPUS 音频编解码
- 基于流式 ASR + LLM + TTS 架构的语音交互
- 声纹识别，识别当前说话人的身份 [3D Speaker](https://github.com/modelscope/3D-Speaker)
- OLED / LCD 显示屏，支持表情显示
- 电量显示与电源管理
- 支持多语言（中文、英文、日文）
- 支持 ESP32-C3、ESP32-S3、ESP32-P4 芯片平台
- 通过设备端 MCP 实现设备控制（音量、灯光、电机、GPIO 等）
- 通过云端 MCP 扩展大模型能力（智能家居控制、PC桌面操作、知识搜索、邮件收发等）
- 自定义唤醒词、字体、表情与聊天背景，支持网页端在线修改 ([自定义Assets生成器](https://github.com/78/xiaozhi-assets-generator))
>>>>>>> upstream/main
# An MCP-based Chatbot | Một chatbot dựa trên MCP

（中文 | [English](README_en.md) | [日本語](README_ja.md) | [Tiếng Việt](README_vi.md)）

## 介绍

👉 [Con người: Lắp camera cho AI vs AI: Phát hiện chủ nhân ba ngày không gội đầu ngay tại chỗ【bilibili】](https://www.bilibili.com/video/BV1bpjgzKEhd/)

👉 [Tự tay chế tạo bạn gái AI của bạn, hướng dẫn cho người mới bắt đầu【bilibili】](https://www.bilibili.com/video/BV1XnmFYLEJN/)

小智 AI 聊天机器人作为一个语音交互入口，利用 Qwen / DeepSeek 等大模型的 AI 能力，通过 MCP 协议实现多端控制。

<img src="docs/mcp-based-graph.jpg" alt="通过MCP控制万物" width="320">

### 版本说明

当前 v2 版本与 v1 版本分区表不兼容，所以无法从 v1 版本通过 OTA 升级到 v2 版本。分区表说明参见 [partitions/v2/README.md](partitions/v2/README.md)。

使用 v1 版本的所有硬件，可以通过手动烧录固件来升级到 v2 版本。

v1 的稳定版本为 1.9.2，可以通过 `git checkout v1` 来切换到 v1 版本，该分支会持续维护到 2026 年 2 月。

### Các chức năng đã được triển khai

- Wi-Fi / ML307 Cat.1 4G
- 离线语音唤醒 [ESP-SR](https://github.com/espressif/esp-sr)
- 支持两种通信协议（[Websocket](docs/websocket.md) 或 MQTT+UDP）
- 采用 OPUS 音频编解码
- 基于流式 ASR + LLM + TTS 架构的语音交互
- 声纹识别，识别当前说话人的身份 [3D Speaker](https://github.com/modelscope/3D-Speaker)
- OLED / LCD 显示屏，支持表情显示
- 电量显示与电源管理
- 支持多语言（中文、英文、日文、越南语）
- 支持 ESP32-C3、ESP32-S3、ESP32-P4 芯片平台
- 通过设备端 MCP 实现设备控制（音量、灯光、电机、GPIO 等）
- 通过云端 MCP 扩展大模型能力（智能家居控制、PC桌面操作、知识搜索、邮件收发等）
- 自定义唤醒词、字体、表情与聊天背景，支持网页端在线修改 ([自定义Assets生成器](https://github.com/78/xiaozhi-assets-generator))
=======
# An MCP-based Chatbot
>>>>>>> upstream/main

（中文 | [English](README_en.md) | [日本語](README_ja.md) | [Tiếng Việt](README_vi.md)）

<<<<<<< HEAD
## Video
=======
## 介绍
>>>>>>> upstream/main

👉 [Con người: Lắp camera cho AI vs AI: Phát hiện chủ nhân ba ngày không gội đầu ngay tại chỗ【bilibili】](https://www.bilibili.com/video/BV1bpjgzKEhd/)

👉 [Tự tay chế tạo bạn gái AI của bạn, hướng dẫn cho người mới bắt đầu【bilibili】](https://www.bilibili.com/video/BV1XnmFYLEJN/)

<<<<<<< HEAD
## Giới thiệu

Đây là một dự án ESP32 mã nguồn mở của Xiaoge, được phát hành theo giấy phép MIT, cho phép bất kỳ ai sử dụng miễn phí hoặc cho mục đích thương mại.

Chúng tôi hy vọng thông qua dự án này, có thể giúp mọi người hiểu về phát triển phần cứng AI, áp dụng các mô hình ngôn ngữ lớn đang phát triển nhanh chóng vào các thiết bị phần cứng thực tế.

Nếu bạn có bất kỳ ý tưởng hoặc đề xuất nào, vui lòng gửi Issues hoặc tham gia nhóm QQ: 1011329060

### Điều khiển mọi thứ dựa trên MCP

Robot trò chuyện AI Xiaozhi, với tư cách là một cổng tương tác giọng nói, sử dụng khả năng AI của các mô hình lớn như Qwen / DeepSeek để thực hiện điều khiển đa thiết bị thông qua giao thức MCP.

![Điều khiển mọi thứ bằng MCP](docs/mcp-based-graph.jpg)
=======
小智 AI 聊天机器人作为一个语音交互入口，利用 Qwen / DeepSeek 等大模型的 AI 能力，通过 MCP 协议实现多端控制。

<img src="docs/mcp-based-graph.jpg" alt="通过MCP控制万物" width="320">

### 版本说明

当前 v2 版本与 v1 版本分区表不兼容，所以无法从 v1 版本通过 OTA 升级到 v2 版本。分区表说明参见 [partitions/v2/README.md](partitions/v2/README.md)。

使用 v1 版本的所有硬件，可以通过手动烧录固件来升级到 v2 版本。

v1 的稳定版本为 1.9.2，可以通过 `git checkout v1` 来切换到 v1 版本，该分支会持续维护到 2026 年 2 月。
>>>>>>> upstream/main

### Các chức năng đã được triển khai

- Wi-Fi / ML307 Cat.1 4G
<<<<<<< HEAD
- Đánh thức giọng nói ngoại tuyến [ESP-SR](https://github.com/espressif/esp-sr)
- Hỗ trợ hai giao thức truyền thông ([Websocket](docs/websocket.md) hoặc MQTT+UDP)
- Sử dụng mã hóa và giải mã âm thanh OPUS
- Tương tác giọng nói dựa trên kiến trúc ASR + LLM + TTS theo luồng
- Nhận dạng giọng nói, xác định danh tính người nói hiện tại [3D Speaker](https://github.com/modelscope/3D-Speaker)
- Màn hình OLED / LCD, hỗ trợ hiển thị biểu cảm
- Hiển thị pin và quản lý nguồn
- Hỗ trợ đa ngôn ngữ (tiếng Trung, tiếng Anh, tiếng Nhật, tiếng Việt)
- Hỗ trợ nền tảng chip ESP32-C3, ESP32-S3, ESP32-P4
- Điều khiển thiết bị thông qua MCP trên thiết bị (âm lượng, đèn, động cơ, GPIO, v.v.)
- Mở rộng khả năng của mô hình lớn thông qua MCP đám mây (điều khiển nhà thông minh, thao tác máy tính, tìm kiếm kiến thức, gửi và nhận email, v.v.)
=======
- 离线语音唤醒 [ESP-SR](https://github.com/espressif/esp-sr)
- 支持两种通信协议（[Websocket](docs/websocket.md) 或 MQTT+UDP）
- 采用 OPUS 音频编解码
- 基于流式 ASR + LLM + TTS 架构的语音交互
- 声纹识别，识别当前说话人的身份 [3D Speaker](https://github.com/modelscope/3D-Speaker)
- OLED / LCD 显示屏，支持表情显示
- 电量显示与电源管理
- 支持多语言（中文、英文、日文）
- 支持 ESP32-C3、ESP32-S3、ESP32-P4 芯片平台
- 通过设备端 MCP 实现设备控制（音量、灯光、电机、GPIO 等）
- 通过云端 MCP 扩展大模型能力（智能家居控制、PC桌面操作、知识搜索、邮件收发等）
- 自定义唤醒词、字体、表情与聊天背景，支持网页端在线修改 ([自定义Assets生成器](https://github.com/78/xiaozhi-assets-generator))
>>>>>>> upstream/main

## Phần cứng

### Thực hành làm thủ công trên bảng mạch

Xem hướng dẫn tài liệu Feishu để biết chi tiết:

👉 [《Bách khoa toàn thư robot trò chuyện AI Xiaozhi》](https://ccnphfhqs21z.feishu.cn/wiki/F5krwD16viZoF0kKkvDcrZNYnhb?from=from_copylink)

Sơ đồ hiệu ứng bảng mạch như sau:

![Sơ đồ hiệu ứng bảng mạch](docs/v1/wiring2.jpg)

### Hỗ trợ hơn 70 phần cứng mã nguồn mở (chỉ hiển thị một phần)

- <a href="https://oshwhub.com/li-chuang-kai-fa-ban/li-chuang-shi-zhan-pai-esp32-s3-kai-fa-ban" target="_blank" title="Bảng phát triển ESP32-S3 thực chiến của Li Chuang">Bảng phát triển ESP32-S3 thực chiến của Li Chuang</a>
- <a href="https://github.com/espressif/esp-box" target="_blank" title="Espressif ESP32-S3-BOX3">Espressif ESP32-S3-BOX3</a>
- <a href="https://docs.m5stack.com/zh_CN/core/CoreS3" target="_blank" title="M5Stack CoreS3">M5Stack CoreS3</a>
- <a href="https://docs.m5stack.com/en/atom/Atomic%20Echo%20Base" target="_blank" title="AtomS3R + Echo Base">M5Stack AtomS3R + Echo Base</a>
- <a href="https://gf.bilibili.com/item/detail/1108782064" target="_blank" title="Nút thần kỳ 2.4">Nút thần kỳ 2.4</a>
- <a href="https://www.waveshare.net/shop/ESP32-S3-Touch-AMOLED-1.8.htm" target="_blank" title="Waveshare ESP32-S3-Touch-AMOLED-1.8">Waveshare ESP32-S3-Touch-AMOLED-1.8</a>
- <a href="https://github.com/Xinyuan-LilyGO/T-Circle-S3" target="_blank" title="LILYGO T-Circle-S3">LILYGO T-Circle-S3</a>
- <a href="https://oshwhub.com/tenclass01/xmini_c3" target="_blank" title="Xiaoge Mini C3">Xiaoge Mini C3</a>
- <a href="https://oshwhub.com/movecall/cuican-ai-pendant-lights-up-y" target="_blank" title="Movecall CuiCan ESP32S3">Mặt dây chuyền AI CuiCan</a>
- <a href="https://github.com/WMnologo/xingzhi-ai" target="_blank" title="Nologo-Xingzhi-1.54 của Wuming Technology">Nologo-Xingzhi-1.54TFT của Wuming Technology</a>
- <a href="https://www.seeedstudio.com/SenseCAP-Watcher-W1-A-p-5979.html" target="_blank" title="SenseCAP Watcher">SenseCAP Watcher</a>
- <a href="https://www.bilibili.com/video/BV1BHJtz6E2S/" target="_blank" title="Robot chó chi phí cực thấp ESP-HI">Robot chó chi phí cực thấp ESP-HI</a>

<div style="display: flex; justify-content: space-between;">
  <a href="docs/v1/lichuang-s3.jpg" target="_blank" title="Bảng phát triển ESP32-S3 thực chiến của Li Chuang">
    <img src="docs/v1/lichuang-s3.jpg" width="240" />
  </a>
  <a href="docs/v1/espbox3.jpg" target="_blank" title="Espressif ESP32-S3-BOX3">
    <img src="docs/v1/espbox3.jpg" width="240" />
  </a>
  <a href="docs/v1/m5cores3.jpg" target="_blank" title="M5Stack CoreS3">
    <img src="docs/v1/m5cores3.jpg" width="240" />
  </a>
  <a href="docs/v1/atoms3r.jpg" target="_blank" title="AtomS3R + Echo Base">
    <img src="docs/v1/atoms3r.jpg" width="240" />
  </a>
  <a href="docs/v1/magiclick.jpg" target="_blank" title="Nút thần kỳ 2.4">
    <img src="docs/v1/magiclick.jpg" width="240" />
  </a>
  <a href="docs/v1/waveshare.jpg" target="_blank" title="Waveshare ESP32-S3-Touch-AMOLED-1.8">
    <img src="docs/v1/waveshare.jpg" width="240" />
  </a>
  <a href="docs/v1/lilygo-t-circle-s3.jpg" target="_blank" title="LILYGO T-Circle-S3">
    <img src="docs/v1/lilygo-t-circle-s3.jpg" width="240" />
  </a>
  <a href="docs/v1/xmini-c3.jpg" target="_blank" title="Xiaoge Mini C3">
    <img src="docs/v1/xmini-c3.jpg" width="240" />
  </a>
  <a href="docs/v1/movecall-cuican-esp32s3.jpg" target="_blank" title="CuiCan">
    <img src="docs/v1/movecall-cuican-esp32s3.jpg" width="240" />
  </a>
  <a href="docs/v1/wmnologo_xingzhi_1.54.jpg" target="_blank" title="Nologo-Xingzhi-1.54 của Wuming Technology">
    <img src="docs/v1/wmnologo_xingzhi_1.54.jpg" width="240" />
  </a>
  <a href="docs/v1/sensecap_watcher.jpg" target="_blank" title="SenseCAP Watcher">
    <img src="docs/v1/sensecap_watcher.jpg" width="240" />
  </a>
  <a href="docs/v1/esp-hi.jpg" target="_blank" title="Robot chó chi phí cực thấp ESP-HI">
    <img src="docs/v1/esp-hi.jpg" width="240" />
  </a>
</div>

## Phần mềm

### Ghi firmware

Đối với người mới bắt đầu, không nên thiết lập môi trường phát triển lần đầu tiên, hãy sử dụng firmware được ghi sẵn mà không cần môi trường phát triển.

Firmware mặc định kết nối với máy chủ chính thức [xiaozhi.me](https://xiaozhi.me), người dùng cá nhân có thể đăng ký tài khoản để sử dụng miễn phí mô hình thời gian thực Qwen.

👉 [Hướng dẫn ghi firmware cho người mới bắt đầu](https://ccnphfhqs21z.feishu.cn/wiki/Zpz4wXBtdimBrLk25WdcXzxcnNS)

### Môi trường phát triển

- Cursor hoặc VSCode
- Cài đặt plugin ESP-IDF, chọn SDK phiên bản 5.4 trở lên
- Linux tốt hơn Windows, tốc độ biên dịch nhanh hơn và tránh được các vấn đề về driver
- Dự án này sử dụng phong cách mã hóa Google C++, vui lòng đảm bảo tuân thủ các quy tắc khi gửi mã

### Tài liệu dành cho nhà phát triển

<<<<<<< HEAD
- [Hướng dẫn bảng phát triển tùy chỉnh](main/boards/README.md) - Tìm hiểu cách tạo bảng phát triển tùy chỉnh cho Xiaozhi AI
- [Hướng dẫn sử dụng điều khiển IoT giao thức MCP](docs/mcp-usage.md) - Tìm hiểu cách điều khiển thiết bị IoT thông qua giao thức MCP
- [Quy trình tương tác giao thức MCP](docs/mcp-protocol.md) - Cách triển khai giao thức MCP trên thiết bị
- [Tài liệu giao thức truyền thông hỗn hợp MQTT + UDP](docs/mqtt-udp.md)
- [Tài liệu giao thức truyền thông WebSocket chi tiết](docs/websocket.md)
=======
- [自定义开发板指南](docs/custom-board.md) - 学习如何为小智 AI 创建自定义开发板
- [MCP 协议物联网控制用法说明](docs/mcp-usage.md) - 了解如何通过 MCP 协议控制物联网设备
- [MCP 协议交互流程](docs/mcp-protocol.md) - 设备端 MCP 协议的实现方式
- [MQTT + UDP 混合通信协议文档](docs/mqtt-udp.md)
- [一份详细的 WebSocket 通信协议文档](docs/websocket.md)
>>>>>>> upstream/main

## Cấu hình mô hình lớn

Nếu bạn đã có một thiết bị robot trò chuyện AI Xiaozhi và đã kết nối với máy chủ chính thức, bạn có thể đăng nhập vào bảng điều khiển [xiaozhi.me](https://xiaozhi.me) để cấu hình.

👉 [Video hướng dẫn thao tác backend (giao diện cũ)](https://www.bilibili.com/video/BV1jUCUY2EKM/)

## Các dự án mã nguồn mở liên quan

Để triển khai máy chủ trên máy tính cá nhân, bạn có thể tham khảo các dự án mã nguồn mở của bên thứ ba sau:

- [xinnan-tech/xiaozhi-esp32-server](https://github.com/xinnan-tech/xiaozhi-esp32-server) Máy chủ Python
- [joey-zhou/xiaozhi-esp32-server-java](https://github.com/joey-zhou/xiaozhi-esp32-server-java) Máy chủ Java
- [AnimeAIChat/xiaozhi-server-go](https://github.com/AnimeAIChat/xiaozhi-server-go) Máy chủ Golang

Các dự án client của bên thứ ba sử dụng giao thức truyền thông Xiaozhi:

- [huangjunsen0406/py-xiaozhi](https://github.com/huangjunsen0406/py-xiaozhi) Client Python
- [TOM88812/xiaozhi-android-client](https://github.com/TOM88812/xiaozhi-android-client) Client Android
- [100askTeam/xiaozhi-linux](http://github.com/100askTeam/xiaozhi-linux) Client Linux do 100ask Team cung cấp
- [78/xiaozhi-sf32](https://github.com/78/xiaozhi-sf32) Firmware chip Bluetooth của Si Che Technology
- [QuecPython/solution-xiaozhiAI](https://github.com/QuecPython/solution-xiaozhiAI) Firmware QuecPython do Quectel cung cấp

<<<<<<< HEAD
## Lịch sử Star
=======
## 关于项目

这是一个由虾哥开源的 ESP32 项目，以 MIT 许可证发布，允许任何人免费使用，修改或用于商业用途。

我们希望通过这个项目，能够帮助大家了解 AI 硬件开发，将当下飞速发展的大语言模型应用到实际的硬件设备中。

如果你有任何想法或建议，请随时提出 Issues 或加入 QQ 群：1011329060

## Star History
>>>>>>> upstream/main

<a href="https://star-history.com/#78/xiaozhi-esp32&Date">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/svg?repos=78/xiaozhi-esp32&type=Date&theme=dark" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/svg?repos=78/xiaozhi-esp32&type=Date" />
   <img alt="Biểu đồ lịch sử Star" src="https://api.star-history.com/svg?repos=78/xiaozhi-esp32&type=Date" />
 </picture>
</a>
