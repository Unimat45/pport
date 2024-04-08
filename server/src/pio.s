section .data
    PORT equ 0x378

section .text
    global io_init, io_write
    extern ioperm
    
io_init:
    mov rdx, 1
    mov rsi, 1
    mov rdi, PORT
    call ioperm
    ret

io_write:
    mov al, sil
    out PORT, al
    xor rax, rax
    ret
