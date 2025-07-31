hdrs = $(shell ls *.h ) Makefile 

%.o: %.c $(hdrs)
	@echo CC $< && gcc -Os -m16 -fno-builtin -c -o $@ $<

mbr: mbr.o disk.o mbr.ld
	@echo LD mbr && ld -static -nostdlib -e 0x7c00 -melf_i386 -o mbr -T mbr.ld mbr.o disk.o

gettext: gettext.c 
	@echo "CCLD gettext" && gcc gettext.c -O2 -static -g -o gettext

vmlinux.o: vmlinux.c $(hdrs)
	@echo CC vmlinux.c && gcc -m32 -Os -c -fno-builtin vmlinux.c -o vmlinux.o

vmlinux: vmlinux.o disk.o Makefile vmlinux.ld
	@echo CCLD vmlinux && ld -melf_i386 -T vmlinux.ld -e 0x1200 -nostdlib \
	vmlinux.o disk.o -o vmlinux

mbr.img: mbr gettext
	@./gettext mbr mbr.img

linuz.bin: gettext vmlinux
	@./gettext vmlinux ./linuz.bin

.PHONY: clean 
clean:
	-rm gettext mbr *.img *.out *.o *.a *.bin

all: mbr.img linuz.bin
