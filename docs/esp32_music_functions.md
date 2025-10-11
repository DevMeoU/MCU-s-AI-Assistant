# Esp32Music - Chức năng các hàm

## Mô tả tổng quan

File `esp32_music.cc` là thành phần xử lý chức năng phát nhạc trong ứng dụng, hỗ trợ phát nhạc trực tuyến từ xa và hiển thị lời bài hát. File này tích hợp các chức năng như tải nhạc, phát nhạc, điều khiển phát nhạc và hiển thị lời bài hát.

## Danh sách hàm và chức năng chi tiết

### 1. Hàm khởi tạo và giải phóng

#### `Esp32Music::Esp32Music()`
- **Chức năng**: Khởi tạo đối tượng Esp32Music
- **Chi tiết**: 
  - Khởi tạo các biến thành viên
  - Phân bổ bộ nhớ FFT trong PSRAM
  - Khởi tạo bộ giải mã MP3

#### `Esp32Music::~Esp32Music()`
- **Chức năng**: Hàm hủy đối tượng Esp32Music
- **Chi tiết**: 
  - Dừng tất cả các hoạt động đang chạy
  - Thông báo cho các luồng đang chờ
  - Chờ các luồng tải xuống, phát lại và lời bài hát kết thúc
  - Dọn dẹp bộ đệm và bộ giải mã MP3

### 2. Hàm tải và phát nhạc cơ bản

#### `bool Esp32Music::Download(const std::string& song_name, const std::string& artist_name)`
- **Chức năng**: Tải thông tin chi tiết bài hát
- **Chi tiết**:
  - Xóa dữ liệu tải xuống trước đó
  - Lưu tên bài hát và nghệ sĩ
  - Gọi API stream_pcm để lấy thông tin âm thanh
  - Phân tích JSON phản hồi để trích xuất URL âm thanh và lời bài hát
  - Bắt đầu phát trực tuyến nếu có URL âm thanh hợp lệ
  - Khởi động tải xuống và hiển thị lời bài hát nếu ở chế độ hiển thị lời

#### `std::string Esp32Music::GetDownloadResult()`
- **Chức năng**: Lấy kết quả tải xuống
- **Chi tiết**: Trả về dữ liệu JSON phản hồi từ API

#### `bool Esp32Music::StartStreaming(const std::string& music_url)`
- **Chức năng**: Bắt đầu phát trực tuyến
- **Chi tiết**:
  - Đánh dấu nhạc có quyền ưu tiên
  - Dừng phát và tải xuống trước đó
  - Chờ các luồng trước đó kết thúc hoàn toàn
  - Xóa sạch bộ đệm
  - Cấu hình kích thước ngăn xếp luồng
  - Bắt đầu luồng tải xuống và luồng phát

#### `bool Esp32Music::StopStreaming()`
- **Chức năng**: Dừng phát trực tuyến
- **Chi tiết**:
  - Bỏ đánh dấu quyền ưu tiên của nhạc
  - Dừng cờ tải xuống và phát
  - Xóa hiển thị tên bài hát
  - Thông báo cho tất cả các luồng đang chờ
  - Chờ các luồng kết thúc

### 3. Hàm điều khiển phát nhạc

#### `bool Esp32Music::Pause()`
- **Chức năng**: Tạm dừng phát nhạc
- **Chi tiết**: 
  - Kiểm tra nếu đang phát và chưa tạm dừng
  - Đánh dấu trạng thái tạm dừng

#### `bool Esp32Music::Resume()`
- **Chức năng**: Tiếp tục phát nhạc
- **Chi tiết**: 
  - Kiểm tra nếu đang phát và đang tạm dừng
  - Bỏ đánh dấu trạng thái tạm dừng

#### `void Esp32Music::SetVolume(int volume)`
- **Chức năng**: Thiết lập âm lượng
- **Chi tiết**: 
  - Giới hạn giá trị âm lượng trong khoảng 0-100
  - Áp dụng âm lượng cho codec nếu có

#### `int Esp32Music::GetVolume() const`
- **Chức năng**: Lấy mức âm lượng hiện tại
- **Chi tiết**: Trả về giá trị âm lượng

#### `std::string Esp32Music::GetCurrentSong() const`
- **Chức năng**: Lấy tên bài hát hiện tại
- **Chi tiết**: Trả về tên bài hát đang phát

### 4. Hàm kiểm tra trạng thái

#### `bool Esp32Music::IsDownloading() const`
- **Chức năng**: Kiểm tra có đang tải xuống không
- **Chi tiết**: Trả về trạng thái tải xuống

#### `bool Esp32Music::IsPlaying() const`
- **Chức năng**: Kiểm tra có đang phát nhạc không
- **Chi tiết**: Trả về trạng thái phát (không bao gồm trạng thái tạm dừng)

#### `bool Esp32Music::IsPaused() const`
- **Chức năng**: Kiểm tra có đang tạm dừng không
- **Chi tiết**: Trả về trạng thái tạm dừng

#### `size_t Esp32Music::GetBufferSize() const`
- **Chức năng**: Lấy kích thước bộ đệm
- **Chi tiết**: Trả về kích thước bộ đệm âm thanh hiện tại

#### `int16_t* Esp32Music::GetAudioData()`
- **Chức năng**: Lấy dữ liệu âm thanh
- **Chi tiết**: Trả về con trỏ đến dữ liệu PCM cuối cùng

#### `bool Esp32Music::HasPriority() const`
- **Chức năng**: Kiểm tra nhạc có quyền ưu tiên không
- **Chi tiết**: Trả về trạng thái quyền ưu tiên của nhạc

### 5. Hàm xử lý tác vụ âm thanh

#### `void Esp32Music::DownloadAudioStream(const std::string& music_url)`
- **Chức năng**: Tải xuống âm thanh theo luồng
- **Chi tiết**:
  - Mở kết nối HTTP đến URL nhạc
  - Đặt tiêu đề yêu cầu và xác thực
  - Kiểm tra mã trạng thái HTTP
  - Đọc dữ liệu âm thanh theo khối
  - Tạo khối dữ liệu âm thanh và đẩy vào bộ đệm
  - Chờ bộ đệm có không gian nếu đã đầy
  - Thông báo luồng phát có dữ liệu mới

#### `void Esp32Music::PlayAudioStream()`
- **Chức năng**: Phát âm thanh theo luồng
- **Chi tiết**:
  - Khởi tạo các biến theo dõi thời gian
  - Chờ bộ đệm có đủ dữ liệu để bắt đầu phát
  - Kiểm tra trạng thái thiết bị (chỉ phát khi ở trạng thái rảnh)
  - Hiển thị tên bài hát đang phát
  - Đọc dữ liệu từ bộ đệm và giải mã MP3
  - Chuyển đổi stereo sang mono nếu cần
  - Gửi dữ liệu PCM đến hàng đợi âm thanh của Application
  - Cập nhật thời gian phát hiện tại và hiển thị lời bài hát
  - Xử lý lỗi giải mã và khôi phục khi cần

### 6. Hàm quản lý bộ đệm và bộ giải mã

#### `void Esp32Music::ClearAudioBuffer()`
- **Chức năng**: Xóa sạch bộ đệm âm thanh
- **Chi tiết**: 
  - Khóa mutex
  - Xóa tất cả các khối dữ liệu trong bộ đệm
  - Đặt lại kích thước bộ đệm về 0

#### `bool Esp32Music::InitializeMp3Decoder()`
- **Chức năng**: Khởi tạo bộ giải mã MP3
- **Chi tiết**: 
  - Gọi hàm MP3InitDecoder
  - Kiểm tra kết quả và cập nhật trạng thái

#### `void Esp32Music::CleanupMp3Decoder()`
- **Chức năng**: Dọn dẹp bộ giải mã MP3
- **Chi tiết**: 
  - Gọi hàm MP3FreeDecoder nếu decoder đã được khởi tạo
  - Đặt lại trạng thái khởi tạo

#### `void Esp32Music::ResetSampleRate()`
- **Chức năng**: Đặt lại tỷ lệ lấy mẫu về giá trị gốc
- **Chi tiết**: 
  - Kiểm tra nếu tỷ lệ lấy mẫu hiện tại khác tỷ lệ gốc
  - Gọi hàm đặt lại tỷ lệ lấy mẫu trong codec

#### `size_t Esp32Music::SkipId3Tag(uint8_t* data, size_t size)`
- **Chức năng**: Bỏ qua thẻ ID3 ở đầu tệp MP3
- **Chi tiết**: 
  - Kiểm tra tiêu đề thẻ ID3v2
  - Tính kích thước thẻ theo định dạng synchsafe integer
  - Trả về số byte cần bỏ qua

### 7. Hàm xử lý lời bài hát

#### `bool Esp32Music::DownloadLyrics(const std::string& lyric_url)`
- **Chức năng**: Tải xuống lời bài hát
- **Chi tiết**:
  - Kiểm tra URL lời bài hát có hợp lệ không
  - Mở kết nối HTTP với URL lời bài hát
  - Đọc nội dung lời bài hát
  - Xử lý thử lại nếu có lỗi
  - Gọi hàm ParseLyrics để phân tích nội dung

#### `bool Esp32Music::ParseLyrics(const std::string& lyric_content)`
- **Chức năng**: Phân tích lời bài hát
- **Chi tiết**:
  - Khóa mutex để bảo vệ mảng lyrics_
  - Xóa mảng lời bài hát hiện tại
  - Chia nội dung theo dòng
  - Phân tích định dạng LRC [mm:ss.xx] văn bản lời bài hát
  - Bỏ qua các thẻ siêu dữ liệu như [ti:tiêu đề], [ar:nghệ sĩ]
  - Lưu trữ cặp thời gian và nội dung lời bài hát
  - Sắp xếp theo dấu thời gian

#### `void Esp32Music::LyricDisplayThread()`
- **Chức năng**: Luồng hiển thị lời bài hát
- **Chi tiết**:
  - Gọi hàm DownloadLyrics để tải và phân tích lời bài hát
  - Định kỳ kiểm tra xem có cần cập nhật hiển thị không

#### `void Esp32Music::UpdateLyricDisplay(int64_t current_time_ms)`
- **Chức năng**: Cập nhật hiển thị lời bài hát
- **Chi tiết**:
  - Khóa mutex
  - Tìm lời bài hát hiện tại nên hiển thị dựa trên thời gian phát
  - Cập nhật hiển thị nếu chỉ mục lời bài hát thay đổi

### 8. Hàm hỗ trợ khôi phục và đồng bộ

#### `bool Esp32Music::ResyncMp3Stream(uint8_t* base_buffer, uint8_t*& read_ptr, int& bytes_left)`
- **Chức năng**: Tìm từ đồng bộ MP3 (0xFFEx) và điều chỉnh con trỏ đọc
- **Chi tiết**:
  - Quét tìm mẫu đồng bộ MP3 0xFFEx
  - Điều chỉnh con trỏ đọc và số byte còn lại nếu tìm thấy
  - Trả về true nếu tìm thấy, false nếu không tìm thấy

#### `void Esp32Music::ResetMp3Decoder()`
- **Chức năng**: Đặt lại bộ giải mã MP3
- **Chi tiết**:
  - Gọi hàm dọn dẹp bộ giải mã
  - Chờ ngắn
  - Gọi hàm khởi tạo lại bộ giải mã

#### `void Esp32Music::RecoverFromStreamError()`
- **Chức năng**: Khôi phục từ lỗi luồng
- **Chi tiết**:
  - Ghi log cảnh báo
  - Xóa buffer đầu vào
  - Đặt lại các bộ đếm lỗi

### 9. Hàm điều khiển chế độ hiển thị

#### `void Esp32Music::SetDisplayMode(DisplayMode mode)`
- **Chức năng**: Thiết lập chế độ hiển thị
- **Chi tiết**:
  - Cập nhật chế độ hiển thị (phổ tần hoặc lời bài hát)
  - Ghi log thông tin thay đổi chế độ

#### `DisplayMode Esp32Music::GetDisplayMode() const`
- **Chức năng**: Lấy chế độ hiển thị hiện tại
- **Chi tiết**: Trả về chế độ hiển thị hiện tại

### 10. Các hàm hỗ trợ xác thực và mã hóa

#### `static std::string get_device_mac()`
- **Chức năng**: Lấy địa chỉ MAC của thiết bị
- **Chi tiết**: Gọi SystemInfo::GetMacAddress()

#### `static std::string get_device_chip_id()`
- **Chức năng**: Lấy ID chip của thiết bị
- **Chi tiết**: Sử dụng địa chỉ MAC làm ID chip, loại bỏ dấu phân cách

#### `static std::string generate_dynamic_key(int64_t timestamp)`
- **Chức năng**: Tạo khóa động
- **Chi tiết**:
  - Kết hợp MAC, Chip ID, timestamp và khóa bí mật
  - Băm SHA256 và chuyển đổi thành chuỗi hex

#### `static void add_auth_headers(Http* http)`
- **Chức năng**: Thêm tiêu đề xác thực cho yêu cầu HTTP
- **Chi tiết**:
  - Lấy timestamp hiện tại
  - Tạo khóa động
  - Lấy thông tin thiết bị
  - Thêm các tiêu đề X-MAC-Address, X-Chip-ID, X-Timestamp, X-Dynamic-Key

#### `static std::string url_encode(const std::string& str)`
- **Chức năng**: Mã hóa URL
- **Chi tiết**: Mã hóa các ký tự đặc biệt theo chuẩn URL encoding

#### `static std::string buildUrlWithParams(const std::string& base_url, const std::string& path, const std::string& query)`
- **Chức năng**: Xây dựng URL với tham số
- **Chi tiết**: Ghép nối base URL, path và query string, đồng thời mã hóa URL cho các giá trị