# 🎮 Flappy Teemo - Console Game Engine
## Dự án Hoàn chỉnh - System Programming in C

---

## 📋 Tổng quan

Chào mừng bạn đến với **Flappy Teemo** - một dự án Game Engine Console được xây dựng hoàn toàn bằng C, minh họa các khái niệm System Programming trên Linux!

### ✨ Điểm nổi bật

✅ **Hoàn toàn tự xây dựng** - Không sử dụng thư viện game nào  
✅ **System-level Programming** - Fork, signals, termios, fcntl  
✅ **Real-time Processing** - Game loop 30 FPS ổn định  
✅ **Dynamic Memory** - Linked list cho quản lý pipes  
✅ **AI Algorithm** - Chế độ Spectator tự động chơi  
✅ **Professional Docs** - 2200+ dòng documentation  
✅ **Production Ready** - Memory-safe, signal-handled  

---

## 🚀 Bắt đầu nhanh (3 bước)

```bash
# 1. Vào thư mục dự án
cd flappy-teemo

# 2. Build
make

# 3. Chơi!
./flappy-teemo
```

**Hoặc dùng helper script:**
```bash
./helper.sh run              # Chế độ Player
./helper.sh run-spectator    # Xem AI chơi
./helper.sh run-admin        # Debug mode
```

---

## 📂 Cấu trúc Dự án

```
flappy-teemo/
├── 📄 README.md              ⭐ Đọc đầu tiên!
├── 📄 QUICKSTART.md          ⚡ Hướng dẫn nhanh 5 phút
├── 📄 PROJECT_STRUCTURE.md   📊 Chi tiết cấu trúc
├── 📄 CHANGELOG.md           📝 Lịch sử phiên bản
│
├── 🔧 Makefile               🔨 Build system
├── 🔧 helper.sh              🛠️ Development tools
│
├── 📂 src/                   💻 Source code
│   ├── main.c                # Controller - Game Loop
│   ├── engine.c              # Logic - Physics & AI
│   └── utils.c               # System - Terminal & I/O
│
├── 📂 include/               📋 Headers
│   ├── engine.h              # Data structures
│   └── utils.h               # Function declarations
│
├── 📂 docs/                  📚 Documentation
│   ├── TECHNICAL.md          # Deep technical dive
│   └── ROADMAP.md            # Flowcharts & diagrams
│
└── 📂 data/                  💾 Runtime data
    └── highscore.dat         # Your high score
```

---

## 🎯 Features Chính

### 1. **Ba Chế độ Chơi**
- 🎮 **Player Mode**: Chơi bình thường
- 🔧 **Admin Mode**: Xem debug info (FPS, memory, etc.)
- 🤖 **Spectator Mode**: AI tự động chơi

### 2. **System Programming**
```c
✓ Memory Management    - malloc/free, linked lists
✓ Process Control      - fork(), exec(), wait()
✓ Signal Handling      - SIGINT, SIGTERM
✓ Terminal Control     - termios, raw mode
✓ Non-blocking I/O     - fcntl, O_NONBLOCK
✓ File I/O             - Binary format
✓ ANSI Rendering       - Escape codes
```

### 3. **Game Features**
- ⚡ 30 FPS game loop
- 🎨 Colorful ANSI graphics
- 💾 Persistent high score
- 🎯 Collision detection
- 🧠 AI algorithm
- 📊 Score tracking

---

## 📖 Hướng dẫn Sử dụng

### Controls
```
SPACE    - Nhảy (flap)
ESC/P    - Pause / Menu
Q        - Thoát
D        - Debug toggle (admin mode)
```

### Menu
```
[P] Play Game          - Bắt đầu chơi
[H] High Scores        - Xem điểm cao
[S] Settings           - Cài đặt
[A] Spectator Mode     - AI tự chơi
[Q] Quit               - Thoát
```

---

## 🛠️ Development

### Build Commands
```bash
make              # Build
make clean        # Clean
make run          # Build + Run
make install      # Install to system
```

### Helper Script
```bash
./helper.sh build         # Build
./helper.sh test          # Run tests
./helper.sh check-memory  # Valgrind check
./helper.sh reset         # Reset high score
./helper.sh backup        # Backup score
```

---

## 📚 Documentation Structure

### Beginner Path
1. **QUICKSTART.md** (5 min) → Bắt đầu nhanh
2. **README.md** (15 min) → Tổng quan & features
3. Chơi game một lúc!
4. **PROJECT_STRUCTURE.md** → Hiểu cấu trúc

### Advanced Path
1. **docs/TECHNICAL.md** → Chi tiết kỹ thuật
   - Kiến trúc hệ thống
   - Implementation details
   - Algorithms & data structures
   - Performance analysis

2. **docs/ROADMAP.md** → Flowcharts
   - Main program flow
   - State machine diagram
   - Game loop details
   - Memory management
   - AI decision tree

### Developer Path
1. Đọc source code theo thứ tự:
   - `include/engine.h` → Hiểu data structures
   - `src/main.c` → Game loop & controller
   - `src/engine.c` → Game logic
   - `src/utils.c` → System functions

---

## 🎓 Learning Outcomes

Sau khi nghiên cứu dự án này, bạn sẽ hiểu:

### System Programming
- ✅ Process management (fork, exec, wait)
- ✅ Signal handling (SIGINT, SIGTERM)
- ✅ Terminal programming (termios, ANSI)
- ✅ Non-blocking I/O (fcntl, select)
- ✅ File operations (binary I/O)

### C Programming
- ✅ Dynamic memory allocation
- ✅ Linked list implementation
- ✅ Pointer manipulation
- ✅ Struct design
- ✅ Modular architecture

### Game Development
- ✅ Game loop design
- ✅ State machines
- ✅ Physics simulation
- ✅ Collision detection
- ✅ AI algorithms

### Software Engineering
- ✅ Project structure
- ✅ Build systems (Make)
- ✅ Documentation
- ✅ Testing strategies
- ✅ Memory safety

---

## 🔬 Technical Highlights

### Real-time Game Loop
```c
while (running) {
    delta_time = current_time - last_frame;
    
    if (delta_time < FRAME_TIME) {
        sleep_ms(FRAME_TIME - delta_time);  // FPS control
    }
    
    update_physics();
    update_game_state();
    render();
}
```

### Dynamic Memory Management
```c
// Linked list for pipes
typedef struct Pipe {
    int x, gap_y;
    struct Pipe *next;
} Pipe;

// Add pipe
Pipe *new_pipe = malloc(sizeof(Pipe));
// ... use pipe ...
free(old_pipe);  // Clean up when passed
```

### Terminal Control
```c
// Raw mode for instant input
struct termios raw;
tcgetattr(STDIN_FILENO, &orig_termios);
raw.c_lflag &= ~(ICANON | ECHO);
tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

// Non-blocking read
fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
```

### Process Management
```c
// Fork for background music
pid_t pid = fork();
if (pid == 0) {
    // Child process
    execlp("aplay", "aplay", "music.wav", NULL);
} else {
    // Parent continues game
    music_pid = pid;
}
```

---

## 🐛 Troubleshooting

### Terminal bị lỗi sau khi thoát?
```bash
reset
# hoặc
stty sane
```

### Build lỗi?
```bash
# Cài build tools
sudo apt-get install build-essential

# Clean và rebuild
make clean
make
```

### Không thấy màu?
```bash
echo $TERM  # Nên là "xterm-256color"
```

---

## 📊 Statistics

```
Lines of Code:     ~1,000 LOC
Documentation:     ~2,200 lines
Total Files:       17 files
Functions:         ~40 functions
Binary Size:       ~30-50 KB
Memory Usage:      <1 MB
Build Time:        ~2 seconds
FPS:               30 frames/sec
```

---

## 🎯 Future Enhancements

- [ ] Sound effects integration
- [ ] Difficulty levels
- [ ] Multiplayer mode (pipes/sockets)
- [ ] Network leaderboard
- [ ] Custom themes/skins
- [ ] Performance dashboard
- [ ] Replay system

---

## 🤝 Contributing

Dự án này được thiết kế cho mục đích học tập. Contributions welcome!

1. Fork the project
2. Create feature branch
3. Commit changes
4. Push to branch
5. Open Pull Request

---

## 📄 License

MIT License - See `LICENSE` file

---

## ✨ Credits

- **Game Concept**: Flappy Bird inspired
- **Character**: Teemo (League of Legends)
- **Language**: C (C99)
- **Platform**: Linux (POSIX)
- **Purpose**: Educational - System Programming

---

## 🎮 Enjoy!

**Chúc bạn chơi vui vẻ và học được nhiều điều bổ ích!**

Remember:
- Đọc docs trước khi code
- Kiểm tra memory leaks
- Restore terminal khi crash
- Have fun learning! 🚀

---

**Need Help?**
1. Check `QUICKSTART.md` for quick start
2. Read `README.md` for full documentation
3. See `docs/TECHNICAL.md` for deep dive
4. View `docs/ROADMAP.md` for flowcharts

**Ready to Play?**
```bash
cd flappy-teemo
make run
```

**Ready to Learn?**
Start with `README.md` and `docs/TECHNICAL.md`!

---

Last Updated: January 29, 2026  
Version: 1.0.0  
Status: Complete & Production Ready ✅
