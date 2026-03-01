#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "game.h"

#define FRAME_MS (1000 / FPS)

int main(void) {
    Game g;

    if (!game_init(&g)) {
        fprintf(stderr, "Failed to init game!\n");
        return 1;
    }

    bool running = true;
    bool paused  = false;
    Uint32 last  = SDL_GetTicks();

    game_render(&g);

    while (running) {
        Uint32 now   = SDL_GetTicks();
        Uint32 delta = now - last;

        if (delta < FRAME_MS) {
            SDL_Delay(FRAME_MS - delta);
            continue;
        }
        last = now;

        // ── Xử lý event ──────────────────────────────────────────────────────
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                running = false;
                break;
            }

            if (ev.type == SDL_KEYDOWN) {
                SDL_Keycode key = ev.key.keysym.sym;

                // ── GLOBAL: ESC thoát hoặc quay lại menu ─────────────────────
                if (key == SDLK_ESCAPE) {
                    if (g.state == STATE_MENU) {
                        running = false;
                    } else if (g.state == STATE_SHOP || g.state == STATE_ADMIN) {
                        // Admin: nếu đang nhập pass, quay lại menu
                        g.admin_mode = false;
                        g.admin_len  = 0;
                        memset(g.admin_buf, 0, sizeof(g.admin_buf));
                        g.state = STATE_MENU;
                    } else if (g.state == STATE_2P_CHAR_SELECT) {
                        g.state = STATE_MENU;
                    } else {
                        g.state = STATE_MENU;
                    }
                    break;
                }

                switch (g.state) {

                    // ── MENU ─────────────────────────────────────────────────
                    case STATE_MENU:
                        if (key == SDLK_UP || key == SDLK_w) {
                            g.menu_cursor = (g.menu_cursor + 4) % 5;
                        } else if (key == SDLK_DOWN || key == SDLK_s) {
                            g.menu_cursor = (g.menu_cursor + 1) % 5;
                        } else if (key == SDLK_RETURN || key == SDLK_SPACE) {
                            switch (g.menu_cursor) {
                                case 0: // 1 Player
                                    game_reset(&g);
                                    g.state  = STATE_PLAYING;
                                    paused   = false;
                                    break;
                                case 1: // 2 Players → char select
                                    g.char_select_p = 0;
                                    g.char_select_p1_done = false;
                                    g.auto_play  = false;
                                    g.auto_play2 = false;
                                    g.state = STATE_2P_CHAR_SELECT;
                                    break;
                                case 2: // Shop
                                    g.state = STATE_SHOP;
                                    break;
                                case 3: // Admin
                                    g.state = STATE_ADMIN;
                                    g.admin_mode = false;
                                    g.admin_len  = 0;
                                    memset(g.admin_buf, 0, sizeof(g.admin_buf));
                                    break;
                                case 4: // Quit
                                    running = false;
                                    break;
                            }
                        }
                        break;

                    // ── PLAYING (1P) ─────────────────────────────────────────
                    case STATE_PLAYING:
                        if (key == SDLK_SPACE || key == SDLK_UP || key == SDLK_w) {
                            if (!paused) game_jump(&g);
                        }
                        if (key == SDLK_p) {
                            paused = !paused;
                        }
                        if (key == SDLK_a) {
                            g.auto_play = !g.auto_play;
                        }
                        break;

                    // ── GAME OVER (1P) ───────────────────────────────────────
                    case STATE_GAME_OVER:
                        if (key == SDLK_r || key == SDLK_SPACE) {
                            game_reset(&g);
                            g.state = STATE_PLAYING;
                            paused  = false;
                        }
                        if (key == SDLK_m) {
                            g.state = STATE_MENU;
                        }
                        break;

                    // ── 2P CHAR SELECT ─────────────────────────────────────
                    case STATE_2P_CHAR_SELECT: {
                        if (g.char_select_p == 0) {
                            // P1 chọn skin: LEFT/RIGHT
                            if (key == SDLK_LEFT) {
                                g.skin = (g.skin + MAX_SKINS - 1) % MAX_SKINS;
                                // chỉ chọn skin đã sở hữu
                                for (int t=0; t<MAX_SKINS; t++) {
                                    if ((g.owned_skins >> g.skin) & 1) break;
                                    g.skin = (g.skin + MAX_SKINS - 1) % MAX_SKINS;
                                }
                            } else if (key == SDLK_RIGHT) {
                                g.skin = (g.skin + 1) % MAX_SKINS;
                                for (int t=0; t<MAX_SKINS; t++) {
                                    if ((g.owned_skins >> g.skin) & 1) break;
                                    g.skin = (g.skin + 1) % MAX_SKINS;
                                }
                            } else if (key == SDLK_a && !g.char_select_p1_done) {
                                g.auto_play = !g.auto_play;
                            } else if (key == SDLK_RETURN) {
                                if (!g.char_select_p1_done) {
                                    g.char_select_p1_done = true;
                                }
                                g.char_select_p = 1; // chuyển sang P2
                            }
                        } else {
                            // P2 chọn skin: A/D
                            if (key == SDLK_a) {
                                // nếu đang chọn skin → đổi skin, nếu đã ready → toggle auto
                                // phân biệt: khi đang trong char select P2, A/D đổi skin
                                g.skin2 = (g.skin2 + MAX_SKINS - 1) % MAX_SKINS;
                                for (int t=0; t<MAX_SKINS; t++) {
                                    if ((g.owned_skins >> g.skin2) & 1) break;
                                    g.skin2 = (g.skin2 + MAX_SKINS - 1) % MAX_SKINS;
                                }
                            } else if (key == SDLK_d) {
                                g.skin2 = (g.skin2 + 1) % MAX_SKINS;
                                for (int t=0; t<MAX_SKINS; t++) {
                                    if ((g.owned_skins >> g.skin2) & 1) break;
                                    g.skin2 = (g.skin2 + 1) % MAX_SKINS;
                                }
                            } else if (key == SDLK_z || key == SDLK_KP_0) {
                                // Z để toggle auto P2
                                g.auto_play2 = !g.auto_play2;
                            } else if (key == SDLK_RETURN || key == SDLK_1 || key == SDLK_KP_1) {
                                // Start game
                                game_reset_2p(&g);
                                g.state  = STATE_2P_PLAYING;
                                paused   = false;
                            } else if (key == SDLK_BACKSPACE) {
                                // Quay lại chọn P1
                                g.char_select_p = 0;
                                g.char_select_p1_done = false;
                            }
                        }
                        break;
                    }

                    // ── 2P PLAYING ───────────────────────────────────────────
                    case STATE_2P_PLAYING:
                        // P1: W hoặc SPACE
                        if (key == SDLK_w || key == SDLK_SPACE) {
                            if (!paused) game_jump(&g);
                        }
                        // P2: phím số 1 hoặc phím mũi tên lên (nếu không có bàn phím thứ 2)
                        // P2 dùng: UP arrow hoặc phím số 1
                        if (key == SDLK_KP_1 || key == SDLK_1 || key == SDLK_UP) {
                            if (!paused) game_jump_2(&g);
                        }
                        if (key == SDLK_p) {
                            paused = !paused;
                        }
                        // Toggle auto P1
                        if (key == SDLK_a) {
                            g.auto_play = !g.auto_play;
                        }
                        // Toggle auto P2
                        if (key == SDLK_z) {
                            g.auto_play2 = !g.auto_play2;
                        }
                        break;

                    // ── 2P GAME OVER ─────────────────────────────────────────
                    case STATE_2P_GAME_OVER:
                        if (key == SDLK_r || key == SDLK_SPACE) {
                            game_reset_2p(&g);
                            g.state = STATE_2P_PLAYING;
                            paused  = false;
                        }
                        if (key == SDLK_m) {
                            g.state = STATE_MENU;
                        }
                        break;

                    // ── SHOP ─────────────────────────────────────────────────
                    case STATE_SHOP:
                        if (key == SDLK_UP || key == SDLK_w) {
                            g.shop_cursor = (g.shop_cursor + MAX_SKINS - 1) % MAX_SKINS;
                        } else if (key == SDLK_DOWN || key == SDLK_s) {
                            g.shop_cursor = (g.shop_cursor + 1) % MAX_SKINS;
                        } else if (key == SDLK_b) {
                            // Mua skin
                            int i = g.shop_cursor;
                            bool owned = (g.owned_skins >> i) & 1;
                            if (!owned && g.coins >= SKIN_PRICE[i]) {
                                g.coins       -= SKIN_PRICE[i];
                                g.owned_skins |= (1 << i);
                                save_game_data(&g);
                            }
                        } else if (key == SDLK_e) {
                            // Trang bị skin (nếu đã sở hữu)
                            int i = g.shop_cursor;
                            bool owned = (g.owned_skins >> i) & 1;
                            if (owned) {
                                g.skin = i;
                                save_game_data(&g);
                            }
                        }
                        break;

                    // ── ADMIN ─────────────────────────────────────────────────
                    case STATE_ADMIN:
                        if (!g.admin_mode) {
                            // Nhập password
                            if (key == SDLK_RETURN) {
                                g.admin_buf[g.admin_len] = '\0';
                                if (strcmp(g.admin_buf, ADMIN_PASS) == 0) {
                                    g.admin_mode   = true;
                                    g.admin_cursor = 0;
                                } else {
                                    // Sai mật khẩu
                                    g.admin_len = 0;
                                    memset(g.admin_buf, 0, sizeof(g.admin_buf));
                                }
                            } else if (key == SDLK_BACKSPACE) {
                                if (g.admin_len > 0) {
                                    g.admin_len--;
                                    g.admin_buf[g.admin_len] = '\0';
                                }
                            } else {
                                // Nhận ký tự số
                                char ch = 0;
                                if (key >= SDLK_0 && key <= SDLK_9)
                                    ch = (char)('0' + (key - SDLK_0));
                                else if (key >= SDLK_KP_0 && key <= SDLK_KP_9)
                                    ch = (char)('0' + (key - SDLK_KP_0));
                                else if (key >= SDLK_a && key <= SDLK_z)
                                    ch = (char)('a' + (key - SDLK_a));
                                if (ch && g.admin_len < 7) {
                                    g.admin_buf[g.admin_len++] = ch;
                                    g.admin_buf[g.admin_len]   = '\0';
                                }
                            }
                        } else {
                            // Admin đã đăng nhập
                            if (key == SDLK_UP || key == SDLK_w) {
                                g.admin_cursor = (g.admin_cursor + 8) % 9;
                            } else if (key == SDLK_DOWN || key == SDLK_s) {
                                g.admin_cursor = (g.admin_cursor + 1) % 9;
                            } else if (key == SDLK_RETURN) {
                                switch (g.admin_cursor) {
                                    case 0: // +999 coins
                                        g.coins += 999;
                                        snprintf(g.admin_msg, sizeof(g.admin_msg),
                                            "ADDED 999 COINS!");
                                        g.admin_msg_timer = 180;
                                        save_game_data(&g);
                                        break;
                                    case 1: // Reset coins
                                        g.coins = 0;
                                        snprintf(g.admin_msg, sizeof(g.admin_msg),
                                            "COINS RESET!");
                                        g.admin_msg_timer = 180;
                                        save_game_data(&g);
                                        break;
                                    case 2: // Unlock all skins
                                        g.owned_skins = (1 << MAX_SKINS) - 1;
                                        snprintf(g.admin_msg, sizeof(g.admin_msg),
                                            "ALL SKINS UNLOCKED!");
                                        g.admin_msg_timer = 180;
                                        save_game_data(&g);
                                        break;
                                    case 3: // Reset high score
                                        g.high_score = 0;
                                        snprintf(g.admin_msg, sizeof(g.admin_msg),
                                            "HIGH SCORE RESET!");
                                        g.admin_msg_timer = 180;
                                        save_game_data(&g);
                                        break;
                                    case 4: // Speed normal
                                        g.tree_speed_mult = 1.0f;
                                        snprintf(g.admin_msg, sizeof(g.admin_msg),
                                            "SPEED: NORMAL");
                                        g.admin_msg_timer = 120;
                                        break;
                                    case 5: // Speed fast
                                        g.tree_speed_mult = 2.0f;
                                        snprintf(g.admin_msg, sizeof(g.admin_msg),
                                            "SPEED: FAST (2x)");
                                        g.admin_msg_timer = 120;
                                        break;
                                    case 6: // Speed slow
                                        g.tree_speed_mult = 0.5f;
                                        snprintf(g.admin_msg, sizeof(g.admin_msg),
                                            "SPEED: SLOW (0.5x)");
                                        g.admin_msg_timer = 120;
                                        break;
                                    case 7: // God mode toggle
                                        g.god_mode = !g.god_mode;
                                        snprintf(g.admin_msg, sizeof(g.admin_msg),
                                            g.god_mode ? "GOD MODE ON!" : "GOD MODE OFF!");
                                        g.admin_msg_timer = 120;
                                        break;
                                    case 8: // Back
                                        g.admin_mode = false;
                                        g.admin_len  = 0;
                                        memset(g.admin_buf, 0, sizeof(g.admin_buf));
                                        g.state = STATE_MENU;
                                        break;
                                }
                            }
                        }
                        break;

                    default: break;
                }
            }
        }

        // ── Update & Render ───────────────────────────────────────────────────
        if (g.state == STATE_PLAYING && !paused) {
            game_update(&g);
        }
        if (g.state == STATE_2P_PLAYING && !paused) {
            game_update_2p(&g);
        }

        game_render(&g);

        // PAUSED overlay
        if ((g.state == STATE_PLAYING || g.state == STATE_2P_PLAYING) && paused) {
            SDL_Color white  = {255, 255, 255, 255};
            SDL_Color yellow = {255, 220, 50,  255};
            int px = SCREEN_W/2 - 90, py = SCREEN_H/2 - 50;
            render_panel(g.renderer, px, py, 180, 80);
            draw_text_simple(g.renderer, px + 20, py + 15, "PAUSED", yellow);
            draw_text_simple(g.renderer, px + 10, py + 48, "P - RESUME", white);
            SDL_RenderPresent(g.renderer);
        }
    }

    save_game_data(&g);
    game_cleanup(&g);
    printf("Cảm ơn đã chơi Flappy Teemo+!\n");
    return 0;
}
