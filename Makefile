CC = gcc
TARGET = RayTracing.exe
SRCS := $(shell ls | grep c$$)
DEPEND_FILE = depend_file

ifeq ($(MAKECMDGOALS), release)
OBJS_DIR=gcc_release
else
OBJS_DIR=gcc_debug
endif

OBJS = $(SRCS:%.c=$(OBJS_DIR)/%.o)

.SUFFIXES: .c .o
.PHONY: all release debug clean

all:
	@echo "=========================="
	@echo "$(SRCS:%.c=%)"
	@echo "usage: make release"
	@echo "       make debug"
	@echo "       make clean"
	@echo "=========================="

$(OBJS_DIR)/%.o : %.c
	gcc -c $< -o $@

release: chkdir depend $(OBJS)
	gcc $(OBJS) -o $(OBJS_DIR)/$(TARGET)

debug: chkdir depend $(OBJS)
	gcc $(OBJS) -o $(OBJS_DIR)/$(TARGET)

clean:
	@rm -rf $(DEPEND_FILE) debug release


chkdir:
	@`[ -d $(OBJS_DIR) ] || mkdir $(OBJS_DIR)`

depend: chkdir
	@rm -f $(DEPEND_FILE)
	@for FILE in $(SRCS:%.c=%); do \
		gcc -MM -MT $(OBJS_DIR)/$$FILE.o $$FILE.c >> $(DEPEND_FILE); \
	done

-include $(DEPEND_FILE)