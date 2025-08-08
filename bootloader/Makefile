hdrs = $(shell ls *.h ) Makefile 
DEFS = 

%.o: %.c $(hdrs)
	@echo CC $< && gcc -Os -m32 -fno-pic -static $(DEFS) -nostdlib -g -fno-builtin -c -o $@ $<

%_s.o: %.S $(hdrs)
	@echo AS $< && gcc -m32 -static -fno-pic -c -g $(DEFS) -o $@ $<

_%: %_s.o
	@echo CCLD $< && ld $< -melf_i386 -Ttext 0x1000 -o $@ -g

mbr: mbr.o mbr_s.o mbr.ld
	@echo LD mbr && ld -static -nostdlib -e 0x7c00 -melf_i386 -o mbr -T mbr.ld mbr_s.o mbr.o

bzimage: bzimage.o bzimage_s.o bzimage.ld bzsetup_s.o
	@echo LD bzimage && ld -melf_i386 -T bzimage.ld -e _start -o bzimage bzimage_s.o bzimage.o bzsetup_s.o

gettext: gettext.c 
	@echo "CCLD gettext" && gcc gettext.c -O2 -static -g -o gettext

getpayload: getpayload.c $(hdrs)
	@echo "CCLD getpayload" && gcc getpayload.c -O2 -static -g -o getpayload

vmlinux: vmlinux.o vmlinux_s.o Makefile vmlinux.ld
	@echo CCLD vmlinux && ld -melf_i386 -T vmlinux.ld -e 0x1200 -nostdlib \
	vmlinux_s.o vmlinux.o -o vmlinux

vmlinux.asm: vmlinux
	@echo "OBJDUMP vmlinux.asm" && objdump -d -S vmlinux > vmlinux.asm

mbr.img: mbr gettext
	@echo GEN mbr.img && ./gettext mbr mbr.img

bzimage.img: bzimage gettext
	@echo GEN bzimage.img && ./gettext bzimage bzimage.img

linuz.bin: bzimage.img
	@echo LINK linuz.bin && link bzimage.img linuz.bin

.PHONY: clean 
clean:
	-rm gettext mbr *.img *.out *.o *.a *.bin vmlinux *.asm bzimage
	-clear

all: mbr.img linuz.bin vmlinux.asm
