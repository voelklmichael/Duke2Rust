# Native 32-bit Linux build of Duke Nukem 3D.
#   make            -> ../build/linux/duke3d (gcc)
#   make CC=clang   -> same with clang
ifeq ($(origin CC),default)
CC := gcc
endif
BUILD   ?= ../build/linux
TARGET  := $(BUILD)/duke3d

ARCH     := -m32
WARN     := -w
CFLAGS   ?= -O1 -g
CFLAGS   += $(ARCH) $(WARN) -fno-strict-aliasing -fwrapv -funsigned-char \
            -Igame -Iengine -Icompat -Imact -Iaudio -Iplatform
LDFLAGS  += $(ARCH)
LDLIBS   += -lm

GAME_SRC   := game.c actors.c gamedef.c global.c menues.c player.c premap.c \
              sector.c sounds.c rts.c config.c animlib.c
ENGINE_SRC := engine.c cache1d.c mmulti.c
COMPAT_SRC := pragmas.c dos.c
MACT_SRC   :=
AUDIO_SRC  :=
PLAT_SRC   :=

SRCS := $(addprefix game/,$(GAME_SRC)) $(addprefix engine/,$(ENGINE_SRC)) \
        $(addprefix compat/,$(COMPAT_SRC)) $(addprefix mact/,$(MACT_SRC)) \
        $(addprefix audio/,$(AUDIO_SRC)) $(addprefix platform/,$(PLAT_SRC))
OBJS := $(patsubst %.c,$(BUILD)/obj/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BUILD)/obj/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

objs: $(OBJS)

clean:
	rm -rf $(BUILD)

.PHONY: all objs clean
