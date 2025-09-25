This document, based on code implementation, outlines the WebSocket communication protocol, detailing how devices and servers interact via WebSocket.

This document is inferred solely from the provided code; actual deployment may require further confirmation or supplementation in conjunction with the server-side implementation.

---

## 1. Overall Flow Overview

1.  **Device Initialization**
    *   Device powers on and initializes `Application`:
        *   Initializes audio codecs, display, LEDs, etc.
        *   Connects to the network.
        *   Creates and initializes a WebSocket protocol instance (`WebsocketProtocol`) that implements the `Protocol` interface.
    *   Enters the main loop to await events (audio input, audio output, scheduled tasks, etc.).

2.  **Establish WebSocket Connection**
    *   When the device needs to start a voice session (e.g., user wake-up, manual button trigger), it calls `OpenAudioChannel()`:
        *   Obtains the WebSocket URL based on configuration.
        *   Sets several request headers (`Authorization`, `Protocol-Version`, `Device-Id`, `Client-Id`).
        *   Calls `Connect()` to establish a WebSocket connection with the server.

3.  **Device Sends "hello" Message**
    *   After a successful connection, the device sends a JSON message with an example structure as follows:
        ```json
        {
          "type": "hello",
          "version": 1,
          "features": {
            "mcp": true
          },
          "transport": "websocket",
          "audio_params": {
            "format": "opus",
            "sample_rate": 16000,
            "channels": 1,
            "frame_duration": 60
          }
        }
        ```
    *   The `features` field is optional, and its content is automatically generated based on the device's compilation configuration. For example, `"mcp": true` indicates support for the MCP protocol.
    *   The value of `frame_duration` corresponds to `OPUS_FRAME_DURATION_MS` (e.g., 60ms).

4.  **Server Replies "hello"**
    *   The device waits for the server to return a JSON message containing `"type": "hello"` and checks if `"transport": "websocket"` matches.
    *   The server can optionally send a `session_id` field, which the device automatically records upon receipt.
    *   Example:
        ```json
        {
          "type": "hello",
          "transport": "websocket",
          "session_id": "xxx",
          "audio_params": {
            "format": "opus",
            "sample_rate": 24000,
            "channels": 1,
            "frame_duration": 60
          }
        }
        ```
    *   If it matches, the server is considered ready, and the audio channel is marked as successfully opened.
    *   If a correct reply is not received within the timeout period (default 10 seconds), the connection is considered failed, and a network error callback is triggered.

5.  **Subsequent Message Interaction**
    *   The device and server can send two main types of data:
        1.  **Binary audio data** (Opus encoded)
        2.  **Text JSON messages** (used to transmit chat status, TTS/STT events, MCP protocol messages, etc.)

    *   In the code, receive callbacks are mainly divided into:
        *   `OnData(...)`:
            *   When `binary` is `true`, it is considered an audio frame; the device will decode it as Opus data.
            *   When `binary` is `false`, it is considered JSON text, which needs to be parsed by cJSON on the device side and processed according to business logic (e.g., chat, TTS, MCP protocol messages, etc.).

    *   When the server or network disconnects, the `OnDisconnected()` callback is triggered:
        *   The device will call `on_audio_channel_closed_()` and eventually return to an idle state.

6.  **Close WebSocket Connection**
    *   When the device needs to end a voice session, it calls `CloseAudioChannel()` to actively disconnect and return to an idle state.
    *   Alternatively, if the server actively disconnects, the same callback process will be triggered.

---

Tài liệu này, dựa trên việc triển khai mã, phác thảo giao thức giao tiếp WebSocket, trình bày chi tiết cách các thiết bị và máy chủ tương tác qua WebSocket.

Tài liệu này chỉ được suy ra từ mã được cung cấp; việc triển khai thực tế có thể yêu cầu xác nhận hoặc bổ sung thêm cùng với việc triển khai phía máy chủ.

---

## 1. Tổng quan luồng tổng thể

1.  **Khởi tạo thiết bị**
    *   Thiết bị bật nguồn và khởi tạo `Application`:
        *   Khởi tạo bộ mã hóa/giải mã âm thanh, màn hình, đèn LED, v.v.
        *   Kết nối mạng.
        *   Tạo và khởi tạo một phiên bản giao thức WebSocket (`WebsocketProtocol`) triển khai giao diện `Protocol`.
    *   Vào vòng lặp chính để chờ các sự kiện (đầu vào âm thanh, đầu ra âm thanh, các tác vụ đã lên lịch, v.v.).

2.  **Thiết lập kết nối WebSocket**
    *   Khi thiết bị cần bắt đầu phiên thoại (ví dụ: người dùng đánh thức, kích hoạt nút thủ công, v.v.), nó gọi `OpenAudioChannel()`:
        *   Lấy URL WebSocket dựa trên cấu hình.
        *   Đặt một số tiêu đề yêu cầu (`Authorization`, `Protocol-Version`, `Device-Id`, `Client-Id`).
        *   Gọi `Connect()` để thiết lập kết nối WebSocket với máy chủ.

3.  **Thiết bị gửi tin nhắn "hello"**
    *   Sau khi kết nối thành công, thiết bị gửi một tin nhắn JSON với cấu trúc ví dụ như sau:
        ```json
        {
          "type": "hello",
          "version": 1,
          "features": {
            "mcp": true
          },
          "transport": "websocket",
          "audio_params": {
            "format": "opus",
            "sample_rate": 16000,
            "channels": 1,
            "frame_duration": 60
          }
        }
        ```
    *   Trường `features` là tùy chọn và nội dung của nó được tự động tạo dựa trên cấu hình biên dịch của thiết bị. Ví dụ: `"mcp": true` cho biết hỗ trợ giao thức MCP.
    *   Giá trị của `frame_duration` tương ứng với `OPUS_FRAME_DURATION_MS` (ví dụ: 60ms).

4.  **Máy chủ trả lời "hello"**
    *   Thiết bị chờ máy chủ trả về một tin nhắn JSON chứa `"type": "hello"` và kiểm tra xem `"transport": "websocket"` có khớp không.
    *   Máy chủ có thể tùy chọn gửi trường `session_id`, thiết bị sẽ tự động ghi lại khi nhận được.
    *   Ví dụ:
        ```json
        {
          "type": "hello",
          "transport": "websocket",
          "session_id": "xxx",
          "audio_params": {
            "format": "opus",
            "sample_rate": 24000,
            "channels": 1,
            "frame_duration": 60
          }
        }
        ```
    *   If it matches, the server is considered ready, and the audio channel is marked as successfully opened.
    *   If a correct reply is not received within the timeout period (default 10 seconds), the connection is considered failed, and a network error callback is triggered.

5.  **Subsequent Message Interaction**
    *   The device and server can send two main types of data:
        1.  **Binary audio data** (Opus encoded)
        2.  **Text JSON messages** (used to transmit chat status, TTS/STT events, MCP protocol messages, etc.)

    *   In the code, receive callbacks are mainly divided into:
        *   `OnData(...)`:
            *   When `binary` is `true`, it is considered an audio frame; the device will decode it as Opus data.
            *   When `binary` is `false`, it is considered JSON text, which needs to be parsed by cJSON on the device side and processed according to business logic (e.g., chat, TTS, MCP protocol messages, etc.).

    *   When the server or network disconnects, the `OnDisconnected()` callback is triggered:
        *   The device will call `on_audio_channel_closed_()` and eventually return to an idle state.

6.  **Close WebSocket Connection**
    *   When the device needs to end a voice session, it calls `CloseAudioChannel()` to actively disconnect and return to an idle state.
    *   Alternatively, if the server actively disconnects, the same callback process will be triggered.
