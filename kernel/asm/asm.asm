[section .text]
[bits 32]

global get_eip
global jmp_to_high_kernel

; ��ȡeipֵ
get_eip:
    pop eax
    jmp eax

; ��ת�ߵ�ַ�ں˴���
jmp_to_high_kernel:
    pop eax
    add eax,0xC0000000
    push eax
    ret