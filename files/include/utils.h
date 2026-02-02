#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include "engine.h"

// Terminal Control
void setup_terminal();
void restore_terminal();
void clear_screen();
void move_cursor(int x, int y);
void hide_cursor();
void show_cursor();

// Rendering
void render_game(Game *game);
void render_menu();
void render_highscore(int score);
void render_game_over(Game *game);
void render_settings();
void render_debug_info(Game *game);

// Input handling
char get_input();
bool is_key_pressed();

// File I/O
void save_highscore(int score);
int load_highscore();

// Audio & Process Management
void start_background_music();
void stop_background_music();
void play_sound_effect(const char *sound_type);

// Signal handling
void setup_signal_handlers();
void cleanup_handler(int signum);

// Timing
void sleep_ms(int milliseconds);
long long get_time_ms();

// Color codes
#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN "\033[36m"
#define COLOR_WHITE "\033[37m"
#define COLOR_BRIGHT_GREEN "\033[92m"
#define COLOR_BRIGHT_YELLOW "\033[93m"

#define BG_BLACK "\033[40m"
#define BG_RED "\033[41m"
#define BG_GREEN "\033[42m"
#define BG_YELLOW "\033[43m"

#endif // UTILS_H
