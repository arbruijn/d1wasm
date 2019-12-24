#include "joy.h"
#include "mouse.h"
#include "dpmi.h"
#include "gr.h"
#include "player.h"
#include "state.h"
#include "timer.h"
#include "mono.h"

void joy_set_cen() {}
ubyte joy_get_present_mask() { return 0; }

int dpmi_modify_selector_base(ushort selector, void *address) { return 0; }

#define MOUSE_MAX_BUTTONS	11
typedef struct mouse_info {
	fix		ctime;
	ubyte		cyberman;
	int		num_buttons;
	ubyte		pressed[MOUSE_MAX_BUTTONS];
	fix		time_went_down[MOUSE_MAX_BUTTONS];
	fix		time_held_down[MOUSE_MAX_BUTTONS];
	uint		num_downs[MOUSE_MAX_BUTTONS];
	uint		num_ups[MOUSE_MAX_BUTTONS];
	//event_info *x_info;
	ushort	button_status;
} mouse_info;
static mouse_info Mouse;

fix mouse_button_down_time(int button) {
	fix time_down, time;
	if ( !Mouse.pressed[button] ) {
		time_down = Mouse.time_held_down[button];
		Mouse.time_held_down[button] = 0;
	} else	{
		time = timer_get_fixed_seconds();
		time_down =  time - Mouse.time_went_down[button];
		Mouse.time_went_down[button] = time;
	}
	return time_down;
}
fix joy_get_button_down_time() { return 0; }
int joy_get_button_down_cnt() { return 0; }
int joy_get_button_state(int button) { return Mouse.pressed[button]; }
int mouse_button_down_count(int button) {
	int count = Mouse.num_downs[button];
	Mouse.num_downs[button]=0;
	if (count)
		mprintf((0,"down count %d\n", count));
	return count;
}
int mouse_get_btns() {
	int i;
	uint flag=1;
	int status = 0;

	for (i=0; i<MOUSE_MAX_BUTTONS; i++ )	{
		if (Mouse.pressed[i])
			status |= flag;
		flag <<= 1;
	}
	return status;
}
static int mouse_dx, mouse_dy;
void mouse_get_delta(int *dx, int *dy) {
	*dx = mouse_dx * 5; *dy = mouse_dy * 5;
	mouse_dx = mouse_dy = 0;
}
void on_mousemove(int dx, int dy) {
	mouse_dx += dx;
	mouse_dy += dy;
	mprintf((0, "mousemove %d,%d\n", mouse_dx, mouse_dy));
}
static int dombtns[] = { MB_LEFT, MB_MIDDLE, MB_RIGHT, 3, 4, 5, 6, 7, 8, 9, 10 };
void on_mousebutton(int dombtn, int down) {
	if (dombtn >= sizeof(dombtns) / sizeof(dombtns[0]))
		return;
	Mouse.ctime = timer_get_fixed_seconds();
	int btn = dombtns[dombtn];
	if (down) {
		if (!Mouse.pressed[btn]) {
			Mouse.pressed[btn] = 1;
			Mouse.time_went_down[btn] = Mouse.ctime;
		}
		Mouse.num_downs[btn]++;
	} else {
		if (Mouse.pressed[btn]) {
			Mouse.pressed[btn] = 0;
			Mouse.time_held_down[btn] += Mouse.ctime-Mouse.time_went_down[btn];
		}
		Mouse.num_ups[btn]++;
	}
	mprintf((0, "mousebutton %d->%d %d\n", dombtn, btn, down));
}
ubyte joystick_read_raw_axis(ubyte mask, int *axis) { return 0; }
void victor_read_headset_filtered(fix *yaw, fix *pitch, fix *roll) { *yaw = *pitch = *roll = 0; }
int iglasses_read_headset(fix *yaw, fix *pitch, fix *roll) { *yaw = *pitch = *roll = 0; return 0; }
int joy_get_scaled_reading() { return 0; }
int mouse_button_state() { return 0; }
void mouse_get_cyberman_pos(int *x, int *y) { *x = 0; *y = 0; }
void joy_set_btn_values() {}
void joy_get_cal_vals(int *axis_min, int *axis_center, int *axis_max) { *axis_min = *axis_center = *axis_max = 0; }
void joy_set_cal_vals() { }

int dpmi_init(int verbose) { return 0; }
void mouse_flush() {
	int i;
	fix CurTime = timer_get_fixed_seconds();
	for (i=0; i<MOUSE_MAX_BUTTONS; i++ )	{
		Mouse.pressed[i] = 0;
		Mouse.time_went_down[i] = CurTime;
		Mouse.time_held_down[i] = 0;
		Mouse.num_downs[i]=0;
		Mouse.num_ups[i]=0;
	}
}
void joy_flush() {}
//void joydefs_config() {}
//void joydefs_calibrate() {}

void joy_get_pos(int *x, int *y) { *x = *y = 0; }
int joy_get_btns() { return 0; }

void digi_reset() {}
void timer_set_rate(int n) {}
void joy_set_timer_rate(int n) {}

void vfx_close_graphics() {}
void vfx_init_graphics() {}


void victor_init_graphics() {}

char joy_present;


int state_save_all(int between_levels) { return 0; }
int state_save_old_game(int slotnum, char * sg_name, player * sg_player,
                       int sg_difficulty_level, int sg_primary_weapon,
                        int sg_secondary_weapon, int sg_next_level_num) { return 0; }

int Victor_headset_installed;
int iglasses_headset_installed;

int gr_init_A0000() { return 0; }
void gr_modex_setstart(short x, short y, int wait) {}
void gr_vesa_setstart(short x, short y) {}
int gr_vesa_checkmode(int mode) { return 0; }
void vfx_set_palette_sub(ubyte *pal) {}

unsigned int gr_var_color;
unsigned char *gr_var_bitmap;
unsigned int gr_var_bwidth;
