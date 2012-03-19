.PHONY: all install uninstall clean

-include ../gst_plugin/src/rules.make

CC = $(CROSS_COMPILE)g++
CX = $(CROSS_COMPILE)gcc
INCS     ?= -I../gst_plugin/src \
            -I$(GSDK_ROOT)/include/OGLES2 \
            -I$(GSDK_ROOT)/include/bufferclass_ti \
            -I$(CMEM_DIR)/include \
	    -I$(GSTREAMER_SRC_DIR)/gstreamer-0.10.30	\
	    -I$(TGTFS_PATH)/opt/gstreamer/include/glib-2.0 \
	    -I$(TGTFS_PATH)/opt/gstreamer/lib/glib-2.0/include \
	    -I$(TGTFS_PATH)/opt/gstreamer/include/gstreamer-0.10

LIB_PATH ?= -L$(GSDK_ROOT)/gfx_rel_es8.x \
            -Wl,--rpath=$(GSDK_ROOT)/gfx_rel_es8.x \

INST_DEST    ?= $(TGTFS_PATH)/opt/usbVidStream

CFLAGS   := -W -Wall -O2 -DLINUX $(INCS)
LIBS     := -lEGL
LDFLAGS  := $(LIB_PATH)

LIBS    += -lGLESv2
CMEM_LIB     ?= $(CMEM_DIR)/lib/cmem.a470MV
LIBS    += $(CMEM_LIB)

TARGET = bcRenderer 

SOURCES = bc_renderer.c 
HEADRERS = 
OBJFILES += $(SOURCES:%.c=%.o)

all:	$(TARGET)

$(TARGET):	$(OBJFILES) 
	$(CC) $^ -o $@ $(LDFLAGS) $(LIBS)

$(OBJFILES):	%.o: %.c $(HEADRERS)
	$(CX) -c $< -o $@ $(CFLAGS)

install:	$(TARGET)
	install -m 0755 $^ $(INST_DEST)

uninstall:
	cd $(INST_DEST) && rm -f $(TARGET)

.PHONY: clean
clean:
	-rm -f $(OBJFILES) $(TARGET)