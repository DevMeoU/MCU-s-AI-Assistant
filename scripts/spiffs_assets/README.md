# SPIFFS Assets Builder

This script is used to build the SPIFFS resource partition for ESP32 projects, packaging various resource files into a format that can be used on the device.

## Features / Tính năng

- Process WakeNet Model / Xử lý mô hình WakeNet
- Integrate text font files / Tích hợp tệp phông chữ văn bản
- Process emoji image collections / Xử lý bộ sưu tập hình ảnh biểu tượng cảm xúc
- Automatically generate resource index files / Tự động tạo tệp chỉ mục tài nguyên
- Package and generate the final `assets.bin` file / Đóng gói và tạo tệp `assets.bin` cuối cùng

## Dependencies / Yêu cầu phụ thuộc

- Python 3.6+
- Related resource files / Các tệp tài nguyên liên quan

## Usage / Cách sử dụng

### Basic Syntax / Cú pháp cơ bản

```bash
./build.py --wakenet_model <wakenet_model_dir> \
    --text_font <text_font_file> \
    --emoji_collection <emoji_collection_dir>
```

### Parameter Description / Mô tả tham số

| Parameter / Tham số | Type / Kiểu | Required / Bắt buộc | Description / Mô tả |
|------|------|------|------|
| `--wakenet_model` | Directory Path / Đường dẫn thư mục | No / Không | WakeNet model directory path / Đường dẫn thư mục mô hình WakeNet |
| `--text_font` | File Path / Đường dẫn tệp | No / Không | Text font file path / Đường dẫn tệp phông chữ văn bản |
| `--emoji_collection` | Directory Path / Đường dẫn thư mục | No / Không | Emoji image collection directory path / Đường dẫn thư mục bộ sưu tập hình ảnh biểu tượng cảm xúc |

### Usage Example / Ví dụ sử dụng

```bash
# Full parameter example / Ví dụ tham số đầy đủ
./build.py \
    --wakenet_model ../../managed_components/espressif__esp-sr/model/wakenet_model/wn9_nihaoxiaozhi_tts \
    --text_font ../../components/xiaozhi-fonts/build/font_puhui_common_20_4.bin \
    --emoji_collection ../../components/xiaozhi-fonts/build/emojis_64/

# Only process font files / Chỉ xử lý tệp phông chữ
./build.py --text_font ../../components/xiaozhi-fonts/build/font_puhui_common_20_4.bin

# Only process emojis / Chỉ xử lý biểu tượng cảm xúc
./build.py --emoji_collection ../../components/xiaozhi-fonts/build/emojis_64/
```

## Workflow / Quy trình làm việc

1. **Create build directory structure** / **Tạo cấu trúc thư mục xây dựng**
   - `build/` - Main build directory / Thư mục xây dựng chính
   - `build/assets/` - Resource file directory / Thư mục tệp tài nguyên
   - `build/output/` - Output file directory / Thư mục tệp đầu ra

2. **Process WakeNet Model** / **Xử lý mô hình WakeNet**
   - Copy model files to build directory / Sao chép tệp mô hình vào thư mục xây dựng
   - Use `pack_model.py` to generate `srmodels.bin` / Sử dụng `pack_model.py` để tạo `srmodels.bin`
   - Copy generated model files to resource directory / Sao chép tệp mô hình đã tạo vào thư mục tài nguyên

3. **Process text fonts** / **Xử lý phông chữ văn bản**
   - Copy font files to resource directory / Sao chép tệp phông chữ vào thư mục tài nguyên
   - Supports `.bin` format font files / Hỗ trợ tệp phông chữ định dạng `.bin`

4. **Process emoji collection** / **Xử lý bộ sưu tập biểu tượng cảm xúc**
   - Scan image files in the specified directory / Quét các tệp hình ảnh trong thư mục được chỉ định
   - Supports `.png` and `.gif` formats / Hỗ trợ định dạng `.png` và `.gif`
   - Automatically generate emoji index / Tự động tạo chỉ mục biểu tượng cảm xúc

5. **Generate configuration files** / **Tạo tệp cấu hình**
   - `index.json` - Resource index file / Tệp chỉ mục tài nguyên
   - `config.json` - Build configuration file / Tệp cấu hình xây dựng

6. **Package final resources** / **Đóng gói tài nguyên cuối cùng**
   - Use `spiffs_assets_gen.py` to generate `assets.bin` / Sử dụng `spiffs_assets_gen.py` để tạo `assets.bin`
   - Copy to build root directory / Sao chép vào thư mục gốc xây dựng

## Output Files / Tệp đầu ra

After building, the following files will be generated in the `build/` directory:
Sau khi xây dựng, các tệp sau sẽ được tạo trong thư mục `build/`:

- `assets/` - All resource files / Tất cả các tệp tài nguyên
- `assets.bin` - Final SPIFFS resource file / Tệp tài nguyên SPIFFS cuối cùng
- `config.json` - Build configuration / Cấu hình xây dựng
- `output/` - Intermediate output files / Tệp đầu ra trung gian

## Supported Resource Formats / Định dạng tài nguyên được hỗ trợ

- **Model files**: `.bin` (processed by pack_model.py) / Tệp mô hình: `.bin` (được xử lý bởi pack_model.py)
- **Font files**: `.bin` / Tệp phông chữ: `.bin`
- **Image files**: `.png`, `.gif` / Tệp hình ảnh: `.png`, `.gif`
- **Configuration files**: `.json` / Tệp cấu hình: `.json`

## Error Handling / Xử lý lỗi

The script includes a comprehensive error handling mechanism:
Script bao gồm một cơ chế xử lý lỗi toàn diện:

- Check if source files/directories exist / Kiểm tra xem tệp/thư mục nguồn có tồn tại không
- Validate subprocess execution results / Xác thực kết quả thực thi của tiến trình con
- Provide detailed error messages and warnings / Cung cấp thông báo lỗi và cảnh báo chi tiết

## Notes / Lưu ý

1. Ensure all dependent Python scripts are in the same directory / Đảm bảo tất cả các script Python phụ thuộc đều nằm trong cùng một thư mục
2. Resource file paths use absolute paths or paths relative to the script directory / Đường dẫn tệp tài nguyên sử dụng đường dẫn tuyệt đối hoặc đường dẫn tương đối so với thư mục script
3. The build process will clean up previous build files / Quá trình xây dựng sẽ dọn dẹp các tệp xây dựng trước đó
4. The size of the generated `assets.bin` file is limited by the SPIFFS partition size / Kích thước của tệp `assets.bin` được tạo ra bị giới hạn bởi kích thước phân vùng SPIFFS
