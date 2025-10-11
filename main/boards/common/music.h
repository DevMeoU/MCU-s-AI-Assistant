#ifndef MUSIC_H
#define MUSIC_H

#include <string>

class Music {
public:
    virtual ~Music() = default;  // Thêm hàm hủy ảo
    
    virtual bool Download(const std::string& song_name, const std::string& artist_name = "") = 0;
    virtual std::string GetDownloadResult() = 0;
    
    // Thêm các phương thức liên quan đến phát trực tuyến
    virtual bool StartStreaming(const std::string& music_url) = 0;
    virtual bool StopStreaming() = 0;  // Dừng phát trực tuyến
    virtual bool Pause() = 0;          // Tạm dừng phát trực tuyến
    virtual bool Resume() = 0;         // Tiếp tục phát trực tuyến
    virtual size_t GetBufferSize() const = 0;
    virtual bool IsDownloading() const = 0;
    virtual bool IsPlaying() const = 0;   // Kiểm tra xem có đang phát không
    virtual bool IsPaused() const = 0;    // Kiểm tra xem có đang tạm dừng không
    virtual int16_t* GetAudioData() = 0;
    virtual std::string GetCurrentSong() const = 0;  // Lấy tên bài hát hiện tại
    virtual void SetVolume(int volume) = 0;          // Đặt âm lượng (0-100)
    virtual int GetVolume() const = 0;               // Lấy âm lượng hiện tại
    // Kiểm tra quyền ưu tiên của nhạc (mặc định: không có)
    virtual bool HasPriority() const { return false; }
};

#endif // MUSIC_H