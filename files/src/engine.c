#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../include/engine.h"
#include "../include/utils.h"

// Khởi tạo game
void init_game(Game *game, GameMode mode) {
    memset(game, 0, sizeof(Game));
    game->mode = mode;
    game->state = STATE_MENU;
    game->high_score = load_highscore();
    game->debug_mode = (mode == MODE_ADMIN);
    
    init_bird(&game->bird);
    game->pipe_head = NULL;
    game->score = 0;
    game->frame_count = 0;
    
    // Seed random
    srand(time(NULL));
}

// Giải phóng bộ nhớ
void cleanup_game(Game *game) {
    free_pipes(game->pipe_head);
    game->pipe_head = NULL;
}

// Khởi tạo Bird
void init_bird(Bird *bird) {
    bird->x = BIRD_X;
    bird->y = SCREEN_HEIGHT / 2;
    bird->velocity = 0;
    bird->is_alive = true;
}

// Cập nhật vật lý Bird
void update_bird(Bird *bird) {
    if (!bird->is_alive) return;
    
    // Áp dụng trọng lực
    bird->velocity += GRAVITY;
    bird->y += bird->velocity;
    
    // Giới hạn velocity
    if (bird->velocity > 5) bird->velocity = 5;
    if (bird->velocity < -5) bird->velocity = -5;
    
    // Kiểm tra biên
    if (bird->y < 0) {
        bird->y = 0;
        bird->velocity = 0;
    }
    if (bird->y >= SCREEN_HEIGHT - 1) {
        bird->y = SCREEN_HEIGHT - 1;
        bird->is_alive = false;
    }
}

// Nhảy
void bird_jump(Bird *bird) {
    if (bird->is_alive) {
        bird->velocity = JUMP_FORCE;
    }
}

// Tạo pipe mới
Pipe* create_pipe(int x, int gap_y) {
    Pipe *pipe = (Pipe*)malloc(sizeof(Pipe));
    if (!pipe) return NULL;
    
    pipe->x = x;
    pipe->gap_y = gap_y;
    pipe->next = NULL;
    
    return pipe;
}

// Thêm pipe vào danh sách
void add_pipe(Game *game) {
    // Tạo vị trí ngẫu nhiên cho khoảng trống
    int gap_y = (rand() % (SCREEN_HEIGHT - PIPE_GAP - 4)) + 2;
    
    Pipe *new_pipe = create_pipe(SCREEN_WIDTH - 1, gap_y);
    if (!new_pipe) return;
    
    // Thêm vào đầu danh sách
    if (game->pipe_head == NULL) {
        game->pipe_head = new_pipe;
    } else {
        // Tìm pipe cuối cùng
        Pipe *current = game->pipe_head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_pipe;
    }
}

// Cập nhật vị trí các pipe
void update_pipes(Game *game) {
    Pipe *current = game->pipe_head;
    
    while (current != NULL) {
        current->x -= PIPE_SPEED;
        current = current->next;
    }
    
    // Xóa các pipe đã qua màn hình
    remove_passed_pipes(game);
}

// Xóa các pipe đã đi qua
void remove_passed_pipes(Game *game) {
    while (game->pipe_head != NULL && game->pipe_head->x < -PIPE_WIDTH) {
        Pipe *temp = game->pipe_head;
        game->pipe_head = game->pipe_head->next;
        free(temp);
    }
}

// Giải phóng tất cả pipe
void free_pipes(Pipe *head) {
    while (head != NULL) {
        Pipe *temp = head;
        head = head->next;
        free(temp);
    }
}

// Kiểm tra va chạm
bool check_collision(Game *game) {
    Bird *bird = &game->bird;
    Pipe *current = game->pipe_head;
    
    // Kiểm tra va chạm với biên trên/dưới
    if (bird->y <= 0 || bird->y >= SCREEN_HEIGHT - 1) {
        return true;
    }
    
    // Kiểm tra va chạm với pipe
    while (current != NULL) {
        // Chim ở trong phạm vi X của pipe
        if (bird->x + 2 >= current->x && bird->x <= current->x + PIPE_WIDTH) {
            // Kiểm tra va chạm với phần trên hoặc dưới
            if (bird->y < current->gap_y || bird->y > current->gap_y + PIPE_GAP) {
                return true;
            }
        }
        current = current->next;
    }
    
    return false;
}

// Cập nhật điểm
void update_score(Game *game) {
    Pipe *current = game->pipe_head;
    
    while (current != NULL) {
        // Pipe vừa đi qua bird
        if (current->x + PIPE_WIDTH == game->bird.x && game->bird.is_alive) {
            game->score++;
            if (game->score > game->high_score) {
                game->high_score = game->score;
            }
            play_sound_effect("score");
        }
        current = current->next;
    }
}

// AI cho Spectator Mode
int calculate_ai_decision(Game *game) {
    Pipe *nearest_pipe = game->pipe_head;
    
    // Tìm pipe gần nhất
    while (nearest_pipe != NULL && nearest_pipe->x < game->bird.x) {
        nearest_pipe = nearest_pipe->next;
    }
    
    if (nearest_pipe == NULL) {
        return 0; // Không làm gì
    }
    
    // Tính toán điểm giữa khoảng trống
    int target_y = nearest_pipe->gap_y + PIPE_GAP / 2;
    
    // Nếu bird ở dưới target và velocity không quá âm
    if (game->bird.y > target_y + 2 && game->bird.velocity > -2) {
        return 1; // Nhảy
    }
    
    return 0; // Không nhảy
}
