# DFRobot K10 Board

## Button Configuration
* A: Short press - interrupt/wake up, long press 1s - increase volume
* B: Short press - interrupt/wake up, long press 1s - decrease volume

## Compilation Configuration Commands

**Configure the compilation target to ESP32S3:**

```bash
idf.py set-target esp32s3
```

**Open menuconfig:**

```bash
idf.py menuconfig
```

**Select Board:**

```
Xiaozhi Assistant -> Board Type -> DFRobot K10 Board
```

**Modify psram configuration:**

```
Component config -> ESP PSRAM -> SPI RAM config -> Mode (QUAD/OCT) -> Octal Mode PSRAM
```

**Compile:**

```bash
idf.py build
```

---

# Bo mạch DFRobot K10

## Cấu hình nút
* A: Nhấn nhanh - ngắt/đánh thức, nhấn giữ 1s - tăng âm lượng
* B: Nhấn nhanh - ngắt/đánh thức, nhấn giữ 1s - giảm âm lượng

## Lệnh cấu hình biên dịch

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
Xiaozhi Assistant -> Board Type -> DFRobot K10 Board
```

**Sửa đổi cấu hình psram:**

```
Component config -> ESP PSRAM -> SPI RAM config -> Mode (QUAD/OCT) -> Octal Mode PSRAM
```

**Biên dịch:**

```bash
idf.py build
```