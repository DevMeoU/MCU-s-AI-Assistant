# AudioService - Dịch vụ âm thanh

## Chức năng chính của file

File `audio_service.cc` là một phần quan trọng trong hệ thống âm thanh của dự án, chịu trách nhiệm cho các chức năng chính sau:

1. **Quản lý luồng âm thanh đầu vào/đầu ra**: Xử lý việc thu thập âm thanh từ microphone và phát âm thanh ra loa
2. **Mã hóa/giải mã âm thanh**: Sử dụng codec OPUS để nén và giải nén dữ liệu âm thanh
3. **Phát hiện từ khóa đánh thức (Wake Word)**: Nhận diện các từ khóa để kích hoạt thiết bị
4. **Xử lý tiếng nói**: Xử lý âm thanh đầu vào để nhận diện tiếng nói
5. **Quản lý tài nguyên âm thanh**: Tối ưu hóa việc sử dụng bộ nhớ và xử lý năng lượng

## Danh sách và chức năng các hàm

### Hàm khởi tạo và hủy
```cpp
AudioService::AudioService()
```
- **Chức năng**: Khởi tạo dịch vụ âm thanh, tạo nhóm sự kiện (event group)

```cpp
AudioService::~AudioService()
```
- **Chức năng**: Hủy dịch vụ âm thanh, giải phóng tài nguyên

### Hàm khởi tạo và quản lý dịch vụ
```cpp
void AudioService::Initialize(AudioCodec* codec)
```
- **Chức năng**: Khởi tạo dịch vụ âm thanh với codec được cung cấp
- **Chi tiết**: Thiết lập bộ mã hóa/giải mã OPUS, bộ xử lý âm thanh, và bộ hẹn giờ kiểm tra năng lượng

```cpp
void AudioService::Start()
```
- **Chức năng**: Bắt đầu chạy các tác vụ âm thanh
- **Chi tiết**: Khởi động các luồng xử lý âm thanh đầu vào, đầu ra và codec

```cpp
void AudioService::Stop()
```
- **Chức năng**: Dừng dịch vụ âm thanh
- **Chi tiết**: Dừng tất cả các luồng và xóa sạch hàng đợi âm thanh

### Hàm xử lý dữ liệu âm thanh
```cpp
bool AudioService::ReadAudioData(std::vector<int16_t>& data, int sample_rate, int samples)
```
- **Chức năng**: Đọc dữ liệu âm thanh từ codec
- **Chi tiết**: Xử lý resample nếu cần và cập nhật thời gian đọc dữ liệu cuối cùng

```cpp
void AudioService::AudioInputTask()
```
- **Chức năng**: Tác vụ xử lý âm thanh đầu vào
- **Chi tiết**: Đọc dữ liệu âm thanh và phân phối cho các chức năng khác như kiểm tra âm thanh, phát hiện từ khóa, xử lý tiếng nói

```cpp
void AudioService::AudioOutputTask()
```
- **Chức năng**: Tác vụ phát âm thanh đầu ra
- **Chi tiết**: Lấy dữ liệu từ hàng đợi phát lại và gửi đến codec để phát ra loa

```cpp
void AudioService::OpusCodecTask()
```
- **Chức năng**: Tác vụ mã hóa/giải mã OPUS
- **Chi tiết**: Xử lý việc mã hóa dữ liệu âm thanh để gửi và giải mã dữ liệu nhận được

### Hàm quản lý hàng đợi và tác vụ
```cpp
void AudioService::SetDecodeSampleRate(int sample_rate, int frame_duration)
```
- **Chức năng**: Thiết lập tỷ lệ lấy mẫu cho việc giải mã
- **Chi tiết**: Cấu hình lại bộ giải mã OPUS nếu tỷ lệ lấy mẫu thay đổi

```cpp
void AudioService::PushTaskToEncodeQueue(AudioTaskType type, std::vector<int16_t>&& pcm)
```
- **Chức năng**: Thêm tác vụ mã hóa vào hàng đợi
- **Chi tiết**: Đẩy dữ liệu PCM vào hàng đợi mã hóa với loại tác vụ tương ứng

```cpp
bool AudioService::PushPacketToDecodeQueue(std::unique_ptr<AudioStreamPacket> packet, bool wait)
```
- **Chức năng**: Thêm gói dữ liệu vào hàng đợi giải mã
- **Chi tiết**: Đẩy gói âm thanh đã mã hóa vào hàng đợi giải mã

```cpp
std::unique_ptr<AudioStreamPacket> AudioService::PopPacketFromSendQueue()
```
- **Chức năng**: Lấy gói dữ liệu từ hàng đợi gửi
- **Chi tiết**: Trả về gói âm thanh đã sẵn sàng để gửi đi

### Hàm xử lý từ khóa đánh thức (Wake Word)
```cpp
void AudioService::EncodeWakeWord()
```
- **Chức năng**: Mã hóa dữ liệu từ khóa đánh thức
- **Chi tiết**: Gọi hàm mã hóa dữ liệu từ khóa đã được phát hiện

```cpp
const std::string& AudioService::GetLastWakeWord() const
```
- **Chức năng**: Lấy từ khóa đánh thức được phát hiện gần đây nhất
- **Chi tiết**: Trả về chuỗi chứa từ khóa đánh thức

```cpp
std::unique_ptr<AudioStreamPacket> AudioService::PopWakeWordPacket()
```
- **Chức năng**: Lấy gói dữ liệu từ khóa đánh thức
- **Chi tiết**: Trả về gói âm thanh chứa từ khóa đánh thức đã được mã hóa

```cpp
void AudioService::EnableWakeWordDetection(bool enable)
```
- **Chức năng**: Bật/tắt chức năng phát hiện từ khóa đánh thức
- **Chi tiết**: Khởi tạo và bắt đầu/dừng hệ thống phát hiện từ khóa

### Hàm xử lý tiếng nói và kiểm tra âm thanh
```cpp
void AudioService::EnableVoiceProcessing(bool enable)
```
- **Chức năng**: Bật/tắt xử lý tiếng nói
- **Chi tiết**: Kích hoạt hệ thống xử lý âm thanh đầu vào để nhận diện tiếng nói

```cpp
void AudioService::EnableAudioTesting(bool enable)
```
- **Chức năng**: Bật/tắt chế độ kiểm tra âm thanh
- **Chi tiết**: Cho phép kiểm tra âm thanh thông qua nút BOOT trong chế độ cấu hình mạng

```cpp
void AudioService::EnableDeviceAec(bool enable)
```
- **Chức năng**: Bật/tắt chức năng triệt tiếng vang (AEC)
- **Chi tiết**: Kích hoạt hoặc vô hiệu hóa bộ triệt tiếng vang trên thiết bị

### Hàm quản lý và hỗ trợ khác
```cpp
void AudioService::SetCallbacks(AudioServiceCallbacks& callbacks)
```
- **Chức năng**: Thiết lập các hàm callback
- **Chi tiết**: Cấu hình các hàm được gọi khi có sự kiện âm thanh

```cpp
void AudioService::PlaySound(const std::string_view& ogg)
```
- **Chức năng**: Phát âm thanh từ dữ liệu OGG
- **Chi tiết**: Phân tích và phát dữ liệu âm thanh định dạng OGG

```cpp
bool AudioService::IsIdle()
```
- **Chức năng**: Kiểm tra xem dịch vụ âm thanh có đang rảnh không
- **Chi tiết**: Trả về true nếu tất cả các hàng đợi đều trống

```cpp
void AudioService::ResetDecoder()
```
- **Chức năng**: Đặt lại bộ giải mã
- **Chi tiết**: Xóa sạch các hàng đợi và đặt lại trạng thái bộ giải mã

```cpp
void AudioService::CheckAndUpdateAudioPowerState()
```
- **Chức năng**: Kiểm tra và cập nhật trạng thái năng lượng âm thanh
- **Chi tiết**: Tự động bật/tắt microphone và loa để tiết kiệm năng lượng

```cpp
void AudioService::SetModelsList(srmodel_list_t* models_list)
```
- **Chức năng**: Thiết lập danh sách mô hình
- **Chi tiết**: Cấu hình danh sách mô hình cho phát hiện từ khóa và xử lý tiếng nói

```cpp
bool AudioService::IsAfeWakeWord()
```
- **Chức năng**: Kiểm tra xem có sử dụng AFE wake word không
- **Chi tiết**: Trả về true nếu đang sử dụng hệ thống phát hiện từ khóa AFE

```cpp
void AudioService::UpdateOutputTimestamp()
```
- **Chức năng**: Cập nhật thời gian phát âm thanh cuối cùng
- **Chi tiết**: Ghi nhận thời điểm phát âm thanh gần đây nhất