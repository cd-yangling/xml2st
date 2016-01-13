#                            _ooOoo_
#                           o8888888o
#                           88" . "88
#                           (| -_- |)
#                            O\ = /O
#                        ____/`---'\____
#                      .   ' \\| |// `.
#                       / \\||| : |||// \
#                     / _||||| -:- |||||- \
#                       | | \\\ - /// | |
#                     | \_| ''\---/'' | |
#                      \ .-\__ `-` ___/-. /
#                   ___`. .' /--.--\ `. . __
#                ."" '< `.___\_<|>_/___.' >'"".
#               | | : `- \`.;`\ _ /`;.`/ - ` : | |
#                 \ \ `-. \_ __\ /__ _/ .-` / /
#         ======`-.____`-.___\_____/___.-`____.-'======
#                            `=---='
#
#                  佛祖保佑             永无BUG
#
# xml2st Makefile
# Copyright (C) 2016 YangLing(yl.tienon@gmail.com)
# SPDX-License-Identifier: GPL-2.0

# 工具链与编译选项
CC      := gcc
CFLAGS  := -Wall -Wextra -Werror -Iinclude -Isrc -Ilib \
           $(shell pkg-config --cflags libxml-2.0)
LDFLAGS := $(shell pkg-config --libs libxml-2.0)

# 位宽开关: make BITS=32 编译 32 位, 默认 64
BITS    ?= 64
ifeq ($(BITS),32)
CFLAGS  += -m32
LDFLAGS += -m32
endif

# 库源码 (对外头在 include/, 内部实现 + 内部头在 src/, 内核移植在 lib/)
LIB_SRCS := src/xml2st_build.c src/xml2st_check.c src/xml2st_entry.c \
            src/xml2st_field.c src/xml2st_iconv.c src/xml2st_mmblk.c \
            src/xml2st_parse.c src/xml2st_log.c lib/rbtree.c
LIB_OBJS := $(LIB_SRCS:.c=.o)

.PHONY: all example test clean

all: example test

example: example.o $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ example.o $(LIB_OBJS) $(LDFLAGS)

test: tests/test_xml2st.o $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ tests/test_xml2st.o $(LIB_OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f example test $(LIB_OBJS) example.o tests/test_xml2st.o
