# get busybox binary that fits the platform
cd initramfs/bin
wget https://busybox.net/downloads/binaries/1.16.1/busybox-x86_64
# rename the busybox executable
link busybox-x86_64 busybox
rm busybox-x86_64
cd ../..
