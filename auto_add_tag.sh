#!/bin/bash
# Script: Sync branch develop từ origin và auto-tag
# Author: Son (Deesol workflow)

BRANCH="develop"

# 1. Đảm bảo đang ở branch develop
git checkout $BRANCH || {
    echo "!!! Không tìm thấy branch $BRANCH"
    exit 1
}

# 2. Pull code mới nhất từ origin/develop
echo ">>> Pulling latest code from origin/$BRANCH..."
git pull origin $BRANCH || {
    echo "!!! Pull thất bại"
    exit 1
}

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
