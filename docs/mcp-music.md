# MCP Music — Hướng dẫn tích hợp điều khiển nhạc qua MCP

Tài liệu này mô tả cách mở rộng MCP (Model Context Protocol) trên thiết bị để hỗ trợ điều khiển nhạc (phát/đệm/dừng/volume/playlist). Bao gồm:

- Thiết kế các "tool" MCP cho chức năng nhạc
- JSON-RPC ví dụ (tools/list, tools/call)
- Mẫu C++ để đăng ký tool với `McpServer::AddTool`
- Tích hợp với `AudioService` (phát âm thanh cục bộ / TTS) và `Display` (hiển thị thông tin bài hát)
- Bảo mật, kiểm thử và ví dụ server-side đơn giản

> Dựa trên nội dung tài liệu `mcp-protocol.md` và `mcp-usage.md` trong thư mục `docs/` của dự án.

## 1. Mục tiêu
Mục tiêu của extension này là cho phép backend (hoặc 1 ứng dụng điều khiển) gọi các chức năng nhạc trên thiết bị ESP32 thông qua MCP. Các hành vi chính:

- Phát 1 file nhạc có sẵn trong `assets` hoặc từ URL
- Dừng phát
- Chuyển bài (next/prev)
- Quản lý playlist (thêm/xóa/danh sách)
- Set/get volume
- Lấy trạng thái và metadata hiện tại

## 2. Thiết kế tool MCP
Gợi ý đặt tên tool (có form "module.action"):
- `music.play` — phát một bài (bộ args: { "source": "asset" | "url", "path": "assets/.../song.ogg" | "http://..." , "position_ms": optional })
- `music.stop` — dừng phát (no args)
- `music.pause` — tạm dừng (no args)
- `music.resume` — tiếp tục (no args)
- `music.next` / `music.prev` — chuyển bài
- `music.set_volume` — { "volume": int (0-100) }
- `music.get_volume` — returns current volume
- `music.get_state` — trả về trạng thái hiện tại: { state: "playing|paused|stopped", track: {...}, position_ms: ... }
- `music.playlist.add` — { "track": { "id":"..", "source":"asset|url", "path":"...", "title":"..", "artist":".." } }
- `music.playlist.remove` — { "track_id": "..." }
- `music.playlist.list` — returns array of tracks

### Properties data types (PropertyList)
Sử dụng các kiểu chuẩn (bool, integer, string). Ví dụ `Property("volume", kPropertyTypeInteger, 0, 100)`.

## 3. JSON-RPC examples
### 3.1 Gọi `music.play` (asset)
```json
{
  "jsonrpc": "2.0",
  "method": "tools/call",
  "params": {
    "name": "music.play",
    "arguments": {
      "source": "asset",
      "path": "assets/locales/en-US/1.ogg",
      "position_ms": 0
    }
  },
  "id": 101
}
```

### 3.2 Gọi `music.play` (url)
```json
{
  "jsonrpc": "2.0",
  "method": "tools/call",
  "params": {
    "name": "music.play",
    "arguments": {
      "source": "url",
      "path": "https://example.com/song.opus"
    }
  },
  "id": 102
}
```

### 3.3 Set volume
```json
{
  "jsonrpc": "2.0",
  "method": "tools/call",
  "params": {
    "name": "music.set_volume",
    "arguments": { "volume": 75 }
  },
  "id": 103
}
```

### 3.4 Get state
```json
{
  "jsonrpc": "2.0",
  "method": "tools/call",
  "params": {
    "name": "music.get_state",
    "arguments": {}
  },
  "id": 104
}
```

## 4. Mẫu C++ để đăng ký tools (device-side)
Đặt đoạn mã này vào module khởi tạo (ví dụ `boards/common` hoặc `application` init).

```cpp
#include "mcp/mcp_server.h"
#include "audio/audio_service.h"
#include "display/display.h"

void RegisterMusicTools() {
    auto& mcp = McpServer::GetInstance();
    auto& audio = AudioService::GetInstance();
    auto& display = Display::GetInstance();

    // music.play
    mcp.AddTool("music.play", "Play a track from asset or URL",
        PropertyList({
            Property("source", kPropertyTypeString),
            Property("path", kPropertyTypeString),
            Property("position_ms", kPropertyTypeInteger, 0, INT32_MAX)
        }),
        [&audio, &display](const PropertyList& props) -> ReturnValue {
            std::string source = props["source"].value<std::string>();
            std::string path = props["path"].value<std::string>();
            int pos = 0;
            if (props.contains("position_ms")) {
                pos = props["position_ms"].value<int>();
            }

            if (source == "asset") {
                // đơn giản: sử dụng AudioService::PlaySound(path)
                audio.PlaySound(path);
            } else if (source == "url") {
                // nếu hỗ trợ streaming từ URL, cần downloader hoặc stream playback
                audio.PlayStreamFromUrl(path);
            } else {
                return ReturnValue::Error("invalid_source");
            }

            // cập nhật display
            display.ShowTrackInfo(path);
            return true;
        }
    );

    // music.stop
    mcp.AddTool("music.stop", "Stop playback", PropertyList(), [&audio](const PropertyList&) -> ReturnValue {
        audio.StopPlayback();
        return true;
    });

    // music.set_volume
    mcp.AddTool("music.set_volume", "Set playback volume",
        PropertyList({ Property("volume", kPropertyTypeInteger, 0, 100) }),
        [&audio](const PropertyList& props) -> ReturnValue {
            int v = props["volume"].value<int>();
            audio.SetVolume(v);
            return true;
        }
    );

    // music.get_state
    mcp.AddTool("music.get_state", "Get playback state", PropertyList(), [&audio]() -> ReturnValue {
        auto state = audio.GetPlaybackState(); // giả sử trả struct/JSON serializable
        return state;
    });
}
```

> Lưu ý: Mã trên là mẫu; tên hàm `PlayStreamFromUrl`, `PlaySound`, `StopPlayback`, `SetVolume`, `GetPlaybackState`, `ShowTrackInfo` có thể cần được hiện thực trong `AudioService` / `Display` nếu chưa có.

## 5. Tích hợp với `AudioService` và `Display`
- AudioService: nếu chưa có API phát asset/stream, bạn nên thêm các phương thức sau:
  - `bool PlaySound(const std::string& asset_path);` — tìm file trong partition assets và phát bằng audio output task.
  - `bool PlayStreamFromUrl(const std::string& url);` — tải/stream dữ liệu Opus/PCM rồi phát (cần có buffer streaming và decoder nếu là opus/raw).
  - `void StopPlayback();`
  - `void PausePlayback();`
  - `void ResumePlayback();`
  - `void SetVolume(int volume);`
  - `PlaybackState GetPlaybackState();` — struct chứa { state, current_track, position_ms, duration_ms }

- Display: cần phương thức để hiển thị metadata (title/artist/position). Ví dụ `void ShowTrackInfo(const std::string& path_or_title);`.

## 6. Bảo mật và xác thực
- MCP giao tiếp qua kênh transport hiện có (WebSocket hoặc MQTT). Hãy đảm bảo:
  - WebSocket hoặc MQTT được kết nối qua TLS (TLS cho MQTT: 8883; WebSocket: wss://)
  - Xác thực token/authorization header hợp lệ
  - Khi cung cấp URL để stream từ nguồn bên ngoài, kiểm tra domain hoặc whitelist để tránh SSRF.

## 7. Kiểm thử
1. Unit test (logic):
   - Tạo mock `AudioService` và `Display`, đăng ký tools, gọi `McpServer::HandleRequest` giả lập `tools/call` JSON và assert kết quả.
2. Tích hợp: chạy firmware trên board, dùng server-side gọi `tools/call` qua MCP (trên WebSocket hoặc MQTT) để gọi `music.play` với `asset` và `url`.
3. Edge cases:
   - File không tồn tại -> trả về Error
   - Volume out of bounds -> trả về Error
   - Stream disconnect giữa chừng -> báo trạng thái "stopped" và trả event MCP (nếu cần)

## 8. Ví dụ server-side (pseudo)
- Server có thể dùng MCP client để gọi `tools/call` tương tự các ví dụ JSON-RPC ở phần 3. Khi backend muốn phát 1 asset mặc định, gửi `music.play` với `source: "asset"`.

## 9. Event / Notifications (Optional)
Thiết bị có thể gửi events (notifications) về server khi trạng thái thay đổi: ví dụ publish MCP event `music/state_changed` (cách gửi) hoặc gửi JSON qua kênh control:

```json
{ "type": "music_state", "session_id": "...", "state": "playing", "track": {"id":"...","title":"..."}, "position_ms": 12345 }
```

## 10. Triển khai & rollout
- Bước 1: Triển khai code mẫu lên 1 board dev
- Bước 2: Kiểm thử asset playback
- Bước 3: Thêm stream URL support (nếu cần), test latency và buffering
- Bước 4: Thu thập feedback và tối ưu memory/stack (Esp32 có hạn chế bộ nhớ)

---

Nếu bạn muốn, tôi có thể tiếp tục:
- Thêm file mẫu `examples/mcp_music_example.cpp` vào `examples/` (non-breaking) để dễ tham chiếu.
- Chỉnh `docs/main_overview.md` để thêm link tới `docs/mcp-music.md` và ví dụ flash cho board mà bạn đang dùng (nêu board tên nào tôi sẽ chèn lệnh `idf.py -p /dev/ttySx flash`).
- Viết unit-tests giả lập cho `McpServer::AddTool` (C++ gtest) hoặc tạo mock runtime.

Hãy cho biết bước tiếp theo bạn muốn tôi làm.