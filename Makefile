.PHONY: initramfs run clean libc
KERNEL = initramfs/boot/kernel-6
QEMU = qemu-system-x86_64

libc: 
	@cd tlibc && make && cd ..

initramfs:
	@cd rootfs && find . -print0 | cpio --null -ov --format=newc | gzip -9 \
	  > ../initramfs.cpio.gz; cd ..; mv initramfs.cpio.gz initramfs/boot/
	
run: bootable.iso
	$(QEMU) -hda bootable.iso \

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

bootable.iso: initramfs/boot/grub/grub.cfg vm.img
	@grub-mkrescue -o bootable.iso ./initramfs/ -append_partition 2 Linux ./vm.img

vm.img:
	@dd if=/dev/zero of=./vm.img bs=1M count=256
	@mkfs.ext4 -d ./rootfs vm.img

.PHONY: help
help:
	@echo "To run on real machine: make run; to simulate: make qemu or make debug."
