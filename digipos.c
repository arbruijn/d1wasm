/*
THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF PARALLAX
SOFTWARE CORPORATION ("PARALLAX").  PARALLAX, IN DISTRIBUTING THE CODE TO
END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.  
COPYRIGHT 1993-1998 PARALLAX SOFTWARE CORPORATION.  ALL RIGHTS RESERVED.
*/
/*
 * $Source: f:/miner/source/main/rcs/digi.c $
 * $Revision: 2.5 $
 * $Author: john $
 * $Date: 1996/01/05 16:51:51 $
 * 
 * Routines to access digital sound hardware
 * 
 * $Log: digi.c $
 * Revision 2.5  1996/01/05  16:51:51  john
 * Made the midi handler lock down to +4K.
 * 
 * Revision 2.4  1996/01/05  16:46:54  john
 * Made code lock down midicallback +4K.
 * 
 * Revision 2.3  1995/05/26  16:16:03  john
 * Split SATURN into define's for requiring cd, using cd, etc.
 * Also started adding all the Rockwell stuff.
 * 
 * Revision 2.2  1995/03/30  16:36:04  mike
 * text localization.
 * 
 * Revision 2.1  1995/03/15  11:41:23  john
 * Better Saturn CD-ROM support.
 * 
 * Revision 2.0  1995/02/27  11:29:15  john
 * New version 2.0, which has no anonymous unions, builds with
 * Watcom 10.0, and doesn't require parsing BITMAPS.TBL.
 * 
 * Revision 1.172  1995/02/15  09:57:53  john
 * Fixed bug with loading new banks while song is playing.
 * 
 * Revision 1.171  1995/02/14  16:36:26  john
 * Fixed bug with countdown voice getting cut off.
 * 
 * Revision 1.170  1995/02/13  20:34:51  john
 * Lintized
 * 
 * Revision 1.169  1995/02/13  15:18:21  john
 * Added code to reset sounds.
 * 
 * Revision 1.168  1995/02/13  12:53:11  john
 * Fixed bug with demos recording wrong volumes.
 * 
 * Revision 1.167  1995/02/11  15:04:29  john
 * Made FM files load as if you have FM card.
 * 
 * Revision 1.166  1995/02/11  12:41:35  john
 * Added new song method, with FM bank switching..
 * 
 * Revision 1.165  1995/02/10  16:24:58  john
 * MAde previous change only reverse for RAP10.
 * 
 * Revision 1.164  1995/02/10  13:47:37  john
 * Made digi init before midi to maybe fix RAP10 bug.
 * 
 * Revision 1.163  1995/02/08  21:05:22  john
 * Added code that loads patches for every FM song.
 * 
 * Revision 1.162  1995/02/03  17:17:51  john
 * Made digi lowmem default to off.
 * 
 * Revision 1.161  1995/02/03  17:08:22  john
 * Changed sound stuff to allow low memory usage.
 * Also, changed so that Sounds isn't an array of digi_sounds, it
 * is a ubyte pointing into GameSounds, this way the digi.c code that
 * locks sounds won't accidentally unlock a sound that is already playing, but
 * since it's Sounds[soundno] is different, it would erroneously be unlocked.
 * 
 * Revision 1.160  1995/02/01  22:37:06  john
 * Reduced sound travel distance to 1.25 times original.
 * 
 * Revision 1.159  1995/02/01  22:20:41  john
 * Added digi_is_sound_playing.
 * 
 * Revision 1.158  1995/01/28  15:56:56  john
 * Made sounds carry 1.5 times farther.
 * 
 * Revision 1.157  1995/01/27  17:17:09  john
 * Made max sounds relate better to MAX_SOUND_OBJECTS
 * 
 * Revision 1.156  1995/01/25  12:18:06  john
 * Fixed bug with not closing MIDI files when midi volume is 0.
 * 
 * Revision 1.155  1995/01/24  17:52:17  john
 * MAde midi music stop playing when volume is 0.
 * 
 * Revision 1.154  1995/01/21  21:22:31  mike
 * Correct bogus error message.
 * 
 * Revision 1.153  1995/01/17  14:53:38  john
 * IFDEF'D out digital drums.
 * 
 * Revision 1.152  1995/01/11  16:26:50  john
 * Restored MIDI pausing to actually setting volume to 0,.
 * 
 * Revision 1.151  1995/01/10  16:38:46  john
 * Made MIDI songs pause, not lower volume when pausing, and
 * had to fix an HMI bug by clearing the TRACKINFO array or something.
 * 
 * Revision 1.150  1995/01/05  19:46:27  john
 * Added code to reset the midi tracks to use the ones 
 * in the midi file before each song is played.
 * 
 * Revision 1.149  1994/12/21  15:08:59  matt
 * Bumped MAX_SOUND_OBJECTS back up to match v1.00 of shareware
 * 
 * Revision 1.148  1994/12/20  18:22:54  john
 * Added code to support non-looping songs, and put
 * it in for endlevel and credits.
 * 
 * Revision 1.147  1994/12/19  17:58:19  john
 * Changed Assert for too many sounds to gracefully exit.
 * 
 * Revision 1.146  1994/12/14  16:03:27  john
 * Made the digi/midi deinit in reverse order for anything
 * other than GUS.
 * 
 * Revision 1.145  1994/12/14  14:51:06  john
 * Added assert.
 * 
 * Revision 1.144  1994/12/14  12:14:40  john
 * Relplaced a bunch of (a*b)/c with fixmuldiv
 * to get rid of a overflow bug mike found.
 * 
 * Revision 1.143  1994/12/13  17:30:16  john
 * Made the timer rate be changed right after initializing it.
 * 
 * Revision 1.142  1994/12/13  14:08:35  john
 * Made Pause key set midi volume to 0.
 * Made digi_init set midi volume to proper level.
 * 
 * Revision 1.141  1994/12/13  12:42:31  jasen
 * Fixed sound priority bugs... -john
 * 
 * Revision 1.140  1994/12/13  12:11:38  john
 * Added debugging code.
 * 
 * Revision 1.139  1994/12/13  11:45:19  john
 * Disabled interrupts around the midisetvolume because awe32
 * hangs if you don't.
 * 
 * Revision 1.138  1994/12/13  11:33:45  john
 * MAde so that sounds with volumes > f1_0 don't cut off.
 * 
 * Revision 1.137  1994/12/13  02:24:29  matt
 * digi_init() now doesn't return error when no sound card
 * 
 * Revision 1.136  1994/12/13  00:46:27  john
 * Split digi and midi volume into 2 seperate functions.
 * 
 * Revision 1.135  1994/12/12  22:19:20  john
 * Made general midi versions of files load...
 * .hmq instead of .hmp.
 * 
 * Revision 1.134  1994/12/12  21:32:49  john
 * Made volume work better by making sure volumes are valid
 * and set correctly at program startup.
 * 
 * Revision 1.133  1994/12/12  20:52:35  john
 * Fixed bug with pause calling set mastervolume to 0.
 * 
 * Revision 1.132  1994/12/12  20:39:52  john
 * Changed so that instead of using MasterVolume for 
 * digital sounds, I just scale the volume I play the
 * sound by.
 * 
 * Revision 1.131  1994/12/12  13:58:21  john
 * MAde -nomusic work.
 * Fixed GUS hang at exit by deinitializing digi before midi.
 * 
 * Revision 1.130  1994/12/11  23:29:39  john
 * *** empty log message ***
 * 
 * Revision 1.129  1994/12/11  23:18:07  john
 * Added -nomusic.
 * Added RealFrameTime.
 * Put in a pause when sound initialization error.
 * Made controlcen countdown and framerate use RealFrameTime.
 * 
 * Revision 1.128  1994/12/11  13:35:26  john
 * Let sounds play louder than F1_0.  btw, 4*f1_0 is maxed.
 * 
 * Revision 1.127  1994/12/11  00:43:45  john
 * Fixed bug with sounds taking over channels that sound objects own.
 * 
 * Revision 1.126  1994/12/10  20:35:03  john
 * Added digi_kill_sound_linked_to_object.
 * 
 * Revision 1.125  1994/12/10  15:44:20  john
 * Added max_distance passing for sound objects.
 * 
 * Revision 1.124  1994/12/09  20:16:37  john
 * Made it so that when Mike's AI code passes bogus values, the
 * digi code will saturate them.
 * 
 * Revision 1.123  1994/12/09  17:07:52  john
 * Fixed abrupt fan cutoff.
 * 
 * Revision 1.122  1994/12/08  17:19:14  yuan
 * Cfiling stuff.
 * 
 * Revision 1.121  1994/12/06  19:24:08  john
 * CLosed HMI timer after making our not call it.
 * 
 * Revision 1.120  1994/12/06  19:00:42  john
 * Moved digi_reset_sounds to reset instead of digi_close.
 * 
 * Revision 1.119  1994/12/06  18:23:48  matt
 * Don't pause midi songs, becuase it can lock up
 * 
 * Revision 1.118  1994/12/06  16:07:09  john
 * MAde the gus pats only load if using midi==midi_gus.
 * 
 * Revision 1.117  1994/12/06  10:17:07  john
 * MAde digi_close call reset_sounds.
 * 
 * Revision 1.116  1994/12/05  23:36:50  john
 * Took out lock down of GETDS, because, I, john, was wrong, and it
 * was me, not the HMI people, that was calling GETDS in an interrupt.
 * I un-formally apologize to HMI.
 * 
 * Revision 1.115  1994/12/05  22:50:09  john
 * Put in code to lock down GETDS, because the HMI
 * people don't lock it down, even tho they use it 
 * in their timerhandler interrupt.
 * 
 * Revision 1.114  1994/12/05  18:54:09  john
 * *** empty log message ***
 * 
 * Revision 1.113  1994/12/05  18:52:11  john
 * Locked down the TrackMap array, since SOS references it in an interupt.
 * Added -noloadpats command line option.
 * 
 * Revision 1.112  1994/12/05  16:55:45  john
 * Made so that digi_init_System is  called before 
 * the timer system calls the timer handler.
 * 
 * Revision 1.111  1994/12/05  12:56:19  john
 * Made the intruments patches only load for FM devices.
 * 
 * Revision 1.110  1994/12/05  12:17:16  john
 * Added code that locks/unlocks digital sounds on demand.
 * 
 * Revision 1.109  1994/12/04  14:30:10  john
 * Added hooks for music..
 * 
 * Revision 1.108  1994/12/02  13:58:27  matt
 * Put in Int3()s when try to play a non-existant sound
 * 
 * Revision 1.107  1994/12/02  10:35:58  john
 * Took out loadpats.c
 * 
 * Revision 1.106  1994/12/01  02:22:33  john
 * Incorporated LOADPATS.EXE into our code.
 * 
 * Revision 1.105  1994/12/01  00:59:51  john
 * Fixed some pot. bugs with closing.
 * 
 * Revision 1.104  1994/11/30  23:54:40  rob
 * Tweaked some volume params due to a fix in find_connected_distance.
 * 
 * Revision 1.103  1994/11/30  19:36:44  john
 * Made Gravis Ultrasound work again.  Made the scores blink
 * at a constant rate.  Revamped the newmenu background storage,
 * which hopefully fixed some bugs.  Made menus in ame not pause
 * sound, except for the pause key.               ^== Game!
 * 
 * Revision 1.102  1994/11/30  15:14:25  rob
 * Removed unused include file..
 * 
 * Revision 1.101  1994/11/30  15:08:45  john
 * Changed some open/close stuff with sound.
 * 
 * Revision 1.100  1994/11/29  13:35:41  john
 * Test code.
 * 
 * Revision 1.99  1994/11/29  03:46:32  john
 * Added joystick sensitivity; Added sound channels to detail menu.  Removed -maxchannels
 * command line arg.
 * 
 * Revision 1.98  1994/11/28  18:37:59  john
 * Made sample play once work.
 * 
 * Revision 1.97  1994/11/28  18:35:19  john
 * Made the digi_max_channels cut of an old sound instead of
 * not playing a new sound.
 * 
 * Revision 1.96  1994/11/28  01:32:02  mike
 * use quick form of matrix function.
 * 
 * Revision 1.95  1994/11/27  23:12:14  matt
 * Made changes for new mprintf calling convention
 * 
 * Revision 1.94  1994/11/22  17:13:31  john
 * Fixed bug with the digital drums hanging.
 * 
 * Revision 1.93  1994/11/21  19:09:21  john
 * Made so if digi and midi both 0, same as -nosound.
 * 
 * Revision 1.92  1994/11/21  16:46:46  john
 * Limited digital channels to 10.
 * 
 * Revision 1.91  1994/11/21  16:28:42  john
 * Fixed bug with digi_close hanging.
 * 
 * Revision 1.90  1994/11/21  15:40:28  john
 * Locked down the instrument data.
 * 
 * Revision 1.89  1994/11/21  14:43:55  john
 * Fixed some bugs with setting volumes even when -nosound was used. Duh!
 * 
 * Revision 1.88  1994/11/21  11:55:26  john
 * Fixed some sound pausing in menus bugs.
 * 
 * Revision 1.87  1994/11/21  11:02:21  john
 * Made fan sounds be 1/2 the volume so that they
 * don'
 * don't carry as far.
 * 
 * Revision 1.86  1994/11/20  17:47:51  john
 * Fixed a potential bug with sound initializing.
 * 
 * Revision 1.85  1994/11/20  17:29:07  john
 * Fixed bug with page fault during digi_close.
 * 
 * Revision 1.84  1994/11/19  15:19:24  mike
 * rip out unused code and data.
 * 
 * Revision 1.83  1994/11/16  23:38:33  mike
 * new improved boss teleportation behavior.
 * 
 * Revision 1.82  1994/11/14  18:12:46  john
 * Took out some sound objects stuff.
 * 
 */


//#pragma off (unreferenced)
//static char rcsid[] = "$Id: digi.c 2.5 1996/01/05 16:51:51 john Exp $";
//#pragma on (unreferenced)

#include "fix.h"
#include "object.h"
#include "mono.h"
#include "timer.h"
#include "joy.h"
#include "digi.h"
#include "sounds.h"
#include "args.h"
#include "key.h"
#include "newdemo.h"
#include "game.h"
#include "dpmi.h"
#include "error.h"
#include "wall.h"
#include "cfile.h"
#include "piggy.h"
#include "text.h"
#include "kconfig.h"

void digi_get_sound_loc( vms_matrix * listener, vms_vector * listener_pos, int listener_seg, vms_vector * sound_pos, int sound_seg, fix max_volume, int *volume, int *pan, fix max_distance )	
{	  
	vms_vector	vector_to_sound;
	fix angle_from_ear, cosang,sinang;
	fix distance;
	fix path_distance;

	*volume = 0;
	*pan = 0;

	max_distance = (max_distance*5)/4;		// Make all sounds travel 1.25 times as far.

	//	Warning: Made the vm_vec_normalized_dir be vm_vec_normalized_dir_quick and got illegal values to acos in the fang computation.
	distance = vm_vec_normalized_dir_quick( &vector_to_sound, sound_pos, listener_pos );
		
	if (distance < max_distance )	{
		int num_search_segs = f2i(max_distance/20);
		if ( num_search_segs < 1 ) num_search_segs = 1;

		path_distance = find_connected_distance(listener_pos, listener_seg, sound_pos, sound_seg, num_search_segs, WID_RENDPAST_FLAG );
		if ( path_distance > -1 )	{
			*volume = max_volume - fixdiv(path_distance,max_distance);
			//mprintf( (0, "Sound path distance %.2f, volume is %d / %d\n", f2fl(distance), *volume, max_volume ));
			if (*volume > 0 )	{
				angle_from_ear = vm_vec_delta_ang_norm(&listener->rvec,&vector_to_sound,&listener->uvec);
				fix_sincos(angle_from_ear,&sinang,&cosang);
				//mprintf( (0, "volume is %.2f\n", f2fl(*volume) ));
				if (Config_channels_reversed) cosang *= -1;
				*pan = (cosang + F1_0)/2;
			} else {
				*volume = 0;
			}
		}
	}																					  
}
