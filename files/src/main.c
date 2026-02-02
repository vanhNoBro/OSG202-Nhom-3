#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "../include/engine.h"
#include "../include/utils.h"

#define FPS 30
#define FRAME_TIME_MS (1000 / FPS)

// Biến toàn cục cho cleanup
Game *global_game = NULL;

// Forward declarations
void handle_menu_input(Game *game);
void handle_game_input(Game *game);
void handle_game_over_input(Game *game);
void game_loop(Game *game);

int main(int argc, char *argv[]) {
    // Xác định game mode từ arguments
    GameMode mode = MODE_PLAYER;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--admin") == 0 || strcmp(argv[i], "-a") == 0) {
            mode = MODE_ADMIN;
            printf("Starting in ADMIN mode...\n");
            sleep(1);
        } else if (strcmp(argv[i], "--spectator") == 0 || strcmp(argv[i], "-s") == 0) {
            mode = MODE_SPECTATOR;
            printf("Starting in SPECTATOR mode (AI)...\n");
            sleep(1);
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf("Flappy Teemo - Console Game Engine\n\n");
            printf("Usage: %s [OPTIONS]\n\n", argv[0]);
            printf("Options:\n");
            printf("  -a, --admin      Enable admin/debug mode\n");
            printf("  -s, --spectator  Enable AI spectator mode\n");
            printf("  -h, --help       Show this help message\n\n");
            printf("Controls:\n");
            printf("  SPACE    - Jump\n");
            printf("  ESC      - Pause/Menu\n");
            printf("  Q        - Quit\n");
            return 0;
        }
    }
    
    // Tạo thư mục data nếu chưa có
    system("mkdir -p data");
    
    // Khởi tạo game
    Game game;
    global_game = &game;
    init_game(&game, mode);
    
    // Setup
    setup_terminal();
    setup_signal_handlers();
    
    // Main game loop
    game_loop(&game);
    
    // Cleanup
    cleanup_game(&game);
    restore_terminal();
    
    return 0;
}

// Game loop chính
void game_loop(Game *game) {
    long long last_frame_time = get_time_ms();
    long long last_pipe_spawn = 0;
    bool running = true;
    
    while (running) {
        long long current_time = get_time_ms();
        long long delta_time = current_time - last_frame_time;
        
        // Giới hạn FPS
        if (delta_time < FRAME_TIME_MS) {
            sleep_ms(FRAME_TIME_MS - delta_time);
            continue;
        }
        
        last_frame_time = current_time;
        game->frame_count++;
        
        // Xử lý theo state
        switch (game->state) {
            case STATE_MENU:
                render_menu();
                handle_menu_input(game);
                break;
                
            case STATE_PLAYING:
                // Spawn pipes
                if (current_time - last_pipe_spawn > PIPE_SPAWN_INTERVAL * FRAME_TIME_MS) {
                    add_pipe(game);
                    last_pipe_spawn = current_time;
                }
                
                // AI input (spectator mode)
                if (game->mode == MODE_SPECTATOR && game->bird.is_alive) {
                    if (calculate_ai_decision(game) == 1) {
                        bird_jump(&game->bird);
                    }
                }
                
                // Update physics
                update_bird(&game->bird);
                update_pipes(game);
                update_score(game);
                
                // Check collision
                if (check_collision(game)) {
                    game->bird.is_alive = false;
                    play_sound_effect("die");
                    
                    // Lưu highscore
                    if (game->score > game->high_score) {
                        save_highscore(game->score);
                    }
                    
                    game->state = STATE_GAME_OVER;
                }
                
                // Render
                render_game(game);
                
                // Handle input
                handle_game_input(game);
                break;
                
            case STATE_GAME_OVER:
                render_game(game);
                render_game_over(game);
                handle_game_over_input(game);
                break;
                
            case STATE_HIGHSCORE:
                render_highscore(game->high_score);
                
                // Chờ input
                char c = get_input();
                if (c != 0) {
                    game->state = STATE_MENU;
                }
                break;
                
            case STATE_SETTINGS:
                render_settings();
                
                c = get_input();
                if (c == 'm' || c == 'M' || c == 27) {
                    game->state = STATE_MENU;
                }
                break;
                
            case STATE_EXIT:
                running = false;
                break;
        }
    }
}

// Xử lý input trong menu
void handle_menu_input(Game *game) {
    char c = get_input();
    
    switch (c) {
        case 'p':
        case 'P':
            // Bắt đầu game mới
            cleanup_game(game);
            init_bird(&game->bird);
            game->pipe_head = NULL;
            game->score = 0;
            game->frame_count = 0;
            game->state = STATE_PLAYING;
            break;
            
        case 'h':
        case 'H':
            game->state = STATE_HIGHSCORE;
            break;
            
        case 's':
        case 'S':
            game->state = STATE_SETTINGS;
            break;
            
        case 'a':
        case 'A':
            // Chuyển sang spectator mode
            cleanup_game(game);
            game->mode = MODE_SPECTATOR;
            init_bird(&game->bird);
            game->pipe_head = NULL;
            game->score = 0;
            game->frame_count = 0;
            game->state = STATE_PLAYING;
            break;
            
        case 'q':
        case 'Q':
        case 27: // ESC
            game->state = STATE_EXIT;
            break;
    }
}

// Xử lý input trong game
void handle_game_input(Game *game) {
    char c = get_input();
    
    if (c == ' ' && game->mode == MODE_PLAYER) {
        // Nhảy
        bird_jump(&game->bird);
        play_sound_effect("jump");
    } else if (c == 27 || c == 'p' || c == 'P') {
        // ESC hoặc P - Pause/Menu
        game->state = STATE_MENU;
    } else if (c == 'q' || c == 'Q') {
        game->state = STATE_EXIT;
    } else if (c == 'd' && game->mode == MODE_ADMIN) {
        // Toggle debug mode
        game->debug_mode = !game->debug_mode;
    }
}

// Xử lý input trong game over
void handle_game_over_input(Game *game) {
    char c = get_input();
    
    if (c == 'r' || c == 'R') {
        // Restart
        cleanup_game(game);
        init_bird(&game->bird);
        game->pipe_head = NULL;
        game->score = 0;
        game->frame_count = 0;
        game->state = STATE_PLAYING;
    } else if (c == 'm' || c == 'M' || c == 27) {
        // Menu
        game->state = STATE_MENU;
    }
}
