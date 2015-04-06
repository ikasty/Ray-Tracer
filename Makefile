CC = gcc
CCLIB = -lm -msse2 -mfpmath=sse
TARGET = RayTracing.exe

DIR_CHECK =

SRCS := $(shell ls | grep c$$)
### kdtree folder add
SRCS += $(addprefix kdtree/, $(shell ls kdtree | grep c$$))
DIR_CHECK += kdtree

RELEASE_DIR = gcc_release
DEBUG_DIR = gcc_debug
DEPEND_FILE = depend_file

ifeq ($(MAKECMDGOALS), release)
OBJS_DIR = $(RELEASE_DIR)
CCOPT = -O2
else
OBJS_DIR = $(DEBUG_DIR)
CCOPT = -O0 -Wall -DDEBUG
endif

#OBJS = $(addprefix $(OBJS_DIR)/, $(notdir $(SRCS)))
OBJS = $(addprefix $(OBJS_DIR)/, $(SRCS))
OBJS := $(OBJS:%.c=%.o)


ifneq ($(strip $(count)),)
counts = $(addprefix -c,$(count))
endif

.SUFFIXES: .c .o
.PHONY: all release debug clean test

all:
	@echo "$(DIR_CHECK)"
	@echo "=========================="
	@echo "usage: make release"
	@echo "       make debug"
	@echo "       make clean"
	@echo "       make test [filename=filename.obj] [count=n]"
	@echo "=========================="

$(OBJS_DIR)/%.o : %.c
	$(CC) $(CCOPT) -c $< -o $@ $(CCLIB)


release: chkdir depend $(OBJS)
	$(CC) $(CCOPT) $(OBJS) -o $(TARGET) $(CCLIB)

debug: chkdir depend $(OBJS)
	$(CC) $(CCOPT) $(OBJS) -o $(TARGET) $(CCLIB)

clean:
	@rm -rf $(DEPEND_FILE) $(DEBUG_DIR) $(RELEASE_DIR) $(TARGET) *.bmp

test:
	./$(TARGET) $(filename) $(counts)

chkdir:
	@`[ -d $(OBJS_DIR) ] || mkdir $(OBJS_DIR)`
	@for DIR in $(DIR_CHECK); do \
		[ -d $(OBJS_DIR)/$$DIR ] || mkdir $(OBJS_DIR)/$$DIR; \
	done

depend: chkdir
	@rm -f $(DEPEND_FILE)
	@for FILE in $(SRCS:%.c=%); do \
		$(CC) -MM -MT $(OBJS_DIR)/$$FILE.o $$FILE.c >> $(DEPEND_FILE); \
	done

-include $(DEPEND_FILE)
