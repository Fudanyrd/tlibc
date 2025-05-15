.PHONY: initramfs run clean libc
KERNEL = initramfs/boot/kernel-6
QEMU = qemu-system-x86_64

libc: 
	@cd tlibc && make && cd ..

# If you want to use an initial ram fs, 
# create a folder named 'usr' and 'make initramfs'
# will create it for you.
# An example of usr layout:
# ./usr
# ├── bin
# │   ├── busybox
# │   ├── cat
# │   └── ls
# ├── dev
# └── init
initramfs:
	@cd usr && find . -print0 | cpio --null -ov --format=newc | gzip -9 \
	  > ../initramfs.cpio.gz; cd ..; mv initramfs.cpio.gz initramfs/boot/
	
run: bootable.iso vm.img usb.img
	$(QEMU) -cdrom bootable.iso -hda ./vm.img \
			-usb  \
			-drive if=none,format=raw,file=./usb.img,id=stick \
			-device usb-storage,drive=stick

#			-netdev tap,id=net0  \
#			-object filter-dump,id=net0,netdev=net0,file=packets.pcap \
#			-device e1000,netdev=net0

qemu: vm.img
	$(QEMU) \
		-kernel $(KERNEL) \
	  	-serial mon:stdio \
		-nographic -m 8G \
		-hda ./vm.img \
		-append "console=ttyS0 acpi=off root=/dev/sda init=/init"

# -initrd initramfs/boot/initramfs.cpio.gz \

clean:
	-rm -rf build bootable.iso *.img initramfs/boot/initramfs.cpio.gz

bootable.iso: initramfs/boot/grub/grub.cfg
	@grub-mkrescue -o bootable.iso ./initramfs/ -no-emul-boot

# If you want to use an initial ram fs, 
# create a folder named 'rootfs' and 'make vm.img'
# will create it for you.
vm.img: rootfs/init
	@dd if=/dev/zero of=./vm.img bs=1M count=512
	@mkfs.ext4 -d ./rootfs vm.img

usb.img:
	@dd if=/dev/zero of=./usb.img bs=1M count=128
	@mkfs.ext4 -d ./usb usb.img

.PHONY: help
help:
	@echo "bootable.iso : the CD/ROM image for linux kernel."
	@echo "help         : print this help message."
	@echo "initramfs    : build an initrd and put it at initramfs/boot/ folder."
	@echo "libc         : build a tiny libc."
	@echo "qemu         : run qemu without using disk images."
	@echo "run          : run qemu with using disk images."
	@echo "vm.img       : a 512MB disk image for linux kernel. "
