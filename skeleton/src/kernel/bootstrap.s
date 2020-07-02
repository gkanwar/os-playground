        .intel_syntax noprefix
        
        /* Multiboot header */
        .set ALIGN, 1<<0
        .set MEMINFO, 1<<1
        .set FLAGS, ALIGN | MEMINFO
        .set MAGIC, 0x1BADB002
        .set CHECKSUM, -(MAGIC + FLAGS)
        
        .section .multiboot
        .align 4
        .long MAGIC
        .long FLAGS
        .long CHECKSUM

        /* Set up stack section */
        .section .bss
        .align 16
stack_bottom:
        .skip 16384
stack_top:

        .section .text
        .global _start
        .type _start, @function
_start:
        mov esp, offset stack_top       # let there be stack!

        /* invoke global ctors */
        call _init

        call kernel_main
        /* shouldn't ret from kernel_main, but who knows what garbage code I
         * will be writing, so catch it in case */
end:
        cli
        hlt
        jmp end

        .size _start, . - _start        # for debugging
        
