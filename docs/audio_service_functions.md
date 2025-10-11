# AudioService - Chức năng các hàm

## Mô tả tổng quan

File `audio_service.cc` là thành phần cốt lõi trong hệ thống xử lý âm thanh, chịu trách nhiệm quản lý toàn bộ luồng dữ liệu âm thanh trong ứng dụng. File này tích hợp các chức năng như thu âm, phát âm, mã hóa/giải mã âm thanh, phát hiện từ khóa đánh thức và xử lý tiếng nói.

## Danh sách hàm và chức năng chi tiết

### 1. Hàm khởi tạo và giải phóng

#### `AudioService::AudioService()`
- **Chức năng**: Khởi tạo đối tượng AudioService
- **Chi tiết**: 
  - Tạo event group để đồng bộ hóa các tác vụ
  - Chuẩn bị các biến cần thiết cho hoạt động của dịch vụ âm thanh

#### `AudioService::~AudioService()`
- **Chức năng**: Hàm hủy đối tượng AudioService
- **Chi tiết**: 
  - Giải phóng event group
  - Dọn dẹp các tài nguyên đã cấp phát

### 2. Hàm khởi tạo và cấu hình

#### `void AudioService::Initialize(AudioCodec* codec)`
- **Chức năng**: Khởi tạo và cấu hình dịch vụ âm thanh
- **Chi tiết**:
  - Nhận codec âm thanh từ bên ngoài
  - Khởi tạo encoder và decoder OPUS
  - Cấu hình resampler cho các tỷ lệ lấy mẫu khác nhau
  - Khởi tạo bộ xử lý âm thanh (AfeAudioProcessor hoặc NoAudioProcessor)
  - Thiết lập các callback cho output và VAD state change
  - Tạo bộ hẹn giờ kiểm tra trạng thái năng lượng âm thanh

#### `void AudioService::Start()`
- **Chức năng**: Bắt đầu chạy dịch vụ âm thanh
- **Chi tiết**:
  - Đánh dấu dịch vụ đã bắt đầu
  - Xóa các cờ sự kiện
  - Khởi động các tác vụ:
    - AudioInputTask: Xử lý dữ liệu âm thanh đầu vào
    - AudioOutputTask: Xử lý dữ liệu âm thanh đầu ra
    - OpusCodecTask: Xử lý mã hóa/giải mã âm thanh

#### `void AudioService::Stop()`
- **Chức năng**: Dừng dịch vụ âm thanh
- **Chi tiết**:
  - Dừng bộ hẹn giờ kiểm tra năng lượng
  - Đánh dấu dịch vụ đã dừng
  - Thiết lập tất cả các cờ sự kiện
  - Xóa sạch các hàng đợi âm thanh
  - Thông báo cho các luồng đang chờ

### 3. Hàm xử lý dữ liệu âm thanh

#### `bool AudioService::ReadAudioData(std::vector<int16_t>& data, int sample_rate, int samples)`
- **Chức năng**: Đọc dữ liệu âm thanh từ codec
- **Chi tiết**:
  - Kích hoạt đầu vào âm thanh nếu chưa được bật
  - Đọc dữ liệu từ codec
  - Resample dữ liệu nếu tỷ lệ lấy mẫu không phù hợp
  - Xử lý trường hợp stereo (2 kênh) bằng cách tách riêng kênh mic và kênh tham chiếu
  - Cập nhật thời gian đọc dữ liệu cuối cùng
  - Gửi dữ liệu debug nếu bật chế độ audio debugger

#### `void AudioService::AudioInputTask()`
- **Chức năng**: Tác vụ xử lý âm thanh đầu vào chạy trong luồng riêng
- **Chi tiết**:
  - Chờ các sự kiện (testing, wake word, audio processor)
  - Đọc dữ liệu âm thanh và phân phối cho các chức năng tương ứng:
    - Audio testing: Đẩy dữ liệu vào hàng đợi testing
    - Wake word: Cung cấp dữ liệu cho hệ thống phát hiện từ khóa
    - Audio processor: Cung cấp dữ liệu cho bộ xử lý âm thanh

#### `void AudioService::AudioOutputTask()`
- **Chức năng**: Tác vụ phát âm thanh đầu ra chạy trong luồng riêng
- **Chi tiết**:
  - Chờ dữ liệu trong hàng đợi phát lại
  - Kích hoạt đầu ra âm thanh nếu chưa được bật
  - Gửi dữ liệu PCM đến codec để phát ra loa
  - Cập nhật thời gian phát âm thanh cuối cùng
  - Lưu timestamp cho AEC phía server nếu được bật

#### `void AudioService::OpusCodecTask()`
- **Chức năng**: Tác vụ mã hóa/giải mã OPUS chạy trong luồng riêng
- **Chi tiết**:
  - Xử lý hai luồng công việc:
    - Giải mã: Lấy gói âm thanh từ hàng đợi decode, giải mã và đẩy vào hàng đợi phát lại
    - Mã hóa: Lấy dữ liệu PCM từ hàng đợi encode, mã hóa và đẩy vào hàng đợi gửi hoặc testing
  - Resample dữ liệu nếu cần thiết
  - Gọi callback khi hàng đợi gửi có sẵn dữ liệu

### 4. Hàm quản lý hàng đợi và tác vụ

#### `void AudioService::SetDecodeSampleRate(int sample_rate, int frame_duration)`
- **Chức năng**: Thiết lập tỷ lệ lấy mẫu cho giải mã
- **Chi tiết**:
  - Kiểm tra nếu tỷ lệ lấy mẫu hoặc độ dài khung không thay đổi thì bỏ qua
  - Tạo lại decoder với thông số mới
  - Cấu hình resampler nếu tỷ lệ lấy mẫu decoder khác với codec đầu ra

#### `void AudioService::PushTaskToEncodeQueue(AudioTaskType type, std::vector<int16_t>&& pcm)`
- **Chức năng**: Đẩy tác vụ mã hóa vào hàng đợi
- **Chi tiết**:
  - Tạo tác vụ âm thanh mới với loại và dữ liệu PCM
  - Gán timestamp nếu là loại tác vụ gửi và có timestamp trong hàng đợi
  - Đẩy tác vụ vào hàng đợi encode
  - Thông báo cho các luồng đang chờ

#### `bool AudioService::PushPacketToDecodeQueue(std::unique_ptr<AudioStreamPacket> packet, bool wait)`
- **Chức năng**: Đẩy gói âm thanh vào hàng đợi giải mã
- **Chi tiết**:
  - Kiểm tra độ đầy của hàng đợi
  - Nếu hàng đợi đầy và có yêu cầu chờ, sẽ chờ đến khi có chỗ trống
  - Nếu hàng đợi đầy và không yêu cầu chờ, trả về false
  - Đẩy gói âm thanh vào hàng đợi decode
  - Thông báo cho các luồng đang chờ

#### `std::unique_ptr<AudioStreamPacket> AudioService::PopPacketFromSendQueue()`
- **Chức năng**: Lấy gói âm thanh từ hàng đợi gửi
- **Chi tiết**:
  - Kiểm tra nếu hàng đợi rỗng thì trả về null
  - Lấy gói âm thanh đầu tiên trong hàng đợi
  - Thông báo cho các luồng đang chờ

### 5. Hàm xử lý từ khóa đánh thức (Wake Word)

#### `void AudioService::EncodeWakeWord()`
- **Chức năng**: Mã hóa dữ liệu từ khóa đánh thức
- **Chi tiết**: Gọi hàm mã hóa dữ liệu từ khóa trong wake_word_

#### `const std::string& AudioService::GetLastWakeWord() const`
- **Chức năng**: Lấy từ khóa đánh thức được phát hiện gần đây nhất
- **Chi tiết**: Trả về chuỗi chứa từ khóa đánh thức từ wake_word_

#### `std::unique_ptr<AudioStreamPacket> AudioService::PopWakeWordPacket()`
- **Chức năng**: Lấy gói âm thanh chứa từ khóa đánh thức
- **Chi tiết**:
  - Tạo gói âm thanh mới
  - Gọi hàm lấy dữ liệu OPUS từ wake_word_
  - Trả về gói âm thanh nếu có dữ liệu, ngược lại trả về null

#### `void AudioService::EnableWakeWordDetection(bool enable)`
- **Chức năng**: Bật/tắt chức năng phát hiện từ khóa đánh thức
- **Chi tiết**:
  - Kiểm tra nếu wake_word_ chưa được tạo thì bỏ qua
  - Nếu bật: Khởi tạo wake_word_ nếu chưa được khởi tạo, sau đó bắt đầu
  - Nếu tắt: Dừng wake_word_
  - Cập nhật cờ sự kiện tương ứng

### 6. Hàm xử lý tiếng nói và kiểm tra âm thanh

#### `void AudioService::EnableVoiceProcessing(bool enable)`
- **Chức năng**: Bật/tắt xử lý tiếng nói
- **Chi tiết**:
  - Nếu bật: Khởi tạo audio processor nếu chưa được khởi tạo, đặt cờ warmup, bắt đầu xử lý
  - Nếu tắt: Dừng audio processor
  - Đặt lại decoder và cập nhật cờ sự kiện

#### `void AudioService::EnableAudioTesting(bool enable)`
- **Chức năng**: Bật/tắt chế độ kiểm tra âm thanh
- **Chi tiết**:
  - Cập nhật cờ sự kiện audio testing
  - Nếu tắt: Chuyển dữ liệu từ hàng đợi testing sang hàng đợi decode

#### `void AudioService::EnableDeviceAec(bool enable)`
- **Chức năng**: Bật/tắt chức năng triệt tiếng vang (AEC) trên thiết bị
- **Chi tiết**:
  - Khởi tạo audio processor nếu chưa được khởi tạo
  - Gọi hàm bật/tắt AEC trong audio processor

### 7. Hàm quản lý và hỗ trợ khác

#### `void AudioService::SetCallbacks(AudioServiceCallbacks& callbacks)`
- **Chức năng**: Thiết lập các hàm callback
- **Chi tiết**: Gán cấu trúc callbacks chứa các hàm sẽ được gọi khi có sự kiện

#### `void AudioService::PlaySound(const std::string_view& ogg)`
- **Chức năng**: Phát âm thanh từ dữ liệu OGG
- **Chi tiết**:
  - Kích hoạt đầu ra âm thanh nếu chưa được bật
  - Phân tích cấu trúc file OGG:
    - Đọc header OpusHead để lấy thông số sample rate
    - Bỏ qua header OpusTags
    - Đẩy các gói âm thanh vào hàng đợi decode
  - Hỗ trợ phân tích trang OGG và xử lý các gói dữ liệu

#### `bool AudioService::IsIdle()`
- **Chức năng**: Kiểm tra xem dịch vụ âm thanh có đang rảnh không
- **Chi tiết**: Trả về true nếu tất cả các hàng đợi đều trống

#### `void AudioService::ResetDecoder()`
- **Chức năng**: Đặt lại bộ giải mã
- **Chi tiết**:
  - Đặt lại trạng thái decoder
  - Xóa sạch các hàng đợi liên quan đến giải mã và phát lại
  - Thông báo cho các luồng đang chờ

#### `void AudioService::CheckAndUpdateAudioPowerState()`
- **Chức năng**: Kiểm tra và cập nhật trạng thái năng lượng âm thanh
- **Chi tiết**:
  - Tính thời gian trôi qua từ lần cuối thao tác với input/output
  - Tắt input/output nếu quá thời gian timeout
  - Dừng bộ hẹn giờ nếu cả input và output đều đã tắt

#### `void AudioService::SetModelsList(srmodel_list_t* models_list)`
- **Chức năng**: Thiết lập danh sách mô hình
- **Chi tiết**:
  - Lưu danh sách mô hình
  - Tạo wake_word_ phù hợp với loại mô hình và kiến trúc chip
  - Đăng ký callback khi phát hiện từ khóa

#### `bool AudioService::IsAfeWakeWord()`
- **Chức năng**: Kiểm tra xem có sử dụng AFE wake word không
- **Chi tiết**: Trả về true nếu wake_word_ là kiểu AfeWakeWord

#### `void AudioService::UpdateOutputTimestamp()`
- **Chức năng**: Cập nhật thời gian phát âm thanh cuối cùng
- **Chi tiết**: Ghi nhận thời điểm hiện tại là thời gian phát âm thanh cuối cùng