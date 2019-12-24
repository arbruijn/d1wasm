#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <emscripten/emscripten.h>
#include "types.h"
#include "fix.h"
#include "text.h"
#include "mono.h"
#include "inferno.h"
#include "titles.h"
#include "unadpcm.h"
#include "gr.h"
#include "piggy.h"

int inferno_init(int argc,char **argv);
void function_loop();
int inferno_done();

char *hog_data, *pig_data;
int hog_len, pig_len;

#define SCR_W 320
#define SCR_H 400
ubyte gr_screen_buffer[SCR_W*SCR_H];

ubyte *gr_video_memory = gr_screen_buffer;

uint32_t texture[SCR_W*SCR_H];

// void play_sample(int gsndnum, int pan, int volume) {}

char *slurp(const char *path, int *plen) {
	FILE *f;
	long len;
	char *buf;
	if (plen)
		*plen = 0;
	if (!(f = fopen(path, "rb")))
		return NULL;
	fseek(f, 0, SEEK_END);
	len = (int)ftell(f);
	fseek(f, 0, SEEK_SET);
	if (len > INT_MAX) {
		fclose(f);
		return NULL;
	}
	if (!(buf = malloc(len))) {
		fclose(f);
		return NULL;
	}
	if (fread(buf, 1, len, f) != len) {
		fclose(f);
		return NULL;
	}
	fclose(f);
	if (plen)
		*plen = (int)len;
	return buf;
}

int main(int argc, char **argv) {
	hog_data = slurp("descent.hog", &hog_len);
	pig_data = slurp("descent.pig", &pig_len);
	if (!hog_data || !pig_data)
		printf("hog %s pig %s\n", hog_data ? "ok" : "failed", pig_data ? "ok" : "failed");
	for (int i = 0; i < N_TEXT_STRINGS; i++)
		asprintf(&Text_string[i], "%d", i);
	Text_string[514] = "done";
	Text_string[516] = "CHEATER!";
	Text_string[517] = "Loading Data";
	Text_string[521] = "Concussion";
	Text_string[522] = "Homing";
	Text_string[523] = "ProxBomb";

	inferno_init(argc, argv);
	for (;;) {
		function_loop();
		extern void show_order_form();
		extern int main_menu_choice;
		show_order_form();
		Function_mode = FMODE_MENU;
		main_menu_choice = 0;
                show_title_screen( "iplogo1.pcx", 1 );
                show_title_screen( "logo.pcx", 1 );
	}
	inferno_done();
	return 0;
}



void hmp_set_volume(int vol) {
}
void hmp_song_stop() {
}

void hmp_song_done() {
}

int hmp_song_play(char *data, int size, int loop) {
	return 0;
}

#include "dos.h"
int _dos_findfirst(const char *spec, int attr, find_t *data) { return 1; }
int _dos_findnext(find_t *data) { return 1; }

void _makepath(char *path, const char *drive, const char *dir, const char *fname, const char *ext) {
	*path = 0;
}
void _splitpath(const char *path, char *drive, char *dir, char *fname, char *ext) {
	const char *p = strchr(path, '.');
	if (p == NULL)
		p = path + strlen(path);
	if (drive)
		*drive = 0;
	if (dir)
		*dir = 0;
	if (fname) {
		memcpy(fname, path, p - path);
		fname[p - path] = 0;
	}
	if (ext)
		strcpy(ext, p);
}

uint32_t *get_screen() {
	return texture;
}

void copy_screen() {
	uint32_t pal[256];
	extern ubyte gr_visible_pal[256 * 3];
	#define UP(x) ((((x) & 0x3f) << 2) | (((x) & 0x3f) >> 4))
	for (int i = 0; i < 256; i++)
		pal[i] = UP(gr_visible_pal[i * 3 + 0]) +
			(UP(gr_visible_pal[i * 3 + 1]) << 8) +
			(UP(gr_visible_pal[i * 3 + 2]) << 16) +
			(0xff << 24);
	/*
	for (int j = 0; j < SCR_H; j++)
		for (int i = 0; i < SCR_W; i++)
			texture[j * SCR_W + i] = pal[gr_screen_buffer[(SCR_H - j - 1) * SCR_W + i]];
	*/
	uint32_t *dst = texture;
	for (ubyte *src = gr_screen_buffer; src < gr_screen_buffer + SCR_W * SCR_H; src++, dst++)
		*dst = pal[*src];
}

//extern void draw();
EM_JS(void, draw, (), {
  mydraw();
});

double last_frame;
void gr_sync_display() {
	copy_screen();
	draw();
	if (!last_frame) {
		emscripten_sleep(16);
	} else {
		int t = last_frame + 1000.0/60.0 - emscripten_get_now();
		if (t > 17)
			t = 17;
		if (t > 0)
			emscripten_sleep(t);
	}
	last_frame = emscripten_get_now();
}

extern ubyte domkeymap[];
extern void key_handle_key(int scancode);
void on_key(int keycode, int down) {
	int c = domkeymap[keycode];
	if (!c)
		return;
	if (c & 0x80)
		key_handle_key(0xe0);
	key_handle_key((c & 0x7f) | (down ? 0 : 0x80));
}

void kconfig_read_external_controls() {
}


static double start_time;
void timer_init() { start_time = emscripten_get_now(); }
fix timer_get_fixed_seconds() { return (fix)((emscripten_get_now() - start_time) * ((double)F1_0 / 1000.0)); }
fix timer_get_approx_seconds() { return timer_get_fixed_seconds(); }

int filelength(int fd) {
	struct stat st;
	if (fstat(fd, &st))
		return -1;
	return st.st_size;
}

void _mprintf_at( int n, int row, int col, char * format, ... ) {
	va_list vp;
	va_start(vp, format);
	vprintf(format, vp);
	va_end(vp);
}
void _mprintf( int n, char * format, ... ) {
	va_list vp;
	va_start(vp, format);
	vprintf(format, vp);
	va_end(vp);
}

static digi_sound *get_sound(int num) {
	if (num < 0 || num >= sizeof(GameSounds) / sizeof(GameSounds[0]))
		return NULL;
	return &GameSounds[num];
}

int get_sample_size(int num) {
	digi_sound *snd = get_sound(num);
	return snd && snd->data ? snd->length : 0;
}

static unsigned char sndbuf[69976]; // largest shareware sound (nuke01)

unsigned char *get_sample_data(int num) {
	digi_sound *snd = get_sound(num);
	if (!snd || !snd->data || snd->length > sizeof(sndbuf))
		return NULL;
	unadpcm(snd->data, sndbuf, snd->length);
	return sndbuf;
}

EM_JS(void, video_set_res, (int w, int h), {
	videoSetRes(w, h);
});

EM_JS(void, goto_steam, (void), {
	location = "https://store.steampowered.com/app/273570/Descent/";
});

EM_JS(void, goto_gog, (void), {
	location = "https://www.gog.com/game/descent";
});

EM_JS(void, play_ad, (void), {
	Asyncify.handleSleep(function(wakeUp) {
		playAd(wakeUp);
	});
});
EM_JS(void, goto_github, (void), {
	location = "https://github.com/arbruijn/d1wasm";
});
EM_JS(void, goto_opl3, (void), {
	location = "https://nukeykt.retrohost.net/";
});
EM_JS(int, load_filenames, (char *buf, int fnsize, int fnmax, const char *filespec, int stripext), {
	return loadFilenames(buf, fnsize, fnmax, filespec, stripext);
});
