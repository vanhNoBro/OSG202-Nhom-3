# Quick Start Guide - Flappy Teemo

## 🚀 5-Minute Setup

### Prerequisites
```bash
# Check if you have GCC
gcc --version

# Check if you have make
make --version
```

### Installation

```bash
# 1. Navigate to project directory
cd flappy-teemo

# 2. Build the game
make

# 3. Run!
./flappy-teemo
```

That's it! 🎉

## 🎮 First Time Playing

### Menu Navigation
```
┌─────────────────────────┐
│  FLAPPY TEEMO MENU      │
├─────────────────────────┤
│  [P] Play Game          │  ← Press P to start
│  [H] High Scores        │
│  [S] Settings           │
│  [A] Spectator Mode     │  ← Watch AI play
│  [Q] Quit               │
└─────────────────────────┘
```

### Basic Controls
- **SPACE** - Make Teemo jump (flap)
- **ESC/P** - Pause game / Return to menu
- **Q** - Quit

### Gameplay Tips
1. **Timing is everything**: Press SPACE at the right moment
2. **Don't spam**: Single taps work better than button mashing
3. **Watch ahead**: Look at upcoming pipes, not just Teemo
4. **Stay centered**: Middle of the screen is usually safest
5. **Practice**: First few tries will be tough - keep going!

## 🤖 Try the AI Mode

Want to see how it's done?

```bash
./flappy-teemo --spectator
```

The AI will play automatically. Perfect for:
- Learning the game mechanics
- Testing your code changes
- Just watching something cool!

## 🔧 Quick Commands

```bash
# Build and run (player mode)
make run

# Build and run (AI mode)
make run-spectator

# Build and run (debug mode)
make run-admin

# Clean build files
make clean

# View all available commands
make help
```

Or use the helper script:

```bash
# Build
./helper.sh build

# Run player mode
./helper.sh run

# Run AI mode
./helper.sh run-spectator

# Run admin mode (with debug info)
./helper.sh run-admin

# Check for memory leaks
./helper.sh check-memory

# View all commands
./helper.sh help
```

## 🎯 Your First Game

### Goal
Navigate Teemo through the pipes without crashing!

### Scoring
- Each pipe you pass = +1 point
- Try to beat your high score!
- High scores are automatically saved

### Game Over
When you crash:
- Press **R** to restart quickly
- Press **M** to return to menu
- Your high score is saved automatically

## 🐛 Something Wrong?

### Terminal looks weird after crash?
```bash
reset
# or
stty sane
```

### Game won't compile?
```bash
# Make sure you have build essentials
sudo apt-get install build-essential

# Clean and rebuild
make clean
make
```

### Can't see colors?
Check your terminal supports ANSI colors:
```bash
echo $TERM  # Should show something like "xterm-256color"
```

## 📚 Next Steps

Once you're comfortable with basic gameplay:

1. **Try Admin Mode**: See debug info and game internals
   ```bash
   ./flappy-teemo --admin
   ```

2. **Read the docs**: Learn about the system programming concepts
   - `README.md` - Full documentation
   - `docs/TECHNICAL.md` - Deep technical details
   - `docs/ROADMAP.md` - Flowcharts and architecture

3. **Modify the code**: 
   - Change `GRAVITY` in `engine.h` (easier/harder)
   - Adjust `PIPE_GAP` for difficulty
   - Modify colors in `utils.c`

4. **Check for memory leaks**:
   ```bash
   ./helper.sh check-memory
   ```

## 🏆 Challenge Yourself

### Difficulty Levels (Manual Modifications)

**Easy Mode** (in `include/engine.h`):
```c
#define PIPE_GAP 12          // Bigger gap (default: 8)
#define GRAVITY 0            // No gravity (default: 1)
#define PIPE_SPEED 1         // Slower pipes (default: 1)
```

**Hard Mode**:
```c
#define PIPE_GAP 6           // Smaller gap
#define GRAVITY 2            // More gravity
#define PIPE_SPEED 2         // Faster pipes
```

**Insane Mode**:
```c
#define PIPE_GAP 4           // Tiny gap
#define GRAVITY 3            // Heavy gravity
#define PIPE_SPEED 3         // Super fast
```

After changes, rebuild: `make clean && make`

## ❓ FAQ

**Q: Why does the game exit when I press Ctrl+C?**  
A: That's normal! We handle SIGINT to clean up properly.

**Q: Where is my high score saved?**  
A: In `data/highscore.dat` (binary format)

**Q: Can I reset my high score?**  
A: Yes! `./helper.sh reset` or delete `data/highscore.dat`

**Q: How do I backup my high score?**  
A: `./helper.sh backup`

**Q: The AI is too good/bad, can I adjust it?**  
A: Yes! Edit `calculate_ai_decision()` in `src/engine.c`

**Q: Can I run this on Windows?**  
A: Not directly. It's designed for Linux. Use WSL (Windows Subsystem for Linux).

**Q: Can I run this on Mac?**  
A: Probably! Most POSIX systems should work. Try it!

## 🎮 Happy Gaming!

Remember: 
- The first 10 tries are the hardest
- Watch the AI to learn strategies
- Have fun learning system programming!

**Now go beat that high score! 🚀**

---

Need more help? Check:
- Full README: `README.md`
- Technical docs: `docs/TECHNICAL.md`
- Roadmap & flowcharts: `docs/ROADMAP.md`
