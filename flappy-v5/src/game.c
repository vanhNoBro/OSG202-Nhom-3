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

// Color mod helper — 14 skins đều dùng texture gốc, không tint
static void set_skin_color_mod(SDL_Texture *tex, int skin_id) {
    (void)skin_id;
    SDL_SetTextureColorMod(tex, 255, 255, 255);
}


// ─────────────────────────────────────────────────────────────────────────────
// Forward declaration — định nghĩa đầy đủ ở phần Music Streaming bên dưới
static inline Sint16 music_stream_next(MusicStream *ms);

// Audio Engine (SDL audio thuần, không cần SDL_mixer)
// ─────────────────────────────────────────────────────────────────────────────

#include <math.h>

// Callback được SDL audio gọi để lấy samples
static void audio_callback(void *userdata, Uint8 *stream, int len) {
    AudioState *a = (AudioState *)userdata;
    int n_samples = len / 2; // Sint16
    Sint16 *out = (Sint16 *)stream;
    memset(out, 0, len);

    SDL_LockMutex(a->lock);

    // Pending jump SFX (atomic, không chờ lock) — volume giữ nguyên như v4
    if (SDL_AtomicGet(&a->jump_pending[0])) {
        SDL_AtomicSet(&a->jump_pending[0], 0);
        a->ch[0].sfx = a->sfx_jump; a->ch[0].pos = 0;
        a->ch[0].volume = 0.70f;   a->ch[0].active = true;
    }
    if (SDL_AtomicGet(&a->jump_pending[1])) {
        SDL_AtomicSet(&a->jump_pending[1], 0);
        a->ch[1].sfx = a->sfx_jump; a->ch[1].pos = 0;
        a->ch[1].volume = 0.70f;   a->ch[1].active = true;
    }

    // ── Float mixing buffer: tránh Sint16 cộng dồn overflow ─────────────────
    float *fbuf = (float *)alloca(n_samples * sizeof(float));
    memset(fbuf, 0, n_samples * sizeof(float));

    // Mix SFX channels → float (Sint16 / 32767 → [-1,1], giữ volume gốc)
    for (int ci = 0; ci < SFX_CHANNELS; ci++) {
        AudioChannel *ch = &a->ch[ci];
        if (!ch->active || !ch->sfx) continue;
        for (int i = 0; i < n_samples; i++) {
            if (ch->pos >= ch->sfx->len) { ch->active = false; break; }
            fbuf[i] += (ch->sfx->buf[ch->pos] / 32767.0f) * ch->volume;
            ch->pos++;
        }
    }

    // Mix nhạc nền → float
    // music_muted dùng riêng để tắt nhạc, KHÔNG dùng bgm_current=-1
    // vì bgm_current=-1 là trạng thái init (chưa set track), không phải tắt
    if (!a->music_muted) {
        if (a->music_from_file && a->music_file_track >= 0
            && a->music_stream[a->music_file_track]) {
            // Stream trực tiếp từ file — chunk nhỏ, không tốn RAM
            MusicStream *ms = a->music_stream[a->music_file_track];
            for (int i = 0; i < n_samples; i++) {
                Sint16 s = music_stream_next(ms);
                fbuf[i] += (s / 32767.0f) * a->bgm_volume;
            }
        } else if (!a->music_from_file && a->bgm_current >= 0) {
            // Procedural 8-bit BGM
            float bpm      = 120.0f + a->bgm_current * 15.0f;
            float beat     = (float)AUDIO_FREQ * 60.0f / bpm;
            int   loop_len = (int)(beat * BGM_NOTES);
            if (a->bgm_loop_len != loop_len) a->bgm_loop_len = loop_len;
            float *melody  = a->bgm_melody[a->bgm_current];

            for (int i = 0; i < n_samples; i++) {
                int   pos        = a->bgm_pos;
                int   note_idx   = (int)((float)pos / beat);
                if (note_idx >= BGM_NOTES) note_idx = BGM_NOTES - 1;

                float note_start = note_idx * beat;
                float t_note     = (pos - note_start) / (float)AUDIO_FREQ;
                float note_dur   = beat * 0.82f / (float)AUDIO_FREQ;
                float env        = (t_note < note_dur) ? (1.0f - t_note / note_dur) : 0.0f;

                float f     = melody[note_idx];
                float t_abs = (float)pos / (float)AUDIO_FREQ;
                float sq    = sinf(2.0f * 3.14159265f * f * t_abs) > 0.0f ? 1.0f : -1.0f;
                float bass  = sinf(3.14159265f * f * t_abs);
                float s     = (sq * 0.55f + bass * 0.25f) * env;

                fbuf[i] += s * a->bgm_volume;

                a->bgm_pos++;
                if (a->bgm_pos >= a->bgm_loop_len) a->bgm_pos = 0;
            }
        }
    }

    // Float → Sint16: clamp ±1 rồi scale, tránh hard clipping
    for (int i = 0; i < n_samples; i++) {
        float v = fbuf[i];
        if (v >  1.0f) v =  1.0f;
        if (v < -1.0f) v = -1.0f;
        out[i] = (Sint16)(v * 32767.0f);
    }

    SDL_UnlockMutex(a->lock);
}

// Tạo SFX buffer từ sóng tổng hợp
SfxBuffer *audio_make_sfx(float freq, float dur, float vol,
                           WaveType wave, float decay) {
    int n = (int)(AUDIO_FREQ * dur);
    Sint16 *buf = (Sint16 *)malloc(n * sizeof(Sint16));
    if (!buf) return NULL;

    for (int i = 0; i < n; i++) {
        float t   = (float)i / AUDIO_FREQ;
        float env = expf(-decay * t);
        float f   = freq;
        if (wave == WAVE_SWEEP_UP)   f = freq * (1.0f + 3.5f * t / dur);
        if (wave == WAVE_SWEEP_DOWN) f = freq * (1.0f + 3.5f * (1.0f - t / dur));

        float s = 0.0f;
        switch (wave) {
            case WAVE_SINE:
                s = sinf(2.0f * 3.14159265f * f * t);
                break;
            case WAVE_SQUARE:
                s = sinf(2.0f * 3.14159265f * f * t) > 0.0f ? 1.0f : -1.0f;
                break;
            case WAVE_SWEEP_UP:
            case WAVE_SWEEP_DOWN:
                s = sinf(2.0f * 3.14159265f * f * t);
                break;
            case WAVE_NOISE:
                s = ((float)(rand() % 65536) / 32768.0f) - 1.0f;
                break;
        }
        buf[i] = (Sint16)(s * env * vol * 32767.0f);
    }

    SfxBuffer *sfx = (SfxBuffer *)malloc(sizeof(SfxBuffer));
    if (!sfx) { free(buf); return NULL; }
    sfx->buf = buf;
    sfx->len = n;
    return sfx;
}

// Tạo jingle intro (3 nốt do-mi-sol)
SfxBuffer *audio_make_intro(void) {
    float notes[3] = {523.25f, 659.25f, 783.99f};
    float total    = 0.75f;
    int   n        = (int)(AUDIO_FREQ * total);
    Sint16 *buf    = (Sint16 *)malloc(n * sizeof(Sint16));
    if (!buf) return NULL;

    float note_dur = total / 3.0f;
    for (int i = 0; i < n; i++) {
        float t     = (float)i / AUDIO_FREQ;
        int   ni    = (int)(t / note_dur);
        if (ni > 2) ni = 2;
        float tn    = t - ni * note_dur;
        float env   = expf(-6.0f * tn);
        float f     = notes[ni];
        float s     = 0.65f * sinf(2.0f * 3.14159265f * f * t)
                    + 0.25f * sinf(4.0f * 3.14159265f * f * t)
                    + 0.10f * sinf(6.0f * 3.14159265f * f * t);
        buf[i] = (Sint16)(s * env * 0.7f * 32767.0f);
    }

    SfxBuffer *sfx = (SfxBuffer *)malloc(sizeof(SfxBuffer));
    if (!sfx) { free(buf); return NULL; }
    sfx->buf = buf;
    sfx->len = n;
    return sfx;
}

void audio_free_sfx(SfxBuffer *sfx) {
    if (!sfx) return;
    free(sfx->buf);
    free(sfx);
}

// Phát một SFX trên channel rảnh
void audio_play_sfx(AudioState *a, SfxBuffer *sfx, float vol) {
    if (!sfx || a->dev == 0) return;
    SDL_LockMutex(a->lock);
    for (int i = 0; i < SFX_CHANNELS; i++) {
        if (!a->ch[i].active) {
            a->ch[i].sfx    = sfx;
            a->ch[i].pos    = 0;
            a->ch[i].volume = vol;
            a->ch[i].active = true;
            break;
        }
    }
    SDL_UnlockMutex(a->lock);
}

// Phát SFX trên channel CỐ ĐỊNH - restart ngay nếu đang phát
// Dùng cho nhảy: đảm bảo âm thanh luôn bắt đầu đúng lúc nhảy
void audio_play_sfx_ch(AudioState *a, SfxBuffer *sfx, float vol, int ch) {
    if (!sfx || a->dev == 0) return;
    if (ch < 0 || ch >= SFX_CHANNELS) return;
    SDL_LockMutex(a->lock);
    a->ch[ch].sfx    = sfx;
    a->ch[ch].pos    = 0;          // restart từ đầu → sync hoàn toàn với nhảy
    a->ch[ch].volume = vol;
    a->ch[ch].active = true;
    SDL_UnlockMutex(a->lock);
}

// ── Music Streaming ──────────────────────────────────────────────────────────
// Đọc WAV header, lấy offset + size của PCM data, stream từng chunk nhỏ.
// Không load cả file vào RAM → không có vấn đề bộ nhớ.

// Đọc 4 bytes little-endian từ RWops
static Uint32 read_le32(SDL_RWops *rw) {
    Uint8 b[4]; SDL_RWread(rw, b, 1, 4);
    return b[0] | (b[1]<<8) | (b[2]<<16) | (b[3]<<24);
}
static Uint16 read_le16(SDL_RWops *rw) {
    Uint8 b[2]; SDL_RWread(rw, b, 1, 2);
    return b[0] | (b[1]<<8);
}

static MusicStream *music_stream_open(const char *path) {
    SDL_RWops *rw = SDL_RWFromFile(path, "rb");
    if (!rw) return NULL;

    // Parse RIFF/WAV header
    Uint8 tag[4];
    SDL_RWread(rw, tag, 1, 4); // "RIFF"
    if (tag[0]!='R'||tag[1]!='I'||tag[2]!='F'||tag[3]!='F') { SDL_RWclose(rw); return NULL; }
    read_le32(rw); // file size
    SDL_RWread(rw, tag, 1, 4); // "WAVE"
    if (tag[0]!='W'||tag[1]!='A'||tag[2]!='V'||tag[3]!='E') { SDL_RWclose(rw); return NULL; }

    int channels = 0, src_freq = 0;
    Sint64 data_start = 0, data_len = 0;

    // Scan chunks để tìm fmt và data
    while (1) {
        if (SDL_RWread(rw, tag, 1, 4) < 4) break;
        Uint32 chunk_size = read_le32(rw);
        Sint64 chunk_end  = SDL_RWtell(rw) + chunk_size;

        if (tag[0]=='f'&&tag[1]=='m'&&tag[2]=='t'&&tag[3]==' ') {
            read_le16(rw); // audio format (PCM=1)
            channels = read_le16(rw);
            src_freq = (int)read_le32(rw);
            read_le32(rw); // byte rate
            read_le16(rw); // block align
            read_le16(rw); // bits per sample (giả sử 16)
        } else if (tag[0]=='d'&&tag[1]=='a'&&tag[2]=='t'&&tag[3]=='a') {
            data_start = SDL_RWtell(rw);
            data_len   = (Sint64)chunk_size;
            break;
        }
        SDL_RWseek(rw, chunk_end, RW_SEEK_SET);
    }

    if (data_start == 0 || channels == 0) { SDL_RWclose(rw); return NULL; }

    SDL_RWseek(rw, data_start, RW_SEEK_SET);

    MusicStream *ms = (MusicStream *)calloc(1, sizeof(MusicStream));
    if (!ms) { SDL_RWclose(rw); return NULL; }
    ms->rw         = rw;
    ms->data_start = data_start;
    ms->data_len   = data_len;
    ms->pos        = 0;
    ms->channels   = channels;
    ms->src_freq   = src_freq;
    ms->chunk_pos  = 0;
    ms->chunk_len  = 0;

    // Tính tổng samples mono
    Sint64 total_frames = data_len / (channels * 2); // 16-bit
    float dur = (float)total_frames / src_freq;
    printf("Stream opened: %s (%.1fs, %dch, %dHz)\n", path, dur, channels, src_freq);
    return ms;
}

static void music_stream_close(MusicStream *ms) {
    if (!ms) return;
    if (ms->rw) SDL_RWclose(ms->rw);
    free(ms);
}

// Đọc 1 sample mono S16 từ stream, tự động loop
static inline Sint16 music_stream_next(MusicStream *ms) {
    if (ms->chunk_pos >= ms->chunk_len) {
        // Nạp chunk mới từ file
        // Số frame stereo/mono cần đọc
        int frames_to_read = MUSIC_CHUNK_SAMPLES;
        int bytes_per_frame = ms->channels * 2;
        Sint64 bytes_left = ms->data_len - ms->pos;

        if (bytes_left <= 0) {
            // Loop: quay về đầu
            ms->pos = 0;
            SDL_RWseek(ms->rw, ms->data_start, RW_SEEK_SET);
            bytes_left = ms->data_len;
        }

        Sint64 bytes_to_read = (Sint64)frames_to_read * bytes_per_frame;
        if (bytes_to_read > bytes_left) bytes_to_read = bytes_left;
        int frames_read = (int)(bytes_to_read / bytes_per_frame);

        // Đọc raw vào chunk buffer (stereo hoặc mono)
        Sint16 raw[MUSIC_CHUNK_SAMPLES * 2];
        int got = (int)SDL_RWread(ms->rw, raw, bytes_per_frame, frames_read);
        ms->pos += (Sint64)got * bytes_per_frame;

        // Mix stereo → mono nếu cần
        if (ms->channels == 2) {
            for (int i = 0; i < got; i++) {
                int m = ((int)raw[i*2] + (int)raw[i*2+1]) / 2;
                ms->chunk[i] = (Sint16)m;
            }
        } else {
            memcpy(ms->chunk, raw, got * 2);
        }
        ms->chunk_len = got;
        ms->chunk_pos = 0;

        if (got == 0) return 0; // file rỗng/lỗi
    }
    return ms->chunk[ms->chunk_pos++];
}

// Load/open tất cả stream nhạc
bool audio_load_music_files(AudioState *a) {
    const char *paths[4] = {
        "assets/music/music1.wav",
        "assets/music/music2.wav",
        "assets/music/music3.wav",
        "assets/music/music4.wav",
    };
    int loaded = 0;
    for (int i = 0; i < 4; i++) {
        a->music_stream[i] = music_stream_open(paths[i]);
        if (a->music_stream[i]) loaded++;
    }
    a->music_from_file  = (loaded > 0);
    a->music_file_track = 0;
    return loaded > 0;
}

// Chuyển đến track nhạc chỉ định
void audio_set_bgm(AudioState *a, int track) {
    if (a->dev == 0) return;
    SDL_LockMutex(a->lock);
    if (track < 0) {
        // Tắt nhạc (MUSIC0)
        a->music_muted = true;
    } else {
        a->music_muted = false;
        if (a->music_from_file) {
            int t = track % 4;
            for (int i = 0; i < 4; i++) {
                if (a->music_stream[t]) break;
                t = (t + 1) % 4;
            }
            a->music_file_track = t;
            // Reset stream về đầu
            MusicStream *ms = a->music_stream[t];
            if (ms) {
                ms->pos = 0;
                ms->chunk_pos = 0;
                ms->chunk_len = 0;
                SDL_RWseek(ms->rw, ms->data_start, RW_SEEK_SET);
            }
        } else {
            a->bgm_current = track % 4;
            a->bgm_pos     = 0;
            float bpm = 120.0f + (track % 4) * 15.0f;
            float beat = (float)AUDIO_FREQ * 60.0f / bpm;
            a->bgm_loop_len = (int)(beat * BGM_NOTES);
        }
    }
    SDL_UnlockMutex(a->lock);
}

void audio_next_bgm(AudioState *a) {
    int cur = a->music_from_file ? a->music_file_track : a->bgm_current;
    int next = (cur + 1) % 4;
    audio_set_bgm(a, next);
}

// Khởi tạo audio engine
void audio_init(AudioState *a) {
    memset(a, 0, sizeof(AudioState));
    a->bgm_current = -1;
    a->bgm_volume  = 0.55f;

    // Định nghĩa 4 giai điệu
    float m0[BGM_NOTES] = {523.25f,659.25f,783.99f,1046.50f,
                            783.99f,659.25f,523.25f,440.00f,
                            493.88f,587.33f,698.46f,880.00f,
                            698.46f,587.33f,493.88f,392.00f};
    float m1[BGM_NOTES] = {392.00f,440.00f,493.88f,523.25f,
                            587.33f,659.25f,698.46f,783.99f,
                            880.00f,783.99f,698.46f,659.25f,
                            587.33f,523.25f,493.88f,440.00f};
    float m2[BGM_NOTES] = {349.23f,369.99f,415.30f,466.16f,
                            523.25f,466.16f,415.30f,369.99f,
                            329.63f,369.99f,415.30f,440.00f,
                            493.88f,440.00f,415.30f,349.23f};
    float m3[BGM_NOTES] = {659.25f,783.99f,659.25f,587.33f,
                            523.25f,587.33f,659.25f,783.99f,
                            880.00f,783.99f,659.25f,523.25f,
                            440.00f,493.88f,523.25f,659.25f};
    memcpy(a->bgm_melody[0], m0, sizeof(m0));
    memcpy(a->bgm_melody[1], m1, sizeof(m1));
    memcpy(a->bgm_melody[2], m2, sizeof(m2));
    memcpy(a->bgm_melody[3], m3, sizeof(m3));

    SDL_AtomicSet(&a->jump_pending[0], 0);
    SDL_AtomicSet(&a->jump_pending[1], 0);

    a->lock = SDL_CreateMutex();
    if (!a->lock) {
        fprintf(stderr, "SDL_CreateMutex: %s\n", SDL_GetError());
        return;
    }

    SDL_AudioSpec want, got;
    SDL_zero(want);
    want.freq     = AUDIO_FREQ;
    want.format   = AUDIO_S16SYS;
    want.channels = 1;
    want.samples  = AUDIO_SAMPLES;
    want.callback = audio_callback;
    want.userdata = a;

    a->dev = SDL_OpenAudioDevice(NULL, 0, &want, &got, 0);
    if (a->dev == 0) {
        fprintf(stderr, "SDL_OpenAudioDevice: %s\n", SDL_GetError());
        return;
    }

    // Tạo SFX
    a->sfx_jump  = audio_make_sfx(520.0f, 0.10f, 0.80f, WAVE_SWEEP_UP,   18.0f);
    a->sfx_coin  = audio_make_sfx(900.0f, 0.14f, 0.45f, WAVE_SINE,       11.0f);
    a->sfx_die   = audio_make_sfx(220.0f, 0.38f, 0.65f, WAVE_SWEEP_DOWN,  4.5f);
    a->sfx_intro = audio_make_intro();

    SDL_PauseAudioDevice(a->dev, 0); // bắt đầu phát
}

void audio_cleanup(AudioState *a) {
    if (a->dev) SDL_CloseAudioDevice(a->dev);
    if (a->lock) SDL_DestroyMutex(a->lock);
    for (int i = 0; i < 4; i++) music_stream_close(a->music_stream[i]);
    audio_free_sfx(a->sfx_jump);
    audio_free_sfx(a->sfx_coin);
    audio_free_sfx(a->sfx_die);
    audio_free_sfx(a->sfx_intro);
}

// ─────────────────────────────────────────────────────────────────────────────
// Init
// ─────────────────────────────────────────────────────────────────────────────

bool game_init(Game *g) {
    memset(g, 0, sizeof(Game));
    srand((unsigned)time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
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

    // Load textures — 14 skins
    // Tên file tương ứng từng skin
    static const char *skin_files[MAX_SKINS] = {
        "assets/teemo.png",      // 0 TEEMO
        "assets/green.png",      // 1 GREEN
        "assets/orange.png",     // 2 ORANGE
        "assets/dino.png",       // 3 DINO
        "assets/bluebird.png",   // 4 BLUEBIRD
        "assets/shark.png",      // 5 SHARK
        "assets/pink.png",       // 6 PINK
        "assets/spike.png",      // 7 SPIKE
        "assets/robot.png",      // 8 ROBOT
        "assets/bomber.png",     // 9 BOMBER
        "assets/dragon.png",     // 10 DRAGON
        "assets/mushroom.png",   // 11 MUSHROOM
        "assets/pig.png",        // 12 PIG
        "assets/demon.png",      // 13 DEMON
    };
    static const char *skin_jump_files[MAX_SKINS] = {
        "assets/teemo-jump.png",
        "assets/green-jump.png",
        "assets/orange-jump.png",
        "assets/dino-jump.png",
        "assets/bluebird-jump.png",
        "assets/shark-jump.png",
        "assets/pink-jump.png",
        "assets/spike-jump.png",
        "assets/robot-jump.png",
        "assets/bomber-jump.png",
        "assets/dragon-jump.png",
        "assets/mushroom-jump.png",
        "assets/pig-jump.png",
        "assets/demon-jump.png",
    };

    SDL_Texture *fallback_n = load_texture(g->renderer, "assets/teemo.png");
    SDL_Texture *fallback_j = load_texture(g->renderer, "assets/teemo-jump.png");
    if (!fallback_n || !fallback_j) {
        fprintf(stderr, "Failed to load teemo textures!\n");
        return false;
    }
    for (int i = 0; i < MAX_SKINS; i++) {
        SDL_Texture *tn = load_texture(g->renderer, skin_files[i]);
        SDL_Texture *tj = load_texture(g->renderer, skin_jump_files[i]);
        g->tex_teemo[i] = tn ? tn : fallback_n;
        g->tex_jump[i]  = tj ? tj : (tn ? tn : fallback_j);
    }
    g->tex_tree = load_texture(g->renderer, "assets/tree.png");
    if (!g->tex_tree) {
        fprintf(stderr, "Failed to load tree texture!\n");
        return false;
    }

    load_game_data(g);
    g->state       = STATE_MENU;
    g->menu_cursor = 0;
    g->skin2       = 0;
    g->auto_play2  = false;
    g->god_mode    = false;
    g->god_mode2   = false;
    g->tree_speed_mult = 1.0f;
    g->char_select_p   = 0;
    g->char_select_p1_done = false;
    g->music_index = 0;

    SDL_RWops *rw = SDL_RWFromFile("data/.keep", "w");
    if (rw) SDL_RWclose(rw);

    // Khởi tạo audio
    audio_init(&g->audio);

    // Thử load file nhạc từ assets/music/ trước
    audio_load_music_files(&g->audio);
    if (g->audio.music_from_file)
        printf("Using music files from assets/music/\n");
    else
        printf("No music files found, using procedural BGM\n");

    // Phát intro jingle
    if (g->audio.sfx_intro)
        audio_play_sfx(&g->audio, g->audio.sfx_intro, 0.8f);

    // Bắt đầu nhạc nền track 0
    audio_set_bgm(&g->audio, 0);

    return true;
}

void game_cleanup(Game *g) {
    // Giải phóng tất cả texture skin (tránh double free)
    for (int i = 0; i < MAX_SKINS; i++) {
        if (g->tex_teemo[i]) {
            // Kiểm tra không free trùng
            bool already = false;
            for (int j = 0; j < i; j++) {
                if (g->tex_teemo[j] == g->tex_teemo[i]) { already = true; break; }
            }
            if (!already) SDL_DestroyTexture(g->tex_teemo[i]);
            g->tex_teemo[i] = NULL;
        }
        if (g->tex_jump[i]) {
            bool already = false;
            for (int j = 0; j < i; j++) {
                if (g->tex_jump[j] == g->tex_jump[i]) { already = true; break; }
            }
            // Cũng không free nếu trùng với tex_teemo[i] gốc
            if (!already && g->tex_jump[i] != g->tex_teemo[i]) {
                SDL_DestroyTexture(g->tex_jump[i]);
            }
            g->tex_jump[i] = NULL;
        }
    }
    if (g->tex_tree)     SDL_DestroyTexture(g->tex_tree);
    if (g->renderer)     SDL_DestroyRenderer(g->renderer);
    if (g->window)       SDL_DestroyWindow(g->window);

    audio_cleanup(&g->audio);

    IMG_Quit();
    SDL_Quit();
}

// ─────────────────────────────────────────────────────────────────────────────
// Audio helpers
// ─────────────────────────────────────────────────────────────────────────────

void game_next_music(Game *g) {
    // Cycle: -1(OFF) → 0 → 1 → 2 → 3 → -1 → ...
    g->music_index = (g->music_index + 2) % 5 - 1;
    audio_set_bgm(&g->audio, g->music_index);
    // Sync index thực tế (file mode có thể nhảy track trống)
    if (g->music_index >= 0 && g->audio.music_from_file)
        g->music_index = g->audio.music_file_track;
}

// ─────────────────────────────────────────────────────────────────────────────
// Reset
// ─────────────────────────────────────────────────────────────────────────────

void game_reset(Game *g) {
    g->teemo.x             = TEEMO_START_X;
    g->teemo.y             = TEEMO_START_Y;
    g->teemo.vy            = 0;
    g->teemo.jump_timer    = 0;
    g->teemo.squash        = 0;
    g->teemo.alive         = true;
    g->teemo.shield_active   = SKIN_HAS_SHIELD(g->skin);
    g->teemo.shield_flash    = 0;
    g->teemo.shield_cooldown = 0;
    g->teemo.shield_regen    = 0;
    g->teemo.ground_shield   = SKIN_GROUND_SHIELD(g->skin);
    g->score               = 0;
    g->coins_run           = 0;
    g->tree_count          = 0;
    game_spawn_tree(g);
}

void game_reset_2p(Game *g) {
    game_reset(g);
    // P1 ở vị trí bình thường (dẫn trước)
    g->teemo.x             = TEEMO_START_X + TEEMO2_OFFSET_X;
    g->teemo.shield_active   = SKIN_HAS_SHIELD(g->skin);
    g->teemo.shield_flash    = 0;
    g->teemo.shield_cooldown = 0;
    g->teemo.shield_regen    = 0;
    g->teemo.ground_shield   = SKIN_GROUND_SHIELD(g->skin);
    // P2 ở phía sau P1, cùng Y ban đầu
    g->teemo2.x            = TEEMO_START_X;
    g->teemo2.y            = TEEMO_START_Y;
    g->teemo2.vy           = 0;
    g->teemo2.jump_timer   = 0;
    g->teemo2.squash       = 0;
    g->teemo2.alive        = true;
    g->teemo2.shield_active   = SKIN_HAS_SHIELD(g->skin2);
    g->teemo2.shield_flash    = 0;
    g->teemo2.shield_cooldown = 0;
    g->teemo2.shield_regen    = 0;
    g->teemo2.ground_shield   = SKIN_GROUND_SHIELD(g->skin2);
    g->score2              = 0;
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
    if (t->y + TEEMO_H >= SCREEN_H - 50) {
        if (t->ground_shield) {
            // SPIKE: chặn va chạm đất 1 lần, bật lên
            t->ground_shield = false;
            t->shield_flash  = 40;
            t->vy = JUMP_VEL * 0.8f;
            return false;
        }
        return true;
    }
    // Trần
    if (t->y < 0) return true;

    if (t->shield_cooldown > 0) return false;

    // DINO: hitbox nhỏ hơn
    float margin = 10.0f;
    // Lấy skin từ con trỏ (so sánh địa chỉ để biết là teemo hay teemo2)
    int skin_id = (t == &g->teemo) ? g->skin : g->skin2;
    if (SKIN_SMALL_HIT(skin_id)) margin = 14.0f; // hitbox nhỏ hơn 4px

    float tx = t->x + margin;
    float ty = t->y + margin;
    float tw = TEEMO_W - margin * 2;
    float th = TEEMO_H - margin * 2;

    for (int i = 0; i < g->tree_count; i++) {
        Tree *tr = &g->trees[i];
        float tl  = tr->x;
        float tr2 = tr->x + TREE_W;
        float gt  = (float)tr->gap_y;
        float gb  = (float)(tr->gap_y + TREE_GAP);

        if (tx + tw > tl && tx < tr2) {
            if (ty < gt || ty + th > gb) {
                if (t->shield_active) {
                    t->shield_active   = false;
                    t->shield_flash    = 60;
                    t->shield_cooldown = 50;
                    t->vy = JUMP_VEL * 0.7f;
                    // ROBOT/DRAGON: bắt đầu đếm hồi khiên
                    if (SKIN_AUTO_SHIELD(skin_id))  t->shield_regen = 300;
                    if (SKIN_REGEN_SHIELD(skin_id)) t->shield_regen = 400;
                    return false;
                }
                return true;
            }
        }
    }
    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
// Jump
// ─────────────────────────────────────────────────────────────────────────────

void game_jump(Game *g) {
    if (!g->teemo.alive) return;
    float jvel = JUMP_VEL;
    if (SKIN_HIGH_JUMP(g->skin)) jvel *= 1.15f; // BLUEBIRD: nhảy cao hơn
    g->teemo.vy         = jvel;
    g->teemo.jump_timer = JUMP_TEXTURE_DURATION;
    g->teemo.squash     = 6;
    SDL_AtomicSet(&g->audio.jump_pending[0], 1);
}

void game_jump_2(Game *g) {
    if (!g->teemo2.alive) return;
    float jvel = JUMP_VEL;
    if (SKIN_HIGH_JUMP(g->skin2)) jvel *= 1.15f;
    g->teemo2.vy         = jvel;
    g->teemo2.jump_timer = JUMP_TEXTURE_DURATION;
    g->teemo2.squash     = 6;
    SDL_AtomicSet(&g->audio.jump_pending[1], 1);
}

// ─────────────────────────────────────────────────────────────────────────────
// Auto-play AI: dự đoán vật lý để vượt 100+ điểm
// ─────────────────────────────────────────────────────────────────────────────

// ── AI bot dùng chung cho cả P1 và P2 ───────────────────────────────────────
static void bot_ai(Game *g, Teemo *t,
                   void (*jump_fn)(Game *),
                   float speed_mult) {
    if (!t->alive) return;

    float tx_center = t->x + TEEMO_W * 0.5f;
    float ty_center = t->y + TEEMO_H * 0.5f;
    float speed = TREE_SPEED * speed_mult;
    if (speed < 0.1f) speed = 0.1f;

    // ── Tìm cây mục tiêu gần nhất phía trước ─────────────────────────────────
    Tree *target = NULL;
    float nearest_right = 9999.0f;
    for (int i = 0; i < g->tree_count; i++) {
        float tr_right = g->trees[i].x + TREE_W;
        if (tr_right > t->x - 4.0f) { // chồng lên hoặc phía trước
            if (g->trees[i].x < nearest_right) {
                nearest_right = g->trees[i].x;
                target = &g->trees[i];
            }
        }
    }
    if (!target) return;

    float gap_top    = (float)target->gap_y;
    float gap_bottom = (float)(target->gap_y + TREE_GAP);
    float gap_center = (gap_top + gap_bottom) * 0.5f;
    float margin     = TREE_GAP * 0.20f; // vùng an toàn 20%
    float ceil_safe  = gap_top    + margin + TEEMO_H * 0.5f;
    float floor_safe = gap_bottom - margin - TEEMO_H * 0.5f;
    // Điểm nhắm: hơi thấp hơn tâm khe
    float aim_y = gap_center + TREE_GAP * 0.06f;

    // ── Dự đoán vị trí khi đến MÉP TRÁI cây ─────────────────────────────────
    float dist_left = target->x - tx_center;
    float frames_left = (dist_left > 0.0f) ? (dist_left / speed) : 0.0f;
    if (frames_left > 80.0f) frames_left = 80.0f;

    // ── Dự đoán vị trí khi đến GIỮA cây ─────────────────────────────────────
    float dist_mid = (target->x + TREE_W * 0.5f) - tx_center;
    float frames_mid = (dist_mid > 0.0f) ? (dist_mid / speed) : 1.0f;
    if (frames_mid > 100.0f) frames_mid = 100.0f;

    float pvy_m = t->vy, py_m = ty_center;
    for (int f = 0; f < (int)frames_mid; f++) {
        pvy_m += GRAVITY;
        if (pvy_m > VEL_MAX) pvy_m = VEL_MAX;
        py_m += pvy_m;
    }

    // ── Mô phỏng NẾU nhảy ngay bây giờ ──────────────────────────────────────
    float pvy_j = -9.0f; // JUMP_VEL
    float py_j  = ty_center;
    for (int f = 0; f < (int)frames_mid; f++) {
        pvy_j += GRAVITY;
        if (pvy_j > VEL_MAX) pvy_j = VEL_MAX;
        py_j += pvy_j;
    }

    // ── Quyết định ───────────────────────────────────────────────────────────
    // Nguy hiểm: sẽ đâm đáy cột
    bool will_hit_floor = (py_m > floor_safe);
    // Nguy hiểm tức thì: đang ở rất thấp
    bool in_danger_low  = (ty_center > floor_safe);
    // Đang rơi nhanh về phía thấp hơn điểm nhắm
    bool falling_toward_floor = (t->vy > 2.0f) && (ty_center > aim_y);

    // Chặn nhảy: đang lên rất mạnh
    bool rising_fast = (t->vy < -4.0f);
    // Chặn nhảy: nhảy sẽ gây đâm trần (trừ khi đang nguy hiểm dưới)
    bool jump_causes_ceil = (py_j < ceil_safe) && !in_danger_low;
    // Chặn nhảy: đang ở sát trần khe
    bool near_ceiling = (ty_center < ceil_safe) && !in_danger_low;

    bool should_jump  = will_hit_floor || in_danger_low || falling_toward_floor;
    bool should_block = rising_fast || jump_causes_ceil || near_ceiling;

    if (should_jump && !should_block) {
        jump_fn(g);
    }
}

// P1 bot wrapper
void game_auto_update(Game *g) {
    if (!g->auto_play) return;
    bot_ai(g, &g->teemo, game_jump, g->tree_speed_mult);
}

// P2 bot wrapper (dùng cùng logic, khác hàm nhảy)
static void game_auto_update_p2(Game *g) {
    if (!g->auto_play2) return;
    bot_ai(g, &g->teemo2, game_jump_2, g->tree_speed_mult);
}

// ─────────────────────────────────────────────────────────────────────────────
// Update 1P
// ─────────────────────────────────────────────────────────────────────────────

void game_update(Game *g) {
    // Auto AI
    game_auto_update(g);

    // Timers
    if (g->teemo.jump_timer > 0) g->teemo.jump_timer--;
    if (g->teemo.squash > 0)     g->teemo.squash--;
    if (g->teemo.shield_flash    > 0) g->teemo.shield_flash--;
    if (g->teemo.shield_cooldown > 0) g->teemo.shield_cooldown--;
    // Shield regen (ROBOT/DRAGON)
    if (g->teemo.shield_regen > 0) {
        g->teemo.shield_regen--;
        if (g->teemo.shield_regen == 0 && !g->teemo.shield_active)
            g->teemo.shield_active = true;
    }

    // Physics — BOMBER: gravity nhẹ hơn
    float grav = GRAVITY;
    if (SKIN_LOW_GRAV(g->skin)) grav *= 0.80f;
    g->teemo.vy += grav;
    if (g->teemo.vy >  VEL_MAX) g->teemo.vy =  VEL_MAX;
    if (g->teemo.vy < -VEL_MAX) g->teemo.vy = -VEL_MAX;
    g->teemo.y += g->teemo.vy;
    if (g->teemo.y < 0) { g->teemo.y = 0; g->teemo.vy = 0; }

    // Tốc độ cây — SHARK: chậm hơn 10%
    float speed_mult = g->tree_speed_mult;
    if (SKIN_SLOW_TREE(g->skin)) speed_mult *= 0.90f;

    for (int i = 0; i < g->tree_count; i++) {
        g->trees[i].x -= TREE_SPEED * speed_mult;

        if (!g->trees[i].scored && g->trees[i].x + TREE_W < g->teemo.x) {
            g->trees[i].scored = true;
            g->score++;
            if (g->score > g->high_score) g->high_score = g->score;
        }

        // Coin pickup — nhân hệ số theo skin
        if (g->trees[i].coin_active && !g->trees[i].coin_taken) {
            float coin_cx = g->trees[i].x + TREE_W * 0.5f;
            float coin_cy = g->trees[i].gap_y + TREE_GAP * 0.5f;
            float dx = (g->teemo.x + TEEMO_W*0.5f) - coin_cx;
            float dy = (g->teemo.y + TEEMO_H*0.5f) - coin_cy;
            if (sqrtf(dx*dx + dy*dy) < COIN_R + TEEMO_W * 0.4f) {
                g->trees[i].coin_taken = true;
                int val = COIN_VALUE;
                if (SKIN_COIN_X2(g->skin))    val = COIN_VALUE * 2;
                else if (SKIN_COIN_X1_5(g->skin)) val = COIN_VALUE + COIN_VALUE/2;
                else if (SKIN_COIN_BONUS(g->skin)) val = COIN_VALUE + 1;
                g->coins_run += val;
                g->coins     += val;
                audio_play_sfx(&g->audio, g->audio.sfx_coin, 0.5f);
            }
        }
    }

    // Xóa cây
    int i = 0;
    while (i < g->tree_count) {
        if (g->trees[i].x + TREE_W < -10) {
            for (int j = i; j < g->tree_count - 1; j++) g->trees[j] = g->trees[j+1];
            g->tree_count--;
        } else i++;
    }
    if (g->tree_count == 0 || g->trees[g->tree_count-1].x < SCREEN_W - TREE_SPACING)
        game_spawn_tree(g);

    if (!g->god_mode && check_col(g, &g->teemo)) {
        audio_play_sfx(&g->audio, g->audio.sfx_die, 0.7f);
        g->state = STATE_GAME_OVER;
        save_game_data(g);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Update 2P
// ─────────────────────────────────────────────────────────────────────────────

void game_update_2p(Game *g) {
    game_auto_update(g);
    game_auto_update_p2(g);

    // ── Physics P1 ────────────────────────────────────────────────────────────
    if (g->teemo.alive) {
        if (g->teemo.jump_timer > 0) g->teemo.jump_timer--;
        if (g->teemo.squash > 0) g->teemo.squash--;
        if (g->teemo.shield_flash    > 0) g->teemo.shield_flash--;
        if (g->teemo.shield_cooldown > 0) g->teemo.shield_cooldown--;
        if (g->teemo.shield_regen > 0) {
            g->teemo.shield_regen--;
            if (g->teemo.shield_regen == 0 && !g->teemo.shield_active)
                g->teemo.shield_active = true;
        }
        float grav1 = GRAVITY;
        if (SKIN_LOW_GRAV(g->skin)) grav1 *= 0.80f;
        g->teemo.vy += grav1;
        if (g->teemo.vy >  VEL_MAX) g->teemo.vy =  VEL_MAX;
        if (g->teemo.vy < -VEL_MAX) g->teemo.vy = -VEL_MAX;
        g->teemo.y += g->teemo.vy;
        if (g->teemo.y < 0) { g->teemo.y = 0; g->teemo.vy = 0; }
        if (!g->god_mode && check_col(g, &g->teemo)) {
            audio_play_sfx(&g->audio, g->audio.sfx_die, 0.7f);
            g->teemo.alive = false;
        }
    }

    // ── Physics P2 ────────────────────────────────────────────────────────────
    if (g->teemo2.alive) {
        if (g->teemo2.jump_timer > 0) g->teemo2.jump_timer--;
        if (g->teemo2.squash > 0) g->teemo2.squash--;
        if (g->teemo2.shield_flash    > 0) g->teemo2.shield_flash--;
        if (g->teemo2.shield_cooldown > 0) g->teemo2.shield_cooldown--;
        if (g->teemo2.shield_regen > 0) {
            g->teemo2.shield_regen--;
            if (g->teemo2.shield_regen == 0 && !g->teemo2.shield_active)
                g->teemo2.shield_active = true;
        }
        float grav2 = GRAVITY;
        if (SKIN_LOW_GRAV(g->skin2)) grav2 *= 0.80f;
        g->teemo2.vy += grav2;
        if (g->teemo2.vy >  VEL_MAX) g->teemo2.vy =  VEL_MAX;
        if (g->teemo2.vy < -VEL_MAX) g->teemo2.vy = -VEL_MAX;
        g->teemo2.y += g->teemo2.vy;
        if (g->teemo2.y < 0) { g->teemo2.y = 0; g->teemo2.vy = 0; }
        if (!g->god_mode2 && check_col(g, &g->teemo2)) {
            audio_play_sfx(&g->audio, g->audio.sfx_die, 0.7f);
            g->teemo2.alive = false;
        }
    }

    // ── Trees & coins ─────────────────────────────────────────────────────────
    // Tốc độ cây: lấy min của 2 player nếu cả hai dùng SHARK
    float speed_mult = g->tree_speed_mult;
    if (SKIN_SLOW_TREE(g->skin) && SKIN_SLOW_TREE(g->skin2)) speed_mult *= 0.90f;
    else if (SKIN_SLOW_TREE(g->skin) && g->teemo.alive)  speed_mult *= 0.90f;
    else if (SKIN_SLOW_TREE(g->skin2) && g->teemo2.alive) speed_mult *= 0.90f;

    for (int i = 0; i < g->tree_count; i++) {
        g->trees[i].x -= TREE_SPEED * speed_mult;

        if (!g->trees[i].scored && g->trees[i].x + TREE_W < g->teemo.x) {
            g->trees[i].scored = true;
            if (g->teemo.alive)  g->score++;
            if (g->teemo2.alive) g->score2++;
        }

        // Coin P1
        if (g->trees[i].coin_active && !g->trees[i].coin_taken && g->teemo.alive) {
            float ccx = g->trees[i].x + TREE_W * 0.5f;
            float ccy = g->trees[i].gap_y + TREE_GAP * 0.5f;
            float dx = (g->teemo.x + TEEMO_W*0.5f) - ccx;
            float dy = (g->teemo.y + TEEMO_H*0.5f) - ccy;
            if (sqrtf(dx*dx + dy*dy) < COIN_R + TEEMO_W*0.4f) {
                g->trees[i].coin_taken = true;
                int val = COIN_VALUE;
                if (SKIN_COIN_X2(g->skin))     val = COIN_VALUE * 2;
                else if (SKIN_COIN_X1_5(g->skin)) val = COIN_VALUE + COIN_VALUE/2;
                else if (SKIN_COIN_BONUS(g->skin)) val = COIN_VALUE + 1;
                g->coins_run += val; g->coins += val;
                audio_play_sfx(&g->audio, g->audio.sfx_coin, 0.5f);
            }
        }
        // Coin P2
        if (g->trees[i].coin_active && !g->trees[i].coin_taken && g->teemo2.alive) {
            float ccx = g->trees[i].x + TREE_W * 0.5f;
            float ccy = g->trees[i].gap_y + TREE_GAP * 0.5f;
            float dx = (g->teemo2.x + TEEMO_W*0.5f) - ccx;
            float dy = (g->teemo2.y + TEEMO_H*0.5f) - ccy;
            if (sqrtf(dx*dx + dy*dy) < COIN_R + TEEMO_W*0.4f) {
                g->trees[i].coin_taken = true;
                int val = COIN_VALUE;
                if (SKIN_COIN_X2(g->skin2))     val = COIN_VALUE * 2;
                else if (SKIN_COIN_X1_5(g->skin2)) val = COIN_VALUE + COIN_VALUE/2;
                else if (SKIN_COIN_BONUS(g->skin2)) val = COIN_VALUE + 1;
                g->coins_run += val; g->coins += val;
                audio_play_sfx(&g->audio, g->audio.sfx_coin, 0.5f);
            }
        }
    }

    int i = 0;
    while (i < g->tree_count) {
        if (g->trees[i].x + TREE_W < -10) {
            for (int j = i; j < g->tree_count - 1; j++) g->trees[j] = g->trees[j+1];
            g->tree_count--;
        } else i++;
    }
    if (g->tree_count == 0 || g->trees[g->tree_count-1].x < SCREEN_W - TREE_SPACING)
        game_spawn_tree(g);

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

        // ── Cột TRÊN: đỉnh (TIP) hướng XUỐNG vào khe ────────────────────────
        // Chiến lược: tile từ gap_y ngược lên màn hình
        // Đoạn đầu tiên (sát gap_y nhất) dùng TOP của texture = TIP (đỉnh nhọn)
        // Các đoạn tiếp theo tile lên trên = thân cột
        if (tr->gap_y > 0) {
            int total   = tr->gap_y;
            int filled  = 0; // đã vẽ từ gap_y lên trên

            while (filled < total) {
                int remaining = total - filled;
                int seg = (remaining < TREE_TEX_H) ? remaining : TREE_TEX_H;

                SDL_Rect src, dst;
                // Luôn lấy từ đầu texture (TIP ở row 0)
                src.x = 0; src.y = 0; src.w = TREE_W; src.h = seg;
                // Vị trí trên màn hình: bắt đầu từ gap_y-filled-seg lên
                dst.x = x;
                dst.y = tr->gap_y - filled - seg;
                dst.w = TREE_W;
                dst.h = seg;

                // Đoạn đầu tiên (filled==0): TIP texture ở dưới dst → đỉnh nhọn tại gap
                // → KHÔNG flip để TIP (row 0) hiện ở đỉnh dst, tức là xa gap
                // → CẦN flip để TIP ở đáy dst (gần gap)
                // SDL_FLIP_VERTICAL: row 0 (TIP) → đáy dst (gần gap) ✓
                SDL_RenderCopyEx(g->renderer, g->tex_tree, &src, &dst,
                    0.0, NULL, SDL_FLIP_VERTICAL);

                filled += seg;
            }
        }

        // ── Cột DƯỚI: đỉnh (TIP) hướng LÊN vào khe ──────────────────────────
        // Đoạn đầu tiên (sát gap) dùng TOP của texture = TIP
        // Không flip, tile xuống dưới = thân cột bên dưới
        int bot_y = tr->gap_y + TREE_GAP;
        int bot_h = (SCREEN_H - 60) - bot_y;
        if (bot_h > 0) {
            int filled = 0;
            while (filled < bot_h) {
                int remaining = bot_h - filled;
                int seg = (remaining < TREE_TEX_H) ? remaining : TREE_TEX_H;

                SDL_Rect src, dst;
                // Đoạn đầu (TIP): src.y=0, đoạn sau tile thân cột từ src.y=seg tiếp
                // Để TIP luôn ở sát gap: đoạn filled==0 dùng tip (src.y=0)
                // Tile tiếp theo: thân (src.y=0 lặp lại - staff thân giống nhau)
                src.x = 0; src.y = 0; src.w = TREE_W; src.h = seg;
                dst.x = x; dst.y = bot_y + filled; dst.w = TREE_W; dst.h = seg;

                SDL_RenderCopy(g->renderer, g->tex_tree, &src, &dst);
                filled += seg;
            }
        }

        // ── Coin ở giữa khe ──────────────────────────────────────────────────
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

    // Shield flash effect: nhấp nháy đỏ/cam khi vừa hấp thụ va chạm
    bool shield_blink = (t->shield_flash > 0) && ((t->shield_flash / 5) % 2 == 0);
    if (shield_blink) {
        SDL_SetTextureColorMod(tex, 255, 80, 0); // tint đỏ cam
    }

    SDL_Rect dst = { rx, ry, rw, rh };
    SDL_RenderCopyEx(g->renderer, tex, NULL, &dst, angle, NULL, SDL_FLIP_NONE);

    // Shield aura: vẽ vòng sáng đỏ quanh nhân vật nếu khiên còn
    if (t->shield_active) {
        Uint32 tks = SDL_GetTicks();
        // Pulsing glow
        float pulse = 0.5f + 0.5f * sinf(tks * 0.008f);
        Uint8 alpha_glow = (Uint8)(80 + 120 * pulse);
        SDL_SetRenderDrawColor(g->renderer, 255, 60, 0, alpha_glow);
        SDL_Rect glow1 = {rx - 4, ry - 4, rw + 8, rh + 8};
        SDL_Rect glow2 = {rx - 2, ry - 2, rw + 4, rh + 4};
        SDL_RenderDrawRect(g->renderer, &glow1);
        SDL_RenderDrawRect(g->renderer, &glow2);
        // Hiện chữ SHIELD nhỏ
        SDL_Color shield_col = {255, 120, 0, 255};
        draw_text_scaled(g->renderer, rx + 8, ry - 14, "SHD", shield_col, 1);
    }

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
    SDL_Color yellow = {255, 220, 50,  255};
    SDL_Color white  = {255, 255, 255, 255};
    SDL_Color cyan   = {50,  230, 200, 255};
    SDL_Color pink   = {255, 100, 200, 255};
    SDL_Color green  = {80,  255, 100, 255};
    SDL_Color orange = {255, 160,  40, 255};

    // Panel trái: score
    render_panel(g->renderer, 5, 5, 175, 60);
    snprintf(buf, sizeof(buf), "SCORE %d", g->score);
    draw_text_scaled(g->renderer, 12, 12, buf, yellow, 2);
    snprintf(buf, sizeof(buf), "BEST  %d", g->high_score);
    draw_text_scaled(g->renderer, 12, 34, buf, white, 2);

    // Skin name badge trên cùng bên phải score
    render_panel(g->renderer, 5, 70, 130, 20);
    draw_text_scaled(g->renderer, 10, 74, SKIN_NAME[g->skin], yellow, 1);

    // Badges bên dưới
    int badge_y = 94;
    if (g->auto_play) {
        render_panel(g->renderer, 5, badge_y, 88, 20);
        draw_text_scaled(g->renderer, 10, badge_y + 4, "AUTO ON", cyan, 1);
        badge_y += 24;
    }
    if (g->god_mode) {
        render_panel(g->renderer, 5, badge_y, 104, 20);
        SDL_SetRenderDrawColor(g->renderer, pink.r, pink.g, pink.b, 60);
        SDL_Rect glow = {5, badge_y, 104, 20};
        SDL_RenderFillRect(g->renderer, &glow);
        draw_text_scaled(g->renderer, 10, badge_y + 4, "GOD MODE", pink, 1);
        badge_y += 24;
    }
    // Shield badge
    if (g->teemo.shield_active && SKIN_HAS_SHIELD(g->skin)) {
        render_panel(g->renderer, 5, badge_y, 88, 20);
        SDL_SetRenderDrawColor(g->renderer, orange.r, orange.g, orange.b, 60);
        SDL_Rect sg = {5, badge_y, 88, 20};
        SDL_RenderFillRect(g->renderer, &sg);
        draw_text_scaled(g->renderer, 10, badge_y + 4, "SHIELD ON", orange, 1);
    } else if (!g->teemo.shield_active && g->teemo.shield_regen > 0) {
        render_panel(g->renderer, 5, badge_y, 88, 20);
        SDL_Color gray_b = {150, 150, 150, 255};
        snprintf(buf, sizeof(buf), "SHD %ds", g->teemo.shield_regen/60 + 1);
        draw_text_scaled(g->renderer, 10, badge_y + 4, buf, gray_b, 1);
    }

    // Panel phải: xu
    render_coin_hud(g->renderer, SCREEN_W - 105, 5, g->coins_run, g->coins);

    // Hints bar ở đáy
    render_panel(g->renderer, 0, SCREEN_H - 22, SCREEN_W, 22);
    SDL_Color hint = {180, 180, 180, 255};
    if (g->god_mode)
        draw_text_scaled(g->renderer, 8, SCREEN_H - 17, "W/SPC=JUMP  A=AUTO  G=GOD:ON  P=PAUSE  M=MUSIC", hint, 1);
    else
        draw_text_scaled(g->renderer, 8, SCREEN_H - 17, "W/SPC=JUMP  A=AUTO  G=GOD  P=PAUSE  M=MUSIC", hint, 1);

    (void)green;
}

// ─────────────────────────────────────────────────────────────────────────────
// render_menu
// ─────────────────────────────────────────────────────────────────────────────

void render_menu(Game *g) {
    render_background(g);

    SDL_Texture *tex = g->tex_teemo[g->skin];
    set_skin_color_mod(tex, g->skin);
    // Teemo mascot
    SDL_Rect teemo_big = { SCREEN_W/2 - 70, 30, 140, 140 };
    SDL_RenderCopy(g->renderer, tex, NULL, &teemo_big);
    SDL_SetTextureColorMod(tex, 255, 255, 255);

    // Title panel (gradient feel via double draw)
    SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, 160);
    SDL_Rect title_bg = {30, 180, SCREEN_W - 60, 58};
    SDL_RenderFillRect(g->renderer, &title_bg);
    SDL_Color yellow = {255, 220, 50,  255};
    SDL_Color white  = {255, 255, 255, 255};
    SDL_Color gold   = {255, 200, 30,  255};
    // Border vàng đôi
    SDL_SetRenderDrawColor(g->renderer, yellow.r, yellow.g, yellow.b, 255);
    SDL_RenderDrawRect(g->renderer, &title_bg);
    SDL_Rect tb2 = {title_bg.x+2, title_bg.y+2, title_bg.w-4, title_bg.h-4};
    SDL_SetRenderDrawColor(g->renderer, 200, 140, 20, 180);
    SDL_RenderDrawRect(g->renderer, &tb2);

    draw_text_scaled(g->renderer, SCREEN_W/2 - 115, 189, "FLAPPY TEEMO+", yellow, 3);
    draw_text_scaled(g->renderer, SCREEN_W/2 - 95, 222, "ENHANCED EDITION", white, 2);

    // Menu items
    const char *items[] = { "1 PLAYER", "2 PLAYERS", "SHOP", "ADMIN", "QUIT" };
    SDL_Color sel_col = {80,  255, 100, 255};
    SDL_Color nor_col = {210, 210, 210, 255};
    SDL_Color dim_col = {130, 130, 130, 255};

    int my = 252;
    int item_h = 38;
    int panel_h = 5 * item_h + 16;
    render_panel(g->renderer, 70, my - 8, SCREEN_W - 140, panel_h);

    for (int i = 0; i < 5; i++) {
        bool sel = (i == g->menu_cursor);
        SDL_Color c = sel ? sel_col : (i == 4 ? dim_col : nor_col);

        if (sel) {
            // Highlight bar
            SDL_SetRenderDrawColor(g->renderer, 80, 255, 100, 30);
            SDL_Rect hl = {72, my - 6 + i * item_h, SCREEN_W - 144, item_h - 2};
            SDL_RenderFillRect(g->renderer, &hl);
            SDL_SetRenderDrawColor(g->renderer, 80, 255, 100, 120);
            SDL_RenderDrawRect(g->renderer, &hl);
        }

        char line[32];
        snprintf(line, sizeof(line), "%s%s", sel ? "> " : "  ", items[i]);
        draw_text_scaled(g->renderer, 95, my + 10 + i * item_h, line, c, 2);
    }

    // Bottom info bar
    render_panel(g->renderer, 0, SCREEN_H - 44, SCREEN_W, 44);

    // Coins
    char buf[32];
    snprintf(buf, sizeof(buf), "COIN %d", g->coins);
    draw_text_scaled(g->renderer, 10, SCREEN_H - 38, buf, gold, 2);

    // Best score
    snprintf(buf, sizeof(buf), "BEST %d", g->high_score);
    draw_text_scaled(g->renderer, SCREEN_W - 120, SCREEN_H - 38, buf, yellow, 2);

    // Music indicator
    const char *file_names[] = {"TRACK1", "TRACK2", "TRACK3", "TRACK4"};
    const char *proc_names[] = {"CHILL",  "GROOVE", "MYSTIC", "RUSH"  };
    SDL_Color cyan   = {100, 220, 255, 255};
    SDL_Color gray_c = {150, 150, 150, 255};
    char mtrack[32];
    if (g->audio.music_muted) {
        snprintf(mtrack, sizeof(mtrack), "M: OFF");
        draw_text_scaled(g->renderer, SCREEN_W/2 - 38, SCREEN_H - 38, mtrack, gray_c, 2);
    } else {
        int cur_track = g->audio.music_from_file
            ? g->audio.music_file_track : (g->music_index % 4);
        if (cur_track < 0) cur_track = 0;
        if (g->audio.music_from_file)
            snprintf(mtrack, sizeof(mtrack), "M:%s[F]", file_names[cur_track]);
        else
            snprintf(mtrack, sizeof(mtrack), "M:%s", proc_names[cur_track]);
        draw_text_scaled(g->renderer, SCREEN_W/2 - 45, SCREEN_H - 38, mtrack, cyan, 2);
    }

    // Hint: M change music
    SDL_Color hint = {120, 120, 120, 255};
    draw_text_scaled(g->renderer, SCREEN_W/2 - 35, SCREEN_H - 18, "M=MUSIC", hint, 1);

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
// Mô tả tính năng nhân vật (dùng chung toàn game)
// ─────────────────────────────────────────────────────────────────────────────
static const char *CHAR_ABILITY_LINE1[MAX_SKINS] = {
    "+1 xu moi lan nhat",    // TEEMO
    "Khong co dac biet",     // GREEN
    "Khong co dac biet",     // ORANGE
    "Vung cham nho hon",     // DINO
    "Nhay cao hon 15%",      // BLUEBIRD
    "Cay di cham hon 10%",   // SHARK
    "Xu x1.5 moi lan nhat",  // PINK
    "Chan cham dat 1 lan",   // SPIKE
    "Khien + tu hoi 5 giay", // ROBOT
    "Trong luc nhe hon",     // BOMBER
    "Khien + tu hoi 7 giay", // DRAGON
    "Xu x2 moi lan nhat",    // MUSHROOM
    "Xu x2 moi lan nhat",    // PIG
    "Khien 1 lan / luot",    // DEMON
};
static const char *CHAR_ABILITY_LINE2[MAX_SKINS] = {
    "(Them 1 xu/xu pickup)",
    "(Nhan vat co ban)",
    "(Nhan vat co ban)",
    "(De qua cot hon)",
    "(SPACE manh hon)",
    "(De vuot qua cot)",
    "(Kiem xu nhanh)",
    "(Bao ve roi dat 1x)",
    "(Khien tu nap sau 5s)",
    "(Roi cham, de xu ly)",
    "(Khien tu nap sau 7s)",
    "(Gap doi xu nhat duoc)",
    "(Gap doi xu nhat duoc)",
    "(Hap thu 1 va cham)",
};

// ─────────────────────────────────────────────────────────────────────────────
// render_1p_char_select
// ─────────────────────────────────────────────────────────────────────────────

void render_1p_char_select(Game *g) {
    render_background(g);

    SDL_Color yellow = {255, 220,  50, 255};
    SDL_Color white  = {255, 255, 255, 255};
    SDL_Color green  = { 80, 255, 100, 255};
    SDL_Color gray   = {130, 130, 130, 255};
    SDL_Color cyan   = { 80, 220, 255, 255};
    SDL_Color pink   = {255, 100, 200, 255};
    SDL_Color orange = {255, 160,  40, 255};
    SDL_Color gold   = {255, 200,  30, 255};

    // Title
    render_panel(g->renderer, 20, 5, SCREEN_W - 40, 42);
    draw_text_scaled(g->renderer, SCREEN_W/2 - 105, 13, "CHON NHAN VAT 1P", yellow, 2);

    // Preview nhân vật
    int cx = SCREEN_W / 2;
    draw_text_scaled(g->renderer, cx - 86, 86, "<", white, 4);
    draw_text_scaled(g->renderer, cx + 58, 86, ">", white, 4);
    SDL_Texture *tx = g->tex_teemo[g->skin];
    SDL_Rect dst = {cx - 50, 50, 100, 100};
    SDL_RenderCopy(g->renderer, tx, NULL, &dst);

    // Tên nhân vật
    render_panel(g->renderer, cx - 65, 156, 130, 22);
    draw_text_scaled(g->renderer, cx - 55, 161, SKIN_NAME[g->skin], yellow, 2);
    char idx[20];
    snprintf(idx, sizeof(idx), "< %d / %d >", g->skin + 1, MAX_SKINS);
    draw_text_scaled(g->renderer, cx - 44, 184, idx, gray, 1);

    // Panel tính năng
    render_panel(g->renderer, 10, 200, SCREEN_W - 20, 72);
    draw_text_scaled(g->renderer, 16, 206, "TINH NANG:", orange, 1);
    draw_text_scaled(g->renderer, 16, 220, CHAR_ABILITY_LINE1[g->skin], cyan, 1);
    draw_text_scaled(g->renderer, 16, 234, CHAR_ABILITY_LINE2[g->skin], gray, 1);
    bool owned = (g->owned_skins >> g->skin) & 1;
    if (owned) {
        draw_text_scaled(g->renderer, 16, 252, "Trang thai: Da so huu", green, 1);
    } else {
        char pb[40];
        snprintf(pb, sizeof(pb), "Gia: %d xu  (chua so huu)", SKIN_PRICE[g->skin]);
        SDL_Color pc = (g->coins >= SKIN_PRICE[g->skin]) ?
            (SDL_Color){255,200,30,255} : (SDL_Color){255,80,80,255};
        draw_text_scaled(g->renderer, 16, 252, pb, pc, 1);
    }

    // Panel cài đặt
    render_panel(g->renderer, 10, 280, SCREEN_W - 20, 138);
    draw_text_scaled(g->renderer, 16, 288, "CAI DAT:", white, 1);

    // AUTO row
    int oy = 306;
    {
        if (g->auto_play) {
            SDL_SetRenderDrawColor(g->renderer, 80,255,100,25);
            SDL_Rect hl={12,oy-2,SCREEN_W-24,22}; SDL_RenderFillRect(g->renderer,&hl);
        }
        render_panel(g->renderer, 12, oy - 2, SCREEN_W - 24, 22);
        SDL_Color ac = g->auto_play ? green : gray;
        draw_text_scaled(g->renderer, 18, oy + 3, "A  =  Tu dong choi (AUTO):", ac, 1);
        draw_text_scaled(g->renderer, SCREEN_W - 78, oy + 3,
            g->auto_play ? "[ BAT ]" : "[ TAT ]", ac, 1);
    }
    oy += 26;
    // GOD MODE row
    {
        if (g->god_mode) {
            SDL_SetRenderDrawColor(g->renderer, 255,100,200,25);
            SDL_Rect hl={12,oy-2,SCREEN_W-24,22}; SDL_RenderFillRect(g->renderer,&hl);
        }
        render_panel(g->renderer, 12, oy - 2, SCREEN_W - 24, 22);
        SDL_Color gc = g->god_mode ? pink : gray;
        draw_text_scaled(g->renderer, 18, oy + 3, "G  =  Bat tu (GOD MODE):", gc, 1);
        draw_text_scaled(g->renderer, SCREEN_W - 78, oy + 3,
            g->god_mode ? "[ BAT ]" : "[ TAT ]", gc, 1);
    }
    oy += 26;
    // Xu
    char coinbuf[40];
    snprintf(coinbuf, sizeof(coinbuf), "Xu hien co: %d", g->coins);
    draw_text_scaled(g->renderer, 18, oy, coinbuf, gold, 1);

    // Nút bắt đầu
    render_panel(g->renderer, 35, SCREEN_H - 88, SCREEN_W - 70, 38);
    SDL_SetRenderDrawColor(g->renderer, 80,255,100,40);
    SDL_Rect shl={35, SCREEN_H-88, SCREEN_W-70, 38};
    SDL_RenderFillRect(g->renderer, &shl);
    draw_text_scaled(g->renderer, SCREEN_W/2 - 100, SCREEN_H - 77,
        "ENTER / SPACE = BAT DAU", green, 2);

    // Hints
    render_panel(g->renderer, 0, SCREEN_H - 44, SCREEN_W, 44);
    draw_text_scaled(g->renderer, 8, SCREEN_H - 38,
        "LEFT/RIGHT = Doi nhan vat   A = Auto   G = Bat tu", white, 1);
    draw_text_scaled(g->renderer, 8, SCREEN_H - 22, "ESC = Quay lai menu", gray, 1);

    SDL_RenderPresent(g->renderer);
}

// ─────────────────────────────────────────────────────────────────────────────
// render_2p_char_select
// ─────────────────────────────────────────────────────────────────────────────

void render_2p_char_select(Game *g) {
    render_background(g);

    SDL_Color yellow = {255, 220, 50,  255};
    SDL_Color white  = {255, 255, 255, 255};
    SDL_Color p1col  = {100, 200, 255, 255};
    SDL_Color p2col  = {255, 150, 80,  255};
    SDL_Color green  = { 80, 255, 100, 255};
    SDL_Color gray   = {120, 120, 120, 255};
    SDL_Color pink   = {255, 100, 200, 255};

    // Title
    render_panel(g->renderer, 30, 5, SCREEN_W - 60, 42);
    draw_text_scaled(g->renderer, SCREEN_W/2 - 110, 12, "CHON NHAN VAT 2P", yellow, 2);

    int pw = SCREEN_W / 2 - 15;
    int ph_panel = SCREEN_H - 110;
    int py_panel = 52;

    // ── P1 area ──────────────────────────────────────────────────────────────
    {
        bool active = (g->char_select_p == 0);
        SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, 150);
        SDL_Rect pr = {5, py_panel, pw, ph_panel};
        SDL_RenderFillRect(g->renderer, &pr);
        SDL_SetRenderDrawColor(g->renderer,
            active ? p1col.r : 80, active ? p1col.g : 80, active ? p1col.b : 80, 255);
        SDL_RenderDrawRect(g->renderer, &pr);

        draw_text_scaled(g->renderer, 5 + pw/2 - 18, py_panel + 5, "P1", p1col, 2);
        SDL_Texture *tx = g->tex_teemo[g->skin];
        SDL_Rect tdst = {5 + pw/2 - 40, py_panel + 26, 80, 80};
        SDL_RenderCopy(g->renderer, tx, NULL, &tdst);

        draw_text_scaled(g->renderer, 9, py_panel + 113, SKIN_NAME[g->skin], yellow, 2);
        draw_text_scaled(g->renderer, 9, py_panel + 133, CHAR_ABILITY_LINE1[g->skin],
            (SDL_Color){180,220,255,255}, 1);
        draw_text_scaled(g->renderer, 9, py_panel + 145, CHAR_ABILITY_LINE2[g->skin],
            (SDL_Color){140,160,180,255}, 1);

        char idx_buf[16];
        snprintf(idx_buf, sizeof(idx_buf), "< %d/%d >", g->skin + 1, MAX_SKINS);
        draw_text_scaled(g->renderer, 9, py_panel + 160, idx_buf, gray, 1);

        // AUTO toggle
        int oy = py_panel + 177;
        {
            SDL_Color ac = g->auto_play ? green : gray;
            render_panel(g->renderer, 7, oy, pw - 4, 17);
            if (g->auto_play) {
                SDL_SetRenderDrawColor(g->renderer,80,255,100,25);
                SDL_Rect hl={7,oy,pw-4,17}; SDL_RenderFillRect(g->renderer,&hl);
            }
            draw_text_scaled(g->renderer, 11, oy+4, "A = Tu dong:", ac, 1);
            draw_text_scaled(g->renderer, pw-42, oy+4,
                g->auto_play ? "[BAT]":"[TAT]", ac, 1);
        }
        oy += 21;
        // GOD toggle P1
        {
            SDL_Color gc = g->god_mode ? pink : gray;
            render_panel(g->renderer, 7, oy, pw - 4, 17);
            if (g->god_mode) {
                SDL_SetRenderDrawColor(g->renderer,255,100,200,25);
                SDL_Rect hl={7,oy,pw-4,17}; SDL_RenderFillRect(g->renderer,&hl);
            }
            draw_text_scaled(g->renderer, 11, oy+4, "G = Bat tu:", gc, 1);
            draw_text_scaled(g->renderer, pw-42, oy+4,
                g->god_mode ? "[BAT]":"[TAT]", gc, 1);
        }
        oy += 21;
        if (g->char_select_p1_done)
            draw_text_scaled(g->renderer, 9, oy, "DA CHON!", green, 2);
        else if (active)
            draw_text_scaled(g->renderer, 9, oy, "ENTER=Xac nhan", green, 1);
    }

    // ── P2 area ──────────────────────────────────────────────────────────────
    {
        bool active = (g->char_select_p == 1);
        int px2 = SCREEN_W/2 + 10;
        SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, 150);
        SDL_Rect pr = {px2, py_panel, pw, ph_panel};
        SDL_RenderFillRect(g->renderer, &pr);
        SDL_SetRenderDrawColor(g->renderer,
            active ? p2col.r : 80, active ? p2col.g : 80, active ? p2col.b : 80, 255);
        SDL_RenderDrawRect(g->renderer, &pr);

        draw_text_scaled(g->renderer, px2 + pw/2 - 18, py_panel + 5, "P2", p2col, 2);
        SDL_Texture *tx = g->tex_teemo[g->skin2];
        SDL_Rect tdst = {px2 + pw/2 - 40, py_panel + 26, 80, 80};
        SDL_RenderCopy(g->renderer, tx, NULL, &tdst);

        draw_text_scaled(g->renderer, px2+4, py_panel+113, SKIN_NAME[g->skin2], yellow, 2);
        draw_text_scaled(g->renderer, px2+4, py_panel+133, CHAR_ABILITY_LINE1[g->skin2],
            (SDL_Color){255,210,160,255}, 1);
        draw_text_scaled(g->renderer, px2+4, py_panel+145, CHAR_ABILITY_LINE2[g->skin2],
            (SDL_Color){180,160,140,255}, 1);

        char idx_buf[16];
        snprintf(idx_buf, sizeof(idx_buf), "< %d/%d >", g->skin2 + 1, MAX_SKINS);
        draw_text_scaled(g->renderer, px2+4, py_panel+160, idx_buf, gray, 1);

        // AUTO2 toggle
        int oy = py_panel + 177;
        {
            SDL_Color ac2 = g->auto_play2 ? green : gray;
            render_panel(g->renderer, px2+2, oy, pw-4, 17);
            if (g->auto_play2) {
                SDL_SetRenderDrawColor(g->renderer,80,255,100,25);
                SDL_Rect hl={px2+2,oy,pw-4,17}; SDL_RenderFillRect(g->renderer,&hl);
            }
            draw_text_scaled(g->renderer, px2+6, oy+4, "Z = Tu dong:", ac2, 1);
            draw_text_scaled(g->renderer, px2+pw-42, oy+4,
                g->auto_play2 ? "[BAT]":"[TAT]", ac2, 1);
        }
        oy += 21;
        // GOD2 toggle
        {
            SDL_Color gc2 = g->god_mode2 ? pink : gray;
            render_panel(g->renderer, px2+2, oy, pw-4, 17);
            if (g->god_mode2) {
                SDL_SetRenderDrawColor(g->renderer,255,100,200,25);
                SDL_Rect hl={px2+2,oy,pw-4,17}; SDL_RenderFillRect(g->renderer,&hl);
            }
            draw_text_scaled(g->renderer, px2+6, oy+4, "2 = Bat tu:", gc2, 1);
            draw_text_scaled(g->renderer, px2+pw-42, oy+4,
                g->god_mode2 ? "[BAT]":"[TAT]", gc2, 1);
        }
        oy += 21;
        if (active)
            draw_text_scaled(g->renderer, px2+4, oy, "ENTER=Vao game", green, 1);
    }

    // Hints
    render_panel(g->renderer, 0, SCREEN_H - 50, SCREEN_W, 50);
    draw_text_scaled(g->renderer, 8, SCREEN_H-44, "P1: </>=Nhan vat  ENTER=OK  A=Auto  G=Bat tu", p1col, 1);
    draw_text_scaled(g->renderer, 8, SCREEN_H-26, "P2: A/D=Nhan vat  ENTER=OK  Z=Auto  2=Bat tu  ESC=Quay lai", p2col, 1);

    SDL_RenderPresent(g->renderer);
}

// ─────────────────────────────────────────────────────────────────────────────

void render_2p_playing(Game *g) {
    render_background(g);
    render_trees(g);

    SDL_Color p1_col = {100, 200, 255, 255}; // xanh lam - P1
    SDL_Color p2_col = {255, 150, 80,  255}; // cam - P2
    SDL_Color pink   = {255, 100, 200, 255};
    SDL_Color cyan   = {50,  230, 200, 255};

    if (g->teemo.alive)
        render_teemo_at(g, &g->teemo,  g->skin,  p1_col, "P1");
    if (g->teemo2.alive)
        render_teemo_at(g, &g->teemo2, g->skin2, p2_col, "P2");

    // ── HUD P1 (trái) ──────────────────────────────────────────────────────────
    render_panel(g->renderer, 5, 5, 130, 60);
    char buf[32];
    draw_text_scaled(g->renderer, 12, 10, "P1", p1_col, 2);
    snprintf(buf, sizeof(buf), "SC %d", g->score);
    draw_text_scaled(g->renderer, 40, 10, buf, p1_col, 2);
    if (!g->teemo.alive) {
        SDL_Color red = {255, 80, 80, 255};
        draw_text_scaled(g->renderer, 12, 34, "X DEAD", red, 2);
    } else {
        draw_text_scaled(g->renderer, 12, 34, "ALIVE", p1_col, 2);
    }
    // Badges P1
    int by1 = 70;
    if (g->auto_play) {
        render_panel(g->renderer, 5, by1, 84, 20); by1 += 24;
        draw_text_scaled(g->renderer, 9, by1 - 18, "AUTO", cyan, 1);
    }
    if (g->god_mode) {
        render_panel(g->renderer, 5, by1, 96, 20);
        SDL_SetRenderDrawColor(g->renderer, pink.r, pink.g, pink.b, 50);
        SDL_Rect gp1 = {5, by1, 96, 20}; SDL_RenderFillRect(g->renderer, &gp1);
        draw_text_scaled(g->renderer, 9, by1 + 4, "GOD:ON", pink, 1);
        by1 += 24;
    }

    // ── HUD P2 (phải) ─────────────────────────────────────────────────────────
    render_panel(g->renderer, SCREEN_W - 135, 5, 130, 60);
    draw_text_scaled(g->renderer, SCREEN_W - 125, 10, "P2", p2_col, 2);
    snprintf(buf, sizeof(buf), "SC %d", g->score2);
    draw_text_scaled(g->renderer, SCREEN_W - 95, 10, buf, p2_col, 2);
    if (!g->teemo2.alive) {
        SDL_Color red = {255, 80, 80, 255};
        draw_text_scaled(g->renderer, SCREEN_W - 125, 34, "X DEAD", red, 2);
    } else {
        draw_text_scaled(g->renderer, SCREEN_W - 125, 34, "ALIVE", p2_col, 2);
    }
    // Badges P2
    int by2 = 70;
    if (g->auto_play2) {
        render_panel(g->renderer, SCREEN_W - 89, by2, 84, 20); by2 += 24;
        draw_text_scaled(g->renderer, SCREEN_W - 85, by2 - 18, "AUTO", cyan, 1);
    }
    if (g->god_mode2) {
        render_panel(g->renderer, SCREEN_W - 101, by2, 96, 20);
        SDL_SetRenderDrawColor(g->renderer, pink.r, pink.g, pink.b, 50);
        SDL_Rect gp2 = {SCREEN_W - 101, by2, 96, 20}; SDL_RenderFillRect(g->renderer, &gp2);
        draw_text_scaled(g->renderer, SCREEN_W - 97, by2 + 4, "GOD:ON", pink, 1);
        by2 += 24;
    }
    (void)by2;

    // Xu ở giữa trên
    render_coin_hud(g->renderer, SCREEN_W/2 - 50, 10, g->coins_run, g->coins);

    // Hints bar
    render_panel(g->renderer, 0, SCREEN_H - 22, SCREEN_W, 22);
    SDL_Color hint = {180, 180, 180, 255};
    draw_text_scaled(g->renderer, 5, SCREEN_H - 17, "W=P1  1/UP=P2  A=AUTO1  Z=AUTO2  G=GOD1  2=GOD2  P=PAUSE", hint, 1);

    SDL_RenderPresent(g->renderer);
}

// ─────────────────────────────────────────────────────────────────────────────
// game_update_2p_ready — logic đếm ngược, chuyển STATE_2P_PLAYING khi xong
// ─────────────────────────────────────────────────────────────────────────────

void game_update_2p_ready(Game *g) {
    // Auto-play players tự động sẵn sàng
    if (g->auto_play)  g->ready_p1 = true;
    if (g->auto_play2) g->ready_p2 = true;

    if (!g->ready_p1 || !g->ready_p2) return; // chờ cả 2 sẵn sàng

    if (g->countdown > 0) {
        // Beep mỗi khi số đếm thay đổi (frame 180, 120, 60)
        int prev = g->countdown;
        g->countdown--;
        // Phát tiếng beep tại đầu mỗi giây đếm ngược
        if (prev == 180 || prev == 120 || prev == 60) {
            audio_play_sfx(&g->audio, g->audio.sfx_coin, 0.5f); // dùng coin sfx làm beep
        }
    } else {
        // GO! — phát sfx intro rồi chuyển state
        if (g->audio.sfx_intro)
            audio_play_sfx(&g->audio, g->audio.sfx_intro, 0.8f);
        game_reset_2p(g);
        g->state = STATE_2P_PLAYING;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// render_2p_ready  — màn chờ cả 2 sẵn sàng + đếm ngược 3-2-1-GO
// ─────────────────────────────────────────────────────────────────────────────

void render_2p_ready(Game *g) {
    render_background(g);

    SDL_Color white  = {255, 255, 255, 255};
    SDL_Color yellow = {255, 220, 50,  255};
    SDL_Color green  = {80,  255, 100, 255};
    SDL_Color gray   = {140, 140, 140, 255};
    SDL_Color p1col  = {100, 200, 255, 255};
    SDL_Color p2col  = {255, 150, 80,  255};
    SDL_Color red    = {255, 80,  80,  255};

    // Tiêu đề
    render_panel(g->renderer, 60, 30, SCREEN_W - 120, 50);
    draw_text_scaled(g->renderer, SCREEN_W/2 - 110, 42, "2P - GET READY!", yellow, 2);

    // Hiển thị avatar 2 player
    int avatar_y = 105;
    // P1
    {
        SDL_Rect box = {30, avatar_y, 180, 190};
        SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, 160);
        SDL_RenderFillRect(g->renderer, &box);
        SDL_SetRenderDrawColor(g->renderer, p1col.r, p1col.g, p1col.b, 255);
        SDL_RenderDrawRect(g->renderer, &box);
        draw_text_scaled(g->renderer, 30 + 60, avatar_y + 8, "P1", p1col, 3);

        SDL_Texture *tx = g->tex_teemo[g->skin];
        set_skin_color_mod(tx, g->skin);
        SDL_Rect td = {30 + 40, avatar_y + 55, 100, 100};
        SDL_RenderCopy(g->renderer, tx, NULL, &td);
        SDL_SetTextureColorMod(tx, 255, 255, 255);

        // Trạng thái sẵn sàng
        if (g->ready_p1) {
            draw_text_scaled(g->renderer, 30 + 18, avatar_y + 160, "READY!",  green, 2);
        } else if (g->auto_play) {
            draw_text_scaled(g->renderer, 30 + 18, avatar_y + 160, "AUTO RDY", green, 2);
        } else {
            // Nhấp nháy hướng dẫn
            Uint32 t = SDL_GetTicks();
            if ((t / 500) % 2 == 0)
                draw_text_scaled(g->renderer, 30 + 5, avatar_y + 160, "PRESS W/SPC", gray, 2);
        }
    }
    // P2
    {
        SDL_Rect box = {SCREEN_W - 210, avatar_y, 180, 190};
        SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, 160);
        SDL_RenderFillRect(g->renderer, &box);
        SDL_SetRenderDrawColor(g->renderer, p2col.r, p2col.g, p2col.b, 255);
        SDL_RenderDrawRect(g->renderer, &box);
        draw_text_scaled(g->renderer, SCREEN_W - 210 + 60, avatar_y + 8, "P2", p2col, 3);

        SDL_Texture *tx = g->tex_teemo[g->skin2];
        set_skin_color_mod(tx, g->skin2);
        SDL_Rect td = {SCREEN_W - 210 + 40, avatar_y + 55, 100, 100};
        SDL_RenderCopy(g->renderer, tx, NULL, &td);
        SDL_SetTextureColorMod(tx, 255, 255, 255);

        if (g->ready_p2) {
            draw_text_scaled(g->renderer, SCREEN_W - 210 + 18, avatar_y + 160, "READY!", green, 2);
        } else if (g->auto_play2) {
            draw_text_scaled(g->renderer, SCREEN_W - 210 + 18, avatar_y + 160, "AUTO RDY", green, 2);
        } else {
            Uint32 t = SDL_GetTicks();
            if ((t / 500) % 2 == 0)
                draw_text_scaled(g->renderer, SCREEN_W - 210 + 2, avatar_y + 160, "PRESS 1/UP", gray, 2);
        }
    }

    // ── Đếm ngược ──────────────────────────────────────────────────────────
    if (g->ready_p1 && g->ready_p2 && g->countdown > 0) {
        int secs = (g->countdown + 59) / 60; // 3, 2, 1
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", secs);

        // Vòng tròn nền to
        int cx = SCREEN_W / 2, cy = 330;
        int radius = 60;
        // Vẽ vòng tròn bằng SDL_RenderDrawLine (gần đúng)
        SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, 180);
        SDL_Rect circ_bg = {cx - radius, cy - radius, radius*2, radius*2};
        SDL_RenderFillRect(g->renderer, &circ_bg);

        // Màu đổi theo số đếm
        SDL_Color cnt_col = (secs == 3) ? red : (secs == 2) ? yellow : green;
        SDL_SetRenderDrawColor(g->renderer, cnt_col.r, cnt_col.g, cnt_col.b, 255);
        SDL_RenderDrawRect(g->renderer, &circ_bg);

        // Con số to ở giữa
        draw_text_scaled(g->renderer, cx - 16, cy - 28, buf, cnt_col, 5);
    } else if (g->ready_p1 && g->ready_p2 && g->countdown == 0) {
        // GO!
        draw_text_scaled(g->renderer, SCREEN_W/2 - 44, 310, "GO!", green, 5);
    } else if (!g->ready_p1 || !g->ready_p2) {
        // Cả 2 chưa ready: hiện "Waiting..."
        Uint32 t = SDL_GetTicks();
        if ((t / 600) % 2 == 0)
            draw_text_scaled(g->renderer, SCREEN_W/2 - 100, 320, "WAITING...", gray, 2);
    }

    // Hướng dẫn dưới cùng
    render_panel(g->renderer, 5, SCREEN_H - 45, SCREEN_W - 10, 38);
    draw_text_scaled(g->renderer, 12, SCREEN_H - 40, "P1: W/SPACE   P2: 1/UP   G=GOD MODE   ESC: BACK", white, 1);

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

    SDL_Color yellow = {255, 220,  50, 255};
    SDL_Color white  = {255, 255, 255, 255};
    SDL_Color green  = { 80, 255, 100, 255};
    SDL_Color red    = {255,  80,  80, 255};
    SDL_Color gray   = {150, 150, 150, 255};
    SDL_Color gold   = {255, 200,  30, 255};
    SDL_Color cyan   = {100, 220, 255, 255};
    SDL_Color orange = {255, 160,  40, 255};

    // Mô tả ability từng skin
    static const char *SKIN_ABILITY[MAX_SKINS] = {
        "+1 xu moi lan nhat",    // TEEMO
        "Khong co dac biet",     // GREEN
        "Khong co dac biet",     // ORANGE
        "Vung cham nho hon",     // DINO
        "Nhay cao hon 15%",      // BLUEBIRD
        "Cay di cham hon 10%",   // SHARK
        "Xu x1.5 moi lan nhat",  // PINK
        "Chan cham dat 1 lan",   // SPIKE
        "Khien + tu hoi 5 giay", // ROBOT
        "Trong luc nhe hon",     // BOMBER
        "Khien + tu hoi 7 giay", // DRAGON
        "Xu x2 moi lan nhat",    // MUSHROOM
        "Xu x2 moi lan nhat",    // PIG
        "Khien 1 lan / luot",    // DEMON
    };
    static const SDL_Color ABILITY_COL[MAX_SKINS] = {
        {255,220, 50,255}, // TEEMO gold
        {160,160,160,255}, // GREEN gray
        {160,160,160,255}, // ORANGE gray
        {100,220,255,255}, // DINO cyan
        {100,220,255,255}, // BLUEBIRD cyan
        {100,220,255,255}, // SHARK cyan
        {255,200, 80,255}, // PINK gold
        {255,140,  0,255}, // SPIKE orange
        { 80,200,255,255}, // ROBOT blue
        {140,255,140,255}, // BOMBER green
        {255, 80,200,255}, // DRAGON pink
        {255,220, 50,255}, // MUSHROOM gold
        {255,220, 50,255}, // PIG gold
        {255, 80,  0,255}, // DEMON red
    };

    // Header
    render_panel(g->renderer, 15, 5, SCREEN_W - 30, 50);
    draw_text_scaled(g->renderer, 55, 14, "CHARACTER SHOP", yellow, 3);
    char buf[32];
    snprintf(buf, sizeof(buf), "COIN %d", g->coins);
    draw_text_scaled(g->renderer, SCREEN_W - 115, 20, buf, gold, 2);

    // Scrollable list: hiện 7 items, mỗi item cao 72px
    int item_h   = 72;
    int vis_items = 7;
    int list_y0  = 62;

    // Scroll để cursor luôn nhìn thấy
    int scroll = g->shop_cursor - vis_items/2;
    if (scroll < 0) scroll = 0;
    if (scroll > MAX_SKINS - vis_items) scroll = MAX_SKINS - vis_items;
    if (scroll < 0) scroll = 0;

    for (int vi = 0; vi < vis_items; vi++) {
        int i = vi + scroll;
        if (i >= MAX_SKINS) break;

        int sy = list_y0 + vi * item_h;
        bool owned    = (g->owned_skins >> i) & 1;
        bool selected = (g->shop_cursor == i);
        bool equipped = (g->skin == i);

        int bx = 10, bw = SCREEN_W - 20, bh = item_h - 4;

        // Background highlight
        if (selected) {
            SDL_SetRenderDrawColor(g->renderer,
                ABILITY_COL[i].r, ABILITY_COL[i].g, ABILITY_COL[i].b, 40);
            SDL_Rect hl = {bx, sy, bw, bh};
            SDL_RenderFillRect(g->renderer, &hl);
        }
        render_panel(g->renderer, bx, sy, bw, bh);

        // Thumbnail skin
        SDL_Texture *ptex = g->tex_teemo[i];
        set_skin_color_mod(ptex, i);
        SDL_Rect pdst = { bx + 5, sy + 4, 58, 58 };
        SDL_RenderCopy(g->renderer, ptex, NULL, &pdst);
        SDL_SetTextureColorMod(ptex, 255, 255, 255);

        // Tên
        SDL_Color name_col = selected ? yellow : white;
        if (i == DEMON_SKIN_ID) name_col = (SDL_Color){255,80,0,255};
        draw_text_scaled(g->renderer, bx + 70, sy + 5, SKIN_NAME[i], name_col, 2);

        // Ability
        draw_text_scaled(g->renderer, bx + 70, sy + 28, SKIN_ABILITY[i], ABILITY_COL[i], 1);

        // Giá / trạng thái (bên phải)
        if (owned) {
            draw_text_scaled(g->renderer, bx + bw - 115, sy + 5,  "OWNED",  green, 2);
            if (equipped)
                draw_text_scaled(g->renderer, bx + bw - 125, sy + 28, "EQUIPPED", green, 1);
            else
                draw_text_scaled(g->renderer, bx + bw - 85, sy + 28, "E:EQUIP", cyan, 1);
        } else {
            bool afford = (g->coins >= SKIN_PRICE[i]);
            snprintf(buf, sizeof(buf), "%d", SKIN_PRICE[i]);
            draw_text_scaled(g->renderer, bx + bw - 80, sy + 5, buf, afford ? gold : red, 2);
            draw_text_scaled(g->renderer, bx + bw - 80, sy + 28,
                afford ? "B:BUY" : "NO COIN", afford ? white : gray, 1);
        }

        // Mũi tên chọn
        if (selected)
            draw_text_scaled(g->renderer, bx + 2, sy + 22, ">", orange, 2);
    }

    // Scroll indicator
    if (MAX_SKINS > vis_items) {
        snprintf(buf, sizeof(buf), "%d/%d", g->shop_cursor + 1, MAX_SKINS);
        draw_text_scaled(g->renderer, SCREEN_W/2 - 20, SCREEN_H - 42, buf, gray, 1);
    }

    // Hướng dẫn
    render_panel(g->renderer, 0, SCREEN_H - 30, SCREEN_W, 30);
    draw_text_scaled(g->renderer, 8, SCREEN_H - 24,
        "W/S=SCROLL  B=BUY  E=EQUIP  ESC=BACK", white, 1);

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
        case STATE_1P_CHAR_SELECT:   render_1p_char_select(g);  break;
        case STATE_PLAYING:          render_playing(g);          break;
        case STATE_GAME_OVER:        render_game_over(g);        break;
        case STATE_SHOP:             render_shop(g);             break;
        case STATE_ADMIN:            render_admin(g);            break;
        case STATE_2P_CHAR_SELECT:   render_2p_char_select(g);  break;
        case STATE_2P_READY:         render_2p_ready(g);         break;
        case STATE_2P_PLAYING:       render_2p_playing(g);       break;
        case STATE_2P_GAME_OVER:     render_2p_game_over(g);     break;
    }
}
