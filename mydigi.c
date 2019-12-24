#include "cfile.h"
#include "digi.h"
//#include "hmpweb/hmpfile.h"
#include "object.h"
#include "sounds.h"
#include "mono.h"
#include <emscripten.h>

//extern void play_sample(int gsndnum, int pan, int volume);

int digi_volume = 32768; // 0 .. 32768
void digi_set_digi_volume(int dvolume) {
    digi_volume = dvolume;
}
void digi_play_sample_once(int sndnum, fix max_volume) {
    digi_play_sample(sndnum, max_volume);
}
EM_JS(void, music_volume_set, (int mvolume), {
	musicVolumeSet(mvolume);
});

void digi_set_midi_volume(int mvolume) {
	music_volume_set(mvolume);
	//hmp_set_volume(mvolume);
	//printf("mvolume %d\n", mvolume);
	//extern void playerweb_set_volume(int volume);
	//playerweb_set_volume(mvolume);
}
void digi_set_max_channels(int n) {}
extern void *cfiledata(CFILE *f);
extern const char *playerweb_play_data(void *song_data, int song_len,
	void *melobnk_data, int melobnk_len,
	void *drumbnk_data, int drumbnk_len, int loop);
//extern void hmp_song_stop() {}

EM_JS(void, play_stop, (), {
	playStop();
});

EM_JS(void, play_start, (), {
	playStart();
});
EM_JS(int, play_sample, (int num, int pan, int vol, int repeat), {
	return playSample(num, pan, vol, repeat);
});
EM_JS(void, update_sample, (int idx, int pan, int vol), {
	updateSample(idx, pan, vol);
});
EM_JS(void, stop_sample, (int idx), {
	stopSample(idx);
});

void digi_play_midi_song(char *filename, char *melodic_bank, char *drum_bank, int loop) {
	if (!filename) {
		play_stop();
		return;
	}
	CFILE *f = cfopen(filename, "rb");
	CFILE *mf = cfopen(melodic_bank, "rb");
	CFILE *df = cfopen(drum_bank, "rb");
	//const char *msg;
	if (!f || !mf || !df) {
		if (f) cfclose(f);
		if (mf) cfclose(mf);
		if (df) cfclose(df);
		play_stop();
		mprintf((0, "playing %s %s %s failed\n", filename, melodic_bank, drum_bank));
		return;
	}
	//hmp_song_play(cfiledata(f), cfilelength(f), loop);
	const char *msg = playerweb_play_data(cfiledata(f), cfilelength(f),
    		cfiledata(mf), cfilelength(mf),
	    	cfiledata(df), cfilelength(df),
	    	loop);
	    	play_start();
    	(void)msg;
    	mprintf((0, "playing %s %s %s %d, msg: %s\n", filename, melodic_bank, drum_bank, loop, msg));
    	cfclose(f);
    	cfclose(mf);
    	cfclose(df);
}
int digi_midi_port, digi_midi_type, digi_driver_dma, digi_driver_irq, digi_driver_port, digi_driver_board;

#define SOF_USED				1 		// Set if this sample is used
#define SOF_PLAYING			2		// Set if this sample is playing on a channel
#define SOF_LINK_TO_OBJ		4		// Sound is linked to a moving object. If object dies, then finishes play and quits.
#define SOF_LINK_TO_POS		8		// Sound is linked to segment, pos
#define SOF_PLAY_FOREVER	16		// Play forever (or until level is stopped), otherwise plays once

typedef struct sound_object {
	short			signature;		// A unique signature to this sound
	ubyte			flags;			// Used to tell if this slot is used and/or currently playing, and how long.
	fix			max_volume;		// Max volume that this sound is playing at
	fix			max_distance;	// The max distance that this sound can be heard at...
	int			volume;			// Volume that this sound is playing at
	int			pan;				// Pan value that this sound is playing at
	int			handle;			// What handle this sound is playing on.  Valid only if SOF_PLAYING is set.
	short			soundnum;		// The sound number that is playing
	union {	
		struct {
			short			segnum;				// Used if SOF_LINK_TO_POS field is used
			short			sidenum;				
			vms_vector	position;
		};
		struct {
			short			objnum;				// Used if SOF_LINK_TO_OBJ field is used
			short			objsignature;
		};
	};
} sound_object;

#define MAX_SOUND_OBJECTS 16
static sound_object SoundObjects[MAX_SOUND_OBJECTS];
static short next_signature=0;
static int digi_sounds_initialized=0;

void digi_set_volume(int dvolume, int mvolume) {
    digi_set_digi_volume(dvolume);
    digi_set_midi_volume(mvolume);
}
void digi_init_sounds() {
	int i;
	for (i=0; i<MAX_SOUND_OBJECTS; i++ )	{
		if (digi_sounds_initialized) {
			if ( SoundObjects[i].flags & SOF_PLAYING )	{
				stop_sample( SoundObjects[i].handle );
			}
		}
		SoundObjects[i].flags = 0;	// Mark as dead, so some other sound can use this sound
	}
	digi_sounds_initialized = 1;
}
int digi_link_sound_to_pos( int soundnum, short segnum, short sidenum, vms_vector * pos, int forever, fix max_volume ) {
	return digi_link_sound_to_pos2( soundnum, segnum, sidenum, pos, forever, max_volume, F1_0 * 256 );
}
int digi_link_sound_to_object( int soundnum, short objnum, int forever, fix max_volume ) {
	return digi_link_sound_to_object2( soundnum, objnum, forever, max_volume, 256*F1_0  );
}
void digi_play_sample(int sndnum, fix max_volume) {
	digi_play_sample_3d(sndnum, 0x8000, max_volume, 0);
}
int digi_lomem;
int digi_timer_rate;
void digi_pause_all() {
	int i;
	for (i=0; i<MAX_SOUND_OBJECTS; i++ )	{
		if ( (SoundObjects[i].flags & SOF_USED) && (SoundObjects[i].flags & SOF_PLAYING)&& (SoundObjects[i].flags & SOF_PLAY_FOREVER) )	{
			stop_sample( SoundObjects[i].handle );
			SoundObjects[i].flags &= ~SOF_PLAYING;		// Mark sound as not playing
		}
	}
}
void digi_resume_all() {
}
void digi_stop_all() {
	play_stop();
	int i;
	for (i=0; i<MAX_SOUND_OBJECTS; i++ )	{
		if ( SoundObjects[i].flags & SOF_USED )	{
			if ( SoundObjects[i].flags & SOF_PLAYING )	{
				stop_sample( SoundObjects[i].handle );
			}
			SoundObjects[i].flags = 0;	// Mark as dead, so some other sound can use this sound
		}
	}
}

int digi_new_sound( int org_soundnum, fix max_volume, fix max_distance, int volume, int pan )
{
	int i;
	if ( max_volume < 0 ) return -1;
	//if (!Digi_initialized) return -1;
        int soundnum = Sounds[org_soundnum];
	if (soundnum == 255) return -1;
	if (GameSounds[soundnum].data==NULL) {
		//Int3();
		return -1;
	}

	//if (digi_driver_board<1) return -1;

	for (i=0; i<MAX_SOUND_OBJECTS; i++ )
		if (SoundObjects[i].flags==0)
			break;
	
	if (i==MAX_SOUND_OBJECTS) {
		mprintf((1, "Too many sound objects!\n" ));
		return -1;
	}

	SoundObjects[i].signature=next_signature++;
	SoundObjects[i].flags = SOF_USED;
	SoundObjects[i].flags |= SOF_PLAY_FOREVER;
	SoundObjects[i].max_volume = max_volume;
	SoundObjects[i].max_distance = max_distance;
	SoundObjects[i].volume = volume;
	SoundObjects[i].pan = pan;
	SoundObjects[i].soundnum = soundnum;

	return i;
}

static void digi_start_sound_object(int i)
{
	if (SoundObjects[i].volume < 1)
		return;
	SoundObjects[i].handle = play_sample(
		SoundObjects[i].soundnum,
		SoundObjects[i].pan,
		fixmuldiv(SoundObjects[i].volume,digi_volume,F1_0),
		1);
	SoundObjects[i].flags |= SOF_PLAYING;
	mprintf((0, "started sound object %d handle %d\n", i, SoundObjects[i].handle));
}

void digi_get_sound_loc( vms_matrix * listener, vms_vector * listener_pos, int listener_seg, vms_vector * sound_pos, int sound_seg, fix max_volume, int *volume, int *pan, fix max_distance );

void digi_sync_sounds() {
	int i;
	for (i=0; i<MAX_SOUND_OBJECTS; i++ )	{
		if ( SoundObjects[i].flags & SOF_USED )	{
			int oldvolume = SoundObjects[i].volume;
			int oldpan = SoundObjects[i].pan;

			if ( SoundObjects[i].flags & SOF_LINK_TO_POS )	{
				digi_get_sound_loc( &Viewer->orient, &Viewer->pos, Viewer->segnum, 
                                &SoundObjects[i].position, SoundObjects[i].segnum, SoundObjects[i].max_volume,
                                &SoundObjects[i].volume, &SoundObjects[i].pan, SoundObjects[i].max_distance );

			} else if ( SoundObjects[i].flags & SOF_LINK_TO_OBJ )	{
				object * objp;
	
				objp = &Objects[SoundObjects[i].objnum];
		
				if ((objp->type==OBJ_NONE) || (objp->signature!=SoundObjects[i].objsignature))	{
					// The object that this is linked to is dead, so just end this sound if it is looping.
					if ( (SoundObjects[i].flags & SOF_PLAYING)  && (SoundObjects[i].flags & SOF_PLAY_FOREVER))	{
						stop_sample( SoundObjects[i].handle );
						mprintf(( 0, "stopped sound object %d (object dead)\n", i));
					}
					SoundObjects[i].flags = 0;	// Mark as dead, so some other sound can use this sound
					continue;		// Go on to next sound...
				} else {
					digi_get_sound_loc( &Viewer->orient, &Viewer->pos, Viewer->segnum, 
						&objp->pos, objp->segnum, SoundObjects[i].max_volume,
						&SoundObjects[i].volume, &SoundObjects[i].pan, SoundObjects[i].max_distance );
				}
			}
			 
			if (oldvolume != SoundObjects[i].volume || oldpan != SoundObjects[i].pan) 	{
				if ( SoundObjects[i].volume < 1 )	{
					// Sound is too far away, so stop it from playing.
					if ((SoundObjects[i].flags & SOF_PLAYING)&&(SoundObjects[i].flags & SOF_PLAY_FOREVER))	{
						stop_sample( SoundObjects[i].handle );
						SoundObjects[i].flags &= ~SOF_PLAYING;		// Mark sound as not playing
						mprintf(( 0, "stopped sound object %d (low volume)\n", i));
					}
				} else {
					if (!(SoundObjects[i].flags & SOF_PLAYING))	{
						digi_start_sound_object(i);
					} else {
						update_sample( SoundObjects[i].handle,
							SoundObjects[i].pan,
							fixmuldiv(SoundObjects[i].volume,digi_volume,F1_0) );
					}
				}
			}
		}
	}
}

int digi_link_sound_to_object2( int soundnum, short objnum, int forever, fix max_volume, fix  max_distance ) {
	int i, volume, pan;
	digi_get_sound_loc( &Viewer->orient, &Viewer->pos, Viewer->segnum, &Objects[objnum].pos, Objects[objnum].segnum, max_volume,&volume, &pan, max_distance );
	if (!forever) {
		digi_play_sample_3d( soundnum, pan, volume, 0 );
		return -1;
	}
	if ((i = digi_new_sound(soundnum, max_volume, max_distance, volume, pan)) == -1)
		return -1;
	SoundObjects[i].flags |= SOF_LINK_TO_OBJ;
	SoundObjects[i].objnum = objnum;
	SoundObjects[i].objsignature = Objects[objnum].signature;
	digi_start_sound_object(i);
	return SoundObjects[i].signature;
}
int digi_link_sound_to_pos2( int soundnum, short segnum, short sidenum, vms_vector * pos, int forever, fix max_volume, fix max_distance ) {
	int i, volume, pan;
	digi_get_sound_loc( &Viewer->orient, &Viewer->pos, Viewer->segnum, pos, segnum, max_volume, &volume, &pan, max_distance );
	if (!forever) {
		digi_play_sample_3d( soundnum, pan, volume, 0 );
		return -1;
	}
	i = digi_new_sound(soundnum, max_volume, max_distance, volume, pan);
	mprintf((0, "link_sound_to_pos2 %d idx %d\n", soundnum, i));
	if (i == -1)
		return -1;
	SoundObjects[i].flags |= SOF_LINK_TO_POS;
	SoundObjects[i].segnum = segnum;
	SoundObjects[i].sidenum = sidenum;
	SoundObjects[i].position = *pos;
	digi_start_sound_object(i);
	return SoundObjects[i].signature;
}
void digi_kill_sound_linked_to_object(int objnum) {
	int i, killed = 0;
	for (i=0; i<MAX_SOUND_OBJECTS; i++ )	{
		if ( (SoundObjects[i].flags & SOF_USED) && (SoundObjects[i].flags & SOF_LINK_TO_OBJ ) )	{
			if (SoundObjects[i].objnum == objnum)	{
				if ( SoundObjects[i].flags & SOF_PLAYING )	{
					stop_sample( SoundObjects[i].handle );
					mprintf((0, "stop sound object %d, manual kill\n", i));
				}
				SoundObjects[i].flags = 0;	// Mark as dead, so some other sound can use this sound
				killed++;
			}
		}
	}
}
void digi_play_sample_3d(int sndnum, int angle, int volume, int no_dups ) {
	if (volume < 10)
		return;
        int gsndnum = Sounds[sndnum];
	if (gsndnum == 255)
		return;
	play_sample(gsndnum, angle, fixmuldiv(volume, digi_volume, F1_0), 0);
}
void digi_kill_sound_linked_to_segment(int segnum, int sidenum, int soundnum) {
	int i, killed = 0;
	for (i=0; i<MAX_SOUND_OBJECTS; i++ )	{
		if ( (SoundObjects[i].flags & SOF_USED) && (SoundObjects[i].flags & SOF_LINK_TO_POS) )	{
			if ((SoundObjects[i].segnum == segnum) && (SoundObjects[i].soundnum==soundnum ) && (SoundObjects[i].sidenum==sidenum) )	{
				if ( SoundObjects[i].flags & SOF_PLAYING )	{
					stop_sample( SoundObjects[i].handle );
				}
				SoundObjects[i].flags = 0;	// Mark as dead, so some other sound can use this sound
				killed++;
			}
		}
	}
}
