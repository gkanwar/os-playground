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

        .section .init_gdt, "aw", @progbits
        .set LIMIT_fffff, 0xf << 48 | 0xffff
        .set ACCESS_KERN_CODE, 0b10011010 << 40
        .set ACCESS_KERN_DATA, 0b10010010 << 40
        .set FLAGS_SIZE_32, 1 << 54
        .set FLAGS_PAGE_GRAN, 1 << 55
        .set KERN_CODE_DESC, LIMIT_fffff | ACCESS_KERN_CODE | FLAGS_SIZE_32 | FLAGS_PAGE_GRAN
        .set KERN_DATA_DESC, LIMIT_fffff | ACCESS_KERN_DATA | FLAGS_SIZE_32 | FLAGS_PAGE_GRAN
init_gdt:
        .skip 8 # null descriptor
        .8byte KERN_CODE_DESC
        .8byte KERN_DATA_DESC
        .skip 40 # some room for good measure
init_gdt_end:
init_gdtr:
        .2byte init_gdt_end - init_gdt - 1
        .4byte init_gdt


        .section .text
        .global _start
        .type _start, @function
_start:
        mov esp, offset stack_top       # let there be stack!

        /* set up initial GDT */
        call load_init_gdt

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

        /* reload segments with code selector 0x8 and data selector 0x10 */
load_init_gdt:
        lgdt [init_gdtr]
        jmp 0x8:reload_data_segs
reload_data_segs:
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        ret
