#!/bin/bash
# Script: Fetch tất cả branch từ nhiều remote và push sang fork (origin)
# Nhánh làm việc chính: deesol_version

MAIN_BRANCH="deesol_version"
REMOTES=("upstream" "feature_music")

# 1. Fetch từ tất cả remote
for REM in "${REMOTES[@]}"; do
    echo ">>> Fetching from $REM..."
    git fetch $REM
done

# 2. Lặp qua từng remote và branch
for REM in "${REMOTES[@]}"; do
    echo ">>> Đồng bộ các branch từ $REM"

    # Lấy danh sách branch remote (bỏ qua HEAD ->)
    for BR in $(git branch -r | grep "$REM/" | grep -v '\->' | sed "s#$REM/##"); do
        echo ">>> Xử lý branch: $REM/$BR"

        # Tạo branch local tạm dựa trên remote
        TMP_BRANCH="${REM}_${BR}"
        if git show-ref --verify --quiet refs/heads/$TMP_BRANCH; then
            git branch -D $TMP_BRANCH   # xóa nếu tồn tại cũ
        fi
        git checkout -b $TMP_BRANCH $REM/$BR || {
            echo "!!! Branch $BR không tồn tại ở $REM"
            continue
        }

        # Push branch này lên origin với tên riêng
        echo ">>> Push $TMP_BRANCH lên origin..."
        git push origin $TMP_BRANCH:$TMP_BRANCH --force
    done
done

# 3. Quay lại nhánh làm việc chính
git checkout $MAIN_BRANCH
echo ">>> Hoàn tất sync, đang ở branch $MAIN_BRANCH"
