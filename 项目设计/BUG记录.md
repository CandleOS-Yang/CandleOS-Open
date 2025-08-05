# VM输出错误BUG

```asm
; loader.asm
    .IsBpp32:
        cmp byte [es:di + 25],32
        je .IsRes
        je .SaveModeInfo
        jmp .GetNextMode

    .IsRes:
        cmp word [es:di + 18],1024
        cmp word [es:di + 20],768
        je .SaveModeInfo
        jmp .GetNextMode
    ; 这个似乎和_GetBiggestResolution有冲突
```