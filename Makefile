CC = gcc
CCLIB = -lm -msse2 -mfpmath=sse -L ./lib/amd64/linux/ -ljpeg
#CCLIB = -lm -msse2 -mfpmath=sse -L ./lib/i386/linux/ -ljpeg
TARGET = RayTracing.exe

### include current folder (for headers)
CFLAGS = -I ./

SRCS := $(shell ls | grep c$$)

### include folder add
SRCS += $(addprefix include/, $(shell ls include | grep c$$))
DIR_CHECK += include

### include/images folder add
SRCS += $(addprefix include/images/, $(shell ls include/images | grep c$$))
DIR_CHECK += include/images

### naive folder add
SRCS += $(addprefix naive/, $(shell ls naive | grep c$$))
DIR_CHECK += naive

### kdtree folder add
SRCS += $(addprefix kdtree/, $(shell ls kdtree | grep c$$))
DIR_CHECK += kdtree

### shading folder add
SRCS += $(addprefix shading/, $(shell ls shading | grep c$$))
DIR_CHECK += shading

### images folder add
SRCS += $(addprefix images/, $(shell ls images | grep c$$))
DIR_CHECK += images

RELEASE_DIR = gcc_release
DEBUG_DIR = gcc_debug
DEPEND_FILE = depend_file

ifeq ($(MAKECMDGOALS), release)
OBJS_DIR = $(RELEASE_DIR)
CCOPT = -O2
else
OBJS_DIR = $(DEBUG_DIR)
CCOPT = -O2 -Wall -DDEBUG
endif

#OBJS = $(addprefix $(OBJS_DIR)/, $(notdir $(SRCS)))
OBJS = $(addprefix $(OBJS_DIR)/, $(SRCS))
OBJS := $(OBJS:%.c=%.o)

.SUFFIXES: .c .o
.PHONY: all release debug clean test

all:
	@echo "=========================="
	@echo "usage: make release"
	@echo "       make debug"
	@echo "       make clean"
	@echo "       make test [option=\"-cnf\"]"
	@echo "=========================="

$(OBJS_DIR)/%.o : %.c
	$(CC) $(CFLAGS) $(CCOPT) -c $< -o $@ $(CCLIB)


release: chkdir depend $(OBJS)
	$(CC) $(CFLAGS) $(CCOPT) $(OBJS) -o $(TARGET) $(CCLIB)

debug: chkdir depend $(OBJS)
	$(CC) $(CFLAGS) $(CCOPT) $(OBJS) -o $(TARGET) $(CCLIB)

clean:
	@rm -rf $(DEPEND_FILE) $(DEBUG_DIR) $(RELEASE_DIR) $(TARGET) *.bmp *.nlogn *.nlog2n Debug Release

test:
	./$(TARGET) $(option)

chkdir:
	@`[ -d $(OBJS_DIR) ] || mkdir $(OBJS_DIR)`
	@for DIR in $(DIR_CHECK); do \
		[ -d $(OBJS_DIR)/$$DIR ] || mkdir $(OBJS_DIR)/$$DIR; \
	done

depend: chkdir
	@rm -f $(DEPEND_FILE)
	@for FILE in $(SRCS:%.c=%); do \
		$(CC) $(CFLAGS) -MM -MT $(OBJS_DIR)/$$FILE.o $$FILE.c >> $(DEPEND_FILE); \
	done

-include $(DEPEND_FILE)
