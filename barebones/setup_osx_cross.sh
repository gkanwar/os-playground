#!/bin/bash

# On OSX, llvm is a generic compiler, so we can make it do the right targeting
# with some flags. TBD whether this is painless enough to be worth it, maybe
# best to just build a GCC cross compiler in the OSX dev environment too.

# export AS="as -target i686-elf"
# export CC="gcc -target i686-elf"
# export LD="ld -target i686-elf"
# export LGCC="" # don't link libgcc

# Results: the cross-compiler is okay, but Apple's ld doesn't seem to accept
# any sort of linker script. Seems like too much of a pain to hack around for
# now, so best to just install a real cross-compiler (LLVM works, but Apple's
# LLVM is a real monster).
echo "Don't try to cross-compile with Apple's LLVM!"

