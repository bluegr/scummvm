MODULE := engines/yaga

MODULE_OBJS = \
	animation.o \
	audiofile.o \
	console.o \
	metaengine.o \
	scripthandler.o \
	text.o \
	video.o \
	yaga.o

# This module can be built as a plugin
ifeq ($(ENABLE_YAGA), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
DETECT_OBJS += $(MODULE)/detection.o
