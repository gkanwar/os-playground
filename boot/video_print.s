# NOTE: expects es = 0xb800 so (es:di) addresses video text memory

video_clear:
        xor ax, ax
        xor di, di
video_clear_loop:
        cld     # auto-advance
        cmp di, 0x1000
        jge video_clear_done
        stosw
        jmp video_clear_loop
video_clear_done:
        ret

dochar: call cprint
sprint: # video mem output
        lodsb
        cmp al, 0
        jne dochar
        add byte ptr [ypos], 1 # end line
        mov byte ptr [xpos], 0
        ret

cprint:
        mov ah, attrib # set global attrib
        mov cx, ax # stash (char,attrib) word
        movzx ax, byte ptr [ypos] # mem offset due to ypos
        mov dx, 160
        mul dx
        movzx bx, byte ptr [xpos] # mem offset due to xpos
        shl bx, 1
        mov di, 0 # final mem loc
        add di, ax
        add di, bx

        mov ax, cx # pop (char,attrib) word
        stosw # write ax (char,attrib) to text video memory (es:di)
        add byte ptr [xpos], 1
        ret

printreg16:
        lea di, outstr16val
        mov ax, word ptr [reg16]
        lea si, hexstr
        mov cx, 4       # 4 hex places
hexloop:
        rol ax, 4
        mov bx, ax
        and bx, 0x0f
        mov bl, byte ptr [si + bx]
        mov byte ptr [di], bl
        inc di
        dec cx
        jnz hexloop
        lea si, outstr16
        call sprint
        ret

xpos:
        .byte 0
ypos:
        .byte 0
attrib:
        .byte 0x07 # default: gray on black
hexstr:
        .ascii "0123456789abcdef"
reg16:
        .word 0
outstr16:
        .ascii "reg16 = 0x"
outstr16val:
        .ascii "XXXX\0"
