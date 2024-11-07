#!/usr/bin/sh

# install the programs in the /bin directory.
for p in $( make collect ); do cp _$p ../initramfs/bin/$p; done