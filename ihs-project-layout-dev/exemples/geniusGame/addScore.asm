SECTION .text
global addScore
addScore:
    enter 0, 0
    mov edx, [ebp+8]
    add [edx], 1
    leave
    ret