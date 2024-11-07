# Build a Linux Distribution

## Step 1
Compile linux kernel. 
```
make menuconfig
make -j 8
```

The kernel image is the file `project-root/arch/x86/boot/bzImage` 
in my case. Use the `file` command to check its type:
```
# file /path/to/bzImage
bzImage: Linux kernel x86 boot executable bzImage, version 6.11.2 (root@yrd) #3 SMP PREEMPT_DYNAMIC Thu Oct 31 19:12:51 CST 2024, RO-rootFS, swap_dev 0X8, Normal VGA
```

## Step 2 
Create a directory named `initramfs` with the following layout:
```
bin
    prog1
    prog2
boot
    grub
        grub.cfg
    bzImage
    initramfs.cpio.gz
device
home
    README.txt
```

The `grub.cfg` configuation is for `GNU grub`([link](https://www.gnu.org/software/grub/)).
Write the following into it:

```
menuentry "linux-version" {
	linux /boot/bzImage
	initrd /boot/initramfs.cpio.gz
}
```

## Step 3
Generate bootable image and boot it in qemu.

```
grub-mkrescue -o bootable.iso initramfs
```

Run qemu by `qemu-system-x86_64 -cdrom bootable.iso`.