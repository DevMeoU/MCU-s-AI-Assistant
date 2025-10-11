#ifndef ESP32_MUSIC_H
#define ESP32_MUSIC_H

#include <string>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <memory>

#include "music.h"
#include <cstring>

// Hỗ trợ bộ giải mã MP3
extern "C" {
#include "mp3dec.h"
}

// Cấu trúc khối dữ liệu âm thanh với quản lý bộ nhớ thông minh
struct AudioChunk {
    std::unique_ptr<uint8_t[]> data;
    size_t size;
    
    AudioChunk() : size(0) {}
    
    // Constructor với dữ liệu từ bộ nhớ được quản lý bên ngoài
    AudioChunk(uint8_t* external_data, size_t s) : size(s) {
        if (external_data && s > 0) {
            data = std::unique_ptr<uint8_t[]>(new uint8_t[s]);
            memcpy(data.get(), external_data, s);
        }
    }
    
    // Constructor chuyển động
    AudioChunk(AudioChunk&& other) noexcept : data(std::move(other.data)), size(other.size) {
        other.size = 0;
    }
    
    // Toán tử gán chuyển động
    AudioChunk& operator=(AudioChunk&& other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
            size = other.size;
            other.size = 0;
        }
        return *this;
    }
    
    // Xóa constructor sao chép và toán tử gán sao chép
    AudioChunk(const AudioChunk&) = delete;
    AudioChunk& operator=(const AudioChunk&) = delete;
};

class Esp32Music : public Music {
public:
    // Điều khiển chế độ hiển thị - Di chuyển đến vùng public
    enum DisplayMode {
        DISPLAY_MODE_SPECTRUM = 0,  // Hiển thị phổ tần mặc định
        DISPLAY_MODE_LYRICS = 1     // Hiển thị lời bài hát
    };

private:
    std::string last_downloaded_data_;
    std::string current_music_url_;
    std::string current_song_name_;
    std::string current_artist_name_;
    bool song_name_displayed_;
    
    // Liên quan đến lời bài hát
    std::string current_lyric_url_;
    std::vector<std::pair<int, std::string>> lyrics_;  // Dấu thời gian và văn bản lời bài hát
    std::mutex lyrics_mutex_;  // Khóa mutex bảo vệ mảng lyrics_
    std::atomic<int> current_lyric_index_;
    std::thread lyric_thread_;
    std::atomic<bool> is_lyric_running_;
    
    std::atomic<DisplayMode> display_mode_;
    std::atomic<bool> is_playing_;
    std::atomic<bool> is_paused_;
    std::atomic<bool> is_downloading_;
    std::thread play_thread_;
    std::thread download_thread_;
    int64_t current_play_time_ms_;  // Thời gian phát hiện tại (mili giây)
    int64_t last_frame_time_ms_;    // Dấu thời gian của khung hình trước
    int total_frames_decoded_;      // Số khung hình đã giải mã

    // Bộ đệm âm thanh với quản lý bộ nhớ cải tiến
    std::queue<AudioChunk> audio_buffer_;
    std::mutex buffer_mutex_;
    std::condition_variable buffer_cv_;
    size_t buffer_size_;
    
    // Tối ưu kích thước bộ đệm cho ESP32-S3 với 8MB PSRAM
    static constexpr size_t MAX_BUFFER_SIZE = 512 * 1024;  // Tăng bộ đệm lên 512KB để tận dụng PSRAM
    static constexpr size_t MIN_BUFFER_SIZE = 64 * 1024;   // Tăng bộ đệm phát tối thiểu lên 64KB
    
    // Liên quan đến bộ giải mã MP3
    HMP3Decoder mp3_decoder_;
    MP3FrameInfo mp3_frame_info_;
    bool mp3_decoder_initialized_;
    
    // Bộ đệm đầu vào MP3 với kích thước tối ưu cho ESP32-S3
    static constexpr size_t MP3_INPUT_BUFFER_SIZE = 32768;  // Tăng lên 32KB để cải thiện hiệu suất

    // Các ngưỡng khôi phục MP3
    static constexpr int MAX_CONSECUTIVE_DECODE_ERRORS = 8; // số lần decode lỗi trước khi thử reset decoder
    static constexpr int MAX_DECODER_RESETS = 3;           // số lần reset decoder trước khi bỏ cuộc
    
    // Dữ liệu FFT với quản lý bộ nhớ
    std::unique_ptr<int16_t[]> final_pcm_data_fft;
    size_t fft_data_size_;
    
    // Âm lượng (0-100)
    std::atomic<int> volume_;
    // Nếu true thì nhạc có quyền ưu tiên (ngăn Application tự động dừng nhạc khi chuyển trạng thái)
    std::atomic<bool> music_has_priority_;

    // Phương thức riêng tư
    void DownloadAudioStream(const std::string& music_url);
    void PlayAudioStream();
    void ClearAudioBuffer();
    bool InitializeMp3Decoder();
    void CleanupMp3Decoder();
    void ResetSampleRate();  // Đặt lại tỷ lệ lấy mẫu về giá trị gốc

    // MP3 recovery helpers
    bool ResyncMp3Stream(uint8_t* base_buffer, uint8_t*& read_ptr, int& bytes_left);
    void ResetMp3Decoder();
    void RecoverFromStreamError();

    // Counters for decoder errors/resets
    std::atomic<int> consecutive_decode_errors_;
    std::atomic<int> decoder_reset_count_;
    
    // Phương thức riêng tư liên quan đến lời bài hát
    bool DownloadLyrics(const std::string& lyric_url);
    bool ParseLyrics(const std::string& lyric_content);
    void LyricDisplayThread();
    void UpdateLyricDisplay(int64_t current_time_ms);
    
    // Xử lý thẻ ID3
    size_t SkipId3Tag(uint8_t* data, size_t size);

public:
    Esp32Music();
    ~Esp32Music();

    virtual bool Download(const std::string& song_name, const std::string& artist_name = "") override;
    virtual std::string GetDownloadResult() override;
    
    // Phương thức mới thêm
    virtual bool StartStreaming(const std::string& music_url) override;
    virtual bool StopStreaming() override;  // Dừng phát trực tuyến
    virtual bool Pause() override;          // Tạm dừng phát trực tuyến
    virtual bool Resume() override;         // Tiếp tục phát trực tuyến
    virtual size_t GetBufferSize() const override { return buffer_size_; }
    virtual bool IsDownloading() const override { return is_downloading_; }
    virtual bool IsPlaying() const override { return is_playing_ && !is_paused_; }
    virtual bool IsPaused() const override { return is_paused_; }
    virtual int16_t* GetAudioData() override { return final_pcm_data_fft.get(); }
    virtual std::string GetCurrentSong() const override { return current_song_name_; }
    virtual void SetVolume(int volume) override;
    virtual int GetVolume() const override { return volume_.load(); }
    
    // Phương thức điều khiển chế độ hiển thị
    void SetDisplayMode(DisplayMode mode);
    DisplayMode GetDisplayMode() const { return display_mode_.load(); }

    // Kiểm tra quyền ưu tiên của nhạc
    bool HasPriority() const override { return music_has_priority_.load(); }
};

#endif // ESP32_MUSIC_H