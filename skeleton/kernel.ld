/* Custom linker script to combine bootstrap and kernel */
ENTRY(_start)
SECTIONS {
         /* conventional memory load location for a kernel */
         . = 1M;
         _kernel_start = .;
         .text BLOCK(4K) : ALIGN(4K) {
               *(.multiboot)
               *(.text)
         }
         .rodata BLOCK(4K) : ALIGN(4K) {
                 *(.rodata)
         }
         .data BLOCK(4K) : ALIGN(4K) {
               *(.data)
         }
         .bss BLOCK(4K) : ALIGN(4K) {
              *(COMMON) /* what is this?? */
              *(.bss)
         }
         _kernel_end = .;
}
