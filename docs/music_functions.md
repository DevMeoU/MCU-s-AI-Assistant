# Music - Chức năng các hàm

## Mô tả tổng quan

File `music.h` định nghĩa giao diện cơ bản cho chức năng phát nhạc trong ứng dụng. Đây là lớp trừu tượng cung cấp các phương thức cơ bản mà bất kỳ lớp phát nhạc nào cũng cần triển khai.

## Danh sách hàm và chức năng chi tiết

### 1. Hàm hủy ảo

#### `virtual ~Music() = default;`
- **Chức năng**: Hàm hủy ảo
- **Chi tiết**: 
  - Cho phép hủy đối tượng thông qua con trỏ lớp cơ sở
  - Đảm bảo hàm hủy của lớp dẫn xuất được gọi đúng cách

### 2. Hàm tải nhạc

#### `virtual bool Download(const std::string& song_name, const std::string& artist_name = "") = 0;`
- **Chức năng**: Tải thông tin bài hát
- **Chi tiết**:
  - Là phương thức thuần ảo cần được triển khai trong lớp dẫn xuất
  - Nhận tên bài hát và tên nghệ sĩ (tùy chọn)
  - Trả về true nếu tải thành công, false nếu thất bại

#### `virtual std::string GetDownloadResult() = 0;`
- **Chức năng**: Lấy kết quả tải xuống
- **Chi tiết**:
  - Là phương thức thuần ảo cần được triển khai trong lớp dẫn xuất
  - Trả về dữ liệu kết quả tải xuống (thường là JSON)

### 3. Hàm phát trực tuyến

#### `virtual bool StartStreaming(const std::string& music_url) = 0;`
- **Chức năng**: Bắt đầu phát trực tuyến
- **Chi tiết**:
  - Là phương thức thuần ảo cần được triển khai trong lớp dẫn xuất
  - Nhận URL nhạc để phát trực tuyến
  - Trả về true nếu bắt đầu thành công, false nếu thất bại

#### `virtual bool StopStreaming() = 0;`
- **Chức năng**: Dừng phát trực tuyến
- **Chi tiết**:
  - Là phương thức thuần ảo cần được triển khai trong lớp dẫn xuất
  - Dừng việc phát nhạc trực tuyến đang chạy
  - Trả về true nếu dừng thành công, false nếu thất bại

### 4. Hàm kiểm tra trạng thái

#### `virtual size_t GetBufferSize() const = 0;`
- **Chức năng**: Lấy kích thước bộ đệm
- **Chi tiết**:
  - Là phương thức thuần ảo cần được triển khai trong lớp dẫn xuất
  - Trả về kích thước bộ đệm âm thanh hiện tại

#### `virtual bool IsDownloading() const = 0;`
- **Chức năng**: Kiểm tra có đang tải xuống không
- **Chi tiết**:
  - Là phương thức thuần ảo cần được triển khai trong lớp dẫn xuất
  - Trả về true nếu đang trong quá trình tải xuống, false nếu không

#### `virtual int16_t* GetAudioData() = 0;`
- **Chức năng**: Lấy dữ liệu âm thanh
- **Chi tiết**:
  - Là phương thức thuần ảo cần được triển khai trong lớp dẫn xuất
  - Trả về con trỏ đến dữ liệu âm thanh PCM