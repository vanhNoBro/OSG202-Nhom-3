# 🎮 Flappy Teemo - Console Game Engine

[![C](https://img.shields.io/badge/Language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Linux](https://img.shields.io/badge/Platform-Linux-green.svg)](https://www.linux.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

## 📖 Giới thiệu

**Flappy Teemo** không chỉ là một trò chơi console đơn thuần - đây là một **Game Engine** được thiết kế để minh họa các khái niệm System Programming trong C trên Linux, bao gồm:

- ✅ **Quản lý bộ nhớ động**: `malloc`/`realloc`/`free` với cấu trúc danh sách liên kết
- ✅ **Đa tiến trình**: `fork()`, `pipe()`, `signal()` để xử lý âm thanh và tín hiệu hệ thống
- ✅ **System Calls**: `termios.h`, `fcntl.h` để điều khiển terminal và input không chặn
- ✅ **Real-time Processing**: Game loop với FPS cố định (30 FPS)
- ✅ **File I/O**: Lưu trữ high score dưới dạng binary file
- ✅ **AI Automation**: Chế độ Spectator với thuật toán điều khiển tự động

## 🏗️ Kiến trúc Dự án

```
flappy-teemo/
├── src/
│   ├── main.c      # Controller - Game Loop & State Management
│   ├── engine.c    # Logic - Physics, Collision, AI
│   └── utils.c     # System - Terminal, Rendering, I/O
├── include/
│   ├── engine.h    # Data Structures & Function Declarations
│   └── utils.h     # Utility Function Declarations
├── data/
│   └── highscore.dat  # Binary file lưu điểm cao
├── Makefile        # Build automation
└── README.md
```

### 📊 Module Design

#### 1️⃣ **main.c** (Controller)
- Quản lý vòng lặp game (Game Loop)
- Điều phối các trạng thái: Menu, Playing, Game Over
- Xử lý input từ người chơi
- FPS control (30 FPS)

#### 2️⃣ **engine.c** (Logic & Data)
- Định nghĩa cấu trúc `Bird`, `Pipe`, `Game`
- Xử lý vật lý: trọng lực, velocity
- Quản lý danh sách liên kết động cho pipes
- Phát hiện va chạm (collision detection)
- AI cho Spectator Mode

#### 3️⃣ **utils.c** (System & UI)
- Cấu hình terminal (Raw mode, non-blocking input)
- Rendering engine với ANSI escape codes
- Xử lý file I/O (save/load high score)
- Process management (fork cho audio)
- Signal handling (SIGINT, SIGTERM)

## 🚀 Cài đặt & Chạy

### Yêu cầu hệ thống
- Linux (Ubuntu/Debian recommended)
- GCC compiler
- Make
- Terminal hỗ trợ ANSI colors

### Build dự án

```bash
# Clone/Download project
cd flappy-teemo

# Build
make

# Hoặc build và chạy ngay
make run
```

### Các chế độ chơi

```bash
# Chế độ Player (mặc định)
./flappy-teemo

# Chế độ Admin (debug mode)
./flappy-teemo --admin
# hoặc
make run-admin

# Chế độ Spectator (AI tự chơi)
./flappy-teemo --spectator
# hoặc
make run-spectator
```

## 🎮 Hướng dẫn chơi

### Controls
- **SPACE**: Nhảy (flap)
- **P/ESC**: Pause / Menu
- **Q**: Thoát game
- **D**: Toggle debug info (chỉ trong Admin mode)

### Menu Options
- **[P]** Play Game - Bắt đầu chơi
- **[H]** High Scores - Xem điểm cao
- **[S]** Settings - Cài đặt (coming soon)
- **[A]** Spectator Mode - Xem AI chơi
- **[Q]** Quit - Thoát

### Gameplay
1. Điều khiển Teemo bay qua các cột chướng ngại vật
2. Nhấn SPACE để flap (nhảy lên)
3. Tránh va chạm với cột và biên màn hình
4. Mỗi cột vượt qua = +1 điểm
5. Phá vỡ kỷ lục của bạn!

## 🔬 Các khái niệm System Programming được áp dụng

### 1. Memory Management
```c
// Dynamic allocation cho pipes
Pipe* create_pipe(int x, int gap_y) {
    Pipe *pipe = (Pipe*)malloc(sizeof(Pipe));
    if (!pipe) return NULL;
    // ... setup pipe
    return pipe;
}

// Linked list management
void free_pipes(Pipe *head) {
    while (head != NULL) {
        Pipe *temp = head;
        head = head->next;
        free(temp);  // Giải phóng từng node
    }
}
```

### 2. Multiprocessing
```c
// Fork process cho background music
pid_t music_pid = fork();
if (music_pid == 0) {
    // Child process - phát nhạc
    execlp("aplay", "aplay", "music.wav", NULL);
    exit(0);
} else {
    // Parent process - tiếp tục game
}
```

### 3. Terminal Control (System Calls)
```c
// Raw mode - tắt canonical input
struct termios raw;
tcgetattr(STDIN_FILENO, &orig_termios);
raw = orig_termios;
raw.c_lflag &= ~(ICANON | ECHO);  // Tắt buffering & echo
tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

// Non-blocking input
int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
```

### 4. Signal Handling
```c
// Cleanup khi Ctrl+C
void cleanup_handler(int signum) {
    stop_background_music();
    restore_terminal();
    exit(0);
}

signal(SIGINT, cleanup_handler);
```

### 5. File I/O (Binary)
```c
// Lưu high score dưới dạng binary
void save_highscore(int score) {
    FILE *file = fopen("data/highscore.dat", "wb");
    fwrite(&score, sizeof(int), 1, file);
    fclose(file);
}
```

## 🤖 Spectator Mode (AI)

AI sử dụng thuật toán đơn giản:
1. Tìm pipe gần nhất phía trước
2. Tính toán điểm giữa của khoảng trống
3. Nếu bird đang ở dưới target → Jump
4. Nếu bird đang ở trên target → Không làm gì (trọng lực kéo xuống)

```c
int calculate_ai_decision(Game *game) {
    Pipe *nearest_pipe = game->pipe_head;
    // Find nearest pipe ahead
    while (nearest_pipe && nearest_pipe->x < game->bird.x) {
        nearest_pipe = nearest_pipe->next;
    }
    
    int target_y = nearest_pipe->gap_y + PIPE_GAP / 2;
    
    if (game->bird.y > target_y + 2 && game->bird.velocity > -2) {
        return 1; // Jump
    }
    return 0;
}
```

## 🔧 Admin Mode Features

Kích hoạt bằng: `./flappy-teemo --admin`

Hiển thị thông tin debug:
- Tọa độ Bird (X, Y)
- Velocity hiện tại
- Số lượng pipes đang active
- Frame count
- Memory usage (planned)

## 📁 Data Storage

### High Score File Format
- **Path**: `data/highscore.dat`
- **Format**: Binary (4 bytes integer)
- **Access**: Read/Write with error checking

```c
// Structure (conceptual)
[4 bytes: int high_score]
```

## 🛡️ Safety & Ethics

✅ **Không yêu cầu quyền sudo**  
✅ **Mọi `malloc` đều được `free`**  
✅ **Process cleanup triệt để** (tránh zombie processes)  
✅ **Terminal được restore khi exit**  
✅ **Signal handling đầy đủ** (SIGINT, SIGTERM)

## 🧪 Testing & Development

```bash
# Clean build
make clean
make

# Test các mode
make run              # Player mode
make run-admin        # Admin mode  
make run-spectator    # AI mode

# Memory leak check (requires valgrind)
valgrind --leak-check=full ./flappy-teemo
```

## 📚 Tài liệu tham khảo

- **ANSI Escape Codes**: https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
- **termios.h**: https://man7.org/linux/man-pages/man3/termios.3.html
- **fork() and exec()**: https://man7.org/linux/man-pages/man2/fork.2.html
- **Linux System Programming**: Advanced C course materials

## 🎯 Roadmap & Future Features

- [ ] Sound effects với `beep` command
- [ ] Multiplayer mode qua named pipes
- [ ] Networking với sockets
- [ ] Save/Load game state
- [ ] Difficulty levels
- [ ] Custom skins cho Teemo
- [ ] Performance metrics dashboard

## 🐛 Troubleshooting

### Terminal không restore sau crash
```bash
reset
# hoặc
stty sane
```

### Không hiển thị màu
Kiểm tra terminal hỗ trợ ANSI:
```bash
echo $TERM  # Nên là xterm-256color hoặc tương tự
```

### Permission denied khi tạo file
```bash
chmod +x flappy-teemo
mkdir -p data
chmod 755 data
```

## 👥 Contributing

Đây là dự án học tập về System Programming. Contributions welcome!

1. Fork repository
2. Create feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to branch (`git push origin feature/AmazingFeature`)
5. Open Pull Request

## 📄 License

Distributed under the MIT License. See `LICENSE` for more information.

## ✨ Credits

- **Game Concept**: Inspired by Flappy Bird
- **Character**: Teemo from League of Legends
- **Developer**: [Your Name]
- **Course**: System Programming / Operating Systems

---

**Happy Coding! 🚀**

Made with ❤️ and lots of ☕
