#
# $Source: Buggin:miner:source:3d::RCS:makefile $
# $Revision: 1.2 $
# $Author: allender $
# $Date: 1995/08/23 16:58:12 $
#
# Makefile for 3d library
#

LIBDIR = ::lib:
OBJDIR = :obj:
SRCDIR = :

# What libs to make
LIB = 3d.lib

# includes to copy to lib directory
GLOBAL_INCLUDES = globvars.h 3d.h

# test file libraries needed
TEST_LIBS = {LIBDIR}gr.lib {LIBDIR}mem.lib {LIBDIR}io.lib {LIBDIR}cfile.lib �
	{LIBDIR}misc.lib {LIBDIR}macsys.lib

# standard libs to link anything with
STD_LIBS = �
	"{CLibraries}"CSANELib.o �
	"{CLibraries}"Math.o �
	"{CLibraries}"StdClib.o �
	"{Libraries}"Runtime.o �
	"{Libraries}"Interface.o

# resource files to be used in test file
TEST_RESOURCES = {LIBDIR}bios.r {LIBDIR}macsys.r

OBJ_DIR = :obj:

# C compiler options
CFLAGS = {SYM_FLAG}
	
LDFLAGS = �
	{SYM_FLAG}
	
3D_OBJS = �
	{OBJDIR}clipper.c.o {OBJDIR}horizon.c.o {OBJDIR}matrix.c.o {OBJDIR}rod.c.o �
	{OBJDIR}draw.c.o {OBJDIR}instance.c.o {OBJDIR}points.c.o {OBJDIR}setup.c.o �
	{OBJDIR}globvars.c.o {OBJDIR}interp.c.o

All � {LIB}

{LIB} � {3D_OBJS}
	Echo "# `Date -t` ----- Building {LIB}..."
	{Linker} {LibraryFlags} {OBJ_DIR}� -o {LIB}
	Echo "# Copying files to lib directory..."
	duplicate -y {LIB} ::lib
	duplicate -y {GLOBAL_INCLUDES} ::lib
	
clean �
	del -i :obj:�
	del -i "{LIB}"

# dependency for obj and source directories
"{OBJDIR}"		�	"{SRCDIR}"

.c.o	�	.c
	Echo `Date -t` ---- Compiling {default}.c
	{C} "{depDir}{default}.c" {MyCFlags} {CFlags} �
		-o "{targDir}{default}.c.o"		�
		|| ( Echo "#####   ### ERROR ###   #####" )

.a.o	�	.a
	Echo "# `Date -t` ----- Assembling {default}.a..."
	Asm "{depDir}{default}.a"			�
		-o "{targDir}{default}.a.o"		�
		|| ( Echo "#####   ### ERROR ###   #####" )

#  Dependencies below this line
clipper.c.o � makefile clipper.c 3d.h ::lib:fix.h ::lib:dtypes.h ::lib:gr.h globvars.h
draw.c.o � makefile draw.c ::lib:error.h 3d.h ::lib:fix.h ::lib:dtypes.h ::lib:gr.h globvars.h
globvars.c.o � makefile globvars.c 3d.h ::lib:fix.h ::lib:dtypes.h ::lib:gr.h globvars.h
horizon.c.o � makefile horizon.c 3d.h ::lib:fix.h ::lib:dtypes.h ::lib:gr.h
instance.c.o � makefile instance.c 3d.h ::lib:fix.h ::lib:dtypes.h ::lib:gr.h
interp.c.o � makefile interp.c 3d.h ::lib:fix.h ::lib:dtypes.h ::lib:gr.h
matrix.c.o � makefile matrix.c 3d.h ::lib:fix.h ::lib:dtypes.h ::lib:gr.h globvars.h
rod.c.o � makefile rod.c 3d.h ::lib:fix.h ::lib:dtypes.h ::lib:gr.h
setup.c.o � makefile setup.c ::lib:error.h 3d.h ::lib:fix.h ::lib:dtypes.h ::lib:gr.h globvars.h
