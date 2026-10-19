# Native 32-bit Linux build of Duke Nukem 3D.
#   make                 -> ../build/linux/duke3d (gcc, SDL2 window)
#   make CC=clang        -> same with clang
#   make PLATFORM=null   -> headless build (no SDL needed)
ifeq ($(origin CC),default)
CC := gcc
endif
BUILD   ?= ../build/linux
TARGET  := $(BUILD)/duke3d

.DEFAULT_GOAL := all

ARCH     := -m32
WARN     := -w
CFLAGS   ?= -O1 -g
CFLAGS   += -MMD -MP $(ARCH) $(WARN) -fno-strict-aliasing -fwrapv -funsigned-char \
            -Igame -Iengine -Icompat -Imact -Iaudio -Iplatform
LDFLAGS  += $(ARCH)
LDLIBS   += -lm -lpthread

GAME_SRC   := game.c actors.c gamedef.c global.c menues.c player.c premap.c \
              sector.c sounds.c rts.c config.c animlib.c
ENGINE_SRC := engine.c cache1d.c mmulti.c a.c
COMPAT_SRC := pragmas.c dos.c
MACT_SRC   := util_lib.c file_lib.c keyboard.c mouse.c control.c scriplib.c task_man.c
AUDIO_SRC  := audio_stub.c
PLATFORM   ?= sdl
PLAT_SRC   := platform_$(PLATFORM).c

SRCS := $(addprefix game/,$(GAME_SRC)) $(addprefix engine/,$(ENGINE_SRC)) \
        $(addprefix compat/,$(COMPAT_SRC)) $(addprefix mact/,$(MACT_SRC)) \
        $(addprefix audio/,$(AUDIO_SRC)) $(addprefix platform/,$(PLAT_SRC))
OBJS := $(patsubst %.c,$(BUILD)/obj/%.o,$(SRCS))

# 32-bit SDL2: use libsdl2-dev:i386 via pkg-config if installed. Otherwise use
# the (architecture-independent) SDL headers with a shim for Debian's per-arch
# config header, and link the 32-bit runtime library directly.
ifeq ($(PLATFORM),sdl)
SDL_PC := /usr/lib/i386-linux-gnu/pkgconfig/sdl2.pc
ifneq ($(wildcard $(SDL_PC)),)
SDL_CFLAGS := $(shell PKG_CONFIG_PATH=/usr/lib/i386-linux-gnu/pkgconfig pkg-config --cflags sdl2)
SDL_LIBS   := $(shell PKG_CONFIG_PATH=/usr/lib/i386-linux-gnu/pkgconfig pkg-config --libs sdl2)
else
SDL_SHIM   := $(BUILD)/sdlshim
SDL_CFLAGS := -I/usr/include/SDL2 -I$(SDL_SHIM) -D_REENTRANT
SDL_LIBS   := /usr/lib/i386-linux-gnu/libSDL2-2.0.so.0
$(BUILD)/obj/platform/platform_sdl.o: $(SDL_SHIM)/SDL2/_real_SDL_config.h
$(SDL_SHIM)/SDL2/_real_SDL_config.h:
	@mkdir -p $(dir $@)
	ln -sf /usr/include/x86_64-linux-gnu/SDL2/_real_SDL_config.h $@
endif
CFLAGS += $(SDL_CFLAGS)
LDLIBS += $(SDL_LIBS)
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BUILD)/obj/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

objs: $(OBJS)

-include $(OBJS:.o=.d)

clean:
	rm -rf $(BUILD)

.PHONY: all objs clean
