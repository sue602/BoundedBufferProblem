# 可执行文件
TARGET=boundedbuffer
# C文件
SRCS = boundedbuffer.c
# 目标文件
OBJS = $(SRCS:.c=.o)

PREFIX = /Users/ltzd/Downloads/st-1.9/DARWIN_15.4.0_DBG

# 指令编译器和选项
CC=gcc
CFLAGS=-Wall -std=gnu99 
BUILD_CFLAGS = -I$(PREFIX)

LNX_LDFLAGS = -lst -L$(PREFIX)
MYLDFLAGS="$(LNX_LDFLAGS)"

$(TARGET):$(OBJS)
#	@echo TARGET:$@
#	@echo OBJECTS:$^
	$(CC) -lst -L$(PREFIX) -o $@ $^

clean:
	rm -rf $(TARGET) $(OBJS)

%.o:%.c
	$(CC) $(CFLAGS) $(BUILD_CFLAGS) -o $@ -c $<
