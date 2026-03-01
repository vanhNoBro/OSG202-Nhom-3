#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "game.h"

// ─────────────────────────────────────────────────────────────────────────────
// Highscore / Save
// ─────────────────────────────────────────────────────────────────────────────

void save_highscore(int score) {
    FILE *f = fopen(HIGHSCORE_FILE, "wb");
    if (!f) return;
    fwrite(&score, sizeof(int), 1, f);
    fclose(f);
}

int load_highscore(void) {
    FILE *f = fopen(HIGHSCORE_FILE, "rb");
    if (!f) return 0;
    int s = 0;
    if (fread(&s, sizeof(int), 1, f) != 1) s = 0;
    fclose(f);
    return s;
}

typedef struct {
    int coins;
    int owned_skins;
    int current_skin;
    int high_score;
} SaveData;

void save_game_data(Game *g) {
    FILE *f = fopen(COINS_FILE, "wb");
    if (!f) return;
    SaveData d = { g->coins, g->owned_skins, g->skin, g->high_score };
    fwrite(&d, sizeof(SaveData), 1, f);
    fclose(f);
}

void load_game_data(Game *g) {
    FILE *f = fopen(COINS_FILE, "rb");
    if (!f) {
        g->coins = 0;
        g->owned_skins = 1; // skin 0 always owned
        g->skin = 0;
        g->high_score = load_highscore();
        return;
    }
    SaveData d = {0,1,0,0};
    fread(&d, sizeof(SaveData), 1, f);
    fclose(f);
    g->coins       = d.coins;
    g->owned_skins = d.owned_skins | 1;
    g->skin        = d.current_skin;
    g->high_score  = d.high_score;
}

// ─────────────────────────────────────────────────────────────────────────────
// Font bitmap 5×7
// ─────────────────────────────────────────────────────────────────────────────

static const unsigned char font5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, // ' '
    {0x00,0x00,0x5F,0x00,0x00}, // '!'
    {0x00,0x07,0x00,0x07,0x00}, // '"'
    {0x14,0x7F,0x14,0x7F,0x14}, // '#'
    {0x24,0x2A,0x7F,0x2A,0x12}, // '$'
    {0x23,0x13,0x08,0x64,0x62}, // '%'
    {0x36,0x49,0x55,0x22,0x50}, // '&'
    {0x00,0x05,0x03,0x00,0x00}, // '\''
    {0x00,0x1C,0x22,0x41,0x00}, // '('
    {0x00,0x41,0x22,0x1C,0x00}, // ')'
    {0x14,0x08,0x3E,0x08,0x14}, // '*'
    {0x08,0x08,0x3E,0x08,0x08}, // '+'
    {0x00,0x50,0x30,0x00,0x00}, // ','
    {0x08,0x08,0x08,0x08,0x08}, // '-'
    {0x00,0x60,0x60,0x00,0x00}, // '.'
    {0x20,0x10,0x08,0x04,0x02}, // '/'
    {0x3E,0x51,0x49,0x45,0x3E}, // '0'
    {0x00,0x42,0x7F,0x40,0x00}, // '1'
    {0x42,0x61,0x51,0x49,0x46}, // '2'
    {0x21,0x41,0x45,0x4B,0x31}, // '3'
    {0x18,0x14,0x12,0x7F,0x10}, // '4'
    {0x27,0x45,0x45,0x45,0x39}, // '5'
    {0x3C,0x4A,0x49,0x49,0x30}, // '6'
    {0x01,0x71,0x09,0x05,0x03}, // '7'
    {0x36,0x49,0x49,0x49,0x36}, // '8'
    {0x06,0x49,0x49,0x29,0x1E}, // '9'
    {0x00,0x36,0x36,0x00,0x00}, // ':'
    {0x00,0x56,0x36,0x00,0x00}, // ';'
    {0x08,0x14,0x22,0x41,0x00}, // '<'
    {0x14,0x14,0x14,0x14,0x14}, // '='
    {0x00,0x41,0x22,0x14,0x08}, // '>'
    {0x02,0x01,0x51,0x09,0x06}, // '?'
    {0x32,0x49,0x79,0x41,0x3E}, // '@'
    {0x7E,0x11,0x11,0x11,0x7E}, // 'A'
    {0x7F,0x49,0x49,0x49,0x36}, // 'B'
    {0x3E,0x41,0x41,0x41,0x22}, // 'C'
    {0x7F,0x41,0x41,0x22,0x1C}, // 'D'
    {0x7F,0x49,0x49,0x49,0x41}, // 'E'
    {0x7F,0x09,0x09,0x09,0x01}, // 'F'
    {0x3E,0x41,0x49,0x49,0x7A}, // 'G'
    {0x7F,0x08,0x08,0x08,0x7F}, // 'H'
    {0x00,0x41,0x7F,0x41,0x00}, // 'I'
    {0x20,0x40,0x41,0x3F,0x01}, // 'J'
    {0x7F,0x08,0x14,0x22,0x41}, // 'K'
    {0x7F,0x40,0x40,0x40,0x40}, // 'L'
    {0x7F,0x02,0x04,0x02,0x7F}, // 'M'
    {0x7F,0x04,0x08,0x10,0x7F}, // 'N'
    {0x3E,0x41,0x41,0x41,0x3E}, // 'O'
    {0x7F,0x09,0x09,0x09,0x06}, // 'P'
    {0x3E,0x41,0x51,0x21,0x5E}, // 'Q'
    {0x7F,0x09,0x19,0x29,0x46}, // 'R'
    {0x46,0x49,0x49,0x49,0x31}, // 'S'
    {0x01,0x01,0x7F,0x01,0x01}, // 'T'
    {0x3F,0x40,0x40,0x40,0x3F}, // 'U'
    {0x1F,0x20,0x40,0x20,0x1F}, // 'V'
    {0x3F,0x40,0x38,0x40,0x3F}, // 'W'
    {0x63,0x14,0x08,0x14,0x63}, // 'X'
    {0x07,0x08,0x70,0x08,0x07}, // 'Y'
    {0x61,0x51,0x49,0x45,0x43}, // 'Z'
};

void draw_text_scaled(SDL_Renderer *r, int x, int y, const char *text, SDL_Color col, int scale) {
    SDL_SetRenderDrawColor(r, col.r, col.g, col.b, col.a);
    int cx = x;
    for (const char *p = text; *p; p++) {
        char ch = *p >= 'a' && *p <= 'z' ? (*p - 32) : *p; // lowercase to upper
        if (ch < 32 || ch > 90) { cx += (5 + 1) * scale; continue; }
        const unsigned char *glyph = font5x7[(int)(ch - 32)];
        for (int ci = 0; ci < 5; ci++) {
            for (int ri = 0; ri < 7; ri++) {
                if (glyph[ci] & (1 << ri)) {
                    SDL_Rect px = { cx + ci * scale, y + ri * scale, scale, scale };
                    SDL_RenderFillRect(r, &px);
                }
            }
        }
        cx += (5 + 1) * scale;
    }
}

void draw_text_simple(SDL_Renderer *r, int x, int y, const char *text, SDL_Color col) {
    draw_text_scaled(r, x, y, text, col, 3);
}

// ─────────────────────────────────────────────────────────────────────────────
// Load texture
// ─────────────────────────────────────────────────────────────────────────────

SDL_Texture *load_texture(SDL_Renderer *r, const char *path) {
    SDL_Surface *surf = IMG_Load(path);
    if (!surf) {
        fprintf(stderr, "IMG_Load: %s => %s\n", path, IMG_GetError());
        return NULL;
    }
    SDL_Texture *tex = SDL_CreateTextureFromSurface(r, surf);
    SDL_FreeSurface(surf);
    return tex;
}

// Tạo texture tinted từ base texture (cho skin)
SDL_Texture *make_colored_teemo(SDL_Renderer *r, SDL_Texture *base, SDL_Color tint) {
    if (!base) return NULL;
    // Clone texture bằng cách set color mod
    // (SDL không cho clone dễ dàng, dùng color mod thay thế)
    // Trả về base với color mod sẽ được set khi render
    (void)tint; // tint được xử lý lúc render
    return base;
}

// ─────────────────────────────────────────────────────────────────────────────
// Render helpers
// ─────────────────────────────────────────────────────────────────────────────

void render_panel(SDL_Renderer *r, int x, int y, int w, int h) {
    SDL_SetRenderDrawColor(r, 0, 0, 0, 170);
    SDL_Rect bg = {x, y, w, h};
    SDL_RenderFillRect(r, &bg);
    SDL_SetRenderDrawColor(r, 255, 200, 50, 255);
    SDL_RenderDrawRect(r, &bg);
    SDL_Rect inner = {x+1, y+1, w-2, h-2};
    SDL_RenderDrawRect(r, &inner);
}

// Vẽ coin xoay (animated dùng ticks)
void render_coin_sprite(SDL_Renderer *r, int cx, int cy, int rad, Uint32 ticks) {
    // Hiệu ứng xoay: thay đổi chiều rộng theo sin
    float phase = (ticks % 1000) / 1000.0f * 2.0f * 3.14159f;
    float wscale = fabsf(sinf(phase));
    int rw = (int)(rad * wscale) + 2;

    // Viền ngoài
    SDL_SetRenderDrawColor(r, 255, 165, 0, 255); // cam vàng
    for (int dy = -rad; dy <= rad; dy++) {
        int dx = (int)(sqrtf((float)(rad*rad - dy*dy)));
        int dx_scaled = (int)(dx * wscale);
        if (dx_scaled < 1) dx_scaled = 1;
        SDL_RenderDrawLine(r, cx - dx_scaled, cy + dy, cx + dx_scaled, cy + dy);
    }
    // Tâm sáng
    SDL_SetRenderDrawColor(r, 255, 220, 50, 255);
    for (int dy = -rad+3; dy <= rad-3; dy++) {
        int dx = (int)(sqrtf((float)((rad-3)*(rad-3) - dy*dy)));
        int dx_scaled = (int)(dx * wscale);
        if (dx_scaled < 1) dx_scaled = 1;
        SDL_RenderDrawLine(r, cx - dx_scaled, cy + dy, cx + dx_scaled, cy + dy);
    }
    (void)rw;
}

// Hiện HUD xu: bên phải góc trên
void render_coin_hud(SDL_Renderer *r, int x, int y, int coins_run, int coins_total) {
    char buf[32];
    SDL_Color gold = {255, 200, 30, 255};
    SDL_Color white = {255,255,255,255};
    render_coin_sprite(r, x + 8, y + 8, 7, SDL_GetTicks());
    snprintf(buf, sizeof(buf), "+%d", coins_run);
    draw_text_scaled(r, x + 20, y + 2, buf, gold, 2);
    snprintf(buf, sizeof(buf), "COIN %d", coins_total);
    draw_text_scaled(r, x, y + 18, buf, white, 2);
}

// ─────────────────────────────────────────────────────────────────────────────
// Color mod helper cho skin
// ─────────────────────────────────────────────────────────────────────────────

static SDL_Color skin_color[MAX_SKINS] = {
    {255, 255, 255, 255}, // default: không tint
    {255, 220,  50, 255}, // golden: vàng
    { 80,  80, 160, 255}, // shadow: tím xanh
    {255, 100, 200, 255}, // rainbow: hồng (sẽ đổi màu theo thời gian)
};

static void set_skin_color_mod(SDL_Texture *tex, int skin_id) {
    if (skin_id == 0) {
        SDL_SetTextureColorMod(tex, 255, 255, 255);
        return;
    }
    if (skin_id == 3) {
        // rainbow: cycle qua màu
        Uint32 t = SDL_GetTicks();
        Uint8 r2 = (Uint8)(128 + 127 * sinf(t * 0.003f));
        Uint8 g2 = (Uint8)(128 + 127 * sinf(t * 0.003f + 2.1f));
        Uint8 b2 = (Uint8)(128 + 127 * sinf(t * 0.003f + 4.2f));
        SDL_SetTextureColorMod(tex, r2, g2, b2);
        return;
    }
    SDL_Color c = skin_color[skin_id];
    SDL_SetTextureColorMod(tex, c.r, c.g, c.b);
}

// ─────────────────────────────────────────────────────────────────────────────
// Init
// ─────────────────────────────────────────────────────────────────────────────

bool game_init(Game *g) {
    memset(g, 0, sizeof(Game));
    srand((unsigned)time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return false;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "IMG_Init: %s\n", IMG_GetError());
        return false;
    }

    g->window = SDL_CreateWindow(
        "Flappy Teemo +",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_W, SCREEN_H,
        SDL_WINDOW_SHOWN
    );
    if (!g->window) return false;

    g->renderer = SDL_CreateRenderer(g->window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!g->renderer) return false;
    SDL_SetRenderDrawBlendMode(g->renderer, SDL_BLENDMODE_BLEND);

    // Load base textures
    SDL_Texture *base_normal = load_texture(g->renderer, "assets/teemo.png");
    SDL_Texture *base_jump   = load_texture(g->renderer, "assets/teemo-jump.png");

    if (!base_normal || !base_jump) {
        fprintf(stderr, "Failed to load teemo textures!\n");
        return false;
    }

    // Tất cả skin dùng cùng base texture, tint lúc render
    for (int i = 0; i < MAX_SKINS; i++) {
        g->tex_teemo[i] = base_normal;
        g->tex_jump[i]  = base_jump;
    }

    g->tex_tree = load_texture(g->renderer, "assets/tree.png");
    if (!g->tex_tree) {
        fprintf(stderr, "Failed to load tree texture!\n");
        return false;
    }

    load_game_data(g);
    g->state       = STATE_MENU;
    g->menu_cursor = 0;
    g->skin2       = 0; // P2 dùng skin 0 mặc định
    g->auto_play2  = false;
    g->god_mode    = false;
    g->tree_speed_mult = 1.0f;
    g->char_select_p   = 0;
    g->char_select_p1_done = false;

    // mkdir data
    SDL_RWops *rw = SDL_RWFromFile("data/.keep", "w");
    if (rw) SDL_RWclose(rw);

    return true;
}

void game_cleanup(Game *g) {
    // Chỉ destroy base textures (index 0), các index khác dùng chung
    if (g->tex_teemo[0]) SDL_DestroyTexture(g->tex_teemo[0]);
    if (g->tex_jump[0])  SDL_DestroyTexture(g->tex_jump[0]);
    if (g->tex_tree)     SDL_DestroyTexture(g->tex_tree);
    if (g->renderer)     SDL_DestroyRenderer(g->renderer);
    if (g->window)       SDL_DestroyWindow(g->window);
    IMG_Quit();
    SDL_Quit();
}

// ─────────────────────────────────────────────────────────────────────────────
// Reset
// ─────────────────────────────────────────────────────────────────────────────

void game_reset(Game *g) {
    g->teemo.x          = TEEMO_START_X;
    g->teemo.y          = TEEMO_START_Y;
    g->teemo.vy         = 0;
    g->teemo.jump_timer = 0;
    g->teemo.squash     = 0;
    g->teemo.alive      = true;
    g->score            = 0;
    g->coins_run        = 0;
    g->tree_count       = 0;
    game_spawn_tree(g);
}

void game_reset_2p(Game *g) {
    game_reset(g);
    g->teemo2.x          = TEEMO_START_X;
    g->teemo2.y          = TEEMO_START_Y + 80; // P2 bắt đầu thấp hơn
    g->teemo2.vy         = 0;
    g->teemo2.jump_timer = 0;
    g->teemo2.squash     = 0;
    g->teemo2.alive      = true;
    g->score2            = 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// Spawn tree
// ─────────────────────────────────────────────────────────────────────────────

void game_spawn_tree(Game *g) {
    if (g->tree_count >= MAX_TREES) return;
    int min_gap = 80;
    int max_gap = SCREEN_H - TREE_GAP - 120;
    int gap_y   = min_gap + rand() % (max_gap - min_gap);

    Tree *t        = &g->trees[g->tree_count++];
    t->x           = (float)(SCREEN_W + 10);
    t->gap_y       = gap_y;
    t->scored      = false;
    t->coin_taken  = false;
    // 70% chance có coin
    t->coin_active = (rand() % 10) < 7;
}

// ─────────────────────────────────────────────────────────────────────────────
// Collision
// ─────────────────────────────────────────────────────────────────────────────

bool check_col(Game *g, Teemo *t) {
    // Đất
    if (t->y + TEEMO_H >= SCREEN_H - 50) return true;
    // Trần
    if (t->y < 0) return true;

    float margin = 10.0f;
    float tx = t->x + margin;
    float ty = t->y + margin;
    float tw = TEEMO_W - margin * 2;
    float th = TEEMO_H - margin * 2;

    for (int i = 0; i < g->tree_count; i++) {
        Tree *tr = &g->trees[i];
        float tl = tr->x;
        float tr2 = tr->x + TREE_W;
        float gt = (float)tr->gap_y;
        float gb = (float)(tr->gap_y + TREE_GAP);

        if (tx + tw > tl && tx < tr2) {
            if (ty < gt || ty + th > gb) return true;
        }
    }
    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
// Jump
// ─────────────────────────────────────────────────────────────────────────────

void game_jump(Game *g) {
    if (!g->teemo.alive) return;
    g->teemo.vy         = JUMP_VEL;
    g->teemo.jump_timer = JUMP_TEXTURE_DURATION;
    g->teemo.squash     = 8; // squash animation frames
}

void game_jump_2(Game *g) {
    if (!g->teemo2.alive) return;
    g->teemo2.vy         = JUMP_VEL;
    g->teemo2.jump_timer = JUMP_TEXTURE_DURATION;
    g->teemo2.squash     = 8;
}

// ─────────────────────────────────────────────────────────────────────────────
// Auto-play AI: nhảy khi cần thiết để vượt qua khe cây gần nhất
// ─────────────────────────────────────────────────────────────────────────────

void game_auto_update(Game *g) {
    if (!g->auto_play) return;

    // Tìm cây gần nhất phía trước Teemo
    Tree *nearest = NULL;
    float nearest_x = 9999;
    for (int i = 0; i < g->tree_count; i++) {
        float tr_right = g->trees[i].x + TREE_W;
        if (tr_right > g->teemo.x && g->trees[i].x < nearest_x) {
            nearest_x = g->trees[i].x;
            nearest = &g->trees[i];
        }
    }

    if (!nearest) return;

    // Tâm khe
    float gap_center = nearest->gap_y + TREE_GAP * 0.5f;
    float teemo_center = g->teemo.y + TEEMO_H * 0.5f;

    // Nếu Teemo đang cao hơn tâm khe → đừng nhảy
    // Nếu Teemo đang thấp hơn + velocity đang âm/thấp → nhảy
    float safety_margin = TREE_GAP * 0.25f;

    if (teemo_center > gap_center + safety_margin && g->teemo.vy >= 0) {
        game_jump(g);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Update 1P
// ─────────────────────────────────────────────────────────────────────────────

void game_update(Game *g) {
    // Auto AI
    game_auto_update(g);

    // Jump animation squash/stretch
    if (g->teemo.jump_timer > 0) g->teemo.jump_timer--;
    if (g->teemo.squash > 0)     g->teemo.squash--;

    // Physics
    g->teemo.vy += GRAVITY;
    if (g->teemo.vy >  VEL_MAX) g->teemo.vy =  VEL_MAX;
    if (g->teemo.vy < -VEL_MAX) g->teemo.vy = -VEL_MAX;
    g->teemo.y += g->teemo.vy;
    if (g->teemo.y < 0) { g->teemo.y = 0; g->teemo.vy = 0; }

    // Di chuyển cây
    for (int i = 0; i < g->tree_count; i++) {
        g->trees[i].x -= TREE_SPEED * g->tree_speed_mult;

        // Tính điểm qua cây
        if (!g->trees[i].scored && g->trees[i].x + TREE_W < g->teemo.x) {
            g->trees[i].scored = true;
            g->score++;
            if (g->score > g->high_score) {
                g->high_score = g->score;
            }
        }

        // Check lấy coin
        if (g->trees[i].coin_active && !g->trees[i].coin_taken) {
            float coin_cx = g->trees[i].x + TREE_W * 0.5f;
            float coin_cy = g->trees[i].gap_y + TREE_GAP * 0.5f;
            float dx = (g->teemo.x + TEEMO_W*0.5f) - coin_cx;
            float dy = (g->teemo.y + TEEMO_H*0.5f) - coin_cy;
            float dist = sqrtf(dx*dx + dy*dy);
            if (dist < COIN_R + TEEMO_W * 0.4f) {
                g->trees[i].coin_taken = true;
                g->coins_run += COIN_VALUE;
                g->coins     += COIN_VALUE;
            }
        }
    }

    // Xóa cây ra ngoài màn hình
    int i = 0;
    while (i < g->tree_count) {
        if (g->trees[i].x + TREE_W < -10) {
            for (int j = i; j < g->tree_count - 1; j++)
                g->trees[j] = g->trees[j + 1];
            g->tree_count--;
        } else i++;
    }

    // Spawn cây mới
    if (g->tree_count == 0 ||
        g->trees[g->tree_count - 1].x < SCREEN_W - TREE_SPACING) {
        game_spawn_tree(g);
    }

    // Collision
    if (!g->god_mode && check_col(g, &g->teemo)) {
        g->state = STATE_GAME_OVER;
        save_game_data(g);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Update 2P
// ─────────────────────────────────────────────────────────────────────────────

void game_update_2p(Game *g) {
    // Auto AI P2
    if (g->auto_play2 && g->teemo2.alive) {
        Tree *nearest = NULL;
        float nearest_x = 9999;
        for (int i = 0; i < g->tree_count; i++) {
            float tr_right = g->trees[i].x + TREE_W;
            if (tr_right > g->teemo2.x && g->trees[i].x < nearest_x) {
                nearest_x = g->trees[i].x;
                nearest = &g->trees[i];
            }
        }
        if (nearest) {
            float gap_center   = nearest->gap_y + TREE_GAP * 0.5f;
            float teemo_center = g->teemo2.y + TEEMO_H * 0.5f;
            float safety_margin = TREE_GAP * 0.25f;
            if (teemo_center > gap_center + safety_margin && g->teemo2.vy >= 0) {
                game_jump_2(g);
            }
        }
    }

    // Physics P1
    if (g->teemo.alive) {
        if (g->teemo.jump_timer > 0) g->teemo.jump_timer--;
        if (g->teemo.squash > 0) g->teemo.squash--;
        g->teemo.vy += GRAVITY;
        if (g->teemo.vy >  VEL_MAX) g->teemo.vy =  VEL_MAX;
        if (g->teemo.vy < -VEL_MAX) g->teemo.vy = -VEL_MAX;
        g->teemo.y += g->teemo.vy;
        if (g->teemo.y < 0) { g->teemo.y = 0; g->teemo.vy = 0; }
        if (!g->god_mode && check_col(g, &g->teemo)) {
            g->teemo.alive = false;
        }
    }

    // Physics P2
    if (g->teemo2.alive) {
        if (g->teemo2.jump_timer > 0) g->teemo2.jump_timer--;
        if (g->teemo2.squash > 0) g->teemo2.squash--;
        g->teemo2.vy += GRAVITY;
        if (g->teemo2.vy >  VEL_MAX) g->teemo2.vy =  VEL_MAX;
        if (g->teemo2.vy < -VEL_MAX) g->teemo2.vy = -VEL_MAX;
        g->teemo2.y += g->teemo2.vy;
        if (g->teemo2.y < 0) { g->teemo2.y = 0; g->teemo2.vy = 0; }
        if (!g->god_mode && check_col(g, &g->teemo2)) {
            g->teemo2.alive = false;
        }
    }

    // Di chuyển cây & điểm (dựa vào ai còn sống)
    for (int i = 0; i < g->tree_count; i++) {
        g->trees[i].x -= TREE_SPEED * g->tree_speed_mult;

        // Điểm P1
        if (!g->trees[i].scored && g->trees[i].x + TREE_W < g->teemo.x) {
            g->trees[i].scored = true;
            if (g->teemo.alive)  g->score++;
            if (g->teemo2.alive) g->score2++;
        }

        // Coin - P1
        if (g->trees[i].coin_active && !g->trees[i].coin_taken && g->teemo.alive) {
            float ccx = g->trees[i].x + TREE_W * 0.5f;
            float ccy = g->trees[i].gap_y + TREE_GAP * 0.5f;
            float dx = (g->teemo.x + TEEMO_W*0.5f) - ccx;
            float dy = (g->teemo.y + TEEMO_H*0.5f) - ccy;
            if (sqrtf(dx*dx + dy*dy) < COIN_R + TEEMO_W*0.4f) {
                g->trees[i].coin_taken = true;
                g->coins_run += COIN_VALUE;
                g->coins     += COIN_VALUE;
            }
        }
        // Coin - P2
        if (g->trees[i].coin_active && !g->trees[i].coin_taken && g->teemo2.alive) {
            float ccx = g->trees[i].x + TREE_W * 0.5f;
            float ccy = g->trees[i].gap_y + TREE_GAP * 0.5f;
            float dx = (g->teemo2.x + TEEMO_W*0.5f) - ccx;
            float dy = (g->teemo2.y + TEEMO_H*0.5f) - ccy;
            if (sqrtf(dx*dx + dy*dy) < COIN_R + TEEMO_W*0.4f) {
                g->trees[i].coin_taken = true;
                g->coins_run += COIN_VALUE;
                g->coins     += COIN_VALUE;
            }
        }
    }

    // Xóa cây ra ngoài
    int i = 0;
    while (i < g->tree_count) {
        if (g->trees[i].x + TREE_W < -10) {
            for (int j = i; j < g->tree_count - 1; j++)
                g->trees[j] = g->trees[j+1];
            g->tree_count--;
        } else i++;
    }

    // Spawn cây mới
    if (g->tree_count == 0 ||
        g->trees[g->tree_count-1].x < SCREEN_W - TREE_SPACING) {
        game_spawn_tree(g);
    }

    // Kết thúc 2P nếu cả hai đều chết
    if (!g->teemo.alive && !g->teemo2.alive) {
        g->state = STATE_2P_GAME_OVER;
        save_game_data(g);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Render Background
// ─────────────────────────────────────────────────────────────────────────────

static void render_background(Game *g) {
    for (int y = 0; y < SCREEN_H - 60; y++) {
        float t  = (float)y / (SCREEN_H - 60);
        Uint8 rv = (Uint8)(87  + t * (160 - 87));
        Uint8 gv = (Uint8)(181 + t * (220 - 181));
        Uint8 bv = (Uint8)(230 + t * (245 - 230));
        SDL_SetRenderDrawColor(g->renderer, rv, gv, bv, 255);
        SDL_RenderDrawLine(g->renderer, 0, y, SCREEN_W, y);
    }
    SDL_Rect ground = { 0, SCREEN_H - 60, SCREEN_W, 60 };
    SDL_SetRenderDrawColor(g->renderer, 92, 64, 31, 255);
    SDL_RenderFillRect(g->renderer, &ground);
    SDL_Rect grass = { 0, SCREEN_H - 60, SCREEN_W, 18 };
    SDL_SetRenderDrawColor(g->renderer, 67, 168, 54, 255);
    SDL_RenderFillRect(g->renderer, &grass);
}

// ─────────────────────────────────────────────────────────────────────────────
// Render Trees + Coins
// ─────────────────────────────────────────────────────────────────────────────

static void render_trees(Game *g) {
    Uint32 ticks = SDL_GetTicks();
    for (int i = 0; i < g->tree_count; i++) {
        Tree *tr = &g->trees[i];
        int x = (int)tr->x;

        // Cây trên (lật ngược)
        if (tr->gap_y > 0) {
            SDL_Rect dst = { x, 0, TREE_W, tr->gap_y };
            SDL_RenderCopyEx(g->renderer, g->tex_tree, NULL, &dst,
                0.0, NULL, SDL_FLIP_VERTICAL);
        }
        // Cây dưới
        int bot_y = tr->gap_y + TREE_GAP;
        int bot_h = (SCREEN_H - 60) - bot_y;
        if (bot_h > 0) {
            SDL_Rect dst = { x, bot_y, TREE_W, bot_h };
            SDL_RenderCopy(g->renderer, g->tex_tree, NULL, &dst);
        }

        // Coin ở giữa khe
        if (tr->coin_active && !tr->coin_taken) {
            int coin_cx = (int)(tr->x + TREE_W * 0.5f);
            int coin_cy = tr->gap_y + TREE_GAP / 2;
            render_coin_sprite(g->renderer, coin_cx, coin_cy, COIN_R, ticks);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Render Teemo (với squash/stretch animation + skin)
// ─────────────────────────────────────────────────────────────────────────────

static void render_teemo_at(Game *g, Teemo *t, int skin_id, SDL_Color label_col, const char *label) {
    SDL_Texture *tex;
    if (t->jump_timer > 0 && g->tex_jump[skin_id])
        tex = g->tex_jump[skin_id];
    else
        tex = g->tex_teemo[skin_id];

    // Áp color mod theo skin
    set_skin_color_mod(tex, skin_id);
    // Reset cho tex_jump nếu khác tex_teemo
    if (g->tex_jump[skin_id] != g->tex_teemo[skin_id])
        set_skin_color_mod(g->tex_jump[skin_id], skin_id);

    // Squash/stretch: khi vừa nhảy → dẹt ngang (squash), sau đó kéo dài
    float sx = 1.0f, sy = 1.0f;
    if (t->squash > 4) {
        // squash: rộng hơn, thấp hơn
        sx = 1.2f; sy = 0.8f;
    } else if (t->squash > 0) {
        // stretch: hẹp hơn, cao hơn
        sx = 0.85f; sy = 1.15f;
    }

    int rw = (int)(TEEMO_W * sx);
    int rh = (int)(TEEMO_H * sy);
    int rx = (int)t->x - (rw - TEEMO_W) / 2;
    int ry = (int)t->y - (rh - TEEMO_H) / 2;

    // Rotation theo velocity
    double angle = (double)t->vy * 4.5;
    if (angle >  60) angle =  60;
    if (angle < -30) angle = -30;

    SDL_Rect dst = { rx, ry, rw, rh };
    SDL_RenderCopyEx(g->renderer, tex, NULL, &dst, angle, NULL, SDL_FLIP_NONE);

    // Reset color mod
    SDL_SetTextureColorMod(tex, 255, 255, 255);

    // Label (P1/P2)
    if (label) {
        draw_text_scaled(g->renderer, rx + 20, ry - 14, label, label_col, 2);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Render HUD
// ─────────────────────────────────────────────────────────────────────────────

static void render_hud(Game *g) {
    char buf[64];
    SDL_Color yellow = {255, 220, 50, 255};
    SDL_Color white  = {255, 255, 255, 255};

    // Bên trái: điểm
    snprintf(buf, sizeof(buf), "SCORE %d", g->score);
    draw_text_simple(g->renderer, 10, 10, buf, yellow);
    snprintf(buf, sizeof(buf), "BEST %d", g->high_score);
    draw_text_simple(g->renderer, 10, 40, buf, white);

    // Auto-play indicator
    if (g->auto_play) {
        SDL_Color cyan = {50, 230, 200, 255};
        draw_text_scaled(g->renderer, 10, 70, "AUTO ON", cyan, 2);
    }
    // God mode indicator
    if (g->god_mode) {
        SDL_Color pink = {255, 100, 200, 255};
        draw_text_scaled(g->renderer, 10, 90, "GOD MODE", pink, 2);
    }

    // Bên phải: xu
    render_coin_hud(g->renderer, SCREEN_W - 100, 10, g->coins_run, g->coins);
}

// ─────────────────────────────────────────────────────────────────────────────
// render_menu
// ─────────────────────────────────────────────────────────────────────────────

void render_menu(Game *g) {
    render_background(g);

    SDL_Texture *tex = g->tex_teemo[g->skin];
    set_skin_color_mod(tex, g->skin);
    SDL_Rect teemo_big = { SCREEN_W/2 - 80, SCREEN_H/2 - 160, 160, 160 };
    SDL_RenderCopy(g->renderer, tex, NULL, &teemo_big);
    SDL_SetTextureColorMod(tex, 255, 255, 255);

    // Title
    render_panel(g->renderer, 50, 55, SCREEN_W - 100, 80);
    SDL_Color yellow = {255, 220, 50, 255};
    SDL_Color white  = {255, 255, 255, 255};
    draw_text_simple(g->renderer, 75, 68,  "FLAPPY TEEMO+", yellow);
    draw_text_scaled(g->renderer, 130, 105, "ENHANCED EDITION", white, 2);

    // Menu items
    const char *items[] = { "1 PLAYER", "2 PLAYERS", "SHOP", "ADMIN", "QUIT" };
    SDL_Color sel_col = {100, 255, 100, 255};
    SDL_Color nor_col = {200, 200, 200, 255};

    int my = SCREEN_H/2 + 10;
    render_panel(g->renderer, 100, my - 15, SCREEN_W - 200, 190);
    for (int i = 0; i < 5; i++) {
        SDL_Color c = (i == g->menu_cursor) ? sel_col : nor_col;
        const char *prefix = (i == g->menu_cursor) ? "> " : "  ";
        char line[32];
        snprintf(line, sizeof(line), "%s%s", prefix, items[i]);
        draw_text_scaled(g->renderer, 120, my + i * 34, line, c, 2);
    }

    // Coins + highscore
    char buf[32];
    snprintf(buf, sizeof(buf), "COIN %d", g->coins);
    SDL_Color gold = {255, 200, 30, 255};
    draw_text_scaled(g->renderer, 10, SCREEN_H - 30, buf, gold, 2);
    snprintf(buf, sizeof(buf), "BEST %d", g->high_score);
    draw_text_scaled(g->renderer, SCREEN_W - 120, SCREEN_H - 30, buf, yellow, 2);

    SDL_RenderPresent(g->renderer);
}

// ─────────────────────────────────────────────────────────────────────────────
// render_playing
// ─────────────────────────────────────────────────────────────────────────────

void render_playing(Game *g) {
    render_background(g);
    render_trees(g);
    SDL_Color yellow = {255, 220, 50, 255};
    render_teemo_at(g, &g->teemo, g->skin, yellow, NULL);
    render_hud(g);
    SDL_RenderPresent(g->renderer);
}

// ─────────────────────────────────────────────────────────────────────────────
// render_game_over
// ─────────────────────────────────────────────────────────────────────────────

void render_game_over(Game *g) {
    render_background(g);
    render_trees(g);
    SDL_Color yellow = {255, 220, 50, 255};
    render_teemo_at(g, &g->teemo, g->skin, yellow, NULL);

    // Overlay
    SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, 120);
    SDL_Rect full = {0, 0, SCREEN_W, SCREEN_H};
    SDL_RenderFillRect(g->renderer, &full);

    int px = 50, py = SCREEN_H/2 - 130, pw = SCREEN_W - 100, ph = 260;
    render_panel(g->renderer, px, py, pw, ph);

    SDL_Color red   = {255, 80,  80,  255};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {80,  255, 100, 255};
    SDL_Color gold  = {255, 200, 30,  255};

    draw_text_simple(g->renderer, px + 55, py + 15, "GAME OVER", red);

    char buf[64];
    snprintf(buf, sizeof(buf), "SCORE  %d", g->score);
    draw_text_simple(g->renderer, px + 25, py + 65, buf, white);

    snprintf(buf, sizeof(buf), "BEST   %d", g->high_score);
    SDL_Color sc = (g->score >= g->high_score && g->score > 0) ? green : yellow;
    draw_text_simple(g->renderer, px + 25, py + 100, buf, sc);

    snprintf(buf, sizeof(buf), "COIN   +%d", g->coins_run);
    draw_text_simple(g->renderer, px + 25, py + 135, buf, gold);

    snprintf(buf, sizeof(buf), "TOTAL  %d", g->coins);
    draw_text_simple(g->renderer, px + 25, py + 165, buf, gold);

    if (g->score >= g->high_score && g->score > 0)
        draw_text_scaled(g->renderer, px + 40, py + 200, "NEW RECORD!", green, 3);

    draw_text_scaled(g->renderer, px + 10, py + 232, "R-RETRY   M-MENU", white, 2);

    SDL_RenderPresent(g->renderer);
}

// ─────────────────────────────────────────────────────────────────────────────
// render_2p_char_select  (NEW)
// ─────────────────────────────────────────────────────────────────────────────

void render_2p_char_select(Game *g) {
    render_background(g);

    SDL_Color yellow = {255, 220, 50,  255};
    SDL_Color white  = {255, 255, 255, 255};
    SDL_Color p1col  = {100, 200, 255, 255};
    SDL_Color p2col  = {255, 150, 80,  255};
    SDL_Color green  = {80,  255, 100, 255};
    SDL_Color gray   = {120, 120, 120, 255};

    // Title
    render_panel(g->renderer, 40, 20, SCREEN_W - 80, 55);
    draw_text_simple(g->renderer, 70, 32, "CHOOSE CHARACTER", yellow);

    // P1 panel (trái)
    int pw = SCREEN_W / 2 - 20;
    int ph_panel = 380;
    int py_panel = 90;

    // P1 area
    {
        bool active = (g->char_select_p == 0);
        SDL_Color border_col = active ? p1col : gray;
        SDL_SetRenderDrawColor(g->renderer, border_col.r, border_col.g, border_col.b,
            active ? 200 : 80);
        SDL_Rect pr = {10, py_panel, pw, ph_panel};
        SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, 150);
        SDL_RenderFillRect(g->renderer, &pr);
        SDL_SetRenderDrawColor(g->renderer, border_col.r, border_col.g, border_col.b, 255);
        SDL_RenderDrawRect(g->renderer, &pr);

        draw_text_scaled(g->renderer, 10 + pw/2 - 20, py_panel + 8, "P1", p1col, 3);

        if (g->char_select_p1_done) {
            // Hiển thị đã chọn
            SDL_Texture *tx = g->tex_teemo[g->skin];
            set_skin_color_mod(tx, g->skin);
            SDL_Rect dst = {10 + pw/2 - 50, py_panel + 50, 100, 100};
            SDL_RenderCopy(g->renderer, tx, NULL, &dst);
            SDL_SetTextureColorMod(tx, 255, 255, 255);
            draw_text_scaled(g->renderer, 10 + 15, py_panel + 160, SKIN_NAME[g->skin], yellow, 2);
            draw_text_scaled(g->renderer, 10 + 15, py_panel + 185, "CONFIRMED!", green, 2);

            // Auto toggle P1
            SDL_Color ac = g->auto_play ? green : white;
            const char *atxt = g->auto_play ? "> AUTO: ON" : "> AUTO: OFF";
            draw_text_scaled(g->renderer, 10 + 15, py_panel + 215, atxt, ac, 2);
            draw_text_scaled(g->renderer, 10 + 8, py_panel + 240, "A-TOGGLE AUTO", gray, 1);
        } else {
            // Đang chọn
            SDL_Texture *tx = g->tex_teemo[g->skin];
            set_skin_color_mod(tx, g->skin);
            SDL_Rect dst = {10 + pw/2 - 50, py_panel + 50, 100, 100};
            SDL_RenderCopy(g->renderer, tx, NULL, &dst);
            SDL_SetTextureColorMod(tx, 255, 255, 255);
            draw_text_scaled(g->renderer, 10 + 15, py_panel + 160, SKIN_NAME[g->skin], yellow, 2);

            if (active) {
                draw_text_scaled(g->renderer, 10 + 8, py_panel + 185,
                    "< > CHANGE SKIN", white, 2);
                draw_text_scaled(g->renderer, 10 + 8, py_panel + 210,
                    "ENTER-CONFIRM", green, 2);
            }

            // Auto toggle
            SDL_Color ac = g->auto_play ? green : white;
            const char *atxt = g->auto_play ? "AUTO: ON" : "AUTO: OFF";
            draw_text_scaled(g->renderer, 10 + 15, py_panel + 240, atxt, ac, 2);
        }
    }

    // P2 area
    {
        bool active = (g->char_select_p == 1);
        SDL_Color border_col = active ? p2col : gray;

        SDL_Rect pr = {SCREEN_W/2 + 10, py_panel, pw, ph_panel};
        SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, 150);
        SDL_RenderFillRect(g->renderer, &pr);
        SDL_SetRenderDrawColor(g->renderer, border_col.r, border_col.g, border_col.b, 255);
        SDL_RenderDrawRect(g->renderer, &pr);

        draw_text_scaled(g->renderer, SCREEN_W/2 + pw/2 - 10, py_panel + 8, "P2", p2col, 3);

        SDL_Texture *tx = g->tex_teemo[g->skin2];
        set_skin_color_mod(tx, g->skin2);
        SDL_Rect dst = {SCREEN_W/2 + 10 + pw/2 - 50, py_panel + 50, 100, 100};
        SDL_RenderCopy(g->renderer, tx, NULL, &dst);
        SDL_SetTextureColorMod(tx, 255, 255, 255);
        draw_text_scaled(g->renderer, SCREEN_W/2 + 25, py_panel + 160, SKIN_NAME[g->skin2], yellow, 2);

        if (active) {
            draw_text_scaled(g->renderer, SCREEN_W/2 + 18, py_panel + 185,
                "< > CHANGE", white, 2);
            draw_text_scaled(g->renderer, SCREEN_W/2 + 18, py_panel + 210,
                "ENTER-START", green, 2);
        }

        // Auto toggle P2
        SDL_Color ac2 = g->auto_play2 ? green : white;
        const char *atxt2 = g->auto_play2 ? "AUTO: ON" : "AUTO: OFF";
        draw_text_scaled(g->renderer, SCREEN_W/2 + 25, py_panel + 240, atxt2, ac2, 2);
        if (active)
            draw_text_scaled(g->renderer, SCREEN_W/2 + 8, py_panel + 262,
                "A-TOGGLE AUTO", gray, 1);
    }

    // Hướng dẫn dưới
    render_panel(g->renderer, 5, SCREEN_H - 52, SCREEN_W - 10, 44);
    draw_text_scaled(g->renderer, 12, SCREEN_H - 47,
        "P1: LEFT/RIGHT=SKIN ENTER=OK", p1col, 1);
    draw_text_scaled(g->renderer, 12, SCREEN_H - 30,
        "P2: A/D=SKIN  ENTER/1=START  ESC=BACK", p2col, 1);

    SDL_RenderPresent(g->renderer);
}

// ─────────────────────────────────────────────────────────────────────────────

void render_2p_playing(Game *g) {
    render_background(g);
    render_trees(g);

    SDL_Color p1_col = {100, 200, 255, 255}; // xanh lam - P1
    SDL_Color p2_col = {255, 150, 80,  255}; // cam - P2

    if (g->teemo.alive)
        render_teemo_at(g, &g->teemo,  g->skin,  p1_col, "P1");
    if (g->teemo2.alive)
        render_teemo_at(g, &g->teemo2, g->skin2, p2_col, "P2");

    // HUD 2P: P1 bên trái, P2 bên phải
    char buf[32];
    draw_text_scaled(g->renderer, 10, 10, "P1", p1_col, 2);
    snprintf(buf, sizeof(buf), "SCORE %d", g->score);
    draw_text_scaled(g->renderer, 10, 26, buf, p1_col, 2);
    if (!g->teemo.alive) draw_text_scaled(g->renderer, 10, 42, "X DEAD", p1_col, 2);
    if (g->auto_play) draw_text_scaled(g->renderer, 10, 58, "AUTO", p1_col, 2);
    if (g->god_mode) { SDL_Color pink={255,100,200,255}; draw_text_scaled(g->renderer, 10, 72, "GOD", pink, 2); }

    draw_text_scaled(g->renderer, SCREEN_W - 90, 10, "P2", p2_col, 2);
    snprintf(buf, sizeof(buf), "S %d", g->score2);
    draw_text_scaled(g->renderer, SCREEN_W - 80, 26, buf, p2_col, 2);
    if (!g->teemo2.alive) draw_text_scaled(g->renderer, SCREEN_W - 80, 42, "DEAD", p2_col, 2);
    if (g->auto_play2) draw_text_scaled(g->renderer, SCREEN_W - 80, 58, "AUTO", p2_col, 2);

    // Xu ở giữa
    render_coin_hud(g->renderer, SCREEN_W/2 - 50, 10, g->coins_run, g->coins);

    SDL_RenderPresent(g->renderer);
}

// ─────────────────────────────────────────────────────────────────────────────
// render_2p_game_over
// ─────────────────────────────────────────────────────────────────────────────

void render_2p_game_over(Game *g) {
    render_background(g);
    render_trees(g);

    SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, 120);
    SDL_Rect full = {0, 0, SCREEN_W, SCREEN_H};
    SDL_RenderFillRect(g->renderer, &full);

    int px = 50, py = SCREEN_H/2 - 140, pw = SCREEN_W - 100, ph = 280;
    render_panel(g->renderer, px, py, pw, ph);

    SDL_Color red   = {255, 80,  80,  255};
    SDL_Color p1c   = {100, 200, 255, 255};
    SDL_Color p2c   = {255, 150, 80,  255};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {80,  255, 100, 255};
    SDL_Color gold  = {255, 200, 30,  255};

    draw_text_simple(g->renderer, px + 40, py + 12, "GAME OVER", red);

    char buf[64];
    snprintf(buf, sizeof(buf), "P1 SCORE %d", g->score);
    draw_text_scaled(g->renderer, px + 20, py + 65, buf, p1c, 2);

    snprintf(buf, sizeof(buf), "P2 SCORE %d", g->score2);
    draw_text_scaled(g->renderer, px + 20, py + 90, buf, p2c, 2);

    // Winner
    const char *winner = NULL;
    if (g->score > g->score2)       winner = "P1 WINS!";
    else if (g->score2 > g->score)  winner = "P2 WINS!";
    else                             winner = "DRAW!";
    draw_text_simple(g->renderer, px + 60, py + 120, winner, green);

    snprintf(buf, sizeof(buf), "COIN +%d  TOTAL %d", g->coins_run, g->coins);
    draw_text_scaled(g->renderer, px + 15, py + 175, buf, gold, 2);

    draw_text_scaled(g->renderer, px + 10, py + 210, "R-RETRY   M-MENU", white, 2);

    SDL_RenderPresent(g->renderer);
}

// ─────────────────────────────────────────────────────────────────────────────
// render_shop
// ─────────────────────────────────────────────────────────────────────────────

void render_shop(Game *g) {
    render_background(g);

    render_panel(g->renderer, 30, 30, SCREEN_W - 60, 60);
    SDL_Color yellow = {255, 220, 50, 255};
    SDL_Color white  = {255, 255, 255, 255};
    SDL_Color green  = {80, 255, 100, 255};
    SDL_Color red    = {255, 80,  80,  255};
    SDL_Color gray   = {150, 150, 150, 255};
    SDL_Color gold   = {255, 200, 30,  255};

    draw_text_simple(g->renderer, 80, 42, "SKIN SHOP", yellow);

    // Xu hiện có
    char buf[32];
    snprintf(buf, sizeof(buf), "COIN %d", g->coins);
    draw_text_scaled(g->renderer, SCREEN_W - 120, 42, buf, gold, 2);

    for (int i = 0; i < MAX_SKINS; i++) {
        int sy = 110 + i * 110;
        bool owned    = (g->owned_skins >> i) & 1;
        bool selected = (g->shop_cursor == i);
        bool equipped = (g->skin == i);

        // Panel cho item
        int bx = 40, bw = SCREEN_W - 80, bh = 95;
        if (selected) {
            SDL_SetRenderDrawColor(g->renderer, 255, 200, 50, 60);
            SDL_Rect hl = {bx, sy - 5, bw, bh};
            SDL_RenderFillRect(g->renderer, &hl);
        }
        render_panel(g->renderer, bx, sy - 5, bw, bh);

        // Preview Teemo nhỏ
        SDL_Texture *ptex = g->tex_teemo[i];
        set_skin_color_mod(ptex, i);
        SDL_Rect pdst = { bx + 8, sy, 60, 60 };
        SDL_RenderCopy(g->renderer, ptex, NULL, &pdst);
        SDL_SetTextureColorMod(ptex, 255, 255, 255);

        // Tên skin
        draw_text_scaled(g->renderer, bx + 78, sy + 5, SKIN_NAME[i],
            selected ? yellow : white, 2);

        // Giá / trạng thái
        if (owned) {
            const char *status = equipped ? "EQUIPPED" : "PRESS E";
            draw_text_scaled(g->renderer, bx + 78, sy + 28, "OWNED", green, 2);
            draw_text_scaled(g->renderer, bx + 78, sy + 46, status,
                equipped ? green : white, 2);
        } else {
            bool afford = (g->coins >= SKIN_PRICE[i]);
            snprintf(buf, sizeof(buf), "PRICE %d", SKIN_PRICE[i]);
            draw_text_scaled(g->renderer, bx + 78, sy + 28, buf,
                afford ? gold : red, 2);
            draw_text_scaled(g->renderer, bx + 78, sy + 46,
                afford ? "PRESS B BUY" : "NOT ENOUGH", afford ? white : gray, 2);
        }

        // Arrow chỉ selected
        if (selected)
            draw_text_scaled(g->renderer, bx + bw - 25, sy + 28, "<", yellow, 2);
    }

    // Hướng dẫn
    render_panel(g->renderer, 10, SCREEN_H - 50, SCREEN_W - 20, 40);
    draw_text_scaled(g->renderer, 15, SCREEN_H - 42,
        "UP/DOWN-NAV  B-BUY  E-EQUIP  ESC-BACK", white, 1);

    SDL_RenderPresent(g->renderer);
}

// ─────────────────────────────────────────────────────────────────────────────
// render_admin
// ─────────────────────────────────────────────────────────────────────────────

void render_admin(Game *g) {
    render_background(g);

    render_panel(g->renderer, 30, 30, SCREEN_W - 60, 55);
    SDL_Color yellow = {255, 220, 50, 255};
    SDL_Color white  = {255, 255, 255, 255};
    SDL_Color red    = {255, 80,  80,  255};
    SDL_Color green  = {80,  255, 100, 255};
    SDL_Color cyan   = {100, 220, 255, 255};

    draw_text_simple(g->renderer, 100, 42, "ADMIN PANEL", yellow);

    if (!g->admin_mode) {
        // Nhập password
        render_panel(g->renderer, 60, 150, SCREEN_W - 120, 120);
        draw_text_scaled(g->renderer, 80, 165, "ENTER PASSWORD:", white, 2);

        // Hiện dấu *
        char stars[8] = {0};
        for (int i = 0; i < g->admin_len && i < 7; i++) stars[i] = '*';
        render_panel(g->renderer, 90, 195, SCREEN_W - 180, 35);
        draw_text_scaled(g->renderer, 100, 202, stars, cyan, 3);

        draw_text_scaled(g->renderer, 80, 248, "ENTER-CONFIRM  ESC-BACK", white, 2);
    } else {
        // Admin menu
        const char *opts[] = {
            "ADD 999 COINS",
            "RESET COINS",
            "UNLOCK ALL SKINS",
            "RESET HIGH SCORE",
            "SPEED: NORMAL",
            "SPEED: FAST",
            "SPEED: SLOW",
            "GOD MODE TOGGLE",
            "BACK"
        };
        int n_opts = 9;
        render_panel(g->renderer, 50, 95, SCREEN_W - 100, 370);
        draw_text_scaled(g->renderer, 65, 103, "** ADMIN MODE **", red, 2);

        char buf[48];
        // Speed label
        const char *speed_label = "NORMAL";
        if (g->tree_speed_mult > 1.4f)      speed_label = "FAST";
        else if (g->tree_speed_mult < 0.7f) speed_label = "SLOW";
        snprintf(buf, sizeof(buf), "COINS %d  SPD %s  GOD %s",
            g->coins, speed_label, g->god_mode ? "ON" : "OFF");
        draw_text_scaled(g->renderer, 65, 120, buf, cyan, 2);

        for (int i = 0; i < n_opts; i++) {
            SDL_Color c = (g->admin_cursor == i) ? yellow : white;
            const char *pfx = (g->admin_cursor == i) ? "> " : "  ";
            char line[48];
            // Special coloring for god mode
            if (i == 7) {
                c = g->god_mode ? (SDL_Color){255,100,200,255} : c;
                snprintf(line, sizeof(line), "%s%s [%s]", pfx, opts[i],
                    g->god_mode ? "ON" : "OFF");
            } else {
                snprintf(line, sizeof(line), "%s%s", pfx, opts[i]);
            }
            draw_text_scaled(g->renderer, 70, 145 + i * 30, line, c, 2);
        }

        // Thông báo
        if (g->admin_msg_timer > 0) {
            draw_text_scaled(g->renderer, 65, 425, g->admin_msg, green, 2);
        }

        draw_text_scaled(g->renderer, 55, 450, "UP/DN-NAV  ENTER-DO  ESC-BACK", white, 1);
    }

    SDL_RenderPresent(g->renderer);
}

// ─────────────────────────────────────────────────────────────────────────────
// game_render dispatcher
// ─────────────────────────────────────────────────────────────────────────────

void game_render(Game *g) {
    SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, 255);
    SDL_RenderClear(g->renderer);

    // Giảm timer admin msg
    if (g->admin_msg_timer > 0) g->admin_msg_timer--;

    switch (g->state) {
        case STATE_MENU:             render_menu(g);             break;
        case STATE_PLAYING:          render_playing(g);          break;
        case STATE_GAME_OVER:        render_game_over(g);        break;
        case STATE_SHOP:             render_shop(g);             break;
        case STATE_ADMIN:            render_admin(g);            break;
        case STATE_2P_CHAR_SELECT:   render_2p_char_select(g);  break;
        case STATE_2P_PLAYING:       render_2p_playing(g);       break;
        case STATE_2P_GAME_OVER:     render_2p_game_over(g);     break;
    }
}
