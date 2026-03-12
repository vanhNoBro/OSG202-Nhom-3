# Flappy Teemo

## Tính năng mới

### 1. Hoạt ảnh nhảy cải tiến
- Squash & Stretch: Teemo dẹt khi nhảy, kéo dài khi bay lên
- Texture đổi khi nhảy
- Rotation theo velocity

### 2. Tự động chơi (Auto-play AI)
- Nhấn **A** trong lúc chơi để bật/tắt
- AI tự tính toán thời điểm nhảy qua khe cây

### 3. Tiền vàng (Coins)
- Coin xuất hiện ngẫu nhiên (70%) ở giữa mỗi khe cây
- Animation coin xoay
- Mỗi coin = **+5 xu**
- Điểm xu hiển thị riêng góc phải (lần chơi + tổng)
- Xu được lưu vào file `data/coins.dat`

### 4. Shop Mua Skin
- Vào menu → SHOP
- 4 skin: DEFAULT (free), GOLDEN (50xu), SHADOW (100xu), RAINBOW (200xu), DEMON(999xu)
- RAINBOW skin đổi màu theo thời gian
- Điều hướng: **↑↓**, Mua: **B**, Trang bị: **E**

### 5. Chế độ 2 Người Chơi
- Vào menu → 2 PLAYERS
- **P1**: W hoặc SPACE để nhảy
- **P2**: Phím số 1 / UP ARROW để nhảy
- Điểm P1 và P2 hiển thị hai bên
- Sau khi cả hai chết: hiện người thắng

### 6. Admin Panel
- Vào menu → ADMIN
- Mật khẩu: **1234**
- Chức năng: +999 xu, reset xu, mở khóa tất cả skin, reset high score

## Điều khiển

| Phím | Chức năng |
|------|-----------|
| SPACE / W / UP | Nhảy (P1) |
| 1 / Numpad 1 / UP | Nhảy (P2) |
| P | Tạm dừng |
| A | Bật/tắt Auto-play |
| ESC | Thoát / Quay lại |

## Build

```bash
make        # Build
./flappy-teemo  # Chạy
make run    # Build + Chạy
```

**Yêu cầu**: SDL2, SDL2_image
