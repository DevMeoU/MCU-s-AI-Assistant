# Protocol - Chức năng các hàm

## Mô tả tổng quan

File `protocol.h` định nghĩa giao diện cơ bản cho các giao thức mạng trong ứng dụng. Lớp Protocol là lớp trừu tượng cung cấp các phương thức chung cho cả MQTT và WebSocket, bao gồm gửi/nhận âm thanh, xử lý kết nối và quản lý kênh âm thanh.

## Danh sách cấu trúc và hàm

### Cấu trúc dữ liệu

#### `struct AudioStreamPacket`
- **Chức năng**: Đại diện cho một gói dữ liệu âm thanh
- **Chi tiết**:
  - `sample_rate`: Tỷ lệ lấy mẫu
  - `frame_duration`: Độ dài khung (ms)
  - `timestamp`: Dấu thời gian (ms)
  - `payload`: Dữ liệu âm thanh (dưới dạng vector byte)

#### `struct BinaryProtocol2`
- **Chức năng**: Cấu trúc giao thức nhị phân phiên bản 2
- **Chi tiết**:
  - `version`: Phiên bản giao thức
  - `type`: Loại tin nhắn (0: OPUS, 1: JSON)
  - `reserved`: Dành cho sử dụng trong tương lai
  - `timestamp`: Dấu thời gian (dùng cho AEC phía server)
  - `payload_size`: Kích thước payload
  - `payload`: Dữ liệu payload

#### `struct BinaryProtocol3`
- **Chức năng**: Cấu trúc giao thức nhị phân phiên bản 3
- **Chi tiết**:
  - `type`: Loại tin nhắn
  - `reserved`: Dành cho sử dụng trong tương lai
  - `payload_size`: Kích thước payload
  - `payload`: Dữ liệu payload

### Enumerations

#### `enum AbortReason`
- **Chức năng**: Lý do ngắt nói
- **Giá trị**:
  - `kAbortReasonNone`: Không có lý do
  - `kAbortReasonWakeWordDetected`: Phát hiện từ khóa đánh thức

#### `enum ListeningMode`
- **Chức năng**: Chế độ nghe
- **Giá trị**:
  - `kListeningModeAutoStop`: Tự động dừng
  - `kListeningModeManualStop`: Dừng bằng tay
  - `kListeningModeRealtime`: Thời gian thực (cần hỗ trợ AEC)

### Hàm public

#### `virtual ~Protocol() = default;`
- **Chức năng**: Hàm hủy ảo

#### `int server_sample_rate() const`
- **Chức năng**: Lấy tỷ lệ lấy mẫu của server
- **Chi tiết**: Trả về tỷ lệ lấy mẫu được cung cấp bởi server

#### `int server_frame_duration() const`
- **Chức năng**: Lấy độ dài khung của server
- **Chi tiết**: Trả về độ dài khung (ms) được cung cấp bởi server

#### `const std::string& session_id() const`
- **Chức năng**: Lấy ID phiên
- **Chi tiết**: Trả về ID phiên hiện tại

### Hàm callback registration

#### `void OnIncomingAudio(std::function<void(std::unique_ptr<AudioStreamPacket> packet)> callback)`
- **Chức năng**: Đăng ký callback khi nhận dữ liệu âm thanh
- **Chi tiết**: Thiết lập hàm sẽ được gọi khi có gói âm thanh đến

#### `void OnIncomingJson(std::function<void(const cJSON* root)> callback)`
- **Chức năng**: Đăng ký callback khi nhận JSON
- **Chi tiết**: Thiết lập hàm sẽ được gọi khi có tin nhắn JSON đến

#### `void OnAudioChannelOpened(std::function<void()> callback)`
- **Chức năng**: Đăng ký callback khi kênh âm thanh mở
- **Chi tiết**: Thiết lập hàm sẽ được gọi khi kênh âm thanh được mở

#### `void OnAudioChannelClosed(std::function<void()> callback)`
- **Chức năng**: Đăng ký callback khi kênh âm thanh đóng
- **Chi tiết**: Thiết lập hàm sẽ được gọi khi kênh âm thanh được đóng

#### `void OnNetworkError(std::function<void(const std::string& message)> callback)`
- **Chức năng**: Đăng ký callback khi có lỗi mạng
- **Chi tiết**: Thiết lập hàm sẽ được gọi khi xảy ra lỗi mạng

#### `void OnConnected(std::function<void()> callback)`
- **Chức năng**: Đăng ký callback khi kết nối
- **Chi tiết**: Thiết lập hàm sẽ được gọi khi kết nối thành công

#### `void OnDisconnected(std::function<void()> callback)`
- **Chức năng**: Đăng ký callback khi ngắt kết nối
- **Chi tiết**: Thiết lập hàm sẽ được gọi khi bị ngắt kết nối

### Hàm thuần ảo (cần triển khai trong lớp dẫn xuất)

#### `virtual bool Start() = 0`
- **Chức năng**: Bắt đầu giao thức
- **Chi tiết**: Khởi động kết nối giao thức

#### `virtual bool OpenAudioChannel() = 0`
- **Chức năng**: Mở kênh âm thanh
- **Chi tiết**: Mở kênh để gửi/nhận dữ liệu âm thanh

#### `virtual void CloseAudioChannel() = 0`
- **Chức năng**: Đóng kênh âm thanh
- **Chi tiết**: Đóng kênh âm thanh đang mở

#### `virtual bool IsAudioChannelOpened() const = 0`
- **Chức năng**: Kiểm tra kênh âm thanh có đang mở không
- **Chi tiết**: Trả về true nếu kênh âm thanh đang mở

#### `virtual bool SendAudio(std::unique_ptr<AudioStreamPacket> packet) = 0`
- **Chức năng**: Gửi dữ liệu âm thanh
- **Chi tiết**: Gửi gói âm thanh đến server

#### `virtual bool SendText(const std::string& text) = 0`
- **Chức năng**: Gửi văn bản
- **Chi tiết**: Gửi tin nhắn văn bản đến server (phải được triển khai trong lớp dẫn xuất)

### Hàm gửi tin nhắn

#### `void SendWakeWordDetected(const std::string& wake_word)`
- **Chức năng**: Gửi thông báo phát hiện từ khóa
- **Chi tiết**: Gửi tin nhắn JSON thông báo đã phát hiện từ khóa đánh thức

#### `void SendStartListening(ListeningMode mode)`
- **Chức năng**: Gửi lệnh bắt đầu nghe
- **Chi tiết**: Gửi tin nhắn yêu cầu bắt đầu chế độ nghe với chế độ chỉ định

#### `void SendStopListening()`
- **Chức năng**: Gửi lệnh dừng nghe
- **Chi tiết**: Gửi tin nhắn yêu cầu dừng chế độ nghe

#### `void SendAbortSpeaking(AbortReason reason)`
- **Chức năng**: Gửi lệnh ngắt nói
- **Chi tiết**: Gửi tin nhắn yêu cầu ngắt quá trình nói với lý do chỉ định

#### `void SendMcpMessage(const std::string& message)`
- **Chức năng**: Gửi tin nhắn MCP
- **Chi tiết**: Gửi tin nhắn MCP đến server

### Hàm protected

#### `void SetError(const std::string& message)`
- **Chức năng**: Thiết lập lỗi
- **Chi tiết**: Đánh dấu có lỗi xảy ra và gọi callback lỗi nếu đã được đăng ký

#### `bool IsTimeout() const`
- **Chức năng**: Kiểm tra timeout
- **Chi tiết**: Kiểm tra xem đã quá thời gian timeout (120 giây) kể từ lần nhận dữ liệu cuối cùng