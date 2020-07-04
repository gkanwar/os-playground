/* Custom linker script to combine bootstrap and kernel */
ENTRY(_start)
SECTIONS {
         /* conventional memory load location for a kernel */
         . = 1M;
         _kernel_start = .;
         /* pre-paging sections */
         .pretext ALIGN(4K) : {
                  *(.multiboot)
                  *(.pretext)
         }
         .prerodata : {
                    *(.prerodata)
         }
         .text : {
               *(.multiboot)
               *(.text)
         }
         .rodata : {
                 *(.rodata)
         }
         .data : {
               *(.data)
         }
         .bss : {
              *(COMMON) /* TODO: need to split between pre/post? */
              *(.bss)
         }
         _kernel_end = .;
}
