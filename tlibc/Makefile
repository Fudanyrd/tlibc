LIBC_OBJS = std.o sys.o
OBJS = $(LIBC_OBJS) echo.o wc.o crash.o cat.o seq.o sh.o ls.o
OBJS += pause.o
CC = gcc
CFLAGS = -MD -static
UPROG = crash echo wc cat seq ls pause link stat mkdir yes sh
all: _crash _echo _wc _cat _seq _ls _pause _link _stat _mkdir _yes _sh

# not implemented
# UPROG += sh

include Makefile.config

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -ffreestanding -c $< -o $@

_%: %.o $(LIBC_OBJS)
	ld $(LIBC_OBJS) $< -o $@

.PHONY: clean
clean:
	-rm -f $(OBJS) *.d _*

.PHONY: collect
collect:
	@echo $(UPROG)

.PHONY: archive
archive:
	@zip tlibc.zip ./*
