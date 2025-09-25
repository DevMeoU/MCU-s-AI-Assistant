# LVGL Image Conversion Tool

This directory contains two Python scripts for processing and converting images to LVGL format:

## 1. LVGLImage (LVGLImage.py)

Referenced from LVGL's [official repo](https://github.com/lvgl/lvgl) conversion script [LVGLImage.py](https://github.com/lvgl/lvgl/blob/master/scripts/LVGLImage.py)

## 2. LVGL Image Conversion Tool (lvgl_tools_gui.py)

Calls `LVGLImage.py` to batch convert images to LVGL image format.
Can be used to modify Xiaozhi's default expressions. The specific modification tutorial is [here](https://www.bilibili.com/video/BV12FQkYeEJ3/)

### Features

- Graphical operation, more user-friendly interface
- Supports batch conversion of images
- Automatically identifies image formats and selects the best color format for conversion
- Multi-resolution support

### Usage

Create a virtual environment
```bash
# Create venv
python -m venv venv
# Activate environment
source venv/bin/activate  # Linux/Mac
venc\Scripts\activate      # Windows
```

Install dependencies
```bash
pip install -r requirements.txt
```

Run the conversion tool

```bash
# Activate environment
source venv/bin/activate  # Linux/Mac
venc\Scripts\activate      # Windows
# Run
python lvgl_tools_gui.py
```

# Công cụ chuyển đổi hình ảnh LVGL

Thư mục này chứa hai tập lệnh Python để xử lý và chuyển đổi hình ảnh sang định dạng LVGL:

## 1. LVGLImage (LVGLImage.py)

Tham chiếu từ tập lệnh chuyển đổi [LVGLImage.py](https://github.com/lvgl/lvgl/blob/master/scripts/LVGLImage.py) của [kho lưu trữ chính thức](https://github.com/lvgl/lvgl) của LVGL

## 2. Công cụ chuyển đổi hình ảnh LVGL (lvgl_tools_gui.py)

Gọi `LVGLImage.py` để chuyển đổi hàng loạt hình ảnh sang định dạng hình ảnh LVGL.
Có thể được sử dụng để sửa đổi các biểu cảm mặc định của Xiaozhi. Hướng dẫn sửa đổi cụ thể có [tại đây](https://www.bilibili.com/video/BV12FQkYeEJ3/)

### Tính năng

- Thao tác đồ họa, giao diện thân thiện hơn với người dùng
- Hỗ trợ chuyển đổi hàng loạt hình ảnh
- Tự động nhận dạng định dạng hình ảnh và chọn định dạng màu tốt nhất để chuyển đổi
- Hỗ trợ đa độ phân giải

### Cách sử dụng

Tạo môi trường ảo
```bash
# Tạo venv
python -m venv venv
# Kích hoạt môi trường
source venv/bin/activate  # Linux/Mac
venc\Scripts\activate      # Windows
```

Cài đặt các phụ thuộc
```bash
pip install -r requirements.txt
```

Chạy công cụ chuyển đổi

```bash
# Kích hoạt môi trường
source venv/bin/activate  # Linux/Mac
venc\Scripts\activate      # Windows
# Chạy
python lvgl_tools_gui.py
```
