# WebsocketProtocol - Chức năng các hàm

## Mô tả tổng quan

File `websocket_protocol.h` và `websocket_protocol.cc` triển khai giao thức WebSocket cho ứng dụng. Lớp WebsocketProtocol kế thừa từ lớp Protocol và cung cấp các chức năng kết nối và giao tiếp với server thông qua giao thức WebSocket.

## Danh sách hằng số

#### `WEBSOCKET_PROTOCOL_SERVER_HELLO_EVENT`
- **Giá trị**: (1 << 0)
- **Chức năng**: Sự kiện chào mừng từ server trong event group

## Danh sách hàm và chức năng chi tiết

### Hàm khởi tạo và giải phóng

#### `WebsocketProtocol::WebsocketProtocol()`
- **Chức năng**: Khởi tạo đối tượng WebsocketProtocol
- **Chi tiết**: 
  - Tạo event group
  - Khởi tạo các biến thành viên
  - Đặt phiên bản giao thức mặc định là 1

#### `WebsocketProtocol::~WebsocketProtocol()`
- **Chức năng**: Hàm hủy đối tượng WebsocketProtocol
- **Chi tiết**: 
  - Giải phóng tài nguyên đã cấp phát

### Hàm public (kế thừa từ Protocol)

#### `bool WebsocketProtocol::Start()`
- **Chức năng**: Bắt đầu giao thức WebSocket
- **Chi tiết**:
  - Tạo và cấu hình kết nối WebSocket
  - Kết nối đến server WebSocket
  - Gửi tin nhắn chào mừng
  - Thiết lập các callback xử lý tin nhắn
  - Đăng ký các sự kiện cần thiết

#### `bool WebsocketProtocol::SendAudio(std::unique_ptr<AudioStreamPacket> packet)`
- **Chức năng**: Gửi dữ liệu âm thanh qua WebSocket
- **Chi tiết**:
  - Kiểm tra nếu kênh âm thanh đã mở
  - Đóng gói dữ liệu âm thanh theo giao thức nhị phân
  - Gửi dữ liệu qua kết nối WebSocket

#### `bool WebsocketProtocol::OpenAudioChannel()`
- **Chức năng**: Mở kênh âm thanh
- **Chi tiết**:
  - Gửi yêu cầu mở kênh âm thanh đến server
  - Chờ phản hồi từ server
  - Trả về true nếu mở kênh thành công

#### `void WebsocketProtocol::CloseAudioChannel()`
- **Chức năng**: Đóng kênh âm thanh
- **Chi tiết**:
  - Gửi yêu cầu đóng kênh âm thanh đến server
  - Dọn dẹp tài nguyên kênh âm thanh

#### `bool WebsocketProtocol::IsAudioChannelOpened() const`
- **Chức năng**: Kiểm tra kênh âm thanh có đang mở không
- **Chi tiết**: Trả về true nếu kênh âm thanh đang mở

### Hàm private

#### `void WebsocketProtocol::ParseServerHello(const cJSON* root)`
- **Chức năng**: Phân tích tin nhắn chào mừng từ server
- **Chi tiết**:
  - Trích xuất thông tin cấu hình từ tin nhắn chào mừng
  - Cập nhật tỷ lệ lấy mẫu và độ dài khung
  - Xử lý các thông số cấu hình khác nếu có

#### `bool WebsocketProtocol::SendText(const std::string& text)`
- **Chức năng**: Gửi văn bản qua WebSocket
- **Chi tiết**:
  - Kiểm tra kết nối WebSocket
  - Gửi tin nhắn văn bản qua kết nối WebSocket

#### `std::string WebsocketProtocol::GetHelloMessage()`
- **Chức năng**: Tạo tin nhắn chào mừng
- **Chi tiết**: 
  - Tạo tin nhắn JSON chứa thông tin thiết bị
  - Bao gồm ID phiên, phiên bản giao thức, thông tin hệ thống