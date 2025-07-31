FROM ubuntu:24.04
WORKDIR /home/yrd

# needed by building distribution
RUN apt-get -y update && \
	apt-get -y install cpio wget less file && \
	apt-get -y install libncurses5-dev pkgconf flex bison && \
	apt-get -y install make binutils gcc gdb qemu-system-x86 gcc-multilib && \
	apt-get -y install bc libelf-dev libssl-dev grub-common xorriso grub-pc  && \
	apt-get clean && rm -rf /var/lib/apt/lists/*

# done
CMD ["sleep", "infinity"]

