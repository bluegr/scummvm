MODULE := engines/dune

MODULE_OBJS := \
	archive.o \
	dune.o \
	intro. o \
	logic.o \
	graphics.o \
	metaengine.o \
	statics.o \
	sound/sound.o \
	sound/music.o \
	sound/midiadlib.o \
	sound/mt32.o \
	sound/musicbase.o \
	video.o

# This module can be built as a plugin
ifeq ($(ENABLE_DUNE), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
DETECT_OBJS += $(MODULE)/detection.o
