#!/usr/bin/python3
import json
import os

# configure here
CC = "gcc"
CFLAGS = " -ffreestanding -static -MD "
LD = "ld"

# program prefix
PREFIX = "_"

def main() -> int:
    with open('dependencies.json', 'r') as fobj:
        depl: dict = json.load(fobj);

    allproc: list = []
    uproc: list = []

    with open('Makefile.config', 'w') as fobj:
        # write configure
        fobj.write("# auto generated, do not modify.\n")
        fobj.write(f"CC = {CC}\n")
        fobj.write(f"CFLAGS = {CFLAGS}\n")
        fobj.write(f"LD = {LD}\n")

    with open('Makefile', 'w') as fobj:
        fobj.write("# auto generated, do not modify.\ninclude Makefile.config\n\n")

        # target: all
        for d in depl.keys():
            allproc.append(PREFIX + d);
            uproc.append(d)
        fobj.write(f"all: {' '.join(allproc)}\n");

        # target: objs
        fobj.write("%.o: %.S\n\t$(CC) $(CFLAGS) -c $< -o $@\n");
        fobj.write("%.o: %.c\n\t$(CC) $(CFLAGS) -c $< -o $@\n");
        fobj.write("\n")

        # target: uproc
        for d in depl.keys():
            pn = f"{PREFIX}{d}"
            obj = ' '.join(depl[d])
            fobj.write(f"{pn}: {obj} \n\t$(LD) {obj} -o {pn} \n")
        fobj.write("\n")
        
        # miscellaneous: clean, collect
        fobj.write("\n.PHONY: clean collect\n")
        fobj.write("clean:\n\t-rm -f *.o *.d _*\n")
        fobj.write(f"collect:\n\t@echo {' '.join(uproc)}\n")
    return 0;

if __name__ == "__main__":
    t = input(f"CFLAGS = {CFLAGS}")
    CFLAGS += t
    exit(main())
