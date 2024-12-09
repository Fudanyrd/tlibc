.PHONY: initramfs run clean libc
KERNEL = initramfs/boot/kernel-6
QEMU = qemu-system-x86_64

libc: 
	@cd tlibc && make && cd ..

initramfs:
	@cd initramfs && find . -print0 | cpio --null -ov --format=newc | gzip -9 \
	  > ../initramfs.cpio.gz; cd ..; mv initramfs.cpio.gz initramfs/boot/
	
run: bootable.iso
	$(QEMU) \
		-cdrom bootable.iso

qemu:
	$(QEMU) \
		-kernel $(KERNEL) \
	  	-serial mon:stdio \
		-nographic \
		-initrd initramfs/boot/initramfs.cpio.gz \
		-append "console=ttyS0 quiet acpi=off"

clean:
	-rm -rf build bootable.iso initramfs/boot/initramfs.cpio.gz

bootable.iso: initramfs/boot/grub/grub.cfg
	@grub-mkrescue -o bootable.iso initramfs

.PHONY: help
help:
	@echo "To run on real machine: make run; to simulate: make qemu or make debug."
