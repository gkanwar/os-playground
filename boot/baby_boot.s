# 512-byte bootloader
# Ref: wiki.osdev.org

        .code16
        .intel_syntax noprefix
        .global _start

        .text
_start:
        mov ax, 0x7c0   # establish segment
        mov ds, ax

        mov si, msg
        cld
        
print_loop:
        or al,al
        jz end
        mov ah, 0x0e
        mov bh, 0
        int 0x10
        jmp print_loop

end:
        jmp end

msg:    
        .ascii "Hello world!\n\0"
        .zero 510 - (. - _start)
        .byte 0x55      # bootloader signature
        .byte 0xaa
