[org 0x1000]

jmp short _LoaderStart
nop

_BiosParameterBlock:
    BS_OEMName      db "COS-X   "   ; OEM����
    BPB_BytesPerSec dw 0x0200       ; ÿ���������ֽ���
    BPB_SecPerClus  db 0x01         ; ÿ����������
    BPB_RsvdSecCnt  dw 0x0004       ; ����������
    BPB_NumFATs     db 0x02         ; FAT�����
    BPB_RootEntCnt  dw 0x0200       ; ��Ŀ¼����
    BPB_TotSec16    dw 0xffef       ; 16λ��������
    BPB_Media       db 0xf8         ; ý��������
    BPB_FATSz16     dw 0x0200       ; FAT��������
    BPB_SecPerTrk   dw 0x003f       ; ÿ�ŵ�������
    BPB_NumHeads    dw 0x00ff       ; ��ͷ��
    BPB_HiddSec     dd 0x00000001   ; ����������
    BPB_TotSec32    dd 0x00000000   ; 32λ��������
    BS_DrvNum       db 0x80         ; ��������
    BS_Reserved1    db 0x00         ; ����
    BS_BootSig      db 0x29         ; ������־
    BS_VolID    dd 0x00000000       ; �����к�
    BS_VolLab       db "CandleOS-X "; ���
    BS_FileSysType  db "FAT16   "   ; �ļ�ϵͳ����

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
    ; ����ƺ���_GetBiggestResolution�г�ͻ

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

[bits 16]
_Error:
    mov si,ERROR_MSG
    mov bl,0x0c
    mov dx,0x0100
    call print

    hlt
    jmp $

print:
    ; si=�ַ���
    ; bl=����
    ; dx=λ��(dh=��,dl=��)
    push es
    mov ax,0xb800
    mov es,ax
    xor eax,eax
    
    ; ��������
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
    ; dx=λ��(dh=��,dl=��)
    mov ah,0x02
    mov bh,0x00
    int 0x10
    ret

ArdsCount: dd 0                                     ; ARDS������
LastModeBase: dw 0                                  ; ���һ��ģʽ����Ϣ����ַ
MaxResolutionModeBase: dw 0                         ; ���ֱ���ģʽ
MaxResolution: dd 0                                 ; ���d�ֱ���
Fat16FAT1Start: dw 0                                ; FAT1��ʼ������
Fat16RootDirStart: dw 0                             ; ��Ŀ¼��ʼ������
Fat16DataStart: dw 0                                ; ��������ʼ������

KERNEL_FAT16_NAME: db 'KERNEL  BIN'                 ; �ں��ļ���
KERNEL_FAT16_NAME_LEN equ 11                        ; �ں��ļ�������
FAT16_DIR_FSTCLUS_OFFSET equ 26                     ; Ŀ¼��غ�ƫ��
KERNEL_BASE equ 0x100000                            ; �ں˼��ص�ַ

CODE_SELECTOR equ (1 << 3)                          ; �����ѡ����
DATA_SELECTOR equ (2 << 3)                          ; ���ݶ�ѡ����

LOADER_SECTOR_NUM equ 4                             ; Loader������
VBE_VERSION_2 equ 0x0200                            ; VBE�汾��2.0
ERROR_MSG db "Error...",0                           ; ������Ϣ
MEM_SAVED_MSG db "Memory Info Saved...",0           ; �ڴ���Ϣ������Ϣ
VBE_SAVED_MSG db "VBE Info Saved...",0              ; VBE��Ϣ������Ϣ

VBE_INFO_BLOCK equ 0x9000                           ; VBE��Ϣ��
MODE_INFO_BLOCK equ 0x9100                          ; ģʽ��Ϣ��
VBE_MODE_LIST equ 0x9200                            ; ģʽ�ż�¼��
MEMORY_INFO_BLOCK equ 0xA000                        ; �ڴ���Ϣ��
FILE_SYSTEM_INFO_BLOCK equ 0xA100                   ; �ļ�ϵͳ��Ϣ��
VBE_MODE_INFO equ 0xA200                            ; VBEģʽ��Ϣ

VBE_MODE_MODE equ VBE_MODE_INFO + 0x00              ; ģʽ��
VBE_MODE_VERSION equ VBE_MODE_INFO + 0x02           ; VBE�汾��
VBE_MODE_BPP equ VBE_MODE_INFO + 0x04               ; ÿ����ռ��λ��
VBE_MODE_XRESOLUTION equ VBE_MODE_INFO + 0x06       ; ˮƽ�ֱ���
VBE_MODE_YRESOLUTION equ VBE_MODE_INFO + 0x08       ; ��ֱ�ֱ���
VBE_MODE_FRAMEBUFFER equ VBE_MODE_INFO + 0x0a       ; ֡����������ַ

_PartitionTable:
    .Part1Main:
        db 0x80                                     ; �����
        db 0x00, 0x01, 0x01                         ; ��ʼCHS(����0, ��ͷ0, ����1)
        db 0x0E                                     ; �������ͣ�FAT16 LBA
        db 0xFE, 0xFF, 0xFF                         ; ��ֹCHS(LBAģʽ)
        dd 0x00000001                               ; ��ʼLBA(����1)
        dd 0x0000FFEF                               ; ��������=65551 (32MB)
    .PartOther:
        times 16 * 3 db 0                           ; ʣ�������������

gdt_ptr:
    dw (gdt_end - gdt_base) - 1 
    dd gdt_base

gdt_base:
    dd 0x0                                          ; �λ�ַ
    dd 0x0                                          ; �ν���
gdt_code:
    dw 0xffff                                       ; �ν���(0~15λ)
    dw 0x0000                                       ; �λ�ַ(0~15λ)
    db 0x00                                         ; �λ�ַ(16~23λ)
    db 0x9a                                         ; ������
    db 0xc0 | 0xf                                   ; �ν���(16~19λ)��������
    db 0x00                                         ; �λ�ַ(24~31λ)
gdt_data:
    dw 0xffff                                       ; �ν���(0~15λ)
    dw 0x0000                                       ; �λ�ַ(0~15λ)
    db 0x00                                         ; �λ�ַ(16~23λ)
    db 0x92                                         ; ������
    db 0xc0 | 0xf                                   ; �ν���(16~19λ)��������
    db 0x00                                         ; �λ�ַ(24~31λ)
gdt_end:

times (LOADER_SECTOR_NUM * 512 - 2) - ($ - $$) db 0
dw 0xCDE1