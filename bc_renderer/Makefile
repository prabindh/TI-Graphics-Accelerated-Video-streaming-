.PHONY: all install uninstall clean

-include rules.make

CC = $(CROSS_COMPILE)g++
CX = $(CROSS_COMPILE)gcc
INCS     ?= -I$(GSDK_ROOT)/include/OGLES2 \
            -I$(GSDK_ROOT)/include/bufferclass_ti \
	    -I$(GSDK_ROOT)/GFX_Linux_KM/services4/3rdparty/bufferclass_ti \
	 	-I$(GSDK_ROOT)/GFX_Linux_KM/include4 \
            -I$(CMEM_DIR)/include \
		-I$(V3DFX_DIR)/api/include


LIB_PATH ?= -L$(GSDK_ROOT)/gfx_rel_es5.x \
            -Wl,--rpath=$(GSDK_ROOT)/gfx_rel_es5.x \

INST_DEST    ?= $(TGTFS_PATH)/opt/usbVidStream

CFLAGS   := -W -Wall -O2 -DLINUX $(INCS)
LIBS     := -lEGL
LDFLAGS  := $(LIB_PATH)

LIBS    += -lGLESv2
CMEM_LIB     ?= $(CMEM_DIR)/lib/cmem.a470MV
LIBS    += $(CMEM_LIB)

TARGET = piper

all:	$(TARGET)

TEST_CPP_FILES := $(wildcard *.cpp)
TEST_OBJ_FILES := $(patsubst %.cpp,%.o,$(TEST_CPP_FILES))
$(TARGET): $(TEST_OBJ_FILES)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS) \
	 $(V3DFX_DIR)/api/build/v3dfx-base.a 

%.o: %.cpp
	$(CC) $(INCS) -c -o $@ $<

install:	$(TARGET)
	install -m 0755 $^ $(INST_DEST)

uninstall:
	cd $(INST_DEST) && rm -f $(TARGET)

.PHONY: clean
clean:
	-rm -f $(OBJFILES) $(TARGET)
