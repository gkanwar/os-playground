#!/bin/bash

# NOTE: Use alt-2 to get to QEMU monitor and quit
qemu-system-i386 -curses -fda bin/baby_boot.bin
