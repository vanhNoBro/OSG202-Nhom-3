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
#define TREE_W        64
#define TREE_GAP      180
#define TREE_SPACING  264
#define TREE_SPEED    3.0f
#define TREE_TEX_H    294   // chiều cao tự nhiên của texture cột (px)

// ── Coin ─────────────────────────────────────────────────────────────────────
#define COIN_R        12
#define COIN_VALUE    5

// ── Jump animation ───────────────────────────────────────────────────────────
#define JUMP_TEXTURE_DURATION 18

// ── Files ────────────────────────────────────────────────────────────────────
#define HIGHSCORE_FILE   "data/highscore.dat"
#define COINS_FILE       "data/coins.dat"

// ── Skin ─────────────────────────────────────────────────────────────────────
// 0=TEEMO  1=GREEN  2=ORANGE  3=DINO  4=BLUEBIRD  5=SHARK  6=PINK
// 7=SPIKE  8=ROBOT  9=BOMBER  10=DRAGON  11=MUSHROOM  12=PIG  13=DEMON
#define MAX_SKINS   14

static const int SKIN_PRICE[MAX_SKINS] = {
    0,    // TEEMO    - miễn phí
    30,   // GREEN    - rẻ
    30,   // ORANGE
    50,   // DINO
    50,   // BLUEBIRD
    80,   // SHARK
    80,   // PINK
    120,  // SPIKE
    120,  // ROBOT
    150,  // BOMBER
    150,  // DRAGON
    200,  // MUSHROOM
    200,  // PIG
    500,  // DEMON
};

static const char* SKIN_NAME[MAX_SKINS] = {
    "TEEMO", "GREEN", "ORANGE", "DINO", "BLUEBIRD",
    "SHARK", "PINK", "SPIKE", "ROBOT", "BOMBER",
    "DRAGON", "MUSHROOM", "PIG", "DEMON"
};

// Tính năng đặc biệt theo skin:
// 0 TEEMO:    +10% coin bonus
// 1 GREEN:    không có đặc biệt (starter)
// 2 ORANGE:   không có đặc biệt (starter)
// 3 DINO:     hitbox nhỏ hơn (-3px mỗi chiều)
// 4 BLUEBIRD: nhảy cao hơn (JUMP_VEL * 1.15)
// 5 SHARK:    tốc độ cây chậm hơn 10%
// 6 PINK:     coin x1.5
// 7 SPIKE:    miễn nhiễm va chạm đất 1 lần
// 8 ROBOT:    tự động hồi khiên sau 300 frame
// 9 BOMBER:   gravity nhẹ hơn (rơi chậm)
// 10 DRAGON:  khiên 1 lần va chạm (như demon nhưng hồi phục sau 400 frame)
// 11 MUSHROOM:coin x2
// 12 PIG:     hitbox to hơn nhưng coin x2
// 13 DEMON:   khiên 1 lần va chạm (vĩnh viễn trong run)

#define DEMON_SKIN_ID  13
#define TEEMO_SKIN_ID  0

// Skin abilities flags
#define SKIN_COIN_BONUS(s)     ((s)==0)   // TEEMO: +10% coins
#define SKIN_SMALL_HIT(s)      ((s)==3)   // DINO: hitbox nhỏ
#define SKIN_HIGH_JUMP(s)      ((s)==4)   // BLUEBIRD: nhảy cao
#define SKIN_SLOW_TREE(s)      ((s)==5)   // SHARK: cây chậm
#define SKIN_COIN_X1_5(s)      ((s)==6)   // PINK: x1.5 coin
#define SKIN_GROUND_SHIELD(s)  ((s)==7)   // SPIKE: ground shield
#define SKIN_AUTO_SHIELD(s)    ((s)==8)   // ROBOT: tự hồi khiên
#define SKIN_LOW_GRAV(s)       ((s)==9)   // BOMBER: rơi chậm
#define SKIN_REGEN_SHIELD(s)   ((s)==10)  // DRAGON: khiên hồi phục
#define SKIN_COIN_X2(s)        ((s)==11 || (s)==12)  // MUSHROOM/PIG: x2 coin
#define SKIN_DEMON_SHIELD(s)   ((s)==13)  // DEMON: khiên 1 lần
#define SKIN_HAS_SHIELD(s)     (SKIN_DEMON_SHIELD(s) || SKIN_REGEN_SHIELD(s) || SKIN_AUTO_SHIELD(s))

// ── Admin ─────────────────────────────────────────────────────────────────────
#define ADMIN_PASS "1234"

// ── 2P layout ────────────────────────────────────────────────────────────────
#define TEEMO2_OFFSET_X   50

// ── Max trees ────────────────────────────────────────────────────────────────
#define MAX_TREES  8

// ─────────────────────────────────────────────────────────────────────────────
// Audio engine (SDL thuần, không cần SDL_mixer)
// ─────────────────────────────────────────────────────────────────────────────

#define AUDIO_FREQ     44100
#define AUDIO_SAMPLES  256
#define SFX_CHANNELS   8

typedef enum {
    WAVE_SINE       = 0,
    WAVE_SQUARE     = 1,
    WAVE_SWEEP_UP   = 2,
    WAVE_SWEEP_DOWN = 3,
    WAVE_NOISE      = 4
} WaveType;

typedef struct {
    Sint16 *buf;
    int     len;
} SfxBuffer;

// Stream nhạc từ file - chỉ giữ 1 chunk nhỏ trong RAM
#define MUSIC_CHUNK_SAMPLES 8192   // ~186ms buffer per chunk
typedef struct {
    SDL_RWops  *rw;           // file handle
    Sint64      data_start;   // offset đầu PCM data trong file
    Sint64      data_len;     // tổng bytes PCM
    Sint64      pos;          // vị trí đọc hiện tại (bytes)
    int         channels;     // 1=mono, 2=stereo
    int         src_freq;     // tần số gốc
    Sint16      chunk[MUSIC_CHUNK_SAMPLES * 2]; // decode buffer
    int         chunk_pos;    // vị trí đọc trong chunk
    int         chunk_len;    // số samples hợp lệ trong chunk
} MusicStream;

typedef struct {
    SfxBuffer *sfx;
    int        pos;
    bool       active;
    float      volume;
} AudioChannel;

#define BGM_NOTES 16

typedef struct {
    SDL_AudioDeviceID dev;
    SDL_mutex        *lock;

    AudioChannel  ch[SFX_CHANNELS];

    // nhạc nền procedural
    float  bgm_melody[4][BGM_NOTES]; // 4 track x 16 nốt
    int    bgm_current;  // -1 = tắt
    float  bgm_volume;
    float  bgm_bpm;
    int    bgm_pos;      // sample position trong 1 vòng loop
    int    bgm_loop_len; // tổng samples 1 vòng

    SfxBuffer *sfx_jump;
    SfxBuffer *sfx_coin;
    SfxBuffer *sfx_die;
    SfxBuffer *sfx_intro;

    // Atomic pending flags: set từ game thread, callback đọc tức thì
    SDL_atomic_t jump_pending[2]; // [0]=P1, [1]=P2

    // Nhạc từ file WAV — stream trực tiếp, không load hết vào RAM
    MusicStream *music_stream[4];  // stream handle (NULL = chưa mở được)
    bool         music_from_file;  // true = dùng file
    bool         music_muted;      // MUSIC0: tắt nhạc
    int          music_file_track; // track đang phát
} AudioState;

// ─────────────────────────────────────────────────────────────────────────────
// Enums
// ─────────────────────────────────────────────────────────────────────────────

typedef enum {
    STATE_MENU,
    STATE_1P_CHAR_SELECT,   // chọn nhân vật 1P
    STATE_PLAYING,
    STATE_GAME_OVER,
    STATE_SHOP,
    STATE_ADMIN,
    STATE_2P_CHAR_SELECT,
    STATE_2P_READY,
    STATE_2P_PLAYING,
    STATE_2P_GAME_OVER
} GameState;

// ─────────────────────────────────────────────────────────────────────────────
// Structs
// ─────────────────────────────────────────────────────────────────────────────

typedef struct {
    float x, y, vy;
    Uint32 jump_timer;
    int    squash;
    bool   alive;
    bool   shield_active;   // khiên: miễn nhiễm 1 lần va chạm
    int    shield_flash;    // hiệu ứng nhấp nháy sau khi hấp thụ va chạm
    int    shield_cooldown; // cooldown sau khi hấp thụ
    int    shield_regen;    // đếm ngược để hồi khiên (ROBOT/DRAGON)
    bool   ground_shield;   // SPIKE: chặn va chạm đất 1 lần
} Teemo;

typedef struct {
    float x;
    int   gap_y;
    bool  scored;
    bool  coin_active;
    bool  coin_taken;
} Tree;

typedef struct {
    Teemo     teemo;
    Teemo     teemo2;

    Tree      trees[MAX_TREES];
    int       tree_count;

    int       score;
    int       score2;
    int       high_score;
    int       coins;
    int       coins_run;

    int       owned_skins;
    int       skin;
    int       skin2;
    int       shop_cursor;

    bool      auto_play;
    bool      auto_play2;
    GameState state;

    bool      admin_mode;
    char      admin_buf[8];
    int       admin_len;
    char      admin_msg[64];
    int       admin_msg_timer;
    int       admin_cursor;
    bool      god_mode;   // P1 bất tử (hoặc toàn bộ trong 1P)
    bool      god_mode2;  // P2 bất tử (chỉ dùng trong 2P)
    float     tree_speed_mult;

    int       char_select_p;
    bool      char_select_p1_done;

    // 2P Ready screen
    bool      ready_p1;          // P1 đã bấm sẵn sàng
    bool      ready_p2;          // P2 đã bấm sẵn sàng
    int       countdown;         // 180→120→60→0 (3 giây ở 60fps)
    Uint32    countdown_start;   // thời điểm bắt đầu đếm ngược

    int       menu_cursor;

    SDL_Window   *window;
    SDL_Renderer *renderer;

    SDL_Texture  *tex_teemo[MAX_SKINS];
    SDL_Texture  *tex_jump[MAX_SKINS];
    SDL_Texture  *tex_tree;

    AudioState    audio;
    int           music_index;
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
void render_2p_char_select(Game *g);
void render_2p_playing(Game *g);
void render_2p_game_over(Game *g);

void save_highscore(int score);
int  load_highscore(void);
void save_game_data(Game *g);
void load_game_data(Game *g);

void game_next_music(Game *g);
void render_1p_char_select(Game *g);
void render_2p_ready(Game *g);
void game_update_2p_ready(Game *g);
bool audio_load_music_files(AudioState *a);

void audio_init(AudioState *a);
void audio_cleanup(AudioState *a);
void audio_play_sfx(AudioState *a, SfxBuffer *sfx, float vol);
void audio_play_sfx_ch(AudioState *a, SfxBuffer *sfx, float vol, int ch);
void audio_next_bgm(AudioState *a);
void audio_set_bgm(AudioState *a, int track);
SfxBuffer *audio_make_sfx(float freq, float dur, float vol, WaveType wave, float decay);
SfxBuffer *audio_make_intro(void);
void audio_free_sfx(SfxBuffer *sfx);

SDL_Texture *load_texture(SDL_Renderer *r, const char *path);
SDL_Texture *make_colored_teemo(SDL_Renderer *r, SDL_Texture *base, SDL_Color tint);
void draw_text_simple(SDL_Renderer *r, int x, int y, const char *text, SDL_Color col);
void draw_text_scaled(SDL_Renderer *r, int x, int y, const char *text, SDL_Color col, int scale);
void render_panel(SDL_Renderer *r, int x, int y, int w, int h);
void render_coin_hud(SDL_Renderer *r, int x, int y, int coins_run, int coins_total);
void render_coin_sprite(SDL_Renderer *r, int cx, int cy, int r_size, Uint32 ticks);

#endif
