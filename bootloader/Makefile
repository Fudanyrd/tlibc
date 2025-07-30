hdrs = $(shell ls *.h ) Makefile 

%.o: %.c $(hdrs)
	@echo CC $< && gcc -Os -m16 -fpic -c -o $@ $<

mbr: mbr.o 
	@echo LD mbr && ld -static -nostdlib -e 0x7c00 -melf_i386 -o mbr mbr.o

gettext: gettext.c 
	@echo "CCLD gettext" && gcc gettext.c -O2 -static -g -o gettext

vmlinux.o: vmlinux.c $(hdrs)
	@echo CC vmlinux.c && gcc -m32 -Os -fpic -c -fno-builtin vmlinux.c -o vmlinux.o

vmlinux: vmlinux.o Makefile
	@echo CCLD vmlinux && ld -melf_i386 -e 0x1200 vmlinux.o -o vmlinux

all: mbr.img 

mbr.img: mbr gettext
	@./gettext mbr mbr.img

linuz.bin: gettext vmlinux
	@./gettext vmlinux ./linuz.bin

.PHONY: clean 
clean:
	-rm gettext mbr *.img *.out *.o *.a 
