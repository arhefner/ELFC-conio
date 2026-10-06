SHELL := /bin/bash
CC = elfc
CFLAGS = -L

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.prg)
TARGET = conio.lib

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p ../lib
	cat $^ > ../lib/$@

%.prg: %.c bios.h ../bios.inc ../include/conio.h
	$(CC) $(CFLAGS) $<

clean:
	rm -f *.asm *.lst *.build $(OBJS) ../lib/$(TARGET)
