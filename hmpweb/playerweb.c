#define NOFILE 1
#define WEB 1
//extern void log_int(int n);

#if 0
typedef unsigned long size_t;
void *memset(void *av, int c, size_t len) {
	char *a = av;
	while (len--)
		*a++ = c;
	return av;
}
int memcmp(const void *av, const void *bv, size_t len) {
	const char *a = av, *b = bv;
	while (len--) {
		int n = (unsigned char)*a++ - (unsigned char)*b++;
		if (n)
			return n;
	}
	return 0;
}
#endif

#if 0
void *memchr(const void *pv, int c, size_t len) {
	const char *p = pv;
	while (len--)
		if (*p == c)
			return (void *)p;
		else
			p++;
	return (void *)0;
}

void *memcpy(void *av, const void *bv, size_t len) {
	char *a = av;
	const char *b = bv;
	while (len--)
		*a++ = *b++;
	return av;
}


size_t strlen(const char *s) {
	const char *p = s;
	while (*p)
		p++;
	return p - s;
}
#endif

#include "player.c"

struct player player;

short sndbuf[48000];
int last_rc;

short *playerweb_gen(int samps) {
    last_rc = player_gen(&player, sndbuf, samps);
    return last_rc ? NULL : sndbuf;
}

int playerweb_get_last_rc() {
	return last_rc;
}

//const char *playerweb_play(void *song, int song_len, void *melobnk, int melobnk_len, void *drumbnk, int drumbnk_len, int loop) {
void playerweb_init(int);
const char *playerweb_play_data(void *song_data, int song_len,
	void *melobnk_data, int melobnk_len,
	void *drumbnk_data, int drumbnk_len, int loop) {
	if (!player.freq)
		playerweb_init(48000); //return "not initialized";
	player.song.msg[0] = 0;
	player.song.gen = 0;
	player.loop = loop;
	player.playing = 1;
	player.song.hf = &player.song.hfbuf;
	if (hmpopl_set_bank(player.song.h, melobnk_data, melobnk_len, 0))
		return "read melobnk failed";
	if (hmpopl_set_bank(player.song.h, drumbnk_data, drumbnk_len, 1))
		return "read drumbnk failed";
	if (hmp_init(player.song.hf, song_data, song_len, 0xa009))
		return "read song failed";
	hmp_reset_tracks(player.song.hf);
	OPL3_Reset(&player.chip, player.freq);
	hmpopl_reset(player.song.h);
	return NULL;
}


#if 0
char song_data[196608];
char melobnk_data[5404];
char drumbnk_data[5404];
const char *playerweb_play(int song_len, int melobnk_len, int drumbnk_len, int loop) {
	return playerweb_play_data(song_data, song_len,
		melobnk_data, melobnk_len, drumbnk_data, drumbnk_len, loop);
}
#endif

void playerweb_init(int freq) {
	player_init(&player, freq);
}

void playerweb_set_volume(int volume) {
	player.song.volume = volume;
}

int playerweb_get_volume() {
	return player.song.volume;
}

int playerweb_get_time() {
	return player.song.hf->cur_time;
}
