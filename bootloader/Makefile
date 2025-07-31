hdrs = $(shell ls *.h ) Makefile 

%.o: %.c $(hdrs)
	@echo CC $< && gcc -Os -m32 -fno-pic -static -nostdlib -g -fno-builtin -c -o $@ $<

%_s.o: %.S $(hdrs)
	@echo AS $< && gcc -m32 -static -fno-pic -c -g -o $@ $<

mbr: mbr.o mbr_s.o mbr.ld
	@echo LD mbr && ld -static -nostdlib -e 0x7c00 -melf_i386 -o mbr -T mbr.ld mbr_s.o mbr.o

gettext: gettext.c 
	@echo "CCLD gettext" && gcc gettext.c -O2 -static -g -o gettext

vmlinux: vmlinux.o vmlinux_s.o Makefile vmlinux.ld
	@echo CCLD vmlinux && ld -melf_i386 -T vmlinux.ld -e 0x1200 -nostdlib \
	vmlinux_s.o vmlinux.o -o vmlinux

vmlinux.asm: vmlinux
	@echo "OBJDUMP vmlinux.asm" && objdump -d -S vmlinux > vmlinux.asm

mbr.img: mbr gettext
	@echo GEN mbr.img && ./gettext mbr mbr.img

linuz.bin: gettext vmlinux
	@echo GEN linuz.bin && ./gettext vmlinux ./linuz.bin

.PHONY: clean 
clean:
	-clear
	-rm gettext mbr *.img *.out *.o *.a *.bin vmlinux

all: mbr.img linuz.bin vmlinux.asm
