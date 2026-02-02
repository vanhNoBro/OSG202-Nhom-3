# Flappy Teemo - Project Structure

## 📁 Complete Directory Tree

```
flappy-teemo/
│
├── 📄 README.md                 # Main documentation
├── 📄 QUICKSTART.md            # Quick start guide
├── 📄 CHANGELOG.md             # Version history
├── 📄 LICENSE                  # MIT License
├── 📄 Makefile                 # Build automation
├── 📄 .gitignore              # Git ignore rules
├── 🔧 helper.sh                # Development helper script
│
├── 📂 src/                     # Source code files
│   ├── main.c                  # Game controller & loop
│   ├── engine.c                # Game logic & physics
│   └── utils.c                 # System utilities
│
├── 📂 include/                 # Header files
│   ├── engine.h                # Engine declarations
│   └── utils.h                 # Utility declarations
│
├── 📂 data/                    # Runtime data
│   └── highscore.dat          # Binary high score file (created at runtime)
│
└── 📂 docs/                    # Documentation
    ├── TECHNICAL.md            # Technical deep dive
    └── ROADMAP.md              # Development roadmap & flowcharts
```

## 📊 File Breakdown

### Core Source Files

#### `src/main.c` (~200 lines)
**Purpose**: Game Controller Layer
- Main entry point
- Game loop management (30 FPS)
- State machine (Menu, Playing, Game Over, etc.)
- Input handling for all states
- Command-line argument parsing

**Key Functions**:
```c
int main(int argc, char *argv[])
void game_loop(Game *game)
void handle_menu_input(Game *game)
void handle_game_input(Game *game)
void handle_game_over_input(Game *game)
```

#### `src/engine.c` (~250 lines)
**Purpose**: Game Logic & Physics Layer
- Physics engine (gravity, velocity)
- Collision detection
- Dynamic pipe management (linked list)
- AI algorithm for spectator mode
- Score tracking

**Key Functions**:
```c
void init_game(Game *game, GameMode mode)
void update_bird(Bird *bird)
void bird_jump(Bird *bird)
Pipe* create_pipe(int x, int gap_y)
void add_pipe(Game *game)
void update_pipes(Game *game)
bool check_collision(Game *game)
int calculate_ai_decision(Game *game)
```

#### `src/utils.c` (~400 lines)
**Purpose**: System & UI Layer
- Terminal control (termios, fcntl)
- ANSI rendering engine
- File I/O (binary format)
- Process management (fork, exec)
- Signal handling
- Sound effects (stub implementation)

**Key Functions**:
```c
void setup_terminal()
void restore_terminal()
void render_game(Game *game)
void render_menu()
char get_input()
void save_highscore(int score)
int load_highscore()
void start_background_music()
void cleanup_handler(int signum)
```

### Header Files

#### `include/engine.h` (~90 lines)
**Purpose**: Core data structures and function declarations

**Key Structures**:
```c
typedef struct Bird {
    int x, y;
    int velocity;
    bool is_alive;
} Bird;

typedef struct Pipe {
    int x;
    int gap_y;
    struct Pipe *next;  // Linked list
} Pipe;

typedef struct Game {
    Bird bird;
    Pipe *pipe_head;
    int score;
    int high_score;
    GameState state;
    GameMode mode;
    int frame_count;
    bool debug_mode;
} Game;
```

**Enumerations**:
```c
typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_GAME_OVER,
    STATE_HIGHSCORE,
    STATE_SETTINGS,
    STATE_EXIT
} GameState;

typedef enum {
    MODE_PLAYER,
    MODE_ADMIN,
    MODE_SPECTATOR
} GameMode;
```

#### `include/utils.h` (~60 lines)
**Purpose**: Utility function declarations and constants

**Categories**:
- Terminal Control Functions
- Rendering Functions
- Input Functions
- File I/O Functions
- Audio & Process Management
- Signal Handling
- Color Constants (ANSI escape codes)

### Build Files

#### `Makefile` (~100 lines)
**Targets**:
```makefile
all              # Build executable
clean            # Remove build artifacts
setup            # Create directories
run              # Build and run (player)
run-admin        # Build and run (admin)
run-spectator    # Build and run (AI)
install          # Install to /usr/local/bin
uninstall        # Remove from system
help             # Show help
```

**Variables**:
```makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
SRCS = src/main.c src/engine.c src/utils.c
```

#### `helper.sh` (~200 lines)
**Commands**:
```bash
build            # Build project
clean            # Clean artifacts
run              # Run player mode
run-admin        # Run admin mode
run-spectator    # Run AI mode
test             # Run tests
check-memory     # Valgrind check
install          # System install
reset            # Reset high score
backup           # Backup score
restore          # Restore score
```

### Documentation

#### `README.md` (~500 lines)
- Project overview
- Feature list
- Installation guide
- Usage instructions
- System programming concepts explained
- Architecture diagrams
- Troubleshooting

#### `docs/TECHNICAL.md` (~800 lines)
- Detailed architecture
- Implementation details
- Algorithm explanations
- Code examples
- Performance analysis
- Complexity analysis
- Debugging tips

#### `docs/ROADMAP.md` (~600 lines)
- Development timeline
- Feature roadmap
- Flowcharts (ASCII art)
- State diagrams
- Memory management flow
- AI decision trees
- Testing checklist

#### `QUICKSTART.md` (~300 lines)
- 5-minute setup
- First-time playing guide
- Quick commands
- FAQ
- Tips and tricks

#### `CHANGELOG.md` (~100 lines)
- Version history
- Feature additions
- Bug fixes
- Future plans

## 📈 Code Metrics

### Lines of Code
```
Source Files:
- main.c:      ~200 LOC
- engine.c:    ~250 LOC
- utils.c:     ~400 LOC
- headers:     ~150 LOC
Total Code:    ~1000 LOC

Documentation:
- README.md:      ~500 lines
- TECHNICAL.md:   ~800 lines
- ROADMAP.md:     ~600 lines
- QUICKSTART.md:  ~300 lines
Total Docs:       ~2200 lines

Scripts:
- Makefile:    ~100 lines
- helper.sh:   ~200 lines
Total Scripts: ~300 lines

Grand Total: ~3500 lines (code + docs + scripts)
```

### Complexity Metrics
```
Functions:       ~40 functions
Structures:      4 main structs
Enumerations:    2 enums
Dependencies:    Standard C library only
External Tools:  make, gcc, (optional: valgrind)
```

## 🔗 Module Dependencies

```
main.c
  ├─ Includes: engine.h, utils.h
  ├─ Calls: All engine.c functions
  └─ Calls: All utils.c functions

engine.c
  ├─ Includes: engine.h, utils.h
  ├─ Implements: All game logic
  └─ Calls: utils.c (load_highscore, play_sound_effect)

utils.c
  ├─ Includes: utils.h, engine.h
  ├─ Implements: All system functions
  └─ Standalone (no dependencies on engine.c)
```

## 🎯 File Purposes Summary

| File | Purpose | Layer | Dependencies |
|------|---------|-------|--------------|
| main.c | Game controller & loop | Controller | engine.h, utils.h |
| engine.c | Game logic & physics | Logic | engine.h, utils.h |
| utils.c | System & rendering | System | utils.h, engine.h |
| engine.h | Data structures | Interface | None |
| utils.h | Utility declarations | Interface | engine.h |
| Makefile | Build automation | Build | None |
| helper.sh | Dev convenience | Tools | None |

## 📦 Runtime Files

### Generated at Build Time
- `src/*.o` - Object files (temporary)
- `flappy-teemo` - Executable binary

### Generated at Runtime
- `data/highscore.dat` - Binary high score (4 bytes)
- `data/highscore.dat.backup.*` - Backups (if created)

## 🛠️ Development Workflow

```
Edit Source
    ↓
make clean
    ↓
make
    ↓
./flappy-teemo --admin    (Test with debug)
    ↓
./helper.sh check-memory  (Check leaks)
    ↓
git commit
```

## 📚 Learning Path

1. **Start Here**: `QUICKSTART.md`
2. **Understand**: `README.md`
3. **Deep Dive**: `docs/TECHNICAL.md`
4. **Visualize**: `docs/ROADMAP.md` (flowcharts)
5. **Code**: Read `src/` files in order: main.c → engine.c → utils.c

## 🎓 Educational Value

This project demonstrates:
- ✅ Modular C programming
- ✅ Data structures (linked lists)
- ✅ Memory management
- ✅ System programming
- ✅ Terminal I/O
- ✅ Process management
- ✅ Signal handling
- ✅ File operations
- ✅ Game loop architecture
- ✅ State machines
- ✅ Real-time systems

---

**Total Project Size**: ~50 KB source code  
**Compiled Binary**: ~30-50 KB  
**Memory Usage**: <1 MB runtime  
**Build Time**: ~2 seconds  

**Last Updated**: January 29, 2026
