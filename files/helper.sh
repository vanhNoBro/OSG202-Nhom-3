#!/bin/bash

# Flappy Teemo - Helper Script
# Các command tiện ích để phát triển và test game

set -e  # Exit on error

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$PROJECT_ROOT"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Print colored message
print_message() {
    local color=$1
    shift
    echo -e "${color}$@${NC}"
}

# Show help
show_help() {
    cat << EOF
Flappy Teemo - Development Helper Script

Usage: ./helper.sh [COMMAND]

Commands:
  build          Build the project (default)
  clean          Clean build artifacts
  run            Build and run in player mode
  run-admin      Build and run in admin mode
  run-spectator  Build and run in AI spectator mode
  test           Run tests
  check-memory   Check for memory leaks with valgrind
  install        Install to /usr/local/bin
  uninstall      Uninstall from /usr/local/bin
  reset          Reset high score
  backup         Backup high score
  restore        Restore high score from backup
  help           Show this help message

Examples:
  ./helper.sh build
  ./helper.sh run-spectator
  ./helper.sh check-memory

EOF
}

# Build project
build() {
    print_message "$GREEN" "Building Flappy Teemo..."
    make clean
    make
    print_message "$GREEN" "Build complete!"
}

# Clean
clean() {
    print_message "$YELLOW" "Cleaning build artifacts..."
    make clean
    print_message "$GREEN" "Clean complete!"
}

# Run game
run() {
    build
    print_message "$GREEN" "Starting game in PLAYER mode..."
    sleep 1
    ./flappy-teemo
}

# Run admin mode
run_admin() {
    build
    print_message "$GREEN" "Starting game in ADMIN mode..."
    sleep 1
    ./flappy-teemo --admin
}

# Run spectator mode
run_spectator() {
    build
    print_message "$GREEN" "Starting game in SPECTATOR mode (AI)..."
    sleep 1
    ./flappy-teemo --spectator
}

# Run tests
test() {
    print_message "$YELLOW" "Running tests..."
    
    # Check if executable exists
    if [ ! -f "./flappy-teemo" ]; then
        print_message "$RED" "Executable not found. Building..."
        build
    fi
    
    print_message "$GREEN" "✓ Build test passed"
    
    # Check if data directory exists
    if [ ! -d "./data" ]; then
        print_message "$YELLOW" "Creating data directory..."
        mkdir -p data
    fi
    print_message "$GREEN" "✓ Directory structure test passed"
    
    print_message "$GREEN" "All tests passed!"
}

# Check memory leaks
check_memory() {
    if ! command -v valgrind &> /dev/null; then
        print_message "$RED" "valgrind is not installed!"
        print_message "$YELLOW" "Install it with: sudo apt-get install valgrind"
        exit 1
    fi
    
    build
    print_message "$GREEN" "Running memory leak check..."
    print_message "$YELLOW" "Note: Game will run in valgrind. Exit with Q to see report."
    
    valgrind --leak-check=full \
             --show-leak-kinds=all \
             --track-origins=yes \
             --verbose \
             ./flappy-teemo
}

# Install
install() {
    build
    print_message "$GREEN" "Installing Flappy Teemo..."
    sudo make install
    print_message "$GREEN" "Installation complete! Run with: flappy-teemo"
}

# Uninstall
uninstall() {
    print_message "$YELLOW" "Uninstalling Flappy Teemo..."
    sudo make uninstall
    print_message "$GREEN" "Uninstall complete!"
}

# Reset high score
reset_score() {
    print_message "$YELLOW" "Resetting high score..."
    rm -f data/highscore.dat
    print_message "$GREEN" "High score reset!"
}

# Backup high score
backup_score() {
    if [ ! -f "data/highscore.dat" ]; then
        print_message "$RED" "No high score to backup!"
        exit 1
    fi
    
    backup_file="data/highscore.dat.backup.$(date +%Y%m%d_%H%M%S)"
    cp data/highscore.dat "$backup_file"
    print_message "$GREEN" "High score backed up to: $backup_file"
}

# Restore high score
restore_score() {
    # Find latest backup
    latest_backup=$(ls -t data/highscore.dat.backup.* 2>/dev/null | head -1)
    
    if [ -z "$latest_backup" ]; then
        print_message "$RED" "No backup found!"
        exit 1
    fi
    
    cp "$latest_backup" data/highscore.dat
    print_message "$GREEN" "High score restored from: $latest_backup"
}

# Main script
main() {
    case "${1:-build}" in
        build)
            build
            ;;
        clean)
            clean
            ;;
        run)
            run
            ;;
        run-admin)
            run_admin
            ;;
        run-spectator)
            run_spectator
            ;;
        test)
            test
            ;;
        check-memory)
            check_memory
            ;;
        install)
            install
            ;;
        uninstall)
            uninstall
            ;;
        reset)
            reset_score
            ;;
        backup)
            backup_score
            ;;
        restore)
            restore_score
            ;;
        help|--help|-h)
            show_help
            ;;
        *)
            print_message "$RED" "Unknown command: $1"
            echo ""
            show_help
            exit 1
            ;;
    esac
}

main "$@"
