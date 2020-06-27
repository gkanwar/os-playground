bios_print:
        lodsb
        or al, al
        jz bios_print_done
        mov ah, 0x0e
        mov bh, 0
        int 0x10
        jmp bios_print
bios_print_done:
        ret
