[org 0x1000]

jmp short _LoaderStart
nop

_BiosParameterBlock:
    BS_OEMName      db "COS-X   "   ; OEM名称
    BPB_BytesPerSec dw 0x0200       ; 每个扇区的字节数
    BPB_SecPerClus  db 0x01         ; 每个簇扇区数
    BPB_RsvdSecCnt  dw 0x0004       ; 保留扇区数
    BPB_NumFATs     db 0x02         ; FAT表份数
    BPB_RootEntCnt  dw 0x0200       ; 根目录项数
    BPB_TotSec16    dw 0xffef       ; 16位总扇区数
    BPB_Media       db 0xf8         ; 媒体描述符
    BPB_FATSz16     dw 0x0200       ; FAT表扇区数
    BPB_SecPerTrk   dw 0x003f       ; 每磁道扇区数
    BPB_NumHeads    dw 0x00ff       ; 磁头数
    BPB_HiddSec     dd 0x00000001   ; 隐藏扇区数
    BPB_TotSec32    dd 0x00000000   ; 32位总扇区数
    BS_DrvNum       db 0x80         ; 驱动器号
    BS_Reserved1    db 0x00         ; 保留
    BS_BootSig      db 0x29         ; 引导标志
    BS_VolID    dd 0x00000000       ; 卷序列号
    BS_VolLab       db "CandleOS-X "; 卷标
    BS_FileSysType  db "FAT16   "   ; 文件系统类型

dw 0xCDE1

_LoaderStart:
    mov ax,0
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov fs,ax
    mov gs,ax
    mov sp,0x7c00

_MemoryInfo:
    mov edi,MEMORY_INFO_BLOCK + 4

    xor ebx,ebx
    mov edx,0x534d4150
    .Loops:
        mov eax,0xe820
        mov ecx,20
        int 0x15
        jc _Error

        add di,cx
        inc dword [ArdsCount]
        cmp ebx,0
        jnz .Loops
    
    .Save:
        mov si,MEM_SAVED_MSG
        mov bl,0x0e
        mov dx,0x0000
        call print

        mov eax,[ArdsCount]
        mov [es:MEMORY_INFO_BLOCK],eax

_GetVbeModes:
    mov bx,VBE_MODE_LIST

    mov di,VBE_INFO_BLOCK
    mov ax,0x4f00
    int 0x10

    cmp ax,0x004f
    jne _Error

    cmp word [es:di + 0x04],VBE_VERSION_2
    jb _Error
    mov ax,[es:di + 0x04]
    mov [VBE_MODE_VERSION],ax

    mov gs,[es:di + 0x0e + 2]
    mov si,[es:di + 0x0e]
    mov cx,[gs:si]
    mov di,MODE_INFO_BLOCK
    .GetAllModes:
        mov ax,0x4f01
        int 0x10

        cmp ax,0x004f
        jne .GetNextMode

    .IsBpp32:
        cmp byte [es:di + 25],32
        ; je .IsRes
        je .SaveModeInfo
        jmp .GetNextMode

    ; .IsRes:
    ;     cmp word [es:di + 18],1024
    ;     cmp word [es:di + 20],768
    ;     je .SaveModeInfo
    ;     jmp .GetNextMode
    ; 这个似乎和_GetBiggestResolution有冲突

    .SaveModeInfo:
        xor eax,eax
        mov dx,[gs:si]
        mov [fs:bx],dx
            
        mov al,[es:di + 25]
        mov [fs:bx + 2],al

        mov ax,[es:di + 18]
        mov [fs:bx + 3],ax

        mov ax,[es:di + 20]
        mov [fs:bx + 5],ax

        mov eax,[es:di + 40]
        mov [fs:bx + 7],eax

        add bx,11

    .GetNextMode:
        add si,2
        mov cx,[gs:si]
        cmp cx,0xffff
        jne .GetAllModes

    .Done:
        sub bx,11
        mov [LastModeBase],bx

        mov si,VBE_SAVED_MSG
        mov bl,0x0e
        mov dx,0x0100
        call print

_GetBiggestResolution:
    mov bx,VBE_MODE_LIST
    mov ax,0
    mov fs,ax
    xor cx,cx
    
    .Loops:
        mov ax,[fs:bx + 3]
        mov dx,[fs:bx + 5]
        mul dx
        
        cmp dx,cx
        jb .NextMode
        ja .UpdateMaxResolution
        cmp ax,[MaxResolution]
        jbe .NextMode

    .UpdateMaxResolution:
        mov [MaxResolution],ax
        mov cx,dx
        mov [MaxResolutionModeBase],bx

    .NextMode:
        add bx,11
        cmp bx,[LastModeBase]
        jbe .Loops

_SetVbeResolution:
    mov ax,0
    mov es,ax
    mov di,[MaxResolutionModeBase]

    xor eax,eax
    mov al,[es:di + 2]
    mov [VBE_MODE_BPP],al
    mov ax,[es:di]
    mov [VBE_MODE_MODE],ax
    mov ax,[es:di + 3]
    mov [VBE_MODE_XRESOLUTION],ax
    mov ax,[es:di + 5]
    mov [VBE_MODE_YRESOLUTION],ax
    mov eax,[es:di + 7]
    mov [VBE_MODE_FRAMEBUFFER],eax

    mov ax,0x4f02
    mov bx,[es:di]
    add bx,0x4000
    int 0x10
    
    cmp ax,0x004f
    jne _Error

_PrepareProtectedMode:
    cli

    in al,0x92
    or al,0000_0010b
    out 0x92,al

    lgdt [gdt_ptr]

    mov eax,cr0
    or eax,0x00000001
    mov cr0,eax

    jmp dword CODE_SELECTOR:_ProtectedMode
    jmp $

times 510 - ($ - $$) db 0
dw 0xAA55

[bits 32]
_ProtectedMode:
    mov ax,DATA_SELECTOR
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov fs,ax
    mov gs,ax
    mov esp,0x10000

_CopyBiosParameterBlock:
    mov ecx,59
    mov edi,_BiosParameterBlock
    mov esi,FILE_SYSTEM_INFO_BLOCK
    .Loops:
        mov al,[edi]
        mov [esi],al

        inc edi
        inc esi
        loop .Loops

_GetFat16SectorInfo:
    mov ax,[BPB_RsvdSecCnt]
    add ax,[_PartitionTable.Part1Main + 8]
    mov [Fat16FAT1Start],ax

    mov ax,[BPB_NumFATs]
    mov bx,[BPB_FATSz16]
    mul bx
    add ax,[Fat16FAT1Start]
    mov [Fat16RootDirStart],ax

    mov ax,[BPB_RootEntCnt]
    mov bx,32
    mul bx
    mov bx,512
    div bx
    add ax,[Fat16RootDirStart]
    mov [Fat16DataStart],ax

_LoadKernelFile:
    mov cx,[Fat16RootDirStart]
    mov edi,0x7e00
    mov bl,1
    call read_disk_pio

    xor esi,esi
    mov si,0x7e00
    mov di,KERNEL_FAT16_NAME
    mov cx,KERNEL_FAT16_NAME_LEN
    mov dx,0
    .CheckName:
        push dx
        shl dx,5
        add si,dx
        repe cmpsb
        jcxz .FoundFile
        pop dx
        inc dx
        cmp dx,32
        jb .CheckName
    .end:
        jmp $

    .FoundFile:
        mov si,0x7e00
        add si,FAT16_DIR_FSTCLUS_OFFSET
        xor edx,edx
        mov dx,[si]
        push dx

        mov cx,[Fat16FAT1Start]
        mov edi,0x7e00
        mov bl,1
        call read_disk_pio

        mov esi,KERNEL_BASE
        mov di,0x7e00
        pop dx
        mov ax,dx
        shl ax,1
        add di,ax
        .ReadFile:
            sub dx,2
            add dx,[Fat16DataStart]

            push edi
            mov ecx,edx
            mov edi,esi
            mov ebx,1
            call read_disk_pio
            pop edi
            
            mov dx,[di]

            add esi,512
            add di,2
            cmp dx,0xfff8
            jb .ReadFile

_JmpToKernel:
    jmp dword CODE_SELECTOR:KERNEL_BASE
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

[bits 16]
_Error:
    mov si,ERROR_MSG
    mov bl,0x0c
    mov dx,0x0100
    call print

    hlt
    jmp $

print:
    ; si=字符串
    ; bl=属性
    ; dx=位置(dh=行,dl=列)
    push es
    mov ax,0xb800
    mov es,ax
    xor eax,eax
    
    ; 处理行列
    mov al,160  
    mul dh
    mov di,ax
    
    .loops:
        mov al,[si]
        cmp al,0
        je .end
    
        mov byte [es:di],al
        mov byte [es:di + 1],bl
        inc si
        add di,2 
        jmp .loops
    .end:
        pop es
        ret
    
set_cur:
    ; dx=位置(dh=行,dl=列)
    mov ah,0x02
    mov bh,0x00
    int 0x10
    ret

ArdsCount: dd 0                                     ; ARDS表数量
LastModeBase: dw 0                                  ; 最后一个模式号信息基地址
MaxResolutionModeBase: dw 0                         ; 最大分辨率模式
MaxResolution: dd 0                                 ; 最大d分辨率
Fat16FAT1Start: dw 0                                ; FAT1起始扇区号
Fat16RootDirStart: dw 0                             ; 根目录起始扇区号
Fat16DataStart: dw 0                                ; 数据区起始扇区号

KERNEL_FAT16_NAME: db 'KERNEL  BIN'                 ; 内核文件名
KERNEL_FAT16_NAME_LEN equ 11                        ; 内核文件名长度
FAT16_DIR_FSTCLUS_OFFSET equ 26                     ; 目录项簇号偏移
KERNEL_BASE equ 0x100000                            ; 内核加载地址

CODE_SELECTOR equ (1 << 3)                          ; 代码段选择子
DATA_SELECTOR equ (2 << 3)                          ; 数据段选择子

LOADER_SECTOR_NUM equ 4                             ; Loader扇区数
VBE_VERSION_2 equ 0x0200                            ; VBE版本号2.0
ERROR_MSG db "Error...",0                           ; 错误信息
MEM_SAVED_MSG db "Memory Info Saved...",0           ; 内存信息保存信息
VBE_SAVED_MSG db "VBE Info Saved...",0              ; VBE信息保存信息

VBE_INFO_BLOCK equ 0x9000                           ; VBE信息块
MODE_INFO_BLOCK equ 0x9100                          ; 模式信息块
VBE_MODE_LIST equ 0x9200                            ; 模式号记录表
MEMORY_INFO_BLOCK equ 0xA000                        ; 内存信息块
FILE_SYSTEM_INFO_BLOCK equ 0xA100                   ; 文件系统信息块
VBE_MODE_INFO equ 0xA200                            ; VBE模式信息

VBE_MODE_MODE equ VBE_MODE_INFO + 0x00              ; 模式号
VBE_MODE_VERSION equ VBE_MODE_INFO + 0x02           ; VBE版本号
VBE_MODE_BPP equ VBE_MODE_INFO + 0x04               ; 每像素占用位宽
VBE_MODE_XRESOLUTION equ VBE_MODE_INFO + 0x06       ; 水平分辨率
VBE_MODE_YRESOLUTION equ VBE_MODE_INFO + 0x08       ; 垂直分辨率
VBE_MODE_FRAMEBUFFER equ VBE_MODE_INFO + 0x0a       ; 帧缓冲区基地址

_PartitionTable:
    .Part1Main:
        db 0x80                                     ; 活动分区
        db 0x00, 0x01, 0x01                         ; 起始CHS(柱面0, 磁头0, 扇区1)
        db 0x0E                                     ; 分区类型：FAT16 LBA
        db 0xFE, 0xFF, 0xFF                         ; 终止CHS(LBA模式)
        dd 0x00000001                               ; 起始LBA(扇区1)
        dd 0x0000FFEF                               ; 总扇区数=65551 (32MB)
    .PartOther:
        times 16 * 3 db 0                           ; 剩余分区表项置零

gdt_ptr:
    dw (gdt_end - gdt_base) - 1 
    dd gdt_base

gdt_base:
    dd 0x0                                          ; 段基址
    dd 0x0                                          ; 段界限
gdt_code:
    dw 0xffff                                       ; 段界限(0~15位)
    dw 0x0000                                       ; 段基址(0~15位)
    db 0x00                                         ; 段基址(16~23位)
    db 0x9a                                         ; 段属性
    db 0xc0 | 0xf                                   ; 段界限(16~19位)、段属性
    db 0x00                                         ; 段基址(24~31位)
gdt_data:
    dw 0xffff                                       ; 段界限(0~15位)
    dw 0x0000                                       ; 段基址(0~15位)
    db 0x00                                         ; 段基址(16~23位)
    db 0x92                                         ; 段属性
    db 0xc0 | 0xf                                   ; 段界限(16~19位)、段属性
    db 0x00                                         ; 段基址(24~31位)
gdt_end:

times (LOADER_SECTOR_NUM * 512 - 2) - ($ - $$) db 0
dw 0xCDE1