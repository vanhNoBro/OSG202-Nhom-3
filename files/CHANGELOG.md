# Changelog

All notable changes to the Flappy Teemo project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2026-01-29

### Added
- ✨ Complete game engine with physics system (gravity, velocity)
- 🎮 Three game modes: Player, Admin, Spectator (AI)
- 🧠 AI algorithm for automatic gameplay
- 💾 High score persistence (binary file format)
- 🎨 ANSI-colored terminal UI with smooth rendering
- ⌨️ Non-blocking keyboard input (termios + fcntl)
- 🔧 Admin mode with debug information
- 📊 Real-time collision detection
- 🔗 Dynamic memory management with linked lists
- 🎯 Score tracking and game state management
- 📝 Comprehensive documentation (README, TECHNICAL, ROADMAP)
- 🛠️ Makefile for easy building
- 🚀 Helper script for common tasks
- 🔒 Signal handling for clean exit (SIGINT, SIGTERM)
- 📦 Modular architecture (main, engine, utils)

### Technical Details
- **Language**: C (C99 standard)
- **Platform**: Linux (POSIX)
- **Dependencies**: Standard C library only
- **FPS**: 30 frames per second
- **Memory**: <1MB runtime usage
- **Code**: ~1000 lines of C code

### File Structure
```
flappy-teemo/
├── src/          # Source files
├── include/      # Header files
├── data/         # Runtime data
├── docs/         # Documentation
├── Makefile      # Build system
├── helper.sh     # Development script
└── README.md     # Main documentation
```

### Known Issues
- Sound effects not yet implemented
- Terminal size must be at least 80x24
- Some warnings on certain GCC versions (non-critical)

### Future Plans
- [ ] Sound effects integration
- [ ] Difficulty levels
- [ ] Multiplayer mode
- [ ] Custom themes
- [ ] Network leaderboard
- [ ] Performance dashboard

---

## [0.9.0] - 2026-01-22 (Beta)

### Added
- Initial prototype with basic gameplay
- Simple collision detection
- Manual memory management

### Fixed
- Memory leaks in pipe management
- Terminal restoration issues

---

## [0.5.0] - 2026-01-15 (Alpha)

### Added
- Project structure setup
- Basic terminal control
- Data structures definition

---

**Legend**:
- ✨ New feature
- 🐛 Bug fix
- 🔧 Maintenance
- 📝 Documentation
- ⚡ Performance
- 🎨 UI/UX improvement
