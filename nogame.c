#if 0
//intptr_t piggy_find_bitmap() { return 0; }
//void piggy_bitmap_page_in() {}
void draw_model_picture() {}
void state_save_all() {}
void save_screen_shot() {}
intptr_t iff_read_bitmap() { return 0; }
void set_screen_mode() {}

int Function_mode;
int Game_mode;
//int Gamefonts;
int Next_level_num;
int Robot_info;
grs_bitmap GameBitmaps[MAX_BITMAP_FILES];

intptr_t FindArg() { return 0; }
char **Args;

void kmatrix_view() {}


void newdemo_start_playback() {}
void write_player_file() {}
int build_mission_list() { return 1; }
void load_mission() {}
int get_highest_level() { return 0; }
//void StartNewGame() {}
void get_game_list() {}
void load_player_game() {}
//void ResumeSavedGame() {}
void save_player_game() {}
void joydefs_config() {}
void joydefs_calibrate() {}
void state_restore_all() {}
void scores_view() {}
//void RegisterPlayer() {}
void do_show_help() {}
void network_start_game() {}
void network_join_game() {}
void com_main_menu() {}
void credits_show() {}
int Menu_pcx_name;
int Last_secret_level;
int Songs;

void stop_time() {}
void game_flush_inputs() {}
void multi_menu_poll() {}
void start_time() {}
void reset_cockpit() {}

int Game_turbo_mode;
int Objects;
int VR_sensitivity;
int Use_player_head_angles;
int Victor_headset_installed;
int iglasses_headset_installed;
int FrameTime;
volatile ubyte keyd_pressed[1000];
char keyd_repeat;
volatile int keyd_time_when_last_pressed;

void HUD_init_message() {}


int VR_offscreen_buffer;

int Endlevel_sequence;
int Player_num;
int Players;
int Last_level;
int Mission_list;
int Detail_level;
int Simple_model_threshhold_scale;
int Max_linear_depth;
int Max_perspective_depth;
int Render_depth;
int Difficulty_level;
int Newdemo_state;
int speedtest_on;

vms_matrix *vm_angles_2_matrix(vms_matrix *m,vms_angvec *a) { return m; }
vms_angvec *vm_matrix_2_angles(vms_angvec *a, vms_matrix *m) { return a; }
vms_matrix *vm_matrix_x_matrix(vms_matrix *dest,vms_matrix *src0,vms_matrix *src1) { return dest; }

int Game_suspended;
int Netgame;
int Network_new_game;
int FrameCount;
int last_drawn_cockpit;
int VR_screen_mode;
int Robot_firing_enabled;
int Fusion_charge;
int Auto_fire_fusion_cannon_time;
int N_robot_types;
int network_endlevel_poll2;
int Cheats_enabled;
int my_segments_checksum;
int Level_names;
int Secret_level_names;
int Default_leveling_on;
int Vclip;
int Viewer;
int Effects;
int TmapInfo;
int Segments;
int LeaveGame;
int Current_mission_num;
int object_owner;
int remote_to_local;
int local_to_remote;
int Secondary_weapon;
int Primary_weapon;
int Last_laser_fired_time;
int Next_laser_fire_time;
int GameTime;
int Player_is_dead;
int Network_rejoined;
int Highest_object_index;
int ConsoleObject;

void obj_delete() {}
void multi_make_player_ghost() {}
void init_gauges() {}
void newdemo_record_laser_level() {}
void newdemo_record_player_weapon() {}
void scores_maybe_add_player() {}
void compute_center_point_on_side() {}
void object_create_explosion() {}
vms_vector *vm_vec_scale_add(vms_vector *dest,vms_vector *src1,vms_vector *src2,fix k) { return dest; }
int new_player_config() { return 1; }
void init_game_list() {}
void read_player_file() {}
void show_boxed_message() {}
void load_level() {}
void netmisc_calc_checksum() {}
void clear_boxed_message() {}
void songs_play_level_song() {}
void add_bonus_points_to_score() {}
void obj_relink() {}
void reset_player_object() {}
void find_connected_distance() {}
void disable_matcens() {}
void clear_transient_objects() {}
void multi_endlevel_score() {}
void reset_palette_add() {}
void dead_player_end() {}
void multi_do_death() {}
void game_disable_cheats() {}
void set_highest_level() {}
void init_cockpit() {}
void HUD_clear_messages() {}
void com_level_sync() {}
void vr_reset_display() {}
void network_do_frame() {}
void reset_time() {}
void reset_rear_view() {}
void load_endlevel_data() {}
void init_morphs() {}
void multi_send_position() {}
void multi_send_reappear() {}
void ai_reset_all_paths() {}
void ai_init_boss_for_ship() {}
void multi_send_score() {}
void automap_clear_visited() {}
void init_robots_for_level() {}
void init_ai_objects() {}
void reset_special_effects() {}
void init_controlcen_for_level() {}
void newdemo_set_new_level() {}
//void piggy_load_level_data() {}
void network_level_sync() {}
void multi_prep_level() {}
void init_all_matcens() {}
void multi_end_level() {}
void newdemo_stop_recording() {}
void newdemo_record_start_frame() {}
void multi_endlevel() {}
int Last_level_path_created;
int Fuelcen_control_center_destroyed;
int Highest_segment_index;

int descent_critical_devcode;
int descent_critical_errcode;
int descent_critical_deverror;

void texmerge_flush() {}
void paging_touch_all() {}

int N_polygon_models;
int N_powerup_types;
int N_weapon_types;
int N_robot_joints;
int Num_wall_anims;
int Num_effects;
int NumTextures;

int destroyed_exit_modelnum;
int exit_modelnum;
int controlcen_gun_dirs;
int controlcen_gun_point;
int N_controlcen_guns;


#endif


#if 0
int key_inkey() { gr_sync_display(); return cur_key; }
void key_flush() {}
char key_to_ascii(int keycode) { return cur_char; }
fix key_down_time(int keycode) { return 0; }
int key_peekkey() { return 0; }
int key_getch() { return 0; }
int key_inkey_time(fix *time) { return 0; }
volatile int keyd_time_when_last_pressed;
char keyd_repeat;
volatile unsigned char keyd_pressed[500];
unsigned int key_down_count(int keycode) { return 0; }
#endif
