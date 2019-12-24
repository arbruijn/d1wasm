CFLAGS += -I main -I includes -I arch -I cfile -g -Wall -I bios -I 2d -I 3d -I fix -I vecmat -I misc
CFLAGS += -D _far= -D far= -funsigned-char
CFLAGS += -I mem -I iff -I pslib -I texmap
CFLAGS += -DSHAREWARE
CFLAGS += -DNASM
#CFLAGS += -DMONO
CFLAGS += -DRELEASE
CFLAGS += -Werror
#CFLAGS += -Og -g
#CFLAGS += -O2
#CFLAGS += -posix
#CFLAGS += -Wno-error=stringop-truncation -fwrapv -fno-strict-aliasing
#CFLAGS += -fmax-errors=5
CFLAGS += -Wno-self-assign -Wno-parentheses-equality -Wno-logical-not-parentheses
#CFLAGS += -fpack-struct=1

SRC = mycfile.c 2d/pcx.c 2d/gr.c 2d/canvas.c 2d/palette.c 2d/bitmap.c 2d/bitblt.c 2d/pixel.c 2d/gpixel.c 2d/rect.c \
	2d/line.c \
	2d/scanline.c 2d/rle.c misc/error.c main/titles.c main/text.c 2d/font.c main/menu.c \
	main/config.c main/newmenu.c main/kconfig.c main/gamefont.c main/gameseq.c main/bm.c main/piggy.c \
	main/hash.c main/cntrlcen.c main/fuelcen.c main/gauges.c main/object.c main/ai.c main/wall.c \
	main/gameseg.c main/laser.c main/physics.c main/collide.c main/fireball.c main/powerup.c \
	main/weapon.c main/game.c main/slew.c main/aipath.c main/lighting.c main/render.c \
	main/automap.c vecmat/vecmat.c main/fvi.c main/hostage.c main/newdemo.c main/switch.c \
	main/endlevel.c iff/iff.c main/terrain.c main/gamemine.c main/hud.c main/mglobal.c \
	texmap/ntmap.c main/vclip.c main/effects.c main/robot.c main/texmerge.c main/polyobj.c \
	main/inferno.c main/args.c main/scores.c main/credits.c 2d/disc.c 2d/scale.c \
	main/mission.c main/songs.c main/playsave.c main/gamesave.c \
	main/morph.c main/paging.c fix/fix.c 2d/ibitblt.c texmap/tmapflat.c fix/tables.c \
	main/controls.c texmap/scanline.c main/bmread.c main/joydefs.c \
	bios/key.c unadpcm.c digipos.c stubs.c psrand.c mydigi.c bfile.c

SRC += 3d/clipper.c 3d/draw.c 3d/globvars.c 3d/horizon.c 3d/instance.c \
	3d/interp.c 3d/matrix.c 3d/points.c 3d/rod.c 3d/setup.c

HMPSRC = hmpweb/playerweb.c

VCSRC = veryclassic.c $(SRC) wintime.c hmpfile.c hmpmidi.c bios/ukey.c
EMSRC = wasmmain.c domkey.c $(SRC) $(HMPSRC)

OBJ = $(patsubst %.c,%.o,$(VCSRC))
EMOBJ = $(patsubst %.c,%.em.o,$(EMSRC))

all: d1wasm.html

veryclassic.dll: $(OBJ) descent.hog.bin.o descent.pig.bin.o
	$(CC) -shared -o $@ $^ -lwinmm

test.exe: test.o veryclassic.dll
	$(CC) -g -o $@ $^ veryclassic.dll

%.em.o: %.c
	emcc $(CFLAGS) -O3 -c -o $@ $^ 

d1wasm.js: $(EMOBJ)
	emcc $(CFLAGS) -O3 -o $@ $^ -s NODERAWFS=1 -s ASYNCIFY -s ASYNCIFY_IGNORE_INDIRECT
	# --preload-file descent.hog --preload-file descent.pig

d1wasm.html: $(EMOBJ)
	emcc $(CFLAGS) -O3 -o $@ $^ -s ASYNCIFY -s ASYNCIFY_IGNORE_INDIRECT -s 'ASYNCIFY_IMPORTS=["play_ad","emscripten_sleep"]' \
	 -s ALLOW_MEMORY_GROWTH \
	 -s EXPORTED_FUNCTIONS='["_main","_get_screen","_on_key","_playerweb_gen","_playerweb_get_last_rc","_playerweb_get_volume","_playerweb_get_time","_get_sample_size","_get_sample_data","_on_mousemove", "_on_mousebutton"]' \
	 --js-library library.js \
	 -s ERROR_ON_UNDEFINED_SYMBOLS=0 \
	 --preload-file descent.hog --preload-file descent.pig

clean:
	rm -f *.o $(OBJ) test test.exe veryclassic.dll *~ d1wasm.js d1wasm.html */*.o

%.bin.o: %
	ld -r -b binary -o $@ $^
