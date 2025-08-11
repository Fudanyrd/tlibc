hdrs = $(shell ls *.h ) Makefile 
DEFS = 

LDFLAGS = -g -nostdlib -static -melf_i386

%.o: %.c $(hdrs)
	@echo CC $< && gcc -Os -m32 -fno-pic -static $(DEFS) -nostdlib -g -fno-builtin -c -o $@ $<

%_s.o: %.S $(hdrs)
	@echo AS $< && gcc -m32 -static -fno-pic -c -g $(DEFS) -o $@ $<

_%: %_s.o
	@echo CCLD $< && ld $< -melf_i386 -Ttext 0x1000 -o $@ -g

testbios: testbios_s.o bzimage.ld bios_s.o
	@echo LD testbios && ld $(LDFLAGS) -T bzimage.ld testbios_s.o bios_s.o -o testbios

