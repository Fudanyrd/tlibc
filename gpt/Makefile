all: a.img myfat.img

headers = $(shell ls *.h )
headers := Makefile

%.o: %.c $(headers)
	@echo "CC $<" && gcc -Wno-overflow -static -O2 -c -g $< -o $@

gpt: gpt.o fat.o
	@echo "CCLD gpt" && gcc -static -O2 -g fat.o gpt.o -o gpt

mkfat: fat.o $(headers)
	@echo "CCLD mkfat" && gcc -static -O2 -g fat.o -o mkfat

myfat.img: mkfat
	@./mkfat

mbr.img: 
	@echo "GEN mbr.img" && make -C ../bootloader mbr.img && cp ../bootloader/mbr.img ./mbr.img

fat.img:
	@dd if=/dev/zero of=fat.img bs=1M count=63
	@mkfs.fat -v -F 32 -S 512 -s 8 ./fat.img

a.img: gpt mbr.img
	@./gpt 


.PHONY: cfdisk
cfdisk: a.img
	cfdisk a.img --read-only 


.PHONY: parted
parted: a.img 
	parted ./a.img 

.PHONY: clean 
clean:
	-rm -f *.img mkfat gpt *.o
	-rm *.out
	-$(MAKE) -C ../bootloader clean

.PHONY: qemu 
qemu: a.img
	@qemu-system-x86_64 -hda a.img -nographic

# display the fat filesystem.
.PHONY: dispfat
dispfat: myfat.img 
	@mount myfat.img /mnt/tmp && stat /mnt/tmp/echo && umount /mnt/tmp
