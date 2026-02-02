# Flappy Teemo - Development Roadmap & Flowcharts

## 📅 Development Roadmap

### Tuần 1-2: Thiết kế & Planning ✅
- [x] Phân tích yêu cầu hệ thống
- [x] Thiết kế kiến trúc module
- [x] Định nghĩa data structures
- [x] Lập kế hoạch phát triển

### Tuần 3-4: Xây dựng Core Engine ✅
- [x] Thiết lập cấu trúc dự án
- [x] Implement `engine.h` - Data structures
- [x] Implement `engine.c` - Game logic
  - [x] Physics engine (gravity, velocity)
  - [x] Collision detection
  - [x] Linked list management (Pipes)
  - [x] Score system

### Tuần 5: System Layer Implementation ✅
- [x] Implement `utils.h` - Utility declarations
- [x] Implement `utils.c` - System functions
  - [x] Terminal control (termios, fcntl)
  - [x] ANSI rendering engine
  - [x] File I/O (binary format)
  - [x] Process management (fork, exec)
  - [x] Signal handling

### Tuần 6: Controller & Game Loop ✅
- [x] Implement `main.c` - Game controller
  - [x] Game loop với FPS control
  - [x] State machine (Menu, Playing, Game Over)
  - [x] Input handling
  - [x] Mode selection (Player, Admin, Spectator)

### Tuần 7: AI & Advanced Features 🔄
- [x] AI algorithm cho Spectator mode
- [ ] Sound effects integration
- [ ] Performance optimization
- [ ] Memory leak detection & fixing

### Tuần 8: Testing & Documentation 🔄
- [x] Unit testing (manual)
- [x] Integration testing
- [x] Documentation (README, TECHNICAL)
- [ ] User guide
- [ ] Video demo

### Tuần 9-10: Polish & Enhancement 📝
- [ ] Bug fixes
- [ ] UI/UX improvements
- [ ] Additional game modes
- [ ] Difficulty levels
- [ ] Leaderboard system

---

## 🔄 Flowcharts

### 1. Main Program Flow

```
┌─────────────────────────────────────────────┐
│              START PROGRAM                   │
└──────────────────┬──────────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────────┐
│         Parse Command Line Args              │
│  --admin    → MODE_ADMIN                     │
│  --spectator → MODE_SPECTATOR                │
│  default    → MODE_PLAYER                    │
└──────────────────┬──────────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────────┐
│          Setup Environment                   │
│  1. Create data directory                    │
│  2. Initialize game struct                   │
│  3. Load high score                          │
│  4. Setup terminal (raw mode)                │
│  5. Setup signal handlers                    │
└──────────────────┬──────────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────────┐
│            GAME LOOP                         │
│  ┌──────────────────────────────┐           │
│  │   Calculate Delta Time       │           │
│  └──────────┬───────────────────┘           │
│             │                                │
│  ┌──────────▼───────────────────┐           │
│  │  Frame Rate Control (30 FPS) │           │
│  └──────────┬───────────────────┘           │
│             │                                │
│  ┌──────────▼───────────────────┐           │
│  │    State Machine Switch      │           │
│  │  ┌────────┬─────────┬─────┐  │           │
│  │  │ MENU   │ PLAYING │ GO  │  │           │
│  │  └────┬───┴────┬────┴──┬──┘  │           │
│  └───────┼────────┼───────┼─────┘           │
│          │        │       │                  │
│          ▼        ▼       ▼                  │
│      [Handler][Handler][Handler]             │
│          │        │       │                  │
│          └────────┴───────┘                  │
│                   │                          │
│  ┌────────────────▼───────────────┐          │
│  │  Render Current State          │          │
│  └────────────────┬───────────────┘          │
│                   │                          │
│  ┌────────────────▼───────────────┐          │
│  │  Check Exit Condition          │          │
│  └────────────────┬───────────────┘          │
│                   │                          │
│             ┌─────┴─────┐                    │
│             │ Continue? │                    │
│             └─────┬─────┘                    │
│                YES│                          │
│                   └──────► Loop Back         │
│                                              │
│                    NO                        │
└────────────────────┬─────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────┐
│              CLEANUP                         │
│  1. Free all pipes (linked list)            │
│  2. Stop background music                   │
│  3. Restore terminal settings               │
│  4. Save high score if needed               │
└──────────────────┬──────────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────────┐
│               EXIT (0)                       │
└─────────────────────────────────────────────┘
```

### 2. State Machine Diagram

```
                    ┌──────────┐
             ┌──────│  START   │──────┐
             │      └──────────┘      │
             │                        │
             ▼                        ▼
      ┌────────────┐            ┌─────────┐
      │    MENU    │◄───────────│  EXIT   │
      │            │            └─────────┘
      │ [P] Play   │                 ▲
      │ [H] Score  │                 │
      │ [S] Settings                 │
      │ [A] AI     │                 │
      │ [Q] Quit   │                 │
      └──┬───┬───┬─┘                 │
         │   │   │                   │
    ┌────┘   │   └────┐              │
    │        │        │              │
    │   ┌────▼────┐   │         [Q] Quit
    │   │ HIGH-   │   │              │
    │   │ SCORE   │   │              │
    │   └────┬────┘   │              │
    │        │        │              │
    │   ┌────▼────┐   │              │
    │   │SETTINGS │   │              │
    │   └────┬────┘   │              │
    │        │        │              │
    │        │[M] Menu│              │
    │        └───┬────┘              │
    │            │                   │
    │            │                   │
    ▼            ▼                   │
┌──────────────────────┐             │
│      PLAYING         │             │
│                      │             │
│ Update Physics       │             │
│ Spawn Pipes          │             │
│ Check Collision      │             │
│ Update Score         │             │
│ [SPACE] Jump         │             │
│ [ESC/P] Pause        │─────────────┤
│                      │             │
│  Collision? ─────────┼──────┐      │
└──────────────────────┘      │      │
                              │      │
                              ▼      │
                      ┌──────────────┤
                      │  GAME OVER   │
                      │              │
                      │ Show Score   │
                      │ [R] Restart  │──┐
                      │ [M] Menu     │  │
                      └──────────────┘  │
                              │         │
                              └─────────┘
```

### 3. Game Loop Detailed Flow

```
┌─────────────────────────────────────────────┐
│            PLAYING STATE                     │
└──────────────────┬──────────────────────────┘
                   │
    ┌──────────────▼───────────────┐
    │   frame_count++              │
    └──────────────┬───────────────┘
                   │
    ┌──────────────▼───────────────┐
    │  Check Pipe Spawn Timer      │
    │  (every 35 frames)           │
    └──────────────┬───────────────┘
                   │
                YES│
    ┌──────────────▼───────────────┐
    │    add_pipe(game)            │
    │  - malloc new Pipe           │
    │  - random gap_y              │
    │  - x = SCREEN_WIDTH          │
    │  - add to linked list        │
    └──────────────┬───────────────┘
                   │
    ┌──────────────▼───────────────┐
    │     Handle Input             │
    │  MODE_PLAYER:                │
    │    if (SPACE) → bird_jump()  │
    │  MODE_SPECTATOR:             │
    │    AI decision → maybe jump  │
    └──────────────┬───────────────┘
                   │
    ┌──────────────▼───────────────┐
    │   update_bird(&bird)         │
    │  - velocity += GRAVITY       │
    │  - y += velocity             │
    │  - clamp velocity            │
    │  - boundary check            │
    └──────────────┬───────────────┘
                   │
    ┌──────────────▼───────────────┐
    │   update_pipes(game)         │
    │  - For each pipe:            │
    │    pipe->x -= PIPE_SPEED     │
    │  - remove_passed_pipes()     │
    └──────────────┬───────────────┘
                   │
    ┌──────────────▼───────────────┐
    │   update_score(game)         │
    │  - Check if pipe passed bird │
    │  - If yes: score++           │
    │  - Update high_score         │
    └──────────────┬───────────────┘
                   │
    ┌──────────────▼───────────────┐
    │   check_collision(game)      │
    │  - Ceiling/floor?            │
    │  - Pipe overlap?             │
    └──────────────┬───────────────┘
                   │
              ┌────┴────┐
              │Collision│
              └────┬────┘
                YES│         NO
                   │          │
    ┌──────────────▼──────┐   │
    │  bird.is_alive=false│   │
    │  play_sound("die")  │   │
    │  state=GAME_OVER    │   │
    └─────────────────────┘   │
                              │
    ┌─────────────────────────▼───┐
    │     render_game(game)        │
    │  - Clear screen              │
    │  - Draw pipes (green █)      │
    │  - Draw bird (⊂( ◕ ‿ ◕ )⊃)  │
    │  - Draw score                │
    │  - Debug info (if admin)     │
    └──────────────────────────────┘
```

### 4. Collision Detection Algorithm

```
┌─────────────────────────────────────┐
│   check_collision(Game *game)       │
└──────────────┬──────────────────────┘
               │
    ┌──────────▼──────────┐
    │ Check Boundaries     │
    │ bird.y <= 0?         │
    │ bird.y >= HEIGHT-1?  │
    └──────────┬───────────┘
               │
          ┌────┴────┐
          │ TRUE?   │
          └────┬────┘
          YES  │  NO
               │  │
               │  ▼
               │  ┌────────────────────────┐
               │  │ For each Pipe in list  │
               │  └────────┬───────────────┘
               │           │
               │  ┌────────▼───────────────┐
               │  │ X Overlap Check:       │
               │  │ bird_x+w >= pipe_x &&  │
               │  │ bird_x <= pipe_x+w?    │
               │  └────────┬───────────────┘
               │           │
               │      ┌────┴────┐
               │      │ TRUE?   │
               │      └────┬────┘
               │      YES  │  NO
               │           │  │
               │           │  └──► Next Pipe
               │           │
               │  ┌────────▼────────────────┐
               │  │ Y Gap Check:            │
               │  │ bird_y < gap_y ||       │
               │  │ bird_y > gap_y+GAP?     │
               │  └────────┬────────────────┘
               │           │
               │      ┌────┴────┐
               │      │ TRUE?   │
               │      └────┬────┘
               │           │
               ▼           ▼
         ┌─────────────────────┐
         │  return TRUE        │
         │  (Collision!)       │
         └─────────────────────┘
                   │
                   │
         ┌─────────▼──────────┐
         │  return FALSE       │
         │  (Safe)             │
         └─────────────────────┘
```

### 5. Memory Management Flow (Pipes)

```
┌────────────────────────────┐
│  Pipe Lifecycle            │
└────────────┬───────────────┘
             │
    ┌────────▼────────┐
    │  SPAWN TIMER    │
    │  (every 35      │
    │   frames)       │
    └────────┬────────┘
             │
    ┌────────▼────────────────┐
    │  malloc(sizeof(Pipe))   │
    │  ┌──────────────────┐   │
    │  │ Pipe Memory:     │   │
    │  │ - x: int         │   │
    │  │ - gap_y: int     │   │
    │  │ - next: Pipe*    │   │
    │  └──────────────────┘   │
    └────────┬────────────────┘
             │
    ┌────────▼────────────────┐
    │  Initialize Values      │
    │  - x = SCREEN_WIDTH     │
    │  - gap_y = random()     │
    │  - next = NULL          │
    └────────┬────────────────┘
             │
    ┌────────▼────────────────┐
    │  Add to Linked List     │
    │  ┌──────┐   ┌──────┐   │
    │  │ Head │->│ New  │    │
    │  └──────┘   └──────┘   │
    └────────┬────────────────┘
             │
             │ GAME LOOP
             │ (each frame)
             │
    ┌────────▼────────────────┐
    │  update_pipes()         │
    │  For each pipe:         │
    │    pipe->x -= SPEED     │
    └────────┬────────────────┘
             │
    ┌────────▼────────────────┐
    │  Check Position         │
    │  pipe->x < -WIDTH?      │
    └────────┬────────────────┘
             │
        ┌────┴────┐
        │  YES?   │
        └────┬────┘
        YES  │  NO
             │  │
             │  └──► Keep in list
             │
    ┌────────▼────────────────┐
    │  remove_passed_pipes()  │
    │  ┌──────────┐           │
    │  │ temp=head│           │
    │  │ head=next│           │
    │  │ free(temp)│ ◄─ FREE! │
    │  └──────────┘           │
    └─────────────────────────┘
             │
             ▼
    Memory returned to system
```

### 6. AI Decision Tree (Spectator Mode)

```
                    START
                      │
        ┌─────────────▼──────────────┐
        │  Find Nearest Pipe Ahead   │
        │  (pipe->x >= bird->x)      │
        └─────────────┬──────────────┘
                      │
                 ┌────┴────┐
                 │ Found?  │
                 └────┬────┘
                 YES  │  NO
                      │  │
                      │  └──► return 0 (no jump)
                      │
        ┌─────────────▼──────────────┐
        │  Calculate Target Position │
        │  target = gap_y + GAP/2    │
        └─────────────┬──────────────┘
                      │
        ┌─────────────▼──────────────┐
        │  Calculate Distance        │
        │  dist = bird.y - target    │
        └─────────────┬──────────────┘
                      │
                 ┌────▼────┐
                 │dist > 2?│
                 └────┬────┘
                 YES  │  NO
                      │  │
                      │  └──► return 0
                      │
        ┌─────────────▼──────────────┐
        │  Check Velocity            │
        │  bird.velocity > -2?       │
        └─────────────┬──────────────┘
                      │
                 ┌────┴────┐
                 │  YES?   │
                 └────┬────┘
                 YES  │  NO
                      │  │
                      │  └──► return 0
                      │
        ┌─────────────▼──────────────┐
        │    return 1 (JUMP!)        │
        └────────────────────────────┘

Strategy:
- Bird below target + velocity not too negative → Jump
- Otherwise → Let gravity do the work
```

---

## 📊 Project Metrics

### Code Statistics
```
Lines of Code:
- main.c:     ~200 lines
- engine.c:   ~250 lines
- utils.c:    ~400 lines
- headers:    ~150 lines
Total:        ~1000 lines

File Count:   8 files
Dependencies: Standard C library only
Platform:     Linux (POSIX)
```

### Performance Targets
```
Target FPS:       30 FPS
Frame Time:       ~33ms
Input Latency:    <5ms
Memory Usage:     <1MB
CPU Usage:        <5% (single core)
Startup Time:     <100ms
```

### Testing Checklist
- [ ] Memory leak free (valgrind)
- [ ] No warnings (gcc -Wall -Wextra)
- [ ] Terminal restore on crash
- [ ] Proper signal handling
- [ ] File I/O error handling
- [ ] Boundary condition tests
- [ ] Collision accuracy tests
- [ ] AI behavior verification

---

**Last Updated**: January 2026  
**Status**: Core features complete ✅  
**Next Phase**: Polish & Enhancement 🚀
