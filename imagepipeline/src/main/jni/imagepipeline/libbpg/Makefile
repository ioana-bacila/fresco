# libbpg Makefile
# 
# Compile options:
#
# Enable compilation of Javascript decoder with Emscripten
#USE_EMCC=y
# Enable x265 for the encoder
#USE_X265=y
# Enable the JCTVC code (best quality but slow) for the encoder
#USE_JCTVC=y
# Compile bpgview (SDL and SDL_image libraries needed)
#USE_BPGVIEW=y
# Enable it to use bit depths > 12 (need more tests to validate encoder)
#USE_JCTVC_HIGH_BIT_DEPTH=y
# Enable the cross compilation for Windows
#CONFIG_WIN32=y
# Enable for compilation on MacOS X
#CONFIG_APPLE=y
# Installation prefix
prefix=/usr/local


#################################

CROSS_PREFIX:=
EXE:=

CC=$(CROSS_PREFIX)gcc
CXX=$(CROSS_PREFIX)g++
AR=$(CROSS_PREFIX)ar

PWD:=$(shell pwd)

CFLAGS:=-Os -Wall -MMD -fno-asynchronous-unwind-tables -fdata-sections -ffunction-sections -fno-math-errno -fno-signed-zeros -fno-tree-vectorize -fomit-frame-pointer
CFLAGS+=-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_REENTRANT
CFLAGS+=-I.
CFLAGS+=-DCONFIG_BPG_VERSION=\"$(shell cat VERSION)\"

LDFLAGS=-g
LDFLAGS+=-Wl,--gc-sections
CFLAGS+=-g
CXXFLAGS=$(CFLAGS)

PROGS=bpgdec$(EXE)
all: $(PROGS)

LIBBPG_OBJS:=$(addprefix libavcodec/, \
hevc_cabac.o  hevc_filter.o  hevc.o         hevcpred.o  hevc_refs.o\
hevcdsp.o     hevc_mvs.o     hevc_ps.o   hevc_sei.o\
utils.o cabac.o golomb.o videodsp.o )
LIBBPG_OBJS+=$(addprefix libavutil/, mem.o buffer.o log2_tab.o frame.o pixdesc.o md5.o )
LIBBPG_OBJS+=libbpg.o

$(LIBBPG_OBJS): CFLAGS+=-D_ISOC99_SOURCE -D_POSIX_C_SOURCE=200112 -D_XOPEN_SOURCE=600 -DHAVE_AV_CONFIG_H -std=c99 -D_GNU_SOURCE=1 -DUSE_VAR_BIT_DEPTH -DUSE_PRED

LIBS:=-lrt
LIBS+=-lm -lpthread

BPGDEC_LIBS:=-lpng $(LIBS)

libbpg.a: $(LIBBPG_OBJS) 
	$(AR) rcs $@ $^

bpgdec$(EXE): bpgdec.o libbpg.a
	$(CC) $(LDFLAGS) -o $@ $^ $(BPGDEC_LIBS)

size:
	strip bpgdec
	size bpgdec libbpg.o libavcodec/*.o libavutil/*.o | sort -n
	gzip < bpgdec | wc

install: bpgdec
	install -s -m 755 $^ $(prefix)/bin

CLEAN_DIRS=doc libavcodec libavutil

clean: x265_clean
	rm -f $(PROGS) *.o *.a *.d *~ $(addsuffix /*.o, $(CLEAN_DIRS)) \
          $(addsuffix /*.d, $(CLEAN_DIRS)) $(addsuffix /*~, $(CLEAN_DIRS)) \
          $(addsuffix /*.a, $(CLEAN_DIRS))

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

-include $(wildcard *.d)
-include $(wildcard libavcodec/*.d)
-include $(wildcard libavutil/*.d)