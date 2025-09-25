# Mainboard Open Source Address:
- V1:[https://oshwhub.com/wdmomo/esp32-xiaozhi-kidpcb](https://oshwhub.com/wdmomo/esp32-xiaozhi-kidpcb)
- V2:[https://oshwhub.com/wdmomo/esp32-xiaozhi-kidpcb_copy](https://oshwhub.com/wdmomo/esp32-xiaozhi-kidpcb_copy)
- More Introduction: [wdmomo.fun](https://www.wdmomo.fun:81/doc/index.html?file=001_%E8%AE%BE%E8%AE%A1%E9%A1%B9%E7%9B%AE/0001_%E5%B0%8F%E6%99%BAAI/002_ESP32-CGC%E5%BC%80%E5%8F%91%E6%9D%90%E5%B0%8F%E6%99%BAAI)

# Compilation Configuration Commands

**Configure the compilation target to ESP32:**

```bash
idf.py set-target esp32
```

**Open menuconfig:**

```bash
idf.py menuconfig
```

**Select Board:**

```
Xiaozhi Assistant -> Board Type -> ESP32 CGC
```

**Select Screen Type:**

```
Xiaozhi Assistant -> LCD Type -> "ST7735, Resolution 128*128"
```

**Compile:**

```bash
idf.py build
```

---

# Địa chỉ mã nguồn mở của bo mạch chủ:
- V1:[https://oshwhub.com/wdmomo/esp32-xiaozhi-kidpcb](https://oshwhub.com/wdmomo/esp32-xiaozhi-kidpcb)
- V2:[https://oshwhub.com/wdmomo/esp32-xiaozhi-kidpcb_copy](https://oshwhub.com/wdmomo/esp32-xiaozhi-kidpcb_copy)
- Giới thiệu thêm: [wdmomo.fun](https://www.wdmomo.fun:81/doc/index.html?file=001_%E8%AE%BE%E8%AE%A1%E9%A1%B9%E7%9B%AE/0001_%E5%B0%8F%E6%99%BAAI/002_ESP32-CGC%E5%BC%80%E5%8F%91%E6%9D%90%E5%B0%8F%E6%99%BAAI)

# Lệnh cấu hình biên dịch

**Cấu hình mục tiêu biên dịch là ESP32:**

```bash
idf.py set-target esp32
```

**Mở menuconfig:**

```bash
idf.py menuconfig
```

**Chọn bo mạch:**

```
Xiaozhi Assistant -> Board Type -> ESP32 CGC
```

**Chọn loại màn hình:**

```
Xiaozhi Assistant -> LCD Type -> "ST7735, Độ phân giải 128*128"
```

**Biên dịch:**

```bash
idf.py build
```
