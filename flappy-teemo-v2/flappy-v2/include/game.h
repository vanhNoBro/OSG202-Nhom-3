#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

// ── Cửa sổ ──────────────────────────────────────────────────────────────────
#define SCREEN_W      480
#define SCREEN_H      640

// ── Vật lý ───────────────────────────────────────────────────────────────────
#define FPS           60
#define GRAVITY       0.5f
#define JUMP_VEL     -9.0f
#define VEL_MAX       12.0f

// ── Teemo ────────────────────────────────────────────────────────────────────
#define TEEMO_W       60
#define TEEMO_H       60
#define TEEMO_START_X  80
#define TEEMO_START_Y 280

// ── Cây ──────────────────────────────────────────────────────────────────────
#define TREE_W        80
#define TREE_GAP      180
#define TREE_SPACING  220
#define TREE_SPEED    3.0f

// ── Coin ─────────────────────────────────────────────────────────────────────
#define COIN_R        12
#define COIN_VALUE    5

// ── Jump animation ───────────────────────────────────────────────────────────
#define JUMP_TEXTURE_DURATION 50

// ── Files ────────────────────────────────────────────────────────────────────
#define HIGHSCORE_FILE   "data/highscore.dat"
#define COINS_FILE       "data/coins.dat"

// ── Skin ─────────────────────────────────────────────────────────────────────
#define MAX_SKINS   4
// Giá các skin (skin 0 miễn phí)
static const int SKIN_PRICE[MAX_SKINS] = {0, 50, 100, 200};
static const char* SKIN_NAME[MAX_SKINS] = {"DEFAULT", "GOLDEN", "SHADOW", "RAINBOW"};

// ── Admin ─────────────────────────────────────────────────────────────────────
#define ADMIN_PASS "1234"

// ── Max trees ────────────────────────────────────────────────────────────────
#define MAX_TREES  8

// ─────────────────────────────────────────────────────────────────────────────
// Enums
// ─────────────────────────────────────────────────────────────────────────────

typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_GAME_OVER,
    STATE_SHOP,
    STATE_ADMIN,
    STATE_2P_CHAR_SELECT,  // NEW: chọn nhân vật 2P
    STATE_2P_PLAYING,
    STATE_2P_GAME_OVER
} GameState;

// ─────────────────────────────────────────────────────────────────────────────
// Structs
// ─────────────────────────────────────────────────────────────────────────────

typedef struct {
    float x, y, vy;
    Uint32 jump_timer;      // đếm ngược hiện jump texture
    int    squash;          // squash counter (0..8)
    bool   alive;
} Teemo;

typedef struct {
    float x;
    int   gap_y;
    bool  scored;
    // Coin
    bool  coin_active;      // có coin không
    bool  coin_taken;       // đã lấy chưa
} Tree;

typedef struct {
    // ── Teemo ──
    Teemo     teemo;
    Teemo     teemo2;       // player 2

    // ── Trees ──
    Tree      trees[MAX_TREES];
    int       tree_count;

    // ── Scores ──
    int       score;        // điểm P1 (lần chơi)
    int       score2;       // điểm P2 (lần chơi)
    int       high_score;
    int       coins;        // xu hiện có
    int       coins_run;    // xu kiếm lần này

    // ── Skins ──
    int       owned_skins;  // bitmask: bit i = sở hữu skin i
    int       skin;         // skin P1 đang dùng
    int       skin2;        // skin P2 đang dùng
    int       shop_cursor;

    // ── Modes ──
    bool      auto_play;
    bool      auto_play2;    // NEW: auto-play P2 trong 2P mode
    GameState state;

    // ── Admin ──
    bool      admin_mode;
    char      admin_buf[8];
    int       admin_len;
    char      admin_msg[64]; // thông báo kết quả lệnh
    int       admin_msg_timer;
    // admin sub-menu
    int       admin_cursor;  // 0=add coins 1=reset 2=unlock all 3=reset score 4=set speed 5=godmode 6=back
    bool      god_mode;      // NEW: godmode - không chết va cây
    float     tree_speed_mult; // NEW: nhân tốc độ cây (1.0 = mặc định)

    // ── Char select (2P) ──
    int       char_select_p;  // 0=đang chọn P1, 1=đang chọn P2
    bool      char_select_p1_done; // P1 đã confirm chưa

    // ── Menu cursor ──
    int       menu_cursor;   // 0=1P 1=2P 2=SHOP 3=ADMIN 4=QUIT

    // ── SDL ──
    SDL_Window   *window;
    SDL_Renderer *renderer;

    SDL_Texture  *tex_teemo[MAX_SKINS];
    SDL_Texture  *tex_jump[MAX_SKINS];
    SDL_Texture  *tex_tree;
} Game;

// ─────────────────────────────────────────────────────────────────────────────
// Function declarations
// ─────────────────────────────────────────────────────────────────────────────

bool game_init(Game *g);
void game_cleanup(Game *g);
void game_reset(Game *g);
void game_reset_2p(Game *g);

void game_jump(Game *g);
void game_jump_2(Game *g);
void game_update(Game *g);
void game_update_2p(Game *g);
void game_auto_update(Game *g);
void game_spawn_tree(Game *g);
bool check_col(Game *g, Teemo *t);

void game_render(Game *g);
void render_menu(Game *g);
void render_playing(Game *g);
void render_game_over(Game *g);
void render_shop(Game *g);
void render_admin(Game *g);
void render_2p_char_select(Game *g);  // NEW
void render_2p_playing(Game *g);
void render_2p_game_over(Game *g);

void save_highscore(int score);
int  load_highscore(void);
void save_game_data(Game *g);
void load_game_data(Game *g);

SDL_Texture *load_texture(SDL_Renderer *r, const char *path);
SDL_Texture *make_colored_teemo(SDL_Renderer *r, SDL_Texture *base, SDL_Color tint);
void draw_text_simple(SDL_Renderer *r, int x, int y, const char *text, SDL_Color col);
void draw_text_scaled(SDL_Renderer *r, int x, int y, const char *text, SDL_Color col, int scale);
void render_panel(SDL_Renderer *r, int x, int y, int w, int h);
void render_coin_hud(SDL_Renderer *r, int x, int y, int coins_run, int coins_total);
void render_coin_sprite(SDL_Renderer *r, int cx, int cy, int r_size, Uint32 ticks);

#endif
