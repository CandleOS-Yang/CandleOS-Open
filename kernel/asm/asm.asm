[section .text]
[bits 32]

global get_eip
global jmp_to_high_kernel

; 获取eip值
get_eip:
    pop eax
    jmp eax

; 跳转高地址内核代码
jmp_to_high_kernel:
    pop eax
    add eax,0xC0000000
    push eax
    ret