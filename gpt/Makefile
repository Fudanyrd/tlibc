all: a.img

headers = $(shell ls *.h )

gpt: gpt.c $(headers)
	@echo "CC gpt.c" && gcc -static -O2 -fsanitize=undefined -g gpt.c -o gpt

fat.img:
	@dd if=/dev/zero of=fat.img bs=1M count=63
	@mkfs.vfat ./fat.img -F 32 -S 512

a.img: gpt fat.img
	@./gpt 


.PHONY: cfdisk
cfdisk: a.img
	cfdisk a.img --read-only 


.PHONY: parted
parted: a.img 
	parted ./a.img 
