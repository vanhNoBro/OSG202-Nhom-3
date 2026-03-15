# Makefile – Flappy Teemo SDL2 (WSL/Linux)

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -Iinclude \
          $(shell sdl2-config --cflags) -O2
LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image -lm
TARGET  = flappy-teemo
SRCS    = src/main.c src/game.c
OBJS    = $(SRCS:.c=.o)

all: setup $(TARGET)

$(TARGET): $(OBJS)
	@echo "Linking..."
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Build xong! Chay: ./flappy-teemo"

%.o: %.c
	@echo "  CC $<"
	$(CC) $(CFLAGS) -c $< -o $@

setup:
	@mkdir -p data

clean:
	rm -f $(OBJS) $(TARGET)

run: all
	./$(TARGET)

.PHONY: all setup clean run