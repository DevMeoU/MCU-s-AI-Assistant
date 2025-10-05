find . \
  -path "./.*" -prune -o \
  -path "./__pycache__" -prune -o \
  -path "./assets" -prune -o \
  -path "./managed_components" -prune -o \
  -path "./partitions" -prune -o \
  -path "./build" -prune -o \
  -type f \
  ! -name "*.pyc" ! -name "*.pyo" ! -name "*.pyd" ! -name "*.so" \
  ! -name "*.swp" ! -name "*.swo" ! -name "*.bak" ! -name "*.log" \
  ! -name "*.db" ! -name "*.sqlite" ! -name "*.sqlite3" ! -name "*.sql" \
  ! -name "*.json" ! -name "*.lock" ! -name "*.pid" ! -name "*.pkl" \
  ! -name "*.pth" ! -name "*.pth.tar" ! -name "*.pt" ! -name "*.onnx" \
  ! -name "*.pb" ! -name "*.dll" ! -name "*.dylib" ! -name "*.jpeg" \
  ! -name "*.gif" ! -name "*.png" ! -name "*.jpg" ! -name "*.mp4" \
  ! -name "*.mp3" ! -name "*.wav" ! -name "*.ogg" ! -name "logs" \
  ! -name "*.exe" ! -name "__init__.py" ! -name "*.md" ! -name "*.obj" \
  ! -name "*.bin" ! -name "*.elf" ! -name "*.ninja_deps" ! -name "*.a" \
  ! -name "*.md" ! -name "*.o" ! -name "*.so" ! -name "*.dll" \
  -exec grep -P -l '\p{Han}' {} \; \
| sed 's|^|[NEED_CHANGE] |' > TODO
