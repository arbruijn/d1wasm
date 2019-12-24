#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <process.h>
#include <fcntl.h>
#include "hmpfile.h"

static struct hmp_file hmp_files[2];
static volatile int hmp_file_playing = -1, hmp_file_ready = -1;
static volatile int hmp_volume = 128;

static void midi_thread(void *arg) {
	HMIDIOUT midi_handle = NULL;
	unsigned char chanvol[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	if (midiOutOpen(&midi_handle, 0, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
		hmp_file_playing = -1;
		return;
	}
	for (;;) {
		while (hmp_file_ready == -1)
			Sleep(50);
		if (hmp_file_ready == -2)
			break;
		//printf("midi thread: playing %d\n", hmp_file_ready);
		hmp_file_playing = hmp_file_ready;
		hmp_file *hf = &hmp_files[hmp_file_playing];
		int last_volume = hmp_volume;
		DWORD time = timeGetTime();
		while (hmp_file_playing == hmp_file_ready) {
			hmp_event ev;
			int rc;
			if ((rc = hmp_get_event(hf, &ev))) {
				hmp_file_ready = -1;
				break;
			}
			if (ev.delta) {
				DWORD now = timeGetTime();
				time += 1000 * ev.delta / hf->tempo;
				if (now < time) {
					Sleep(time - now);
					if (hmp_file_playing != hmp_file_ready)
						break;
				}
			}
			if ((ev.msg[0] & 0xf0) == 0xb0 && ev.msg[1] == 7) { // adjust volume
				chanvol[ev.msg[0] & 0xf] = ev.msg[2];
				int vol = ev.msg[2];
				vol = (hmp_volume * vol) >> 7;
				ev.msg[2] = vol;
			}
			if (hmp_volume != last_volume) {
				last_volume = hmp_volume;
				for (int i = 0; i < 16; i++)
					if (chanvol[i]) {
						int vol = (last_volume * chanvol[i]) >> 7;
						midiOutShortMsg(midi_handle, 0x7b0 + i + (vol << 16));
					}
			}
						
			if (midiOutShortMsg(midi_handle, ev.msg[0] | (ev.msg[1] << 8) | (ev.msg[2] << 16)))
				break;
		}
		if (midiOutReset(midi_handle))
			break;
	}
	hmp_file_playing = -1;
	midiOutClose(midi_handle);
}

void hmp_song_stop() {
	hmp_file_ready = -1;
}

void hmp_song_done() {
	hmp_file_ready = -2;
}

int hmp_song_play(char *data, int size, int loop) {
	if (hmp_file_playing == -1) {
		hmp_file_ready = -1;
		hmp_file_playing = 0;
		__faststorefence();
		//printf("hmp_song_play: starting thread\n");
		_beginthread(midi_thread, 0, NULL);
	}
	if (hmp_file_ready != -1)
		while (hmp_file_playing != hmp_file_ready)
			Sleep(20);
	int next = 1 - hmp_file_playing;
	if (hmp_init(&hmp_files[next], data, size, 0xa001, loop))
		return 1;
	hmp_file_ready = next;
	//printf("hmp_song_play: hmp_file_ready=%d hmp_file_playing=%d\n", hmp_file_ready, hmp_file_playing);
	return 0;
}

void hmp_set_volume(int volume) {
	hmp_volume = volume < 0 ? 0 : volume > 128 ? 128 : volume;
}
