# MqttProtocol - Chức năng các hàm

## Mô tả tổng quan

File `mqtt_protocol.h` và `mqtt_protocol.cc` triển khai giao thức MQTT cho ứng dụng. Lớp MqttProtocol kế thừa từ lớp Protocol và cung cấp các chức năng kết nối và giao tiếp với server thông qua giao thức MQTT.

## Danh sách hằng số

#### `MQTT_PING_INTERVAL_SECONDS`
- **Giá trị**: 90
- **Chức năng**: Khoảng thời gian gửi ping để duy trì kết nối (giây)

#### `MQTT_RECONNECT_INTERVAL_MS`
- **Giá trị**: 60000
- **Chức năng**: Khoảng thời gian thử kết nối lại khi mất kết nối (mili giây)

#### `MQTT_PROTOCOL_SERVER_HELLO_EVENT`
- **Giá trị**: (1 << 0)
- **Chức năng**: Sự kiện chào mừng từ server trong event group

## Danh sách hàm và chức năng chi tiết

### Hàm khởi tạo và giải phóng

#### `MqttProtocol::MqttProtocol()`
- **Chức năng**: Khởi tạo đối tượng MqttProtocol
- **Chi tiết**: 
  - Tạo event group
  - Khởi tạo các biến thành viên
  - Khởi tạo context AES cho mã hóa

#### `MqttProtocol::~MqttProtocol()`
- **Chức năng**: Hàm hủy đối tượng MqttProtocol
- **Chi tiết**: 
  - Giải phóng tài nguyên đã cấp phát
  - Dọn dẹp context AES

### Hàm public (kế thừa từ Protocol)

#### `bool MqttProtocol::Start()`
- **Chức năng**: Bắt đầu giao thức MQTT
- **Chi tiết**:
  - Khởi động client MQTT
  - Đăng ký các topic cần thiết
  - Gửi tin nhắn chào mừng
  - Thiết lập các callback xử lý tin nhắn

#### `bool MqttProtocol::SendAudio(std::unique_ptr<AudioStreamPacket> packet)`
- **Chức năng**: Gửi dữ liệu âm thanh qua MQTT
- **Chi tiết**:
  - Kiểm tra nếu kênh âm thanh đã mở
  - Mã hóa dữ liệu âm thanh nếu cần
  - Gửi dữ liệu qua MQTT hoặc UDP tùy theo phiên bản giao thức

#### `bool MqttProtocol::OpenAudioChannel()`
- **Chức năng**: Mở kênh âm thanh
- **Chi tiết**:
  - Gửi yêu cầu mở kênh âm thanh đến server
  - Chờ phản hồi từ server
  - Trả về true nếu mở kênh thành công

#### `void MqttProtocol::CloseAudioChannel()`
- **Chức năng**: Đóng kênh âm thanh
- **Chi tiết**:
  - Gửi yêu cầu đóng kênh âm thanh đến server
  - Dọn dẹp tài nguyên kênh âm thanh

#### `bool MqttProtocol::IsAudioChannelOpened() const`
- **Chức năng**: Kiểm tra kênh âm thanh có đang mở không
- **Chi tiết**: Trả về true nếu kênh âm thanh đang mở

### Hàm private

#### `bool MqttProtocol::StartMqttClient(bool report_error=false)`
- **Chức năng**: Khởi động client MQTT
- **Chi tiết**:
  - Tạo và cấu hình client MQTT
  - Kết nối đến broker MQTT
  - Đăng ký các topic cần thiết
  - Thiết lập callback xử lý tin nhắn

#### `void MqttProtocol::ParseServerHello(const cJSON* root)`
- **Chức năng**: Phân tích tin nhắn chào mừng từ server
- **Chi tiết**:
  - Trích xuất thông tin cấu hình từ tin nhắn chào mừng
  - Cập nhật tỷ lệ lấy mẫu và độ dài khung
  - Thiết lập khóa mã hóa AES nếu có
  - Cấu hình kết nối UDP nếu được hỗ trợ

#### `std::string MqttProtocol::DecodeHexString(const std::string& hex_string)`
- **Chức năng**: Giải mã chuỗi hex
- **Chi tiết**: Chuyển đổi chuỗi hex thành chuỗi byte

#### `bool MqttProtocol::SendText(const std::string& text)`
- **Chức năng**: Gửi văn bản qua MQTT
- **Chi tiết**:
  - Kiểm tra kết nối MQTT
  - Gửi tin nhắn văn bản đến topic đã đăng ký

#### `std::string MqttProtocol::GetHelloMessage()`
- **Chức năng**: Tạo tin nhắn chào mừng
- **Chi tiết**: 
  - Tạo tin nhắn JSON chứa thông tin thiết bị
  - Bao gồm ID phiên, phiên bản giao thức, thông tin hệ thống