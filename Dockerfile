FROM ubuntu:22.04
WORKDIR /home/yrd

# needed by building distribution
RUN apt-get -y update && apt-get -y upgrade
RUN apt-get -y install cpio wget
RUN apt-get -y install less file

# needed by menu-config
RUN apt-get -y install libncurses5-dev
RUN apt-get -y install pkgconf
RUN apt-get -y install flex
RUN apt-get -y install bison

# compiler, debugger, qemu
RUN apt-get -y install make
RUN apt-get -y install binutils
RUN apt-get -y install gcc
RUN apt-get -y install gdb
RUN apt-get -y install qemu-system-x86
RUN qemu-system-x86_64 -h

# kernel dependencies
RUN apt-get -y install bc
RUN apt-get -y install libelf-dev
RUN apt-get -y install libssl-dev

# for building bootable iso image
RUN apt-get -y install grub-common
RUN apt-get -y install xorriso
RUN apt-get -y install grub-pc

# done
CMD ["sleep", "infinity"]
