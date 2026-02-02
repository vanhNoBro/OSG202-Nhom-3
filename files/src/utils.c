#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "../include/utils.h"
#include "../include/engine.h"

// Biến toàn cục cho terminal
static struct termios orig_termios;
static pid_t music_pid = -1;

// Thiết lập terminal ở chế độ raw
void setup_terminal() {
    struct termios raw;
    
    tcgetattr(STDIN_FILENO, &orig_termios);
    raw = orig_termios;
    
    // Tắt canonical mode và echo
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    
    // Thiết lập non-blocking input
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    
    hide_cursor();
}

// Khôi phục terminal về trạng thái ban đầu
void restore_terminal() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    show_cursor();
    clear_screen();
    move_cursor(0, 0);
}

// Xóa màn hình
void clear_screen() {
    printf("\033[2J");
    fflush(stdout);
}

// Di chuyển con trỏ
void move_cursor(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1);
}

// Ẩn con trỏ
void hide_cursor() {
    printf("\033[?25l");
    fflush(stdout);
}

// Hiện con trỏ
void show_cursor() {
    printf("\033[?25h");
    fflush(stdout);
}

// Đọc input không chặn
char get_input() {
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1) {
        return c;
    }
    return 0;
}

// Kiểm tra phím có được nhấn không
bool is_key_pressed() {
    char c = get_input();
    return (c != 0);
}

// Vẽ game
void render_game(Game *game) {
    clear_screen();
    
    // Vẽ viền trên
    move_cursor(0, 0);
    printf(COLOR_CYAN);
    for (int i = 0; i < SCREEN_WIDTH; i++) printf("=");
    printf(COLOR_RESET);
    
    // Vẽ viền dưới
    move_cursor(0, SCREEN_HEIGHT - 1);
    printf(COLOR_CYAN);
    for (int i = 0; i < SCREEN_WIDTH; i++) printf("=");
    printf(COLOR_RESET);
    
    // Vẽ pipes
    Pipe *current = game->pipe_head;
    while (current != NULL) {
        if (current->x >= 0 && current->x < SCREEN_WIDTH) {
            // Vẽ phần trên
            for (int y = 1; y < current->gap_y; y++) {
                move_cursor(current->x, y);
                printf(COLOR_GREEN BG_GREEN);
                for (int i = 0; i < PIPE_WIDTH && current->x + i < SCREEN_WIDTH; i++) {
                    printf("█");
                }
                printf(COLOR_RESET);
            }
            
            // Vẽ phần dưới
            for (int y = current->gap_y + PIPE_GAP; y < SCREEN_HEIGHT - 1; y++) {
                move_cursor(current->x, y);
                printf(COLOR_GREEN BG_GREEN);
                for (int i = 0; i < PIPE_WIDTH && current->x + i < SCREEN_WIDTH; i++) {
                    printf("█");
                }
                printf(COLOR_RESET);
            }
        }
        current = current->next;
    }
    
    // Vẽ bird (Teemo)
    if (game->bird.is_alive) {
        move_cursor(game->bird.x, game->bird.y);
        printf(COLOR_BRIGHT_YELLOW "⊂( ◕ ‿ ◕ )⊃" COLOR_RESET);
    } else {
        move_cursor(game->bird.x, game->bird.y);
        printf(COLOR_RED "✗ ﹏ ✗" COLOR_RESET);
    }
    
    // Hiển thị điểm
    move_cursor(2, 0);
    printf(COLOR_YELLOW " Score: %d | High: %d " COLOR_RESET, 
           game->score, game->high_score);
    
    // Hiển thị mode
    move_cursor(SCREEN_WIDTH - 20, 0);
    const char *mode_str = game->mode == MODE_PLAYER ? "PLAYER" : 
                          game->mode == MODE_ADMIN ? "ADMIN" : "SPECTATOR";
    printf(COLOR_MAGENTA " [%s] " COLOR_RESET, mode_str);
    
    // Debug info
    if (game->debug_mode) {
        render_debug_info(game);
    }
    
    fflush(stdout);
}

// Vẽ menu
void render_menu() {
    clear_screen();
    
    move_cursor(SCREEN_WIDTH / 2 - 15, 3);
    printf(COLOR_BRIGHT_YELLOW "╔══════════════════════════════╗" COLOR_RESET);
    move_cursor(SCREEN_WIDTH / 2 - 15, 4);
    printf(COLOR_BRIGHT_YELLOW "║   FLAPPY TEEMO - GAME ENGINE ║" COLOR_RESET);
    move_cursor(SCREEN_WIDTH / 2 - 15, 5);
    printf(COLOR_BRIGHT_YELLOW "╚══════════════════════════════╝" COLOR_RESET);
    
    move_cursor(SCREEN_WIDTH / 2 - 10, 8);
    printf(COLOR_GREEN "[P] Play Game" COLOR_RESET);
    
    move_cursor(SCREEN_WIDTH / 2 - 10, 10);
    printf(COLOR_CYAN "[H] High Scores" COLOR_RESET);
    
    move_cursor(SCREEN_WIDTH / 2 - 10, 12);
    printf(COLOR_MAGENTA "[S] Settings" COLOR_RESET);
    
    move_cursor(SCREEN_WIDTH / 2 - 10, 14);
    printf(COLOR_YELLOW "[A] Spectator Mode (AI)" COLOR_RESET);
    
    move_cursor(SCREEN_WIDTH / 2 - 10, 16);
    printf(COLOR_RED "[Q] Quit" COLOR_RESET);
    
    move_cursor(SCREEN_WIDTH / 2 - 15, SCREEN_HEIGHT - 3);
    printf(COLOR_WHITE "Press SPACE to jump | ESC to pause" COLOR_RESET);
    
    fflush(stdout);
}

// Vẽ highscore
void render_highscore(int score) {
    clear_screen();
    
    move_cursor(SCREEN_WIDTH / 2 - 10, 5);
    printf(COLOR_BRIGHT_YELLOW "═══ HIGH SCORE ═══" COLOR_RESET);
    
    move_cursor(SCREEN_WIDTH / 2 - 5, 8);
    printf(COLOR_GREEN "Score: %d" COLOR_RESET, score);
    
    move_cursor(SCREEN_WIDTH / 2 - 15, 12);
    printf(COLOR_WHITE "Press any key to return..." COLOR_RESET);
    
    fflush(stdout);
}

// Vẽ game over
void render_game_over(Game *game) {
    move_cursor(SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2 - 2);
    printf(COLOR_RED "╔════════════════╗" COLOR_RESET);
    move_cursor(SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2 - 1);
    printf(COLOR_RED "║  GAME  OVER!   ║" COLOR_RESET);
    move_cursor(SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2);
    printf(COLOR_RED "╚════════════════╝" COLOR_RESET);
    
    move_cursor(SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2 + 2);
    printf(COLOR_YELLOW "Final Score: %d" COLOR_RESET, game->score);
    
    if (game->score >= game->high_score) {
        move_cursor(SCREEN_WIDTH / 2 - 12, SCREEN_HEIGHT / 2 + 3);
        printf(COLOR_BRIGHT_GREEN "★ NEW HIGH SCORE! ★" COLOR_RESET);
    }
    
    move_cursor(SCREEN_WIDTH / 2 - 15, SCREEN_HEIGHT / 2 + 5);
    printf(COLOR_WHITE "Press R to restart | M for menu" COLOR_RESET);
    
    fflush(stdout);
}

// Vẽ settings
void render_settings() {
    clear_screen();
    
    move_cursor(SCREEN_WIDTH / 2 - 10, 5);
    printf(COLOR_CYAN "═══ SETTINGS ═══" COLOR_RESET);
    
    move_cursor(SCREEN_WIDTH / 2 - 15, 8);
    printf(COLOR_WHITE "Sound: ON (Feature coming soon)" COLOR_RESET);
    
    move_cursor(SCREEN_WIDTH / 2 - 15, 10);
    printf(COLOR_WHITE "Difficulty: Normal" COLOR_RESET);
    
    move_cursor(SCREEN_WIDTH / 2 - 15, 14);
    printf(COLOR_WHITE "Press M to return to menu..." COLOR_RESET);
    
    fflush(stdout);
}

// Debug info
void render_debug_info(Game *game) {
    move_cursor(2, SCREEN_HEIGHT - 2);
    printf(COLOR_CYAN "[DEBUG] Bird Y:%d V:%d | Pipes:%d | Frame:%d" COLOR_RESET,
           game->bird.y, game->bird.velocity, 
           0, // Đếm số pipe
           game->frame_count);
    
    // Đếm pipes
    int pipe_count = 0;
    Pipe *p = game->pipe_head;
    while (p) {
        pipe_count++;
        p = p->next;
    }
    
    move_cursor(2, SCREEN_HEIGHT - 2);
    printf(COLOR_CYAN "[DEBUG] Bird Y:%d V:%d | Pipes:%d | Frame:%d" COLOR_RESET,
           game->bird.y, game->bird.velocity, pipe_count, game->frame_count);
}

// Lưu highscore
void save_highscore(int score) {
    FILE *file = fopen("data/highscore.dat", "wb");
    if (file) {
        fwrite(&score, sizeof(int), 1, file);
        fclose(file);
    }
}

// Tải highscore
int load_highscore() {
    FILE *file = fopen("data/highscore.dat", "rb");
    int score = 0;
    
    if (file) {
        fread(&score, sizeof(int), 1, file);
        fclose(file);
    }
    
    return score;
}

// Phát nhạc nền
void start_background_music() {
    // Fork process để phát nhạc
    music_pid = fork();
    
    if (music_pid == 0) {
        // Child process
        // Redirect output to /dev/null
        int fd = open("/dev/null", O_WRONLY);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);
        
        // Có thể dùng beep hoặc aplay
        // execlp("beep", "beep", "-f", "440", "-l", "10000", NULL);
        exit(0);
    }
}

// Dừng nhạc nền
void stop_background_music() {
    if (music_pid > 0) {
        kill(music_pid, SIGTERM);
        waitpid(music_pid, NULL, 0);
        music_pid = -1;
    }
}

// Phát hiệu ứng âm thanh
void play_sound_effect(const char *sound_type) {
    // Fork để không block game
    pid_t pid = fork();
    
    if (pid == 0) {
        int fd = open("/dev/null", O_WRONLY);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);
        
        // Có thể dùng beep với tần số khác nhau
        if (strcmp(sound_type, "jump") == 0) {
            // execlp("beep", "beep", "-f", "800", "-l", "50", NULL);
        } else if (strcmp(sound_type, "score") == 0) {
            // execlp("beep", "beep", "-f", "1000", "-l", "100", NULL);
        } else if (strcmp(sound_type, "die") == 0) {
            // execlp("beep", "beep", "-f", "200", "-l", "200", NULL);
        }
        
        exit(0);
    } else if (pid > 0) {
        // Parent không chờ
    }
}

// Thiết lập signal handlers
void setup_signal_handlers() {
    signal(SIGINT, cleanup_handler);
    signal(SIGTERM, cleanup_handler);
}

// Cleanup khi nhận signal
void cleanup_handler(int signum) {
    (void)signum;  // Suppress unused warning
    stop_background_music();
    restore_terminal();
    exit(0);
}

// Sleep milliseconds
void sleep_ms(int milliseconds) {
    usleep(milliseconds * 1000);
}

// Lấy thời gian hiện tại (ms)
long long get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000);
}
