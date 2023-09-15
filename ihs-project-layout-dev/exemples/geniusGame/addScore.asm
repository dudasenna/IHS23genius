SECTION .text
global addScore
addScore:
    enter 0, 0
    mov rax, rdi
    add dword[rax], 1
    leave
    ret
