[bits 32]
[section .text]

extern isr_handler_table

global int_entry
global int_exit

%macro ISR_ENTRY 2
global isr_entry_%1

isr_entry_%1:
%ifn %2
    push 0xCDEF1234                 ; 压入CandleOS-X中断魔数
%endif
    push %1                         ; 压入中断号
    jmp int_entry
%endmacro

int_entry:
    push ds
    push es
    push fs
    push gs
    pushad
    
    mov eax,[esp + 12 * 4]
    push esp
    push eax
    call [isr_handler_table + eax * 4]
int_exit:
    add esp,8

    popad
    pop gs
    pop fs
    pop es
    pop ds

    add esp,8                       ; 弹出CandleOS-X中断魔数
    iretd

ISR_ENTRY 0x00,0          ; 除法
ISR_ENTRY 0x01,0          ; 保留
ISR_ENTRY 0x02,0          ; #NMI
ISR_ENTRY 0x03,0          ; 断点
ISR_ENTRY 0x04,0          ; 溢出
ISR_ENTRY 0x05,0          ; 界限错误
ISR_ENTRY 0x06,0          ; 无效操作码
ISR_ENTRY 0x07,0          ; 设备不可用(没有数学协处理器)
ISR_ENTRY 0x08,1          ; 双重故障
ISR_ENTRY 0x09,0          ; 协处理器段溢出
ISR_ENTRY 0x0a,1          ; 无效TSS
ISR_ENTRY 0x0b,1          ; 段不存在
ISR_ENTRY 0x0c,1          ; 栈段溢出
ISR_ENTRY 0x0d,1          ; 常规保护
ISR_ENTRY 0x0e,1          ; 页错误
ISR_ENTRY 0x0f,0          ; 保留
ISR_ENTRY 0x10,0          ; x87 FPU 浮点处理错误
ISR_ENTRY 0x11,1          ; 对齐检查
ISR_ENTRY 0x12,0          ; 机器检查
ISR_ENTRY 0x13,0          ; SIMD 浮点处理错误

ISR_ENTRY 0x14,0          ; 保留
ISR_ENTRY 0x15,0          ; 保留
ISR_ENTRY 0x16,0          ; 保留
ISR_ENTRY 0x17,0          ; 保留
ISR_ENTRY 0x18,0          ; 保留
ISR_ENTRY 0x19,0          ; 保留
ISR_ENTRY 0x1a,0          ; 保留
ISR_ENTRY 0x1b,0          ; 保留
ISR_ENTRY 0x1c,0          ; 保留
ISR_ENTRY 0x1d,0          ; 保留
ISR_ENTRY 0x1e,0          ; 保留
ISR_ENTRY 0x1f,0          ; 保留

ISR_ENTRY 0x20,0          ; 时钟
ISR_ENTRY 0x21,0          ; 键盘
ISR_ENTRY 0x22,0          ; 连接从片
ISR_ENTRY 0x23,0          ; 串口2
ISR_ENTRY 0x24,0          ; 串口1
ISR_ENTRY 0x25,0          ; 并口2
ISR_ENTRY 0x26,0          ; 软盘
ISR_ENTRY 0x27,0          ; 并口1
ISR_ENTRY 0x28,0          ; 实时时钟
ISR_ENTRY 0x29,0          ; 重定向的IRQ2
ISR_ENTRY 0x2a,0          ; 保留
ISR_ENTRY 0x2b,0          ; 保留
ISR_ENTRY 0x2c,0          ; PS/2 鼠标
ISR_ENTRY 0x2d,0          ; FPU 异常
ISR_ENTRY 0x2e,0          ; 硬盘
ISR_ENTRY 0x2f,0          ; 保留

[section .data]
global isr_entry_table
isr_entry_table:
    dd isr_entry_0x00    ; 0x00: 除法错误
    dd isr_entry_0x01    ; 0x01: 调试异常
    dd isr_entry_0x02    ; 0x02: NMI中断
    dd isr_entry_0x03    ; 0x03: 断点
    dd isr_entry_0x04    ; 0x04: 溢出
    dd isr_entry_0x05    ; 0x05: 边界检查
    dd isr_entry_0x06    ; 0x06: 无效操作码
    dd isr_entry_0x07    ; 0x07: 设备不可用
    dd isr_entry_0x08    ; 0x08: 双重故障
    dd isr_entry_0x09    ; 0x09: 协处理器段溢出
    dd isr_entry_0x0a    ; 0x0A: 无效TSS
    dd isr_entry_0x0b    ; 0x0B: 段不存在
    dd isr_entry_0x0c    ; 0x0C: 栈段故障
    dd isr_entry_0x0d    ; 0x0D: 一般保护故障
    dd isr_entry_0x0e    ; 0x0E: 页故障
    dd isr_entry_0x0f    ; 0x0F: 保留
    dd isr_entry_0x10    ; 0x10: x87 FPU浮点异常
    dd isr_entry_0x11    ; 0x11: 对齐检查
    dd isr_entry_0x12    ; 0x12: 机器检查
    dd isr_entry_0x13    ; 0x13: SIMD浮点异常
    dd isr_entry_0x14    ; 0x14: 保留
    dd isr_entry_0x15    ; 0x15: 保留
    dd isr_entry_0x16    ; 0x16: 保留
    dd isr_entry_0x17    ; 0x17: 保留
    dd isr_entry_0x18    ; 0x18: 保留
    dd isr_entry_0x19    ; 0x19: 保留
    dd isr_entry_0x1a    ; 0x1A: 保留
    dd isr_entry_0x1b    ; 0x1B: 保留
    dd isr_entry_0x1c    ; 0x1C: 保留
    dd isr_entry_0x1d    ; 0x1D: 保留
    dd isr_entry_0x1e    ; 0x1E: 保留
    dd isr_entry_0x1f    ; 0x1F: 保留
    dd isr_entry_0x20    ; 0x20: 时钟中断
    dd isr_entry_0x21    ; 0x21: 键盘中断
    dd isr_entry_0x22    ; 0x22: 级联(从8259A)
    dd isr_entry_0x23    ; 0x23: COM2
    dd isr_entry_0x24    ; 0x24: COM1
    dd isr_entry_0x25    ; 0x25: LPT2
    dd isr_entry_0x26    ; 0x26: 软盘
    dd isr_entry_0x27    ; 0x27: LPT1
    dd isr_entry_0x28    ; 0x28: 实时时钟
    dd isr_entry_0x29    ; 0x29: 重定向的IRQ2
    dd isr_entry_0x2a    ; 0x2A: 保留
    dd isr_entry_0x2b    ; 0x2B: 保留
    dd isr_entry_0x2c    ; 0x2C: PS/2鼠标
    dd isr_entry_0x2d    ; 0x2D: FPU异常
    dd isr_entry_0x2e    ; 0x2E: 硬盘
    dd isr_entry_0x2f    ; 0x2F: 保留
    ; dd syscall_handler   ; 0x30: 系统调用