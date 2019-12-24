#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "cfile.h"
#include "timer.h"
#include "gr.h"
#include "pcx.h"
#include "vfx.h"
#include "palette.h"
#include "error.h"
#include "titles.h"
#include "menu.h"
#include "gamefont.h"
#include "text.h"
#include "key.h"
#include "piggy.h"
#include "ukey.h"
#include "piggy.h"
#include "hmpmidi.h"
#include "grdef.h"
#include "object.h"
#include "songs.h"
#include "kconfig.h"
#include "playsave.h"
#include "mission.h"
#include "gameseq.h"

#ifdef WIN32
#include <io.h>
//#define open _open
//#define close(f) _close(f)
//#define read(a,b,c) _read(a,b,c)
#else
#define O_BINARY 0
#endif

#define mprintf(x)
//#define Int3()

#if 0
int descent_critical_error;
#endif

#ifndef WIN32
static int filelength(int fd) {
	struct stat st;
	if (fstat(fd, &st) != 0)
		return -1;
	return st.st_size;
}
#endif

char *hog_data, *pig_data;
int hog_len, pig_len;

#define SCR_W 320
#define SCR_H 200
ubyte gr_screen_buffer[SCR_W*SCR_H];

ubyte *gr_video_memory = gr_screen_buffer;

uint32_t *texture;

LPVOID main_fiber;

#include "piggy.h"
#include "aistruct.h"
#include "robot.h"
#include "weapon.h"
#include "gauges.h"
#include "player.h"
#include "fuelcen.h"
#include "endlevel.h"
#include "cntrlcen.h"
#include "textures.h"
#include "vclip.h"
#include "effects.h"
#include "polyobj.h"
#include "wall.h"
#include "game.h"
#include "powerup.h"

void bm_static_init() {
	NumTextures = 0;
	Num_vclips = 0;
	Num_effects = 0;
	Num_wall_anims = 0;
	N_robot_types = 0;
	N_robot_joints = 0;
	N_weapon_types = 0;
	N_powerup_types = 0;
	N_polygon_models = 0;
	Num_cockpits = 0;
	Num_total_object_types = 0;
	First_multi_bitmap_num = 0;
	N_controlcen_guns = 0;
	exit_modelnum = 0;
	destroyed_exit_modelnum = 0;
}

void gr_sync_display() {
	//printf("sync\n");
	SwitchToFiber(main_fiber);
}

int game_ret;

#if 0
void show_loading() {
	//grs_bitmap title_bm;
	int pcx_error;
	char filename[14];
	ubyte title_pal[768];

	strcpy(filename, "descent.pcx");

	if ((pcx_error=pcx_read_bitmap( filename, &grd_curcanv->cv_bitmap, grd_curcanv->cv_bitmap.bm_type, title_pal ))==PCX_ERROR_NONE)	{
		vfx_set_palette_sub( title_pal );
		gr_palette_clear();
		//gr_bitmap( 0, 0, &title_bm );
		gr_palette_fade_in( title_pal, 32, 0 );
		//free(title_bm.bm_data);
	} else {
		gr_close();
		Error( "Couldn't load pcx file '%s', PCX load error: %s\n",filename, pcx_errormsg(pcx_error));
	}
}

int game_init() {
	load_text();

	extern int gr_installed;
	gr_installed = 0;
	int ret = gr_init(0);
	if (ret) {
		printf("gr_init: %d\n", ret);
		return 5;
	}

	gr_use_palette_table( "PALETTE.256" );
	gamefont_init();

#if 0
	char *filename = "descent.pcx";

	//fix timer;
	int pcx_error;
	grs_bitmap title_bm;
	ubyte New_pal[768];

	title_bm.bm_data=NULL;
	if ((pcx_error=pcx_read_bitmap( filename, &title_bm, BM_LINEAR, New_pal ))!=PCX_ERROR_NONE)	{
		FILE *f = fopen("c:/temp/out.log", "w");
		fprintf(f, "File '%s', PCX load error: %s (%i)\n  (No big deal, just no title screen.)\n",filename, pcx_errormsg(pcx_error), pcx_error);
		fclose(f);
		mprintf((0, "File '%s', PCX load error: %s (%i)\n  (No big deal, just no title screen.)\n",filename, pcx_errormsg(pcx_error), pcx_error));
		Int3();
		return 6;
	}

	vfx_set_palette_sub( New_pal );
	gr_palette_clear();	
	gr_set_current_canvas( NULL );
	gr_bitmap( 0, 0, &title_bm );
	if (gr_palette_fade_in( New_pal, 32, 0 ))
		return 7;
#endif
	//show_title_screen( "iplogo1.pcx", 1 );
	//show_title_screen( "logo.pcx", 1 );
	show_loading();
	for(;;) DoMenu();

	return 0;
}
#endif

void copy_screen() {
	uint32_t pal[256];
	extern ubyte gr_visible_pal[256 * 3];
	#define UP(x) ((((x) & 0x3f) << 2) | (((x) & 0x3f) >> 4))
	for (int i = 0; i < 256; i++)
		pal[i] = UP(gr_visible_pal[i * 3 + 0]) +
			(UP(gr_visible_pal[i * 3 + 1]) << 8) +
			(UP(gr_visible_pal[i * 3 + 2]) << 16) +
			(0xff << 24);
	(void)pal;
	for (int j = 0; j < SCR_H; j++)
		for (int i = 0; i < SCR_W; i++)
			texture[j * SCR_W + i] = pal[gr_screen_buffer[(SCR_H - j - 1) * SCR_W + i]];
}

extern int inferno_init(int argc, char **argv);

static int game_exit = 0;
//static int game_restart = 0;
extern void show_order_form(void);
extern void function_loop(void);

void init_player() {
	Config_joystick_sensitivity = 8;
	Config_control_type =CONTROL_NONE;
	for (int i=0; i<CONTROL_MAX_TYPES; i++ )
		for (int j=0; j<MAX_CONTROLS; j++ )
			kconfig_settings[i][j] = 255; //default_kconfig_settings[i][j];
	kc_set_controls();

	Player_default_difficulty = 1;
	Auto_leveling_on = Default_leveling_on = 1;
	n_highest_levels = 1;
	highest_levels[0].shortname[0] = 0;
	highest_levels[0].level_num = 1;
	Config_joystick_sensitivity = 8;
}

int game_difficulty = 1;
int game_autolevel = 0;
int game_mvolume = 6;
int game_dvolume = 6;
int game_level_num = 1;

__declspec(dllexport) void __stdcall VeryClassicSettings(int difficulty, int autolevel, int dvolume, int mvolume, int level_num) {
	game_difficulty = difficulty < 0 ? 0 : difficulty > 4 ? 4 : difficulty;
	game_autolevel = !!autolevel;
	game_dvolume = dvolume < 0 ? 0 : dvolume > 100 ? 8 : dvolume * 8 / 100;
	game_mvolume = mvolume < 10 ? 0 : mvolume > 80 ? 8 : (mvolume + 20) * 8 / 100;
	game_level_num = level_num < 1 ? 1 : level_num > 7 ? 7 : level_num;
}

uint cc_buttons;
float cc_mouse_x, cc_mouse_y;
float cc_turn_x, cc_turn_y, cc_turn_z;
float cc_move_x, cc_move_y, cc_move_z;
int cc_view_map, cc_rear_view;
int last_view_map, last_rear_view;

__declspec(dllexport) void __stdcall VeryClassicSetControls(uint buttons,
	float mouse_x, float mouse_y,
	float turn_x, float turn_y, float turn_z,
	float move_x, float move_y, float move_z,
	int view_map, int rear_view) {
	cc_buttons = buttons;
	cc_mouse_x = mouse_x, cc_mouse_y = mouse_y;
	cc_turn_x = turn_x, cc_turn_y = turn_y, cc_turn_z = turn_z;
	cc_move_x = move_x, cc_move_y = move_y, cc_move_z = move_z;
	cc_view_map = view_map, cc_rear_view = rear_view;
}

inline int ccb_count(int idx) {
	int val = ((cc_buttons >> (idx * 2)) & 3) - 1;
	return val >= 1 ? 1 : 0;
}

inline int ccb_state(int idx) {
	int val = ((cc_buttons >> (idx * 2)) & 3) - 1;
	return val >= 1;
}

inline fix f2fix(float f) {
	return (fix)(f * F1_0);
}

void kconfig_read_external_controls() {
	Controls.pitch_time += fixmul(f2fix(cc_turn_x),FrameTime);
	Controls.vertical_thrust_time += fixmul(f2fix(cc_move_y),FrameTime);
	Controls.heading_time += fixmul(f2fix(cc_turn_y),FrameTime);
	Controls.sideways_thrust_time += fixmul(f2fix(cc_move_x),FrameTime);
	Controls.bank_time += fixmul(-f2fix(cc_turn_z),FrameTime);
	Controls.forward_thrust_time += fixmul(f2fix(cc_move_z),FrameTime);
	Controls.rear_view_down_count += 0;
	Controls.rear_view_down_state |= 0;
	Controls.fire_primary_down_count += ccb_count(0);
	Controls.fire_primary_state |= ccb_state(0);
	Controls.fire_secondary_down_count += ccb_count(1);
	Controls.fire_secondary_state |= ccb_state(1);
	Controls.fire_flare_down_count += ccb_count(6);
	//Controls.drop_bomb_down_count += kc_external_control->drop_bomb_down_count;
	Controls.automap_down_count += last_view_map <= 0 && cc_view_map > 0;
	Controls.automap_state |= cc_view_map > 0;
	Controls.rear_view_down_count += last_rear_view <= 0 && cc_rear_view > 0;
	Controls.rear_view_down_state |= cc_rear_view > 0;

	Controls.heading_time += fixmul(f2fix(cc_mouse_x),FrameTime) * 2 * Config_joystick_sensitivity / 8;
	Controls.pitch_time += fixmul(-f2fix(cc_mouse_y),FrameTime) * 2 * Config_joystick_sensitivity / 8;

	/*
	if (cc_buttons != 0 && cc_buttons != 21845) {
		printf("kconfig_read_external_controls cc_buttons=%x\n", cc_buttons);
		printf("prim %d,%d sec %d,%d\n", Controls.fire_primary_down_count, Controls.fire_primary_state,
			Controls.fire_secondary_down_count, Controls.fire_secondary_state);
	}
	*/

	last_view_map = cc_view_map;
	last_rear_view = cc_rear_view;
	cc_buttons = 0;
	cc_mouse_x = cc_mouse_y = 0;
	cc_turn_x = cc_turn_y = cc_turn_z = 0;
	cc_move_x = cc_move_y = cc_move_z = 0;
	cc_view_map = cc_rear_view = 0;
}

void __stdcall GameFiber(LPVOID arg) {
	game_ret = -1;
	//game_restart = 0;
		char *argv[] = {"x", "-verbose", "-notitles", NULL};

		//game_ret = game_init();
		//printf("running game main...\n");
		game_ret = inferno_init(3, argv);
		
	extern int kc_use_external_control;
	extern char *kc_external_name;
	kc_use_external_control = 1;
	kc_external_name = "OL CONTROLS";

	init_player();
	char *text = "pilot";
	strncpy(Players[Player_num].callsign, text, CALLSIGN_LEN);
	init_game_list();
	if (read_player_file()) { // new player?
		Auto_leveling_on = Default_leveling_on = game_autolevel;
		Config_digi_volume = game_dvolume;
		Config_midi_volume = game_mvolume;
	}
	write_player_file();

	for (;;) {
		Function_mode = FMODE_GAME;
		Difficulty_level = game_difficulty;
		digi_set_volume((Config_digi_volume*32768)/8, (Config_midi_volume*128)/8);
		StartNewGame(game_level_num);
		game();
		Function_mode = FMODE_EXIT;
		show_order_form();

		//printf("pre song done, fmode=%d\n", Function_mode);
		hmp_song_done();
		//printf("post song done\n");
		
		write_player_file();

		game_exit = 1;	

		//int i = 0;
		while(Function_mode == FMODE_EXIT) {
			//printf("%d\n", i++);
			SwitchToFiber(main_fiber);
		}
		//printf("game relooping\n");
		//songs_play_song( SONG_TITLE, 1 );
		//game_restart = 0;
	}
}

LPVOID game_fiber;
extern void timer_init();
typedef void __stdcall (*SoundPlay3DHandler)(int sndnum, int angle, int volume);
SoundPlay3DHandler play;
__declspec(dllexport) void __stdcall VeryClassicSetPlay(SoundPlay3DHandler play_arg) {
    play = play_arg;
}

// pan -32767 ... 32768 (?)
// volume 0..32768
void play_sample(int gsndnum, int pan, int volume) {
	if (play) {
		//printf("play %p %d %d\n", play, sndnum, gsndnum);
		play(gsndnum, pan, volume);
		//printf("play done\n");
	}
}

extern char _binary_descent_hog_start;
extern char _binary_descent_hog_end;
extern char _binary_descent_pig_start;
extern char _binary_descent_pig_end;

__declspec(dllexport) int __stdcall VeryClassicInit(intptr_t screen_ptr, SoundPlay3DHandler play_arg) {
	#if 0
	//HANDLE console;
	//DuplicateHandle(GetCurrentProcess(), GetStdHandle(STD_OUTPUT_HANDLE), GetCurrentProcess(), &console, 0, FALSE, DUPLICATE_SAME_ACCESS);
	FILE *log = freopen("c:/temp/output_log.txt", "w", stdout);
	FILE *err = freopen("c:/temp/output_err.txt", "w", stderr);
	//fopen("output_log.txt", "w");
	setbuf(log, NULL);
	setbuf(err, NULL);
	//freopen(stdout, log);
	//freopen(stderr, err);
	#endif

	if (!main_fiber && !(main_fiber = ConvertThreadToFiber(0)))
		return 101;
	if (!game_fiber && !(game_fiber = CreateFiber(0, GameFiber, 0)))
		return 102;

	timer_init();

	texture = (uint32_t *)screen_ptr;
	play = play_arg;
	//play(113, 0x8000, 0x10000);
	//play(113, 0x8000, 0x10000);

	if (!hog_data) {
		extern int Hogfile_initialized;
		Hogfile_initialized = 0;
		
		#if 1
		hog_data = &_binary_descent_hog_start;
		hog_len = &_binary_descent_hog_end - &_binary_descent_hog_start;
		pig_data = &_binary_descent_pig_start;
		pig_len = &_binary_descent_pig_end - &_binary_descent_pig_start;
		#else
		int fd;
		if ((fd = open("descent.hog", O_RDONLY | O_BINARY)) < 0) {
			perror("open hog");
			return 1001;
			}
		hog_len = filelength(fd);
		if (!(hog_data = malloc(hog_len))) {
			perror("malloc");
			return 2;
		}
		if ((read(fd, hog_data, hog_len) != hog_len)) {
			perror("read hog");
			return 3;
		}
		close(fd);
		if ((fd = open("descent.pig", O_RDONLY | O_BINARY)) < 0) {
			perror("open pig");
			return 201;
			}
		pig_len = filelength(fd);
		if (!(pig_data = malloc(pig_len))) {
			perror("malloc");
			return 202;
		}
		if ((read(fd, pig_data, pig_len) != pig_len)) {
			perror("read pig");
			return 203;
		}
		close(fd);
		#endif
	}
	
	extern int gr_installed;
	if (gr_installed) { // reset canvas
		gr_installed = 0;
		gr_init(0);
	}
	//bm_static_init();
	Function_mode = FMODE_MENU;
	game_exit = 0;
	game_ret = -2;
	//game_restart = 1;
	SwitchToFiber(game_fiber);

	//free(hog_data);
	return game_ret;
}

int cur_key;
char cur_char;
__declspec(dllexport) int __stdcall VeryClassicFrame(int keyCode, int keyChar, SoundPlay3DHandler play_arg) {
	if  (keyCode) {
		cur_key = keyCode;
		cur_char = keyChar;
	} else {
		cur_key = cur_char = 0;
	}
	play = play_arg;
	SwitchToFiber(game_fiber);
	play = NULL;
	copy_screen();
	return game_exit ? -1 : 0;
}

extern void key_handle_key(int scancode);
__declspec(dllexport) void __stdcall VeryClassicKey(int uKeyCode, int pressed) {
	int c = ukeymap[uKeyCode];
	if (c) {
		if (c & 0x80)
			key_handle_key(0xe0);
		key_handle_key((c & 0x7f) | (pressed ? 0 : 0x80));
	}
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID    lpvReserved) {
	if (fdwReason == DLL_PROCESS_DETACH) {
		if (game_fiber) {
			DeleteFiber(game_fiber);
			game_fiber = NULL;
		}
	}
	return TRUE;
}


extern int Num_sound_files;

digi_sound *get_sound(int sndnum) {
    if (sndnum < 0 || sndnum >= MAX_SOUNDS)
        return NULL;
    int n = Sounds[sndnum];
    return n == 255 ? NULL : &GameSounds[n];
}

__declspec(dllexport) int __stdcall VeryClassicSampleLength(int sndnum) {
    digi_sound *snd = get_sound(sndnum);
    return snd && snd->data ? snd->length : -1;
}

extern void unadpcm(ubyte *src, ubyte *dst, int dstlen);

__declspec(dllexport) void __stdcall VeryClassicSampleRead(int sndnum, ubyte *data, int len) {
    digi_sound *snd = get_sound(sndnum);
    if (!snd)
        return;
    //printf("VeryClassicSampleRead %d\n", sndnum);
    fflush(stdout);
    if (len > snd->length)
        len = snd->length;
    unadpcm(snd->data, data, len);
}

__declspec(dllexport) int __stdcall VeryClassicGameSoundCount(int *datalen) {
	int n = Num_sound_files;
	int size = 0;
	for (int i = 0; i < n; i++)
		size += GameSounds[i].length;
	*datalen = size;
	return n;
}

__declspec(dllexport) void __stdcall VeryClassicGameSoundLoad(ubyte *buf, int *ofs, int *len) {
	int n = Num_sound_files;
	int p = 0;
	for (int i = 0; i < n; i++) {
		digi_sound *snd = &GameSounds[i];
		ofs[i] = p;
		len[i] = snd->length;
        	unadpcm(snd->data, buf + p, snd->length);
        	p += snd->length;
        }
}

__declspec(dllexport) void __stdcall VeryClassicKeyFlush(void) {
	key_flush();
}
