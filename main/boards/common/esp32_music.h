#ifndef ESP32_MUSIC_H
#define ESP32_MUSIC_H

#include <string>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>

#include "music.h"

// Hỗ trợ bộ giải mã MP3
extern "C" {
#include "mp3dec.h"
}

// Cấu trúc khối dữ liệu âm thanh với quản lý bộ nhớ thông minh
struct AudioChunk {
    std::unique_ptr<uint8_t[]> data;
    size_t size;
    
    AudioChunk() : data(nullptr), size(0) {}
    AudioChunk(uint8_t* d, size_t s) : data(d), size(s) {}
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
    
    // Xử lý luồng âm thanh
    void DownloadAudioStream(const std::string& music_url);
    void PlayAudioStream();
    void ClearAudioBuffer();
    bool InitializeMp3Decoder();
    void CleanupMp3Decoder();
    void ResetSampleRate();  // Đặt lại tỷ lệ mẫu về giá trị gốc
    
    // Phương thức riêng liên quan đến lời bài hát
    bool DownloadLyrics(const std::string& lyric_url);
    bool ParseLyrics(const std::string& lyric_content);
    void LyricDisplayThread();
    void UpdateLyricDisplay(int64_t current_time_ms);
    
    // Xử lý thẻ ID3
    size_t SkipId3Tag(uint8_t* data, size_t size);

    int16_t* final_pcm_data_fft = nullptr;

public:
    Esp32Music();
    ~Esp32Music();

    virtual bool Download(const std::string& song_name, const std::string& artist_name) override;
  
    virtual std::string GetDownloadResult() override;
    
    // Phương thức mới thêm
    virtual bool StartStreaming(const std::string& music_url) override;
    virtual bool StopStreaming() override;  // Dừng phát trực tuyến
    virtual size_t GetBufferSize() const override { return buffer_size_; }
    virtual bool IsDownloading() const override { return is_downloading_; }
    virtual int16_t* GetAudioData() override { return final_pcm_data_fft; }
    
    // Phương thức điều khiển chế độ hiển thị
    void SetDisplayMode(DisplayMode mode);
    DisplayMode GetDisplayMode() const { return display_mode_.load(); }
};

#endif // ESP32_MUSIC_H