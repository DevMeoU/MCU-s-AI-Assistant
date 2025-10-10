# Tổng quan `main/` — MCU-s-AI-Assistant

File này gom toàn bộ nội dung bạn yêu cầu:
- Danh sách và mô tả theo thư mục cho `main/` (các file/nhóm chính với ví dụ file)
- Tóm tắt chuyên sâu `AudioService` (API, luồng, tasks, queues, lưu ý)
- Hướng dẫn build & flash ngắn (ESP-IDF trên Windows `bash.exe`)
- Sơ đồ sequence (ASCII/Markdown) giữa Board, AudioService, Protocol, Server, Display

---

## 1. Entry point

- `main/main.cc`
  - `app_main()` là điểm vào của firmware. Khởi tạo event loop mặc định, NVS, sau đó gọi `Application::GetInstance().Start()`.

- Build metadata / config
  - `main/CMakeLists.txt`, `main/idf_component.yml`, `main/Kconfig.projbuild`
  - Root `CMakeLists.txt` (project-level) gọi IDF project.cmake

---

## 2. Cấu trúc `main/` (mô tả theo thư mục chính và các file tiêu biểu)

Ghi chú: repository có nhiều file; dưới đây là mô tả chi tiết cho các thư mục chức năng chính và ví dụ file tiêu biểu. Nếu muốn, tôi có thể mở rộng mô tả file-by-file.

- `main/application/`
  - `application.h`, `application.cc`
    - Lớp singleton `Application` điều phối toàn bộ luồng ứng dụng: khởi tạo board/display/audio, bắt event loop (`MainEventLoop()`), quản lý trạng thái thiết bị (idle/listening/speaking/upgrading/activating), kiểm tra OTA và assets, chọn `Protocol` (MQTT/WebSocket), phản hồi events từ protocol/audio.
  - Tác vụ chính, event flags, phương thức như `Start()`, `Schedule()`, `SetDeviceState()`, `WakeWordInvoke()`, `UpgradeFirmware()`.

- `main/audio/`
  - `audio_service.h`, `audio_service.cc` — service audio trung tâm (chi tiết ở phần 3).
  - `audio_codec.h/.cc` — abstraction codec (input/output enable, InputData/OutputData, sample rates).
  - `audio_processor.*` (thư mục `processors/`) — AFE / VAD / voice processing (ví dụ `afe_audio_processor.h/.cc`, `no_audio_processor.*`).
  - `wake_words/*` — các backend wake-word (AFE-based, esp-wake-word, custom). Ví dụ: `esp_wake_word.cc`, `afe_wake_word.cc`, `custom_wake_word.cc`.
  - `codecs/*` — mã hoá/driver codec (es8311, es8388, es8374, dummy, box_audio_codec, v.v.).

- `main/protocols/`
  - `protocol.h/.cc` — interface chung cho giao tiếp server (một số helper shared behavior)
  - `mqtt_protocol.h/.cc` — giao thức MQTT: mở kênh audio, gửi/nhận audio, gửi/nhận JSON (tts/stt/llm/mcp/system/alert)
  - `websocket_protocol.h/.cc` — implementation qua WebSocket

- `main/ota/`
  - `ota.h/.cc` — kiểm tra phiên bản, download firmware, activation code flow, mark version valid.

- `main/display/`
  - `display.h/.cc` — abstraction hiển thị (chat message, emotion, status bar)
  - drivers: `oled_display.h/.cc`, `lcd_display.h/.cc`, `emote_display.*`, `lvgl_display/*` (LVGL integration)

- `main/boards/` (nhiều subfolders per-board)
  - Mỗi board có `config.json` và driver C++ (ví dụ `xmini_c3_board.cc`, `lilygo-t-circle-s3.cc`, v.v.).
  - Board exposes: audio codec instance, display, led, network helpers, power manager, pin mappings.

- `main/mcp/`
  - `mcp_server.h/.cc` — MCP (device control protocol) tools, đăng ký command/tool, parse MCP messages.

- `main/assets/`
  - `assets.h/.cc`, `assets/locales/*` — tệp âm thanh .ogg, language packs, font packs. Assets partition handling & application of downloaded assets.

- `main/device/`
  - `device_state.h`, `device_state_event.h/.cc` — enums và event manager để broadcast device state changes.

- `main/led/`
  - LED handling: `single_led.h/.cc`, `gpio_led.*`, `circular_strip.*`.

- `main/system/`
  - `system_info/*` — heap/task stats and user agent string
  - `setting/*` — wrapper lưu setting (NVS/partition)

- `main/boards/common/` và `main/display/lvgl_display/`
  - Các shared helper (power manager, backlight, i2c devices, camera helpers, LVGL fonts, gif, jpeg helpers).

- Build / metadata files
  - `main/CMakeLists.txt` — list các source file của component, đảm bảo `audio/audio_service.cc` được build.
  - `main/idf_component.yml` — metadata component

- Assets lớn
  - `main/assets/locales/*/*.ogg` — nhiều file .ogg cho các ngôn ngữ / sound effects (activation, welcome, upgrade, 0..9 sound bytes, etc.).

---

## 3. Tóm tắt chuyên sâu: `AudioService` (đã đọc `audio_service.h` và `audio_service.cc`)

Mục tiêu: xử lý luồng âm thanh đầu vào (mic → processor → opus encode → send) và đầu ra (server opus → decode → resample → speaker). Hỗ trợ wake-word, VAD, audio testing, device-AEC hoặc server-AEC.

1) Kiến trúc high-level
- Thành phần chính:
  - `AudioCodec* codec_` — layer hardware abstraction (input/output enable, InputData/OutputData, sample rates).
  - `AudioProcessor` — AFE / VAD / voice-processing (ví dụ `AfeAudioProcessor` hoặc `NoAudioProcessor`).
  - `WakeWord` — backend wake-word (AFE/C-model/esp model) với callback on detected.
  - `OpusEncoderWrapper` / `OpusDecoderWrapper` — encode/decode Opus packets.
  - Resamplers: `OpusResampler input_resampler_, reference_resampler_, output_resampler_`.
  - Debug: `AudioDebugger` (conditional compile `CONFIG_USE_AUDIO_DEBUGGER`).

2) Task / Event model
- Tasks (FreeRTOS):
  - `AudioInputTask()` — đọc PCM từ codec, chuyển dữ liệu vào wake-word / audio processor / audio testing flow.
  - `AudioOutputTask()` — lấy audio playback tasks và gọi `codec_->OutputData()`
  - `OpusCodecTask()` — xử lý encode (PCM -> Opus -> push send queue) và decode (Opus -> PCM -> playback queue)
- Event group bits: `AS_EVENT_AUDIO_TESTING_RUNNING`, `AS_EVENT_WAKE_WORD_RUNNING`, `AS_EVENT_AUDIO_PROCESSOR_RUNNING`, `AS_EVENT_PLAYBACK_NOT_EMPTY`.

3) Queues và data structures
- `audio_encode_queue_` (AudioTask) — PCM frames chờ được opus encode
- `audio_send_queue_` (AudioStreamPacket) — opus packets sẵn sàng gửi lên server
- `audio_decode_queue_` — opus packets nhận từ server chờ decode
- `audio_playback_queue_` — PCM frames/chunks chờ playback
- `audio_testing_queue_` — cho chế độ audio testing (lưu opus frames)
- `timestamp_queue_` — dùng cho server-side AEC (ghi timestamp của output để server đồng bộ)

4) Luồng dữ liệu chính
- MIC → codec_->InputData() → (resample nếu cần) → audio_processor_->Feed(...) → audio_processor_ emits output → PushTaskToEncodeQueue(kAudioTaskTypeEncodeToSendQueue)
- OpusCodecTask picks encode task → opus_encoder_->Encode → push to `audio_send_queue_` → `Application`/`Protocol` gọi `PopPacketFromSendQueue()` và gửi packet.

- Server → Protocol nhận opus payload → tạo `AudioStreamPacket` → `PushPacketToDecodeQueue()` → OpusCodecTask decodes → resample nếu cần → push to `audio_playback_queue_` → AudioOutputTask calls `codec_->OutputData()`

- Wake word: wake_word_->Feed(data) inside AudioInputTask when wake-word event bit set; when detected callback triggers `callbacks_.on_wake_word_detected`. `EncodeWakeWord()` allows encoding and `PopWakeWordPacket()` returns an opus packet of the wake-word data for sending.

5) Special features & power management
- Audio power timer (`audio_power_timer_`) sẽ check last input/output timestamps and disable input/output to save power after inactivity (`AUDIO_POWER_TIMEOUT_MS`).
- Audio testing mode: collects many frames into `audio_testing_queue_` for debugging.
- Device AEC: `EnableDeviceAec(true/false)` toggles AEC on audio processor.
- Server AEC: `timestamp_queue_` is used so playback timestamps accompany outgoing packets for server-side echo cancellation.

6) Public API (high level):
- `Initialize(AudioCodec* codec)` — init codec, opus encoder/decoder, resamplers, audio_processor
- `Start()`, `Stop()` — start/stop tasks
- `SetModelsList(srmodel_list_t* models_list)` — set wake-word models and initialize wake_word_ accordingly
- `EnableWakeWordDetection(bool)`, `EnableVoiceProcessing(bool)`, `EnableAudioTesting(bool)`, `EnableDeviceAec(bool)`
- `PushPacketToDecodeQueue(...)` — push incoming opus packet to decode queue
- `PopPacketFromSendQueue()` — pop an opus packet ready to send to server
- `EncodeWakeWord()`, `PopWakeWordPacket()`, `GetLastWakeWord()`

7) Points-to-watch (potential pitfalls when modifying):
- Concurrency: multiple queues guarded by `audio_queue_mutex_` plus `audio_queue_cv_` condition variable. Push/pop must respect size limits constants.
- Blocking/wait: `PushPacketToDecodeQueue(... wait=true)` can block; avoid deadlocks in caller.
- Resampling: codec input/output sample rates may differ; resampler config must match.
- Timestamps for server AEC: timestamp queue size and synchronization critical.
- Memory: opus encoder/decoder and large playback buffers increase RAM; OpusCodec task stack size is large (2048*13) for a reason.

---

## 4. Hướng dẫn build & flash (ngắn, cho Windows using `bash.exe`)

Lưu ý: dự án dùng ESP-IDF. Các bước dưới đây giả định bạn đã cài ESP-IDF (IDF v5.x được khuyến nghị trong README) và `IDF_PATH` đã thiết lập.

1) Mở Bash (Git Bash hoặc MSYS2/WSL/bash.exe) mà bạn dùng cho ESP-IDF.

2) Source ESP-IDF export script (nếu dùng Git Bash / msys):

```bash
. $IDF_PATH/export.sh
```

(hoặc, nếu dùng PowerShell/CMD bạn sẽ dùng `export.bat`/`export.ps1` — nhưng bạn yêu cầu `bash.exe` nên dùng `export.sh`.)

3) Đi tới thư mục dự án và build:

```bash
cd /e/Workspace/project/MCU-s-AI-Assistant
idf.py build
```

4) Flash vào thiết bị (ví dụ cổng COM3). Trên môi trường bash, tên port có thể là `/dev/ttyUSB0` (WSL) hoặc `COM3` (minicom via msys); ví dụ cho Windows / Git Bash bạn có thể dùng `COM3`:

```bash
idf.py -p COM3 flash monitor
```

Hoặc nếu thiết bị trên WSL/tty:

```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

5) Một số lệnh hữu ích:
- `idf.py set-target esp32s3` — thay đổi target nếu cần trước khi build (tuỳ target trong repo)
- `idf.py menuconfig` — chỉnh `sdkconfig`
- `idf.py -p COM3 flash` — flash mà không mở monitor
- `idf.py -p COM3 monitor` — mở serial monitor

---

## 5. Sơ đồ sequence (Text/ASCII) — wake-word → capture → send → TTS → play/display

Dưới đây là sơ đồ tuần tự đơn giản mô tả flow chính:

```
User (speaks) -> Board (mic) -> AudioCodec(InputData) -> AudioService.AudioInputTask
AudioService -> AudioProcessor (VAD/AEC) -> PushTaskToEncodeQueue
AudioService.OpusCodecTask -> OpusEncoder -> audio_send_queue
Application / Protocol => PopPacketFromSendQueue -> Protocol.SendAudio() -> Server

Server (LLM/STT/TTS) -> (via MQTT/WebSocket) -> Protocol.OnIncomingAudio/OnIncomingJson
Protocol -> Application (OnIncomingAudio) -> Application pushes to AudioService.PushPacketToDecodeQueue
AudioService.OpusCodecTask -> OpusDecoder -> audio_playback_queue -> AudioOutputTask -> codec->OutputData -> Speaker
Application (OnIncomingJson[type=tts/llm/stt]) -> Display.SetChatMessage / Display.SetEmotion

Wake-word path:
AudioInputTask (wake_word feed) -> wake_word_ detects -> callbacks_.on_wake_word_detected -> Application::OnWakeWordDetected -> Application toggles listening / protocol send wake event

Power management:
AudioService tracks last_input_time_ / last_output_time_ -> audio_power_timer -> disable codec input/output when idle
```

---

## 6. Nội dung file đã tạo & nơi lưu

Tôi đã lưu toàn bộ nội dung này vào file Markdown:

- `docs/main_overview.md`  — mô tả, tóm tắt `AudioService`, build/flash, sơ đồ sequence.

---

## 7. Next steps (gợi ý)

- Muốn tôi generate danh sách file `main/` từng file một (full list) trong file text (ví dụ `docs/main_file_list.txt`)? Tôi có thể export toàn bộ danh sách đường dẫn + kích thước (nhiều file ~1000).
- Muốn tôi tóm tắt thêm 1 module cụ thể: `protocols/mqtt_protocol.cc`, `display/oled_display.cc` hoặc một board driver cụ thể?
- Muốn tôi thêm hướng dẫn debug memory/stack (ESP-IDF monitor commands) hoặc một checklist build matrix cho nhiều targets (esp32s3/p4/c3)?

Nói "export files" để tôi xuất toàn bộ danh sách file, hoặc ghi tên module để tôi tóm tắt tiếp.

---

Hoàn tất — nếu bạn muốn, tôi sẽ mở file `docs/main_overview.md` trong workspace hoặc chỉnh format/độ chi tiết tiếp theo.