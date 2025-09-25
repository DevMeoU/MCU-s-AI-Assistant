# Mixgo_Nova (Yuankong · Youth) Development Board

<img src="https://mixly.cn/public/icon/2024/6/09705006c1c643beb96338791ee1dea0_m.png" alt="Mixgo_Nova" width="200"/>

**[Mixgo_Nova](https://mixly.cn/fredqian/mixgo_nova)** is a multi-functional development board designed for IoT, education, and maker projects. It integrates rich sensors and wireless communication modules, supports graphical programming (Mixly) and offline voice interaction, making it suitable for rapid prototype development and teaching.

---

## 🛠️ Compilation Configuration Commands

**ES8374 CODE MIC acquisition issue:**

```
managed_components\espressif__esp_codec_dev\\device\es8374

static int es8374_config_adc_input(audio_codec_es8374_t *codec, es_adc_input_t input)
{
    int ret = 0;
    int reg = 0;
    ret |= es8374_read_reg(codec, 0x21, &reg);
    if (ret == 0) {
        reg = (reg & 0xcf) | 0x24;
        ret |= es8374_write_reg(codec, 0x21, reg);
    }
    return ret;
}

PS: L386 reg = (reg & 0xcf) | 0x14; should be changed to reg = (reg & 0xcf) | 0x24;
```

**Configure the compilation target as ESP32S3:**

```bash
idf.py set-target esp32s3
```

**Open menuconfig:**

```bash
idf.py menuconfig
```

**Select board:**

```
Xiaozhi Assistant -> Board Type -> Yuankong · Youth
```

**Modify psram configuration:**

```
Component config -> ESP PSRAM -> SPI RAM config -> Mode (QUAD/OCT) -> QUAD Mode PSRAM
```

**Modify Flash configuration:**

```
Serial flasher config -> Flash size -> 8 MB
Partition Table -> Custom partition CSV file -> partitions/v2/8m.csv
```

**Compile:**

```bash
idf.py build
```

**Merge BIN:**

```bash
idf.py merge-bin -o xiaozhi-nova.bin -f raw
```

---

# Bo mạch phát triển Mixgo_Nova (Nguyên Khống · Thanh Xuân)

<img src="https://mixly.cn/public/icon/2024/6/09705006c1c643beb96338791ee1dea0_m.png" alt="Mixgo_Nova" width="200"/>

**[Mixgo_Nova](https://mixly.cn/fredqian/mixgo_nova)** là một bo mạch phát triển đa chức năng được thiết kế dành riêng cho các dự án IoT, giáo dục và sáng tạo. Nó tích hợp nhiều cảm biến phong phú và mô-đun giao tiếp không dây, hỗ trợ lập trình đồ họa (Mixly) và tương tác giọng nói ngoại tuyến, phù hợp cho việc phát triển nguyên mẫu nhanh chóng và giảng dạy.

---

## 🛠️ Lệnh cấu hình biên dịch

**Vấn đề thu thập MIC mã ES8374:**

```
managed_components\espressif__esp_codec_dev\\device\es8374

static int es8374_config_adc_input(audio_codec_es8374_t *codec, es_adc_input_t input)
{
    int ret = 0;
    int reg = 0;
    ret |= es8374_read_reg(codec, 0x21, &reg);
    if (ret == 0) {
        reg = (reg & 0xcf) | 0x24;
        ret |= es8374_write_reg(codec, 0x21, reg);
    }
    return ret;
}

PS: Dòng 386 reg = (reg & 0xcf) | 0x14; nên được thay đổi thành reg = (reg & 0xcf) | 0x24;
```

**Cấu hình mục tiêu biên dịch là ESP32S3:**

```bash
idf.py set-target esp32s3
```

**Mở menuconfig:**

```bash
idf.py menuconfig
```

**Chọn bo mạch:**

```
Trợ lý Xiaozhi -> Loại bo mạch -> Nguyên Khống · Thanh Xuân
```

**Sửa đổi cấu hình psram:**

```
Cấu hình thành phần -> ESP PSRAM -> Cấu hình SPI RAM -> Chế độ (QUAD/OCT) -> Chế độ PSRAM QUAD
```

**Sửa đổi cấu hình Flash:**

```
Cấu hình bộ nạp Flash nối tiếp -> Kích thước Flash -> 8 MB
Bảng phân vùng -> Tệp CSV phân vùng tùy chỉnh -> partitions/v2/8m.csv
```

**Biên dịch:**

```bash
idf.py build
```

**Hợp nhất BIN:**

```bash
idf.py merge-bin -o xiaozhi-nova.bin -f raw
```