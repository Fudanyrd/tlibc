#!/usr/bin/sh
for p in $( make collect ); do cp _$p ../initramfs/bin/$p; done