section .data
    buf db 0

section .text
    global io_init, io_write
    
io_init:
    mov rax, 0
    ret

io_write:
    mov byte [buf], cl
    xor rax, rax
    ret
