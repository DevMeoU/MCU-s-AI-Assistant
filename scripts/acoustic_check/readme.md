# Acoustic Test
This GUI is used to test the `pcm` data returned by the Xiaozhi device via `udp` and convert it to time/frequency domain. It can save sound of window length to determine noise frequency distribution and test the accuracy of acoustic transmission of ASCII.

The firmware test requires `USE_AUDIO_DEBUGGER` to be enabled and `AUDIO_DEBUG_UDP_SERVER` to be set to the local machine address.
The acoustic `demod` can output acoustic tests through `sonic_wifi_config.html` or by uploading to `PinMe`'s [Xiaozhi Acoustic Network Configuration](https://iqf7jnhi.pinit.eth.limo).

# Acoustic Demodulation Test Record

> `✓` means successful decoding when receiving raw PCM signals via I2S DIN, `△` means stable decoding can be achieved with noise reduction or additional operations, `X` means the effect is still poor after noise reduction (may decode partially but very unstable).
> Individual ADCs require more refined noise reduction adjustments during the I2C configuration stage. Since the devices are not universal, testing is currently only performed according to the configurations provided in the boards.

| Device | ADC | MIC | Effect | Notes |
| ---- | ---- | --- | --- | ---- |
| bread-compact | INMP441 | Integrated MEMEMIC | ✓ |
| atk-dnesp32s3-box | ES8311 | | ✓ |
| magiclick-2p5 | ES8311 | | ✓ |
| lichuang-dev | ES7210 | | △ | INPUT_REFERENCE needs to be turned off during testing
| kevin-box-2 | ES7210 | | △ | INPUT_REFERENCE needs to be turned off during testing
| m5stack-core-s3 | ES7210 | | △ | INPUT_REFERENCE needs to be turned off during testing
| xmini-c3 | ES8311 | | △ | Noise reduction required
| atoms3r-echo-base | ES8311 | | △ | Noise reduction required
| atk-dnesp32s3-box0 | ES8311 | | X | Can receive and decode, but packet loss rate is very high
| movecall-moji-esp32s3 | ES8311 | | X | Can receive and decode, but packet loss rate is very high

# Kiểm tra âm thanh
GUI này được sử dụng để kiểm tra dữ liệu `pcm` được thiết bị Xiaozhi trả về qua `udp` và chuyển đổi nó sang miền thời gian/tần số. Nó có thể lưu âm thanh có độ dài cửa sổ để xác định phân bố tần số nhiễu và kiểm tra độ chính xác của việc truyền âm thanh ASCII.

Kiểm tra chương trình cơ sở yêu cầu bật `USE_AUDIO_DEBUGGER` và đặt `AUDIO_DEBUG_UDP_SERVER` thành địa chỉ máy cục bộ.
`demod` âm thanh có thể xuất các kiểm tra âm thanh thông qua `sonic_wifi_config.html` hoặc bằng cách tải lên [Cấu hình mạng âm thanh Xiaozhi](https://iqf7jnhi.pinit.eth.limo) của `PinMe`.

# Bản ghi kiểm tra giải điều chế âm thanh

> `✓` có nghĩa là giải mã thành công khi nhận tín hiệu PCM thô qua I2S DIN, `△` có nghĩa là có thể đạt được giải mã ổn định với giảm nhiễu hoặc các thao tác bổ sung, `X` có nghĩa là hiệu ứng vẫn kém sau khi giảm nhiễu (có thể giải mã một phần nhưng rất không ổn định).
> Các ADC riêng lẻ yêu cầu điều chỉnh giảm nhiễu tinh vi hơn trong giai đoạn cấu hình I2C. Vì các thiết bị không phổ biến, việc kiểm tra hiện chỉ được thực hiện theo các cấu hình được cung cấp trong bảng mạch.

| Thiết bị | ADC | MIC | Hiệu ứng | Ghi chú |
| ---- | ---- | --- | --- | ---- |
| bread-compact | INMP441 | MEMEMIC tích hợp | ✓ |
| atk-dnesp32s3-box | ES8311 | | ✓ |
| magiclick-2p5 | ES8311 | | ✓ |
| lichuang-dev | ES7210 | | △ | Cần tắt INPUT_REFERENCE trong quá trình kiểm tra
| kevin-box-2 | ES7210 | | △ | Cần tắt INPUT_REFERENCE trong quá trình kiểm tra
| m5stack-core-s3 | ES7210 | | △ | Cần tắt INPUT_REFERENCE trong quá trình kiểm tra
| xmini-c3 | ES8311 | | △ | Cần giảm nhiễu
| atoms3r-echo-base | ES8311 | | △ | Cần giảm nhiễu
| atk-dnesp32s3-box0 | ES8311 | | X | Có thể nhận và giải mã, nhưng tỷ lệ mất gói rất cao
| movecall-moji-esp32s3 | ES8311 | | X | Có thể nhận và giải mã, nhưng tỷ lệ mất gói rất cao