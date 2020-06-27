# 512-byte bootloader
# Ref: wiki.osdev.org

        .code16
        .intel_syntax noprefix
        .global _start

        .text
_start:
        xor ax, ax
        mov ds, ax      # segment 0
        mov ss, ax      # stack segment 0
        mov sp, 0x9c00  # stack starts 0x2000 past .text

        cld             # auto-advance string inputs

        mov ax, 0xb800
        mov es, ax      # set up segment for STOS

        call video_clear
        lea si, splash
        mov cl, byte ptr [splash_attrib]
        mov byte ptr [attrib], cl
        call sprint
        # call bios_print

        # test reg print
        # mov word ptr [reg16], 0xabcd
        # call printreg16

        call bind_keyhandler

        # mov ax, 0xb800
        # mov gs, ax
        # mov bx, 0x0000
        # mov ax, [gs:bx]


end:    
        jmp end

# .include "bios_print.s"
.include "video_print.s"        

bind_keyhandler:
        cli
        mov bx, 0x09
        shl bx, 2
        xor ax, ax
        mov gs, ax
        lea si, keyhandler
        mov word ptr gs:[bx], si
        mov word ptr gs:[bx+2], ds
        sti
        ret
        
keyhandler:
        in al, 0x60     # read port 60 = keyboard input
        mov bl, al
        mov byte ptr [port60], al
        
        in al, 0x61     # pulse signal on high bit to get next key
        mov ah, al
        or al, 0x80
        out 0x61, al
        xchg ah, al
        out 0x61, al

        mov al, 0x20    # finish interrupt
        out 0x20, al

        and bl, 0x80    # skip print if key release
        jnz keyhandler_done

        movzx ax, byte ptr [port60]
        mov word ptr [reg16], ax
        call printreg16
keyhandler_done:
        iret

        /*
shutdown:
        mov ax, 0x5307
        mov bx, 0x0001
        mov cx, 0x0003
        int 0x15
        ret
        */

        # data
splash_attrib:
        .byte 0x0c # light red on black
splash:
        .ascii "Welcome to the baby bootloader!\0"
        
port60:
        .word 0

tail:   
        .zero 510 - (. - _start)
        .byte 0x55      # bootloader signature
        .byte 0xaa
