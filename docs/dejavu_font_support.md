# DejaVu Font Support for Vietnamese Language

This document explains how to use the DejaVu Sans font for Vietnamese language support in the MCU's AI Assistant project.

## Overview

The project now supports using the DejaVu Sans font (dejavu-sans.zip) for better Vietnamese character rendering. This font provides comprehensive support for Vietnamese diacritical marks and characters.

## Font Location

The DejaVu Sans font is located at:
```
main/assets/fonts/dejavu-sans.zip
```

## How It Works

1. When the language is set to Vietnamese (vi-VN), the build system automatically selects the DejaVu Sans font
2. During the build process, the font ZIP file is converted to binary format using `lv_font_conv`
3. The converted font is embedded in the firmware assets
4. At runtime, the font is loaded and used for text rendering

## Requirements

To fully utilize the DejaVu font conversion, you need:

1. `lv_font_conv` tool installed (part of LVGL)
2. Python 3.x
3. Font conversion dependencies

Install lv_font_conv:
```bash
npm install -g lv_font_conv
```

## Font Conversion Process

The build system automatically handles font conversion:

1. Extracts the TTF font from dejavu-sans.zip
2. Converts the TTF to binary format with Vietnamese character support
3. Includes the converted font in the assets

## Character Range Support

The converted font includes support for:

- Basic Latin (0x20-0x7F)
- Latin-1 Supplement (0x80-0x17F)
- Latin Extended-A (0x100-0x17F)
- Vietnamese specific characters (0x1A0-0x1A1, 0x1AF-0x1B0, 0x1EA0-0x1EF9)
- Additional Latin Extended characters

## Fallback Mechanism

If font conversion fails, the system automatically falls back to:
- `font_puhui_common_16_4.bin` from the xiaozhi-fonts component

## Customization

To change the font size, modify the `convert_dejavu_font` function in `scripts/build_default_assets.py`:
```python
def convert_dejavu_font(dejavu_zip_path, output_path, font_size=16):  # Change font_size value
```

## Troubleshooting

### Font Conversion Issues

If you encounter font conversion errors:

1. Ensure `lv_font_conv` is installed and in PATH
2. Check that dejavu-sans.zip exists in the correct location
3. Verify Python dependencies are installed

### Font Not Displaying Correctly

If Vietnamese characters are not displaying properly:

1. Verify the converted font includes the required character ranges
2. Check that the font is being loaded correctly in assets.cc
3. Ensure the display driver supports the font format

## Manual Font Conversion

To manually convert the font:

```bash
# Extract the ZIP file first
unzip main/assets/fonts/dejavu-sans.zip

# Convert using lv_font_conv
lv_font_conv --size 16 --bpp 4 \
  --font DejaVuSans.ttf \
  --range 0x20-0x7F,0x80-0x17F,0x1A0-0x1A1,0x1AF-0x1B0,0x1EA0-0x1EF9 \
  --no-compress --no-prefilter \
  --format bin \
  --output dejavu_sans.bin
```

## Testing Vietnamese Support

To test Vietnamese font rendering:

1. Set the language to Vietnamese in menuconfig
2. Build the firmware
3. Flash to device
4. Display Vietnamese text in the UI

Example Vietnamese text for testing:
```
Xin chào thế giới! (Hello world!)
Tiếng Việt có dấu (Vietnamese with accents)
```