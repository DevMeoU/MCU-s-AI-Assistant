#ifndef MUSIC_H
#define MUSIC_H

#include <string>

#define MAINSTREAM_SEARCH_URL                  "http://search.kuwo.cn/r.s"
#define MAINSTREAM_PLAY_URL                    "http://api.xiaodaokg.com/kuwo.php"
#define MAINSTREAM_LYRIC_URL                   "https://api.xiaodaokg.com/kw/kwlyric.php"

#define BACKDOOR_SEARCH_URL                     "http://www.xiaozhishop.xyz:5005"
#define BACKDOOR_SUB_URL                        "/stream_pcm?song="

class Music {
public:
    virtual ~Music() = default;  // Thêm hàm hủy ảo
    
    virtual bool Download(const std::string& song_name, const std::string& artist_name = "") = 0;
    virtual std::string GetDownloadResult() = 0;
    
    // Thêm phương pháp liên quan đến phát trực tuyến
    virtual bool StartStreaming(const std::string& music_url) = 0;
    virtual bool StopStreaming() = 0;  // Dừng phát trực tuyến
    virtual size_t GetBufferSize() const = 0;
    virtual bool IsDownloading() const = 0;
    virtual int16_t* GetAudioData() = 0;
    
    // Phương pháp tìm kiếm nhạc với fallback
    virtual bool SearchMusic(const std::string& song_name, const std::string& artist_name = "") = 0;
};

#endif // MUSIC_H