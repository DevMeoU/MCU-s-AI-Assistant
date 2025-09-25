# P3 Audio Format Conversion and Playback Tools

This directory contains two Python scripts for processing P3 format audio files:

## 1. Audio Conversion Tool (convert_audio_to_p3.py)

Converts ordinary audio files to P3 format (4-byte header + Opus packet stream structure) and performs loudness normalization.

### Usage

```bash
python convert_audio_to_p3.py <input_audio_file> <output_p3_file> [-l LUFS] [-d]
```

Where the optional `-l` specifies the target loudness for normalization, defaulting to -16 LUFS; the optional `-d` can disable loudness normalization.

If the input audio file meets any of the following conditions, it is recommended to use `-d` to disable loudness normalization:
- The audio is too short
- The audio has already been loudness adjusted
- The audio comes from the default TTS (Xiaozhi's current TTS default loudness is -16 LUFS)

For example:
```bash
python convert_audio_to_p3.py input.mp3 output.p3
```

## 2. P3 Audio Playback Tool (play_p3.py)

Plays P3 format audio files.

### Features

- Decodes and plays P3 format audio files
- Applies fade-out effect at the end of playback or user interruption to avoid popping sounds
- Supports specifying the file to play via command line arguments

### Usage

```bash
python play_p3.py <P3_file_path>
```

For example:
```bash
python play_p3.py output.p3
```

## 3. Audio Reconversion Tool (convert_p3_to_audio.py)

Converts P3 format back to ordinary audio files.

### Usage

```bash
python convert_p3_to_audio.py <input_p3_file> <output_audio_file>
```

The output audio file needs to have an extension.

For example:
```bash
python convert_p3_to_audio.py input.p3 output.wav
```
## 4. Audio/P3 Batch Conversion Tool

A graphical tool that supports batch conversion of audio to P3 and P3 to audio.

![](./img/img.png)

### Usage:
```bash
python batch_convert_gui.py
```

## Dependency Installation

Before using these scripts, please ensure that the required Python libraries are installed:

```bash
pip install librosa opuslib numpy tqdm sounddevice pyloudnorm soundfile
```

Or use the provided requirements.txt file:

```bash
pip install -r requirements.txt
```

## P3 Format Description

P3 format is a simple streaming audio format with the following structure:
- Each audio frame consists of a 4-byte header and an Opus encoded packet
- Header format: [1 byte type, 1 byte reserved, 2 bytes length]
- Sampling rate is fixed at 16000Hz, mono channel
- Each frame duration is 60ms

# Công cụ chuyển đổi và phát định dạng âm thanh P3

Thư mục này chứa hai script Python để xử lý các tệp âm thanh định dạng P3:

## 1. Công cụ chuyển đổi âm thanh (convert_audio_to_p3.py)

Chuyển đổi các tệp âm thanh thông thường sang định dạng P3 (tiêu đề 4 byte + cấu trúc luồng gói Opus) và thực hiện chuẩn hóa độ lớn.

### Cách sử dụng

```bash
python convert_audio_to_p3.py <tệp_âm_thanh_đầu_vào> <tệp_p3_đầu_ra> [-l LUFS] [-d]
```

Trong đó, tùy chọn `-l` dùng để chỉ định độ lớn mục tiêu cho chuẩn hóa độ lớn, mặc định là -16 LUFS; tùy chọn `-d` có thể tắt chuẩn hóa độ lớn.

Nếu tệp âm thanh đầu vào đáp ứng bất kỳ điều kiện nào sau đây, nên sử dụng `-d` để tắt chuẩn hóa độ lớn:
- Âm thanh quá ngắn
- Âm thanh đã được điều chỉnh độ lớn
- Âm thanh đến từ TTS mặc định (độ lớn mặc định của TTS hiện tại của Xiaozhi là -16 LUFS)

Ví dụ:
```bash
python convert_audio_to_p3.py input.mp3 output.p3
```

## 2. Công cụ phát âm thanh P3 (play_p3.py)

Phát các tệp âm thanh định dạng P3.

### Tính năng

- Giải mã và phát các tệp âm thanh định dạng P3
- Áp dụng hiệu ứng mờ dần khi kết thúc phát hoặc khi người dùng ngắt để tránh tiếng bốp
- Hỗ trợ chỉ định tệp để phát qua đối số dòng lệnh

### Cách sử dụng

```bash
python play_p3.py <đường_dẫn_tệp_P3>
```

Ví dụ:
```bash
python play_p3.py output.p3
```

## 3. Công cụ chuyển đổi ngược âm thanh (convert_p3_to_audio.py)

Chuyển đổi định dạng P3 trở lại các tệp âm thanh thông thường.

### Cách sử dụng

```bash
python convert_p3_to_audio.py <tệp_p3_đầu_vào> <tệp_âm_thanh_đầu_ra>
```

Tệp âm thanh đầu ra cần có phần mở rộng.

Ví dụ:
```bash
python convert_p3_to_audio.py input.p3 output.wav
```
## 4. Công cụ chuyển đổi hàng loạt Audio/P3

Một công cụ đồ họa hỗ trợ chuyển đổi hàng loạt âm thanh sang P3 và P3 sang âm thanh.

![](./img/img.png)

### Cách sử dụng:
```bash
python batch_convert_gui.py
```

## Cài đặt phụ thuộc

Trước khi sử dụng các script này, vui lòng đảm bảo rằng các thư viện Python cần thiết đã được cài đặt:

```bash
pip install librosa opuslib numpy tqdm sounddevice pyloudnorm soundfile
```

Hoặc sử dụng tệp requirements.txt được cung cấp:

```bash
pip install -r requirements.txt
```

## Mô tả định dạng P3

Định dạng P3 là một định dạng âm thanh luồng đơn giản với cấu trúc sau:
- Mỗi khung âm thanh bao gồm một tiêu đề 4 byte và một gói được mã hóa Opus
- Định dạng tiêu đề: [1 byte loại, 1 byte dành riêng, 2 byte độ dài]
- Tốc độ lấy mẫu cố định ở 16000Hz, kênh đơn
- Thời lượng mỗi khung là 60ms