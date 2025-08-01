[org 0x7c00]

jmp short _BootStart
nop

_BootStart:
    mov ax,0
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov sp,0x7c00

    mov ax,0x0003
    int 0x10

    mov ecx,1   ; ��ȡloader
    mov edi,0x1000
    mov bl,4
    call read_disk_pio

    cmp word [0x103e],0xCDE1
    jne _Error

    jmp 0:0x1000    ; ��ת��loader

_Error:
    hlt
    jmp $

read_disk_pio:
    ; edi=��ȡ��Ŀ���ڴ�
    ; ecx=��ʼ����
    ; bl=��ȡ��������
    mov dx,0x1f2    ;���ö�д����������
    mov al,bl
    out dx,al
    
    inc dx
    mov al,cl   ;��ʼ��ַ�ĵ�8λ
    out dx,al
    
    inc dx
    shr ecx,8
    mov al,cl   ;��ʼ��ַ����8λ
    out dx,al
    
    inc dx
    shr ecx,8
    mov al,cl   ;��ʼ��ַ�ĸ�8λ
    out dx,al
    
    inc dx
    shr ecx,8
    and cl, 0b1111
    
    mov al,0b1110_0000
    or al,cl
    out dx,al
    
    inc dx
    mov al,0x20
    out dx,al
    
    xor ecx,ecx
    mov cl,bl
    .loops:
        push cx
        call .waits
        call .reads
        pop cx
        loop .loops
        ret
    
    .waits:
        mov dx,0x1f7
        .check:
            in al,dx
            and al,0b1000_1000
            cmp al,0b0000_1000
            jnz .check
            ret
    
    .reads:
        mov dx,0x1f0
        mov cx,256
        .readword:
            in ax,dx
            mov word [edi],ax
            add edi,2
            loop .readword
            ret

times 446 - ($ - $$) db 0
_PartitionTable:
    .Part1Main:
        db 0x80                 ; �����
        db 0x00, 0x01, 0x01     ; ��ʼCHS(����0, ��ͷ0, ����1)
        db 0x0E                 ; �������ͣ�FAT16 LBA
        db 0xFE, 0xFF, 0xFF     ; ��ֹCHS(LBAģʽ)
        dd 0x00000001           ; ��ʼLBA(����1)
        dd 0x0000FFEF           ; ��������=65551 (32MB)
    .PartOther:
        times 16 * 3 db 0       ; ʣ�������������

times 510 - ($ - $$) db 0
dw 0xAA55