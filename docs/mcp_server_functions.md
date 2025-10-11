# McpServer - Chức năng các hàm

## Mô tả tổng quan

File `mcp_server.h` và `mcp_server.cc` triển khai giao thức Model Context Protocol (MCP) cho phép AI gọi các công cụ trên thiết bị. Lớp McpServer quản lý danh sách công cụ, xử lý các yêu cầu từ server và thực thi các công cụ được gọi.

## Danh sách cấu trúc và lớp

### `class ImageContent`
- **Chức năng**: Đại diện cho nội dung hình ảnh
- **Chi tiết**:
  - Lưu trữ dữ liệu hình ảnh đã mã hóa base64
  - Cung cấp phương thức chuyển đổi sang JSON

#### `ImageContent(const std::string& mime_type, const std::string& data)`
- **Chức năng**: Khởi tạo đối tượng ImageContent
- **Chi tiết**: 
  - Nhận loại MIME và dữ liệu hình ảnh
  - Mã hóa dữ liệu base64 tự động

#### `std::string to_json() const`
- **Chức năng**: Chuyển đổi sang chuỗi JSON
- **Chi tiết**: Trả về biểu diễn JSON của nội dung hình ảnh

#### `static std::string Base64Encode(const std::string& data)`
- **Chức năng**: Mã hóa base64
- **Chi tiết**: Sử dụng mbedtls để mã hóa dữ liệu

### `class Property`
- **Chức năng**: Đại diện cho một thuộc tính của công cụ
- **Chi tiết**: 
  - Lưu trữ tên, loại và giá trị của thuộc tính
  - Hỗ trợ kiểm tra phạm vi cho số nguyên

#### `Property(const std::string& name, PropertyType type)`
- **Chức năng**: Khởi tạo thuộc tính bắt buộc
- **Chi tiết**: Tạo thuộc tính không có giá trị mặc định

#### `template<typename T> Property(const std::string& name, PropertyType type, const T& default_value)`
- **Chức năng**: Khởi tạo thuộc tính tùy chọn với giá trị mặc định
- **Chi tiết**: Tạo thuộc tính có giá trị mặc định

#### `Property(const std::string& name, PropertyType type, int min_value, int max_value)`
- **Chức năng**: Khởi tạo thuộc tính số nguyên với phạm vi
- **Chi tiết**: Tạo thuộc tính số nguyên có giới hạn tối thiểu và tối đa

#### `template<typename T> inline T value() const`
- **Chức năng**: Lấy giá trị thuộc tính
- **Chi tiết**: Trả về giá trị với kiểu được chỉ định

#### `template<typename T> inline void set_value(const T& value)`
- **Chức năng**: Thiết lập giá trị thuộc tính
- **Chi tiết**: 
  - Kiểm tra phạm vi cho số nguyên
  - Lưu trữ giá trị

#### `std::string to_json() const`
- **Chức năng**: Chuyển đổi sang JSON
- **Chi tiết**: Trả về biểu diễn JSON của thuộc tính

### `class PropertyList`
- **Chức năng**: Danh sách các thuộc tính
- **Chi tiết**: Quản lý tập hợp các thuộc tính của công cụ

#### `PropertyList()`
#### `PropertyList(const std::vector<Property>& properties)`
- **Chức năng**: Khởi tạo danh sách thuộc tính

#### `void AddProperty(const Property& property)`
- **Chức năng**: Thêm thuộc tính vào danh sách

#### `const Property& operator[](const std::string& name) const`
- **Chức năng**: Truy cập thuộc tính theo tên
- **Chi tiết**: Ném ngoại lệ nếu không tìm thấy

#### `std::vector<std::string> GetRequired() const`
- **Chức năng**: Lấy danh sách tên thuộc tính bắt buộc
- **Chi tiết**: Trả về các thuộc tính không có giá trị mặc định

#### `std::string to_json() const`
- **Chức năng**: Chuyển đổi sang JSON
- **Chi tiết**: Trả về biểu diễn JSON của tất cả thuộc tính

### `class McpTool`
- **Chức năng**: Đại diện cho một công cụ MCP
- **Chi tiết**: 
  - Lưu trữ thông tin công cụ
  - Cung cấp phương thức thực thi công cụ

#### `McpTool(const std::string& name, const std::string& description, const PropertyList& properties, std::function<ReturnValue(const PropertyList&)> callback)`
- **Chức năng**: Khởi tạo công cụ
- **Chi tiết**: 
  - Nhận tên, mô tả, danh sách thuộc tính và hàm callback
  - Lưu trữ thông tin công cụ

#### `void set_user_only(bool user_only)`
- **Chức năng**: Thiết lập công cụ chỉ dành cho người dùng
- **Chi tiết**: Khi true, công cụ sẽ không hiển thị với AI

#### `std::string to_json() const`
- **Chức năng**: Chuyển đổi sang JSON
- **Chi tiết**: Trả về biểu diễn JSON của công cụ theo đặc tả MCP

#### `std::string Call(const PropertyList& properties)`
- **Chức năng**: Gọi công cụ
- **Chi tiết**: 
  - Thực thi hàm callback
  - Xử lý giá trị trả về và chuyển đổi sang JSON

### `class McpServer`
- **Chức năng**: Máy chủ MCP chính
- **Chi tiết**: 
  - Quản lý danh sách công cụ
  - Xử lý các tin nhắn MCP từ server
  - Thực thi các công cụ được gọi

## Danh sách enum

### `enum PropertyType`
- **Giá trị**:
  - `kPropertyTypeBoolean`: Kiểu boolean
  - `kPropertyTypeInteger`: Kiểu số nguyên
  - `kPropertyTypeString`: Kiểu chuỗi

### `enum class ReturnValue`
- **Chức năng**: Kiểu giá trị trả về của công cụ
- **Giá trị**: 
  - `bool`: Giá trị boolean
  - `int`: Giá trị số nguyên
  - `std::string`: Giá trị chuỗi
  - `cJSON*`: Đối tượng JSON
  - `ImageContent*`: Nội dung hình ảnh

## Danh sách hàm và chức năng chi tiết

### Hàm khởi tạo và giải phóng

#### `McpServer::McpServer()`
- **Chức năng**: Khởi tạo máy chủ MCP

#### `McpServer::~McpServer()`
- **Chức năng**: Hàm hủy máy chủ MCP
- **Chi tiết**: Giải phóng tất cả các công cụ

### Hàm quản lý công cụ

#### `void McpServer::AddCommonTools()`
- **Chức năng**: Thêm các công cụ chung
- **Chi tiết**:
  - Thêm các công cụ hệ thống như:
    - `self.get_device_status`: Lấy trạng thái thiết bị
    - `self.audio_speaker.set_volume`: Thiết lập âm lượng
    - `self.screen.set_brightness`: Thiết lập độ sáng màn hình
    - `self.screen.set_theme`: Thiết lập chủ đề màn hình
    - `self.camera.take_photo`: Chụp ảnh
    - `self.music.play_song`: Phát nhạc
    - `self.music.set_display_mode`: Thiết lập chế độ hiển thị nhạc

#### `void McpServer::AddTool(McpTool* tool)`
- **Chức năng**: Thêm công cụ
- **Chi tiết**: 
  - Kiểm tra trùng lặp
  - Thêm công cụ vào danh sách

#### `void McpServer::AddTool(const std::string& name, const std::string& description, const PropertyList& properties, std::function<ReturnValue(const PropertyList&)> callback)`
- **Chức năng**: Thêm công cụ với thông số
- **Chi tiết**: Tạo và thêm công cụ mới

#### `void McpServer::AddUserOnlyTool(const std::string& name, const std::string& description, const PropertyList& properties, std::function<ReturnValue(const PropertyList&)> callback)`
- **Chức năng**: Thêm công cụ chỉ dành cho người dùng
- **Chi tiết**: Tạo công cụ với cờ user_only = true

### Hàm xử lý tin nhắn

#### `void McpServer::ParseMessage(const cJSON* json)`
#### `void McpServer::ParseMessage(const std::string& message)`
- **Chức năng**: Phân tích tin nhắn MCP
- **Chi tiết**:
  - Kiểm tra phiên bản JSONRPC
  - Xử lý các phương thức:
    - `initialize`: Khởi tạo kết nối
    - `tools/list`: Liệt kê công cụ
    - `tools/call`: Gọi công cụ

#### `void McpServer::ParseCapabilities(const cJSON* capabilities)`
- **Chức năng**: Phân tích khả năng của server
- **Chi tiết**: Xử lý các khả năng như vision (thị giác)

### Hàm xử lý phương thức MCP

#### `void McpServer::GetToolsList(int id, const std::string& cursor)`
- **Chức năng**: Xử lý yêu cầu liệt kê công cụ
- **Chi tiết**:
  - Trả về danh sách công cụ theo định dạng MCP
  - Hỗ trợ phân trang với cursor

#### `void McpServer::DoToolCall(int id, const std::string& tool_name, const cJSON* tool_arguments)`
- **Chức năng**: Xử lý gọi công cụ
- **Chi tiết**:
  - Tìm công cụ theo tên
  - Kiểm tra và thiết lập tham số
  - Gọi công cụ trong luồng riêng biệt
  - Trả kết quả hoặc lỗi

### Hàm phản hồi

#### `void McpServer::ReplyResult(int id, const std::string& result)`
- **Chức năng**: Gửi phản hồi kết quả
- **Chi tiết**: Gửi tin nhắn JSONRPC kết quả

#### `void McpServer::ReplyError(int id, const std::string& message)`
- **Chức năng**: Gửi phản hồi lỗi
- **Chi tiết**: Gửi tin nhắn JSONRPC lỗi

### Hàm singleton

#### `static McpServer& GetInstance()`
- **Chức năng**: Lấy thể hiện duy nhất của McpServer
- **Chi tiết**: Triển khai mẫu singleton