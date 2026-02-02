# Makefile for Flappy Teemo Game Engine

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
LDFLAGS = 
TARGET = flappy-teemo
SRC_DIR = src
INC_DIR = include
DATA_DIR = data

# Source files
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/engine.c $(SRC_DIR)/utils.c
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Build executable
$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Build successful!"

# Compile object files
%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	@echo "Cleaning up..."
	rm -f $(OBJS) $(TARGET)
	rm -rf $(DATA_DIR)/*.dat
	@echo "Clean complete!"

# Create data directory
setup:
	@echo "Setting up directories..."
	mkdir -p $(DATA_DIR)
	@echo "Setup complete!"

# Run the game (player mode)
run: $(TARGET) setup
	./$(TARGET)

# Run in admin mode
run-admin: $(TARGET) setup
	./$(TARGET) --admin

# Run in spectator mode (AI)
run-spectator: $(TARGET) setup
	./$(TARGET) --spectator

# Install (copy to /usr/local/bin)
install: $(TARGET)
	@echo "Installing $(TARGET)..."
	sudo cp $(TARGET) /usr/local/bin/
	@echo "Installation complete!"

# Uninstall
uninstall:
	@echo "Uninstalling $(TARGET)..."
	sudo rm -f /usr/local/bin/$(TARGET)
	@echo "Uninstallation complete!"

# Help
help:
	@echo "Flappy Teemo - Makefile Help"
	@echo ""
	@echo "Available targets:"
	@echo "  all           - Build the game (default)"
	@echo "  clean         - Remove build files"
	@echo "  setup         - Create necessary directories"
	@echo "  run           - Build and run (player mode)"
	@echo "  run-admin     - Build and run (admin mode)"
	@echo "  run-spectator - Build and run (AI spectator mode)"
	@echo "  install       - Install to /usr/local/bin"
	@echo "  uninstall     - Remove from /usr/local/bin"
	@echo "  help          - Show this help message"

.PHONY: all clean setup run run-admin run-spectator install uninstall help
