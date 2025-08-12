hdrs = $(shell ls ../bootloader/*.h ) Makefile 
DEFS = 
CDEFS = $(DEFS) -D__BIOS__
ASFLAGS = -m32 -fno-pic -static  -g
CFLAGS = $(ASFLAGS) -Os -nostdlib -g -fno-builtin  
LDFLAGS = -g -nostdlib -static -melf_i386

%.o: %.c $(hdrs)
	@echo CC $< && gcc $(CDEFS) $(CFLAGS) -c -o $@ $<

bios_bzimage.o: ../bootloader/bzimage.c $(hdrs)
	@echo CC bios_bzimage.o && gcc $(CDEFS) $(CFLAGS) -c ../bootloader/bzimage.c -o bios_bzimage.o

%_s.o: %.S $(hdrs)
	@echo AS $< && gcc $(ASFLAGS) -c $(CDEFS) -o $@ $<

_%: %_s.o
	@echo CCLD $< && ld $< -melf_i386 -Ttext 0x1000 -o $@ -g

testbios: testbios_s.o bzimage.ld bios_s.o
	@echo LD testbios && ld $(LDFLAGS) -T bzimage.ld testbios_s.o bios_s.o -o testbios

biosboot: biosboot_s.o bios_s.o bios_bzimage.o ../bootloader/bzsetup_s.o bzimage.ld
	@echo LD biosboot && ld $(LDFLAGS) \
	biosboot_s.o bios_s.o bios_bzimage.o ../bootloader/bzsetup_s.o \
	-T bzimage.ld -o biosboot

