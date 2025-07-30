hdrs = $(shell ls *.h ) Makefile 

%.o: %.c $(hdrs)
	@echo CC $< && gcc -Os -m16 -fpic -c -o $@ $<

mbr: mbr.o 
	@echo LD mbr && ld -static -nostdlib -melf_i386 -o mbr mbr.o

gettext: gettext.c 
	@echo "CCLD gettext" && gcc gettext.c -O2 -static -g -o gettext

all: mbr.img 

mbr.img: mbr gettext
	@./gettext mbr mbr.img

.PHONY: clean 
clean:
	-rm gettext mbr *.img *.out *.o *.a 
