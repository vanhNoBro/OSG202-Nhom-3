#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>

// Các hằng số game
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 24
#define BIRD_X 10
#define PIPE_WIDTH 6
#define PIPE_GAP 8
#define GRAVITY 1
#define JUMP_FORCE -3
#define PIPE_SPEED 1
#define PIPE_SPAWN_INTERVAL 35

// Cấu trúc Bird (Teemo)
typedef struct {
    int x;
    int y;
    int velocity;
    bool is_alive;
} Bird;

// Cấu trúc Pipe (Chướng ngại vật)
typedef struct Pipe {
    int x;
    int gap_y;  // Vị trí Y của khoảng trống
    struct Pipe *next;
} Pipe;

// Trạng thái game
typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_GAME_OVER,
    STATE_HIGHSCORE,
    STATE_SETTINGS,
    STATE_EXIT
} GameState;

// Chế độ chơi
typedef enum {
    MODE_PLAYER,
    MODE_ADMIN,
    MODE_SPECTATOR
} GameMode;

// Cấu trúc quản lý toàn bộ game
typedef struct {
    Bird bird;
    Pipe *pipe_head;
    int score;
    int high_score;
    GameState state;
    GameMode mode;
    int frame_count;
    bool debug_mode;
} Game;

// Khởi tạo và giải phóng
void init_game(Game *game, GameMode mode);
void cleanup_game(Game *game);

// Quản lý Bird
void init_bird(Bird *bird);
void update_bird(Bird *bird);
void bird_jump(Bird *bird);

// Quản lý Pipe
Pipe* create_pipe(int x, int gap_y);
void add_pipe(Game *game);
void update_pipes(Game *game);
void free_pipes(Pipe *head);
void remove_passed_pipes(Game *game);

// Va chạm và logic
bool check_collision(Game *game);
void update_score(Game *game);

// AI cho Spectator Mode
int calculate_ai_decision(Game *game);

#endif // ENGINE_H
