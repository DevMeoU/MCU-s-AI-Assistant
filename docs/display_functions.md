# Display - Chức năng các hàm

## Mô tả tổng quan

File `display.h` và `display.cc` định nghĩa lớp Display chịu trách nhiệm quản lý giao diện người dùng trên màn hình. Lớp này cung cấp các phương thức để hiển thị trạng thái, thông báo, cảm xúc, tin nhắn trò chuyện và các thông tin khác trên màn hình LCD/OLED.

## Danh sách cấu trúc và lớp

### `struct DisplayFonts`
- **Chức năng**: Chứa các font chữ được sử dụng trong hiển thị
- **Chi tiết**:
  - `text_font`: Font chữ cho văn bản
  - `icon_font`: Font chữ cho biểu tượng
  - `emoji_font`: Font chữ cho biểu tượng cảm xúc

### `class Theme`
- **Chức năng**: Đại diện cho một chủ đề hiển thị
- **Chi tiết**:
  - Lưu trữ tên chủ đề
  - Là lớp cơ sở cho các chủ đề cụ thể

### `class Display`
- **Chức năng**: Lớp cơ sở trừu tượng cho hiển thị
- **Chi tiết**: 
  - Quản lý các thành phần giao diện người dùng
  - Cung cấp các phương thức hiển thị thông tin
  - Hỗ trợ khóa hiển thị để đảm bảo an toàn đa luồng

### `class DisplayLockGuard`
- **Chức năng**: Lớp bảo vệ khóa hiển thị
- **Chi tiết**: 
  - Tự động khóa hiển thị khi tạo
  - Tự động mở khóa khi hủy

### `class NoDisplay`
- **Chức năng**: Lớp hiển thị rỗng cho thiết bị không có màn hình
- **Chi tiết**: 
  - Kế thừa từ Display
  - Triển khai các phương thức ảo rỗng

## Danh sách hàm và chức năng chi tiết

### Hàm khởi tạo và giải phóng

#### `Display::Display()`
- **Chức năng**: Khởi tạo đối tượng Display
- **Chi tiết**: 
  - Tạo bộ hẹn giờ cho thông báo
  - Tạo khóa quản lý năng lượng
  - Khởi tạo các biến thành viên

#### `Display::~Display()`
- **Chức năng**: Hàm hủy đối tượng Display
- **Chi tiết**: 
  - Dừng và xóa bộ hẹn giờ
  - Xóa các đối tượng LVGL
  - Xóa khóa quản lý năng lượng

### Hàm hiển thị thông tin cơ bản

#### `void Display::SetStatus(const char* status)`
- **Chức năng**: Thiết lập trạng thái hiển thị
- **Chi tiết**:
  - Hiển thị văn bản trạng thái trên thanh trạng thái
  - Ẩn thông báo nếu đang hiển thị
  - Cập nhật thời gian cập nhật trạng thái cuối cùng

#### `void Display::ShowNotification(const char* notification, int duration_ms = 3000)`
#### `void Display::ShowNotification(const std::string &notification, int duration_ms = 3000)`
- **Chức năng**: Hiển thị thông báo tạm thời
- **Chi tiết**:
  - Hiển thị thông báo trong thời gian chỉ định
  - Ẩn trạng thái hiện tại
  - Tự động ẩn thông báo sau thời gian duration_ms

#### `void Display::SetEmotion(const char* emotion)`
- **Chức năng**: Thiết lập biểu tượng cảm xúc
- **Chi tiết**:
  - Ánh xạ tên cảm xúc sang biểu tượng Font Awesome
  - Hiển thị biểu tượng cảm xúc tương ứng
  - Mặc định hiển thị biểu tượng "neutral" nếu không tìm thấy

#### `void Display::SetIcon(const char* icon)`
- **Chức năng**: Thiết lập biểu tượng
- **Chi tiết**: 
  - Hiển thị biểu tượng được chỉ định trực tiếp
  - Thay thế biểu tượng cảm xúc hiện tại

### Hàm hiển thị tin nhắn và thông tin

#### `void Display::SetChatMessage(const char* role, const char* content)`
- **Chức năng**: Thiết lập tin nhắn trò chuyện
- **Chi tiết**:
  - Hiển thị nội dung tin nhắn theo vai trò (user, assistant, system)
  - Cập nhật nhãn tin nhắn trò chuyện

#### `void Display::SetMusicInfo(const char* song_name)`
- **Chức năng**: Thiết lập thông tin nhạc
- **Chi tiết**:
  - Hiển thị tên bài hát trong nhãn tin nhắn trò chuyện
  - Xóa nhãn nếu tên bài hát rỗng

#### `void Display::SetPreviewImage(const lv_img_dsc_t* image)`
- **Chức năng**: Thiết lập ảnh xem trước
- **Chi tiết**: 
  - Mặc định không làm gì (triển khai rỗng)

### Hàm quản lý chủ đề và năng lượng

#### `void Display::SetTheme(const Theme& theme_name)`
#### `void Display::SetTheme(const std::string& theme_name)`
- **Chức năng**: Thiết lập chủ đề hiển thị
- **Chi tiết**:
  - Lưu tên chủ đề hiện tại
  - Lưu chủ đề vào cài đặt

#### `Theme* Display::GetTheme()`
- **Chức năng**: Lấy chủ đề hiện tại
- **Chi tiết**: Trả về con trỏ đến chủ đề hiện tại

#### `void Display::SetPowerSaveMode(bool on)`
- **Chức năng**: Thiết lập chế độ tiết kiệm năng lượng
- **Chi tiết**:
  - Khi bật: Xóa tin nhắn, hiển thị biểu tượng "sleepy"
  - Khi tắt: Xóa tin nhắn, hiển thị biểu tượng "neutral"

### Hàm cập nhật thanh trạng thái

#### `void Display::UpdateStatusBar(bool update_all = false)`
- **Chức năng**: Cập nhật thanh trạng thái
- **Chi tiết**:
  - Cập nhật biểu tượng tắt tiếng
  - Cập nhật thời gian (nếu thiết bị rảnh)
  - Cập nhật biểu tượng pin
  - Cập nhật biểu tượng mạng
  - Hiển thị cảnh báo pin yếu nếu cần

### Hàm quản lý khóa hiển thị

#### `virtual bool Lock(int timeout_ms = 0) = 0`
#### `virtual void Unlock() = 0`
- **Chức năng**: Khóa/mở khóa hiển thị
- **Chi tiết**: 
  - Là phương thức thuần ảo cần triển khai trong lớp dẫn xuất
  - Đảm bảo an toàn đa luồng khi cập nhật giao diện

### Hàm quản lý FFT (triển khai rỗng)

#### `virtual void start() {}`
#### `virtual void clearScreen() {}`
#### `virtual void stopFft() {}`
- **Chức năng**: Các phương thức quản lý hiển thị FFT
- **Chi tiết**: 
  - Mặc định là triển khai rỗng
  - Có thể được ghi đè trong lớp dẫn xuất

### Hàm truy cập thuộc tính

#### `inline int width() const`
#### `inline int height() const`
- **Chức năng**: Lấy kích thước màn hình
- **Chi tiết**: Trả về chiều rộng và chiều cao của màn hình