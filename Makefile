.PHONY: initramfs run clean libc

libc: 
	@cd tlibc && make && cd ..

initramfs:
	@cd initramfs && find . -print0 | cpio --null -ov --format=newc | gzip -9 \
	  > ../initramfs.cpio.gz; cd ..; mv initramfs.cpio.gz initramfs/boot/
	
run: bootable.iso
	@qemu-system-x86_64 \
		-cdrom bootable.iso

clean:
	@rm -rf build bootable.iso

bootable.iso: initramfs/boot/grub/grub.cfg
	@grub-mkrescue -o bootable.iso initramfs
