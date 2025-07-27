all: a.img myfat.img

headers = $(shell ls *.h )

gpt: gpt.c $(headers)
	@echo "CC gpt.c" && gcc -static -O2 -fsanitize=undefined -g gpt.c -o gpt

mkfat: fat.c $(headers)
	@echo "CC fat.c" && gcc -static -O2 -g fat.c -o mkfat

myfat.img: mkfat
	@./mkfat

fat.img:
	@dd if=/dev/zero of=fat.img bs=1M count=63
	@mkfs.fat -v -F 32 -S 512 -s 8 ./fat.img

a.img: gpt fat.img
	@./gpt 


.PHONY: cfdisk
cfdisk: a.img
	cfdisk a.img --read-only 


.PHONY: parted
parted: a.img 
	parted ./a.img 
