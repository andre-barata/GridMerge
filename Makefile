# Makefile for building the executable with optional embedded resource binaries and static/standalone functionality (tested on windows: mingw64/MSYS2)
# License LGPL
# by Andre Barata 2020
#
# usage:
#   make [degub|release] [static=y] [console=y]
#
# Expected paths:
# |- src/
# |  |- *.c           -- c source files
# |  |- include/
# |     |- *.h        -- any local .h files would go here
# |- res/
# |  |- win/
# |  |  |- main.ico   -- application icon on windows
# |  |  |- winres.rc  -- when compiling on windows, resource file defining the main icon and executable metadata
# |  |- *.*           -- any binary files to pack in the executable go here
# |- obj/             -- temporary object and resource files generated and used during compilation
# |- Makefile         -- this file
# |- *.exe            -- ouput executable goes here

### Project Variables:
APPNAME = GridMerge
USE_FREETYPE = n
INCLUDES = -I/mingw64/include/SDL2 `pkg-config freetype2 --cflags`
LIBS = -LD:/Dev/msys64/mingw64/lib `sdl2-config --cflags --static-libs` -lz \
	-Wl,--whole-archive ./lib/libwinpthread.a -Wl,--no-whole-archive
#-lexpat -lz -lzip 
FREETYPE_LIBS = -lz -lbz2 -lpng16 -lz -lharfbuzz -lm -lusp10 -lgdi32 -lrpcrt4 -ldwrite \
	-lglib-2.0 -lintl -lws2_32 -lole32 -lwinmm -lshlwapi -pthread -lm -lpcre -lbrotlidec-static \
	-lbrotlicommon-static -lgraphite2 -lfreetype -lstdc++ 
OBJ_ARCH = -O pe-x86-64 -B i386:x86-64

CC = gcc
# Disable built-in rules and variables
MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += --no-builtin-variables

# static mode defaults to true in release mode
ifeq ($(MAKECMDGOALS),release)
	static=y
endif
# console application mode defaults to true in debug mode
ifeq ($(MAKECMDGOALS),debug)
	console=y
endif

# static and console flags
ifeq ($(static),y)
	CX_START = -O2
	CX_END = 
	LD_START = -Wl,--gc-sections -Wl,-Bstatic 
	LD_END = -Wl,-Bdynamic 
endif
ifeq ($(console),y)
	LD_END += -Wl,-subsystem,console 
endif

EXT = .c
SRCDIR = src
OBJDIR = obj
RESDIR = res

CXFLAGS = $(CX_START) -std=c11 -Wno-unknown-pragmas $(INCLUDES) $(CX_END)
LDFLAGS = $(LD_START) $(LIBS) $(LD_END)
SRC = $(wildcard $(SRCDIR)/*$(EXT)) 

# freetype
ifeq ($(USE_FREETYPE),y)
	LIBS += $(FREETYPE_LIBS)
	CXFLAGS += -DUSE_FREETYPE 
#-msse -msse2 -msse3 -mfpmath=sse
	SRC += $(wildcard $(SRCDIR)/include/thirdparty/*$(EXT))
endif
# minizip 3
CXFLAGS += -DHAVE_WZAES
CXFLAGS += -DHAVE_ZLIB
SRC += $(wildcard $(SRCDIR)/include/thirdparty/minizip3/*$(EXT))

OBJ = $(SRC:$(SRCDIR)/%$(EXT)=$(OBJDIR)/%.o)
RESBIN = $(wildcard res/*.*)
RES = $(RESBIN:$(RESDIR)/%=$(OBJDIR)/%.o)
RM = rm -f 

ifeq ($(OS),Windows_NT)
	WINRC = $(wildcard $(RESDIR)/win/*.rc)
	WINRES = $(WINRC:$(RESDIR)/win/%.rc=$(OBJDIR)/%.res)
endif

all: release
debug: CXFLAGS += -DDEBUG -g
debug: clean_nores $(APPNAME)
release: CXFLAGS += -s
release: clean_nores $(APPNAME)

# Builds the app
$(APPNAME): $(RES) $(OBJ) $(WINRES) 
	$(CC) $(CXFLAGS) -o $(APPNAME) $^ $(LDFLAGS)
ifeq ($(MAKECMDGOALS),release)
#       pack the executable
		@upx --no-progress $(APPNAME).exe>/dev/null
endif

# Building rule for .o files and its .c/.cpp in combination with all .h
$(OBJDIR)/%.o: $(SRCDIR)/%$(EXT)
	$(CC) $(CXFLAGS) -o $@ -c $<

# make file data arrays source
$(OBJDIR)/%.o: $(RESDIR)/%
	objcopy -I binary $(OBJ_ARCH) $< $@

# compile windows resources
$(OBJDIR)/%.res: $(RESDIR)/win/%.rc
	windres $< -O coff -o $@

# Cleans complete project
.PHONY: clean
clean:
	$(RM) $(APPNAME) $(RES) $(WINRES)
	find $(OBJDIR)/ -name "*.o" -type f -delete

.PHONY: clean_nores
clean_nores:
	$(RM) $(APPNAME) $(OBJDIR)/main.o
#	$(RM) $(OBJ) $(APPNAME)

