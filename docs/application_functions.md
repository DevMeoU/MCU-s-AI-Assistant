# Application - Chức năng các hàm

## Mô tả tổng quan

File `application.cc` là thành phần chính của ứng dụng, đóng vai trò như một bộ điều khiển trung tâm quản lý toàn bộ hoạt động của hệ thống. Lớp Application chịu trách nhiệm cho việc điều phối các trạng thái thiết bị, quản lý kết nối mạng, xử lý âm thanh, và tương tác với người dùng.

## Danh sách hàm và chức năng chi tiết

### 1. Hàm khởi tạo và giải phóng

#### `Application::Application()`
- **Chức năng**: Khởi tạo đối tượng Application
- **Chi tiết**: 
  - Tạo event group để đồng bộ hóa các tác vụ
  - Thiết lập chế độ AEC (Acoustic Echo Cancellation) dựa trên cấu hình
  - Tạo bộ hẹn giờ cho đồng hồ
  - Khởi tạo các biến trạng thái

#### `Application::~Application()`
- **Chức năng**: Hàm hủy đối tượng Application
- **Chi tiết**: 
  - Dừng và xóa bộ hẹn giờ
  - Xóa event group
  - Giải phóng các tài nguyên đã cấp phát

### 2. Hàm khởi động và vòng lặp sự kiện chính

#### `void Application::Start()`
- **Chức năng**: Khởi động ứng dụng
- **Chi tiết**:
  - Thiết lập trạng thái thiết bị là "đang khởi động"
  - Khởi tạo và bắt đầu dịch vụ âm thanh
  - Thiết lập các callback cho dịch vụ âm thanh
  - Khởi động bộ hẹn giờ đồng hồ
  - Chờ mạng sẵn sàng
  - Kiểm tra phiên bản firmware mới
  - Khởi tạo giao thức (MQTT hoặc WebSocket)
  - Đăng ký các callback cho giao thức
  - Thiết lập trạng thái thiết bị là "rảnh"

#### `void Application::MainEventLoop()`
- **Chức năng**: Vòng lặp sự kiện chính của ứng dụng
- **Chi tiết**:
  - Nâng cao mức ưu tiên của tác vụ
  - Chờ các sự kiện (lịch trình, gửi âm thanh, phát hiện từ khóa, thay đổi VAD, lỗi)
  - Xử lý từng loại sự kiện:
    - Lỗi: Hiển thị cảnh báo
    - Gửi âm thanh: Gửi gói âm thanh từ hàng đợi gửi
    - Phát hiện từ khóa: Gọi hàm xử lý
    - Thay đổi VAD: Cập nhật trạng thái LED
    - Lịch trình: Thực thi các tác vụ đã lên lịch

### 3. Hàm quản lý trạng thái thiết bị

#### `DeviceState Application::GetDeviceState() const`
- **Chức năng**: Lấy trạng thái thiết bị hiện tại
- **Chi tiết**: Trả về giá trị trạng thái thiết bị

#### `void Application::SetDeviceState(DeviceState state)`
- **Chức năng**: Thiết lập trạng thái thiết bị
- **Chi tiết**:
  - Kiểm tra nếu trạng thái không thay đổi thì bỏ qua
  - Cập nhật trạng thái và ghi log
  - Gửi sự kiện thay đổi trạng thái
  - Cập nhật hiển thị và LED
  - Dừng phát nhạc khi chuyển từ trạng thái rảnh sang trạng thái khác
  - Xử lý logic cho từng trạng thái:
    - Rảnh: Bật phát hiện từ khóa, tắt xử lý tiếng nói
    - Kết nối: Hiển thị trạng thái kết nối
    - Nghe: Bật xử lý tiếng nói, tắt phát hiện từ khóa
    - Nói: Đặt lại decoder, xử lý AEC

#### `void Application::ToggleChatState()`
- **Chức năng**: Chuyển đổi trạng thái chat
- **Chi tiết**:
  - Xử lý các trạng thái đặc biệt (kích hoạt, cấu hình WiFi, kiểm tra âm thanh)
  - Chuyển đổi giữa các trạng thái:
    - Từ rảnh → Kết nối → Nghe
    - Từ nói → Dừng nói
    - Từ nghe → Đóng kênh âm thanh

#### `void Application::StartListening()`
- **Chức năng**: Bắt đầu nghe
- **Chi tiết**:
  - Xử lý các trạng thái đặc biệt
  - Mở kênh âm thanh nếu chưa mở
  - Thiết lập chế độ nghe bằng tay

#### `void Application::StopListening()`
- **Chức năng**: Dừng nghe
- **Chi tiết**:
  - Xử lý trạng thái kiểm tra âm thanh
  - Gửi lệnh dừng nghe nếu đang ở trạng thái nghe

### 4. Hàm xử lý từ khóa đánh thức

#### `void Application::OnWakeWordDetected()`
- **Chức năng**: Xử lý khi phát hiện từ khóa đánh thức
- **Chi tiết**:
  - Mã hóa dữ liệu từ khóa
  - Mở kênh âm thanh nếu chưa mở
  - Gửi dữ liệu từ khóa đến server
  - Thiết lập chế độ nghe

#### `void Application::WakeWordInvoke(const std::string& wake_word)`
- **Chức năng**: Kích hoạt bằng từ khóa đánh thức
- **Chi tiết**:
  - Chuyển đổi trạng thái chat nếu đang rảnh
  - Dừng nói nếu đang nói
  - Đóng kênh âm thanh nếu đang nghe

### 5. Hàm kiểm tra và cập nhật phiên bản

#### `void Application::CheckNewVersion(Ota& ota)`
- **Chức năng**: Kiểm tra phiên bản mới
- **Chi tiết**:
  - Hiển thị trạng thái kiểm tra phiên bản
  - Kiểm tra phiên bản firmware mới
  - Xử lý nâng cấp nếu có phiên bản mới:
    - Hiển thị thông báo nâng cấp
    - Dừng dịch vụ âm thanh
    - Thực hiện nâng cấp firmware
    - Khởi động lại nếu thành công
  - Xử lý mã kích hoạt nếu cần

#### `void Application::ShowActivationCode(const std::string& code, const std::string& message)`
- **Chức năng**: Hiển thị mã kích hoạt
- **Chi tiết**:
  - Hiển thị cảnh báo với mã kích hoạt
  - Phát âm thanh đọc từng chữ số của mã

### 6. Hàm cảnh báo và thông báo

#### `void Application::Alert(const char* status, const char* message, const char* emotion, const std::string_view& sound)`
- **Chức năng**: Hiển thị cảnh báo
- **Chi tiết**:
  - Ghi log cảnh báo
  - Cập nhật trạng thái, cảm xúc và tin nhắn hiển thị
  - Phát âm thanh cảnh báo nếu có

#### `void Application::DismissAlert()`
- **Chức năng**: Bỏ cảnh báo
- **Chi tiết**: 
  - Đặt lại trạng thái hiển thị về bình thường nếu thiết bị đang rảnh

### 7. Hàm xử lý ngắt nói và chế độ nghe

#### `void Application::AbortSpeaking(AbortReason reason)`
- **Chức năng**: Ngắt nói
- **Chi tiết**:
  - Đánh dấu đã ngắt
  - Gửi lệnh ngắt nói đến server

#### `void Application::SetListeningMode(ListeningMode mode)`
- **Chức năng**: Thiết lập chế độ nghe
- **Chi tiết**:
  - Cập nhật chế độ nghe
  - Thiết lập trạng thái thiết bị là "đang nghe"

### 8. Hàm quản lý tác vụ bất đồng bộ

#### `void Application::Schedule(std::function<void()> callback)`
- **Chức năng**: Lên lịch thực thi tác vụ
- **Chi tiết**:
  - Thêm callback vào hàng đợi tác vụ chính
  - Đặt cờ sự kiện lịch trình

### 9. Hàm kiểm tra trạng thái và chức năng hệ thống

#### `bool Application::IsVoiceDetected() const`
- **Chức năng**: Kiểm tra có phát hiện tiếng nói không
- **Chi tiết**: Gọi hàm tương ứng trong audio_service_

#### `bool Application::CanEnterSleepMode()`
- **Chức năng**: Kiểm tra có thể vào chế độ ngủ không
- **Chi tiết**:
  - Kiểm tra thiết bị đang ở trạng thái rảnh
  - Kiểm tra kênh âm thanh đã đóng
  - Kiểm tra dịch vụ âm thanh đang rảnh

#### `void Application::Reboot()`
- **Chức năng**: Khởi động lại thiết bị
- **Chi tiết**: Gọi hàm esp_restart()

#### `AudioService& Application::GetAudioService()`
- **Chức năng**: Lấy dịch vụ âm thanh
- **Chi tiết**: Trả về tham chiếu đến audio_service_

### 10. Hàm xử lý AEC (Acoustic Echo Cancellation)

#### `void Application::SetAecMode(AecMode mode)`
- **Chức năng**: Thiết lập chế độ AEC
- **Chi tiết**:
  - Cập nhật chế độ AEC
  - Bật/tắt AEC trên thiết bị hoặc server
  - Đóng kênh âm thanh nếu đang mở

#### `AecMode Application::GetAecMode() const`
- **Chức năng**: Lấy chế độ AEC hiện tại
- **Chi tiết**: Trả về giá trị chế độ AEC

### 11. Hàm xử lý đồng hồ

#### `void Application::OnClockTimer()`
- **Chức năng**: Xử lý bộ hẹn giờ đồng hồ
- **Chi tiết**:
  - Tăng bộ đếm tick
  - Cập nhật thanh trạng thái
  - In thống kê heap mỗi 10 giây

### 12. Hàm xử lý tin nhắn MCP

#### `void Application::SendMcpMessage(const std::string& payload)`
- **Chức năng**: Gửi tin nhắn MCP
- **Chi tiết**:
  - Lên lịch gửi tin nhắn MCP qua giao thức

### 13. Hàm xử lý âm thanh ngoài (nhạc)

#### `void Application::AddAudioData(AudioStreamPacket&& packet)`
- **Chức năng**: Thêm dữ liệu âm thanh ngoài (như nhạc)
- **Chi tiết**:
  - Kiểm tra thiết bị đang rảnh và đầu ra âm thanh đã bật
  - Xử lý dữ liệu PCM
  - Resample nếu tỷ lệ lấy mẫu không khớp
  - Chuyển đổi tỷ lệ lấy mẫu động nếu cần
  - Gửi dữ liệu PCM đến codec âm thanh

#### `void Application::PlaySound(const std::string_view& sound)`
- **Chức năng**: Phát âm thanh
- **Chi tiết**: Gọi hàm PlaySound trong audio_service_