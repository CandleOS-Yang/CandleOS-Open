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

    mov ecx,1   ; 读取loader
    mov edi,0x1000
    mov bl,4
    call read_disk_pio

    cmp word [0x103e],0xCDE1
    jne _Error

    jmp 0:0x1000    ; 跳转到loader

_Error:
    hlt
    jmp $

read_disk_pio:
    ; edi=读取的目标内存
    ; ecx=起始扇区
    ; bl=读取的扇区数
    mov dx,0x1f2    ;设置读写扇区的数量
    mov al,bl
    out dx,al
    
    inc dx
    mov al,cl   ;起始地址的低8位
    out dx,al
    
    inc dx
    shr ecx,8
    mov al,cl   ;起始地址的中8位
    out dx,al
    
    inc dx
    shr ecx,8
    mov al,cl   ;起始地址的高8位
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
        db 0x80                 ; 活动分区
        db 0x00, 0x01, 0x01     ; 起始CHS(柱面0, 磁头0, 扇区1)
        db 0x0E                 ; 分区类型：FAT16 LBA
        db 0xFE, 0xFF, 0xFF     ; 终止CHS(LBA模式)
        dd 0x00000001           ; 起始LBA(扇区1)
        dd 0x0000FFEF           ; 总扇区数=65551 (32MB)
    .PartOther:
        times 16 * 3 db 0       ; 剩余分区表项置零

times 510 - ($ - $$) db 0
dw 0xAA55