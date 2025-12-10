#!/bin/bash
# Script: Tự động đồng bộ (fetch) toàn bộ branch và tag từ remote gốc (upstream)
# và push lên fork (`origin`).
# - Nếu remote `upstream` không tồn tại, script sẽ thêm nó với `UPSTREAM_URL`.
# - Mặc định script sẽ giữ cùng tên branch khi push lên `origin`.

set -euo pipefail

# --- Cấu hình ---
MAIN_BRANCH="deesol_version"
# URL của remote gốc (cập nhật nếu bạn muốn dùng remote khác)
UPSTREAM_URL="https://github.com/78/xiaozhi-esp32.git"
# Remotes muốn đồng bộ (upstream là remote gốc; bạn có thể thêm tên khác nếu muốn)
REMOTES=("upstream" "feature_music")
# Nếu PUSH_PREFIX=1 thì các branch khi push lên origin sẽ được tiền tố bằng tên remote
# ví dụ: upstream/main -> upstream_main trên origin. Mặc định tắt (0) để giữ tên gốc.
PUSH_PREFIX=0

ensure_remote() {
    local name="$1"
    local url="$2"
    if git remote get-url "$name" >/dev/null 2>&1; then
        echo ">>> Remote '$name' tồn tại, cập nhật URL thành $url"
        git remote set-url "$name" "$url"
    else
        echo ">>> Thêm remote '$name' -> $url"
        git remote add "$name" "$url"
    fi
}

echo ">>> Chuẩn bị đồng bộ. MAIN_BRANCH=$MAIN_BRANCH"

# Thêm hoặc cập nhật upstream nếu có URL cấu hình
ensure_remote "upstream" "$UPSTREAM_URL"

# 1) Fetch tất cả remotes (prune + tags)
echo ">>> Fetching from all remotes (prune + tags)..."
git fetch --all --prune --tags

# 2) Đồng bộ từng remote
for REM in "${REMOTES[@]}"; do
    echo ">>> Đồng bộ các branch từ $REM"

    # Lấy danh sách branch remote (bỏ qua HEAD ->)
    mapfile -t BR_LIST < <(git branch -r | grep "${REM}/" | grep -v '\->' | sed "s#${REM}/##" | sort -u)

    for BR in "${BR_LIST[@]}"; do
        # skip empty
        [ -z "$BR" ] && continue
        echo ">>> Xử lý branch: $REM/$BR"

        if [ "$PUSH_PREFIX" -eq 1 ]; then
            DEST_BRANCH="${REM}_${BR}"
        else
            DEST_BRANCH="$BR"
        fi

        # Push trực tiếp từ remote ref lên origin (không cần tạo branch local)
        echo ">>> Push $REM/$BR -> origin/$DEST_BRANCH (force)"
        if git rev-parse --verify "refs/remotes/${REM}/${BR}" >/dev/null 2>&1; then
            git push origin "${REM}/${BR}:${DEST_BRANCH}" --force
        else
            echo "!!! Không tìm thấy refs/remotes/${REM}/${BR}, bỏ qua"
        fi
    done
done

# 3) Push tags (ghi đè nếu cần)
echo ">>> Push tags lên origin"
# Note: --force sẽ cho phép cập nhật tag đã tồn tại
git push origin --tags --force || echo "!!! Warning: pushing tags failed"

# 4) Quay lại nhánh làm việc chính
if git show-ref --verify --quiet refs/heads/$MAIN_BRANCH; then
    git checkout "$MAIN_BRANCH"
    echo ">>> Hoàn tất sync, đang ở branch $MAIN_BRANCH"
else
    echo ">>> Hoàn tất sync. Lưu ý: nhánh chính '$MAIN_BRANCH' không tồn tại cục bộ. Không checkout." 
fi

echo ">>> Đồng bộ xong. Kiểm tra origin để xác nhận các branch/tags đã được cập nhật." 
