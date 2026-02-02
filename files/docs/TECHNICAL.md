# Flappy Teemo - Technical Documentation

## 📐 Kiến trúc hệ thống

### 1. Tổng quan kiến trúc

```
┌─────────────────────────────────────────────────────────┐
│                     USER INTERFACE                       │
│              (Terminal - ANSI Escape Codes)              │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│                   CONTROLLER LAYER                       │
│                      (main.c)                            │
│  ┌────────────┐  ┌──────────┐  ┌──────────────┐        │
│  │ Game Loop  │  │  State   │  │    Input     │        │
│  │  Manager   │  │ Machine  │  │   Handler    │        │
│  └────────────┘  └──────────┘  └──────────────┘        │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│                    LOGIC LAYER                           │
│                    (engine.c)                            │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌────────┐ │
│  │ Physics  │  │ Collision│  │   AI     │  │ Score  │ │
│  │  Engine  │  │ Detection│  │ System   │  │Manager │ │
│  └──────────┘  └──────────┘  └──────────┘  └────────┘ │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│                   SYSTEM LAYER                           │
│                    (utils.c)                             │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌────────┐ │
│  │Terminal  │  │ Rendering│  │  File    │  │Process │ │
│  │ Control  │  │  Engine  │  │   I/O    │  │Manager │ │
│  └──────────┘  └──────────┘  └──────────┘  └────────┘ │
└─────────────────────────────────────────────────────────┘
```

### 2. Data Flow

```
Input (Keyboard) 
    ↓
[Non-blocking Read] (fcntl + termios)
    ↓
[Input Handler] (main.c)
    ↓
[State Machine] → [MENU | PLAYING | GAME_OVER]
    ↓
[Physics Update] (engine.c)
    ↓
[Collision Check] 
    ↓
[Score Update]
    ↓
[Rendering] (utils.c) → ANSI Escape Codes → Terminal
```

## 🔧 Cài đặt chi tiết

### 1. Game Loop Implementation

```c
void game_loop(Game *game) {
    long long last_frame_time = get_time_ms();
    
    while (running) {
        long long current_time = get_time_ms();
        long long delta_time = current_time - last_frame_time;
        
        // Frame rate limiting (30 FPS)
        if (delta_time < FRAME_TIME_MS) {
            sleep_ms(FRAME_TIME_MS - delta_time);
            continue;
        }
        
        // Update game state
        update_game_state(game, delta_time);
        
        // Render
        render_game(game);
        
        last_frame_time = current_time;
    }
}
```

**Giải thích:**
- **Frame Time**: 1000ms / 30 FPS = ~33ms per frame
- **Delta Time**: Thời gian trôi qua từ frame trước
- **Sleep**: Nếu xử lý nhanh, sleep để giữ FPS ổn định

### 2. Physics Engine

```c
void update_bird(Bird *bird) {
    // Apply gravity
    bird->velocity += GRAVITY;  // 1 pixel/frame
    
    // Update position
    bird->y += bird->velocity;
    
    // Velocity clamping
    if (bird->velocity > MAX_VELOCITY) 
        bird->velocity = MAX_VELOCITY;
    if (bird->velocity < MIN_VELOCITY) 
        bird->velocity = MIN_VELOCITY;
    
    // Boundary check
    if (bird->y < 0) {
        bird->y = 0;
        bird->velocity = 0;
    }
}
```

**Công thức vật lý:**
```
Velocity(t+1) = Velocity(t) + GRAVITY
Position(t+1) = Position(t) + Velocity(t+1)

Khi nhảy:
Velocity = JUMP_FORCE (âm, hướng lên)
```

### 3. Collision Detection

```c
bool check_collision(Game *game) {
    Bird *bird = &game->bird;
    Pipe *pipe = game->pipe_head;
    
    // Ground/Ceiling collision
    if (bird->y <= 0 || bird->y >= SCREEN_HEIGHT - 1)
        return true;
    
    // Pipe collision
    while (pipe != NULL) {
        // Horizontal overlap check
        if (bird->x + BIRD_WIDTH >= pipe->x && 
            bird->x <= pipe->x + PIPE_WIDTH) {
            
            // Vertical gap check
            if (bird->y < pipe->gap_y || 
                bird->y > pipe->gap_y + PIPE_GAP) {
                return true;  // Collision!
            }
        }
        pipe = pipe->next;
    }
    
    return false;
}
```

**Bounding Box Collision:**
```
Bird Box: [bird->x, bird->y, BIRD_WIDTH, BIRD_HEIGHT]
Pipe Box: [pipe->x, 0, PIPE_WIDTH, pipe->gap_y] (top)
          [pipe->x, pipe->gap_y + GAP, PIPE_WIDTH, HEIGHT] (bottom)
```

### 4. Memory Management - Linked List

```c
typedef struct Pipe {
    int x;
    int gap_y;
    struct Pipe *next;  // Con trỏ đến pipe tiếp theo
} Pipe;

// Thêm pipe mới vào cuối danh sách
void add_pipe(Game *game) {
    Pipe *new_pipe = create_pipe(SCREEN_WIDTH, random_gap_y());
    
    if (game->pipe_head == NULL) {
        game->pipe_head = new_pipe;  // Danh sách rỗng
    } else {
        Pipe *current = game->pipe_head;
        while (current->next != NULL) {
            current = current->next;  // Tìm pipe cuối
        }
        current->next = new_pipe;  // Thêm vào cuối
    }
}

// Xóa pipes đã qua màn hình
void remove_passed_pipes(Game *game) {
    while (game->pipe_head != NULL && 
           game->pipe_head->x < -PIPE_WIDTH) {
        Pipe *temp = game->pipe_head;
        game->pipe_head = game->pipe_head->next;
        free(temp);  // Giải phóng bộ nhớ
    }
}
```

**Memory Lifecycle:**
```
malloc() → [Pipe created]
    ↓
[Added to linked list]
    ↓
[Updated each frame: x -= PIPE_SPEED]
    ↓
[x < -PIPE_WIDTH] → free() → [Memory released]
```

### 5. Terminal Control (System Calls)

#### Raw Mode Setup
```c
void setup_terminal() {
    struct termios raw;
    
    // Lưu cấu hình gốc
    tcgetattr(STDIN_FILENO, &orig_termios);
    
    raw = orig_termios;
    
    // Tắt canonical mode (line buffering)
    raw.c_lflag &= ~ICANON;
    
    // Tắt echo (ký tự không hiển thị khi gõ)
    raw.c_lflag &= ~ECHO;
    
    // Đọc từng byte, timeout = 0
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    
    // Apply settings
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
```

**Canonical vs Raw Mode:**
```
Canonical Mode (default):
- Input buffered until Enter
- Line editing enabled (backspace, etc.)
- Good for: CLI apps, shells

Raw Mode:
- No buffering
- Read individual keystrokes
- Good for: Games, text editors
```

#### Non-blocking Input
```c
// Thiết lập file descriptor không chặn
int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

// Đọc input
char get_input() {
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1) {
        return c;  // Có phím được nhấn
    }
    return 0;  // Không có input
}
```

**Blocking vs Non-blocking:**
```
Blocking I/O:
read() → [Waits for input] → Returns
Game freezes while waiting!

Non-blocking I/O:
read() → [Check immediately] → Returns 0 if no input
Game continues running!
```

### 6. ANSI Escape Codes

```c
// Cursor control
#define MOVE_CURSOR(x, y) printf("\033[%d;%dH", y+1, x+1)
#define CLEAR_SCREEN printf("\033[2J")
#define HIDE_CURSOR printf("\033[?25l")
#define SHOW_CURSOR printf("\033[?25h")

// Colors
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_RESET "\033[0m"

// Usage
MOVE_CURSOR(10, 5);
printf(COLOR_GREEN "Hello!" COLOR_RESET);
```

**ANSI Sequence Format:**
```
ESC [ <parameters> <command>

Examples:
\033[2J        → Clear screen
\033[10;20H    → Move cursor to row 10, col 20
\033[31m       → Set foreground color to red
\033[0m        → Reset all attributes
```

### 7. Process Management

```c
// Fork để chạy background music
void start_background_music() {
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child process
        
        // Redirect output to null
        int fd = open("/dev/null", O_WRONLY);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        
        // Execute audio player
        execlp("aplay", "aplay", "music.wav", NULL);
        
        // Nếu exec fail
        exit(1);
        
    } else if (pid > 0) {
        // Parent process
        music_pid = pid;  // Lưu PID để kill sau
    } else {
        // Fork failed
        perror("fork");
    }
}

// Dừng music
void stop_background_music() {
    if (music_pid > 0) {
        kill(music_pid, SIGTERM);
        waitpid(music_pid, NULL, 0);  // Thu hồi zombie
        music_pid = -1;
    }
}
```

**Process Lifecycle:**
```
Parent Process (Game)
    │
    ├─ fork() → Child Process (Music Player)
    │              │
    │              ├─ execlp("aplay", ...)
    │              └─ exit()
    │
    ├─ [Game continues]
    │
    └─ kill(child_pid) + waitpid() → Clean up
```

### 8. Signal Handling

```c
// Global cleanup function
void cleanup_handler(int signum) {
    printf("\nReceived signal %d, cleaning up...\n", signum);
    
    // Stop music
    stop_background_music();
    
    // Restore terminal
    restore_terminal();
    
    // Free memory
    cleanup_game(global_game);
    
    exit(0);
}

// Register handlers
void setup_signal_handlers() {
    signal(SIGINT, cleanup_handler);   // Ctrl+C
    signal(SIGTERM, cleanup_handler);  // kill command
}
```

**Common Signals:**
```
SIGINT (2)  → Ctrl+C (interrupt)
SIGTERM (15) → kill <pid> (terminate)
SIGKILL (9) → kill -9 <pid> (force kill, can't catch)
SIGCHLD     → Child process terminated
```

### 9. File I/O (Binary Format)

```c
// Save high score
void save_highscore(int score) {
    FILE *file = fopen("data/highscore.dat", "wb");
    if (file == NULL) {
        perror("fopen");
        return;
    }
    
    // Write binary data
    size_t written = fwrite(&score, sizeof(int), 1, file);
    if (written != 1) {
        perror("fwrite");
    }
    
    fclose(file);
}

// Load high score
int load_highscore() {
    FILE *file = fopen("data/highscore.dat", "rb");
    int score = 0;
    
    if (file == NULL) {
        return 0;  // File không tồn tại
    }
    
    // Read binary data
    size_t read = fread(&score, sizeof(int), 1, file);
    if (read != 1) {
        score = 0;  // Đọc lỗi
    }
    
    fclose(file);
    return score;
}
```

**Binary vs Text:**
```
Text format:  "12345\n" → 6 bytes
Binary format: 12345 → 4 bytes (int)

Advantages of binary:
- Smaller file size
- Faster read/write
- No parsing needed
- Type-safe
```

### 10. AI Algorithm (Spectator Mode)

```c
int calculate_ai_decision(Game *game) {
    // Find nearest pipe ahead
    Pipe *nearest = game->pipe_head;
    while (nearest && nearest->x < game->bird.x) {
        nearest = nearest->next;
    }
    
    if (!nearest) return 0;
    
    // Calculate target position (center of gap)
    int target_y = nearest->gap_y + (PIPE_GAP / 2);
    
    // Distance to target
    int distance = game->bird.y - target_y;
    
    // Decision logic
    if (distance > 2 && game->bird.velocity > -2) {
        return 1;  // Jump (bird is below target)
    }
    
    return 0;  // Let gravity pull down
}
```

**AI Logic Flow:**
```
┌─────────────────┐
│ Find next pipe  │
└────────┬────────┘
         │
    ┌────▼─────┐
    │ Target = │
    │ gap_mid  │
    └────┬─────┘
         │
    ┌────▼──────────┐
    │ bird.y > target│──YES──> Jump
    │   + velocity   │
    │   < threshold? │
    └────┬───────────┘
         │
        NO
         │
         ▼
    Do nothing (gravity)
```

## 🧪 Performance Analysis

### Memory Usage

```c
sizeof(Bird) = 16 bytes
sizeof(Pipe) = 16 bytes
sizeof(Game) = ~100 bytes

Typical game state:
- 1 Bird: 16 bytes
- 5-10 Pipes: 80-160 bytes
- Game struct: 100 bytes
Total: ~300-400 bytes
```

### CPU Usage

```
Frame time budget: 33ms (30 FPS)

Typical breakdown:
- Input handling: <1ms
- Physics update: <1ms
- Collision check: ~1ms (O(n) where n = pipe count)
- Rendering: ~5-10ms (terminal I/O)
- Sleep: ~20-25ms (to maintain 30 FPS)
```

## 🔍 Debugging Tips

### Memory Leaks
```bash
# Compile with debug symbols
gcc -g -Wall src/*.c -o flappy-teemo

# Run with valgrind
valgrind --leak-check=full --show-leak-kinds=all ./flappy-teemo
```

### GDB Debugging
```bash
gdb ./flappy-teemo

# In GDB:
(gdb) break main
(gdb) run
(gdb) next
(gdb) print game->score
(gdb) backtrace
```

### Strace (System Calls)
```bash
strace -e trace=open,close,read,write ./flappy-teemo
```

## 📊 Complexity Analysis

| Operation | Time Complexity | Space Complexity |
|-----------|----------------|------------------|
| Add pipe | O(n) | O(1) |
| Remove pipe | O(1) amortized | O(1) |
| Collision check | O(n) | O(1) |
| Update pipes | O(n) | O(1) |
| Render | O(n × m) | O(1) |

Where:
- n = number of active pipes (typically 5-10)
- m = screen height (24)

## 🎓 Learning Outcomes

Sau khi hoàn thành dự án này, bạn sẽ nắm vững:

1. ✅ **Memory Management**: malloc, free, linked lists
2. ✅ **Process Management**: fork, exec, wait, signals
3. ✅ **File I/O**: Binary file operations
4. ✅ **Terminal Programming**: termios, ANSI codes
5. ✅ **Real-time Systems**: Game loops, timing
6. ✅ **System Calls**: Low-level I/O, non-blocking reads
7. ✅ **Data Structures**: Linked lists, state machines
8. ✅ **Algorithm Design**: Collision detection, AI

---

**Last Updated**: January 2026  
**Author**: System Programming Course
