#!/bin/bash
# Script: Sync branch develop từ origin và auto-tag an toàn
# Branch chính: develop

BRANCH="develop"

# 1. Đảm bảo đang ở branch develop
git checkout $BRANCH || {
    echo "!!! Không tìm thấy branch $BRANCH"
    exit 1
}

# 2. Pull an toàn (chỉ fast-forward)
echo ">>> Pulling latest code from origin/$BRANCH (safe mode)..."
if ! git pull --ff-only origin $BRANCH; then
    echo "!!! Pull thất bại: có commit local khác với origin/$BRANCH"
    echo ">>> Hãy xử lý thủ công (merge hoặc stash) trước khi chạy lại."
    exit 1
fi

# 3. Tạo tag tự động theo timestamp
TAG="auto-$(date +%Y%m%d-%H%M)"
echo ">>> Tạo tag $TAG"
git tag -a $TAG -m "Auto tag from deesol script at $(date)"

# 4. Push tag lên origin
git push origin $TAG || {
    echo "!!! Push tag thất bại"
    exit 1
}

echo ">>> Hoàn tất: branch $BRANCH đã được sync và gắn tag $TAG"
