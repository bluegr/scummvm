MODULE := engines/dune

MODULE_OBJS = \
	archive.o \
	dune.o \
	graphics.o \
	metaengine.o \
	statics.o \
	video.o

# This module can be built as a plugin
ifeq ($(ENABLE_DUNE), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
DETECT_OBJS += $(MODULE)/detection.o
