#!/bin/bash
# Liệt kê toàn bộ file trong các thư mục và file chỉ định

# Các thư mục cần duyệt
dirs=(
  "main/boards/common/"
  "main/boards/bread-compact-wifi/"
  "main/application/"
  "main/display/"
  "main/led/"
  "main/ota/"
  "main/assets/"
  "main/protocols/"
  "main/audio/"
  "main/device_state_event/"
  "main/mcp_server/"
  "main/system/"
)

# Các file riêng lẻ
files=(
  "main/CMakeLists.txt"
  "main/idf_component.yml"
  "main/main.cc"
  "main/Kconfig.projbuild"
)

echo "=== Danh sách file trong các thư mục ==="
for d in "${dirs[@]}"; do
  if [ -d "$d" ]; then
    echo "--- $d ---"
    find "$d" -type f
  else
    echo "Thư mục $d không tồn tại"
  fi
done

echo ""
echo "=== Danh sách file riêng lẻ ==="
for f in "${files[@]}"; do
  if [ -f "$f" ]; then
    echo "$f"
  else
    echo "File $f không tồn tại"
  fi
done
