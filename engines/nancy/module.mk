MODULE := engines/nancy

MODULE_OBJS = \
  action/actionmanager.o \
  action/arfactory_v1.o \
  action/primaryvideo.o \
  action/recordtypes.o \
  action/secondaryvideo.o \
  action/staticbitmapanim.o \
  ui/frame.o \
  ui/inventorybox.o \
  ui/scrollbar.o \
  ui/textbox.o \
  ui/viewport.o \
  state/logo.o \
  state/map.o \
  state/scene.o\
  audio.o \
  console.o \
  cursor.o \
  decompress.o \
  font.o \
  graphics.o \
  iff.o \
  input.o \
  metaengine.o \
  nancy.o \
  renderobject.o \
  resource.o \
  video.o

# This module can be built as a plugin
ifeq ($(ENABLE_NANCY), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
DETECT_OBJS += $(MODULE)/detection.o