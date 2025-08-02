# 目录
boot := ./boot
loader := ./loader
build := ./build
kernel := ./kernel
include := ./include
init := $(kernel)/init
lib := $(kernel)/lib
drv := $(kernel)/devices
debug := $(kernel)/debug
fonts := $(kernel)/fonts

kernel_dirs := $(init) $(lib) $(debug)
drv_dirs := $(drv)/vbe

# 工具
fd := ./tools/fd
qimg := qemu-img

# 编译选项
cflags := -m32
cflags += -fno-builtin
cflags += -nostdinc
cflags += -fno-pic
cflags += -fno-pie
cflags += -nostdlib
cflags += -fno-stack-protector
cflags += -g
cflags += -msse2
# cflags += -O3
cflags += -masm=intel
cflags += -I$(include)

kernel_start := 0x10000

# 目标
boot_bin := $(build)/boot.bin
loader_bin := $(build)/loader.bin
kernel_elf := $(build)/kernel_elf.bin
kernel_bin := $(build)/kernel.bin

kernel_c := $(foreach dir,$(kernel_dirs),$(wildcard $(dir)/*.c))
kernel_asm := $(foreach dir,$(kernel_dirs),$(wildcard $(dir)/*.asm))
drv_c := $(foreach dir,$(drv_dirs),$(wildcard $(dir)/*.c))

kernel_o := $(patsubst %.c,$(build)/%.o,$(notdir $(kernel_c))) \
    $(patsubst %.asm,$(build)/%.o,$(notdir $(kernel_asm))) \
	$(patsubst %.c,$(build)/%.o,$(notdir $(drv_c))) \
	$(fonts)/SongFont.obj

fd_o := $(fd)/fd.o
fd_out := $(fd)/fd.out
fd_include := $(fd)/include

vhd := ./build/image/CandleOS-X.vhd

# 规则
$(build)/%.bin: $(boot)/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f bin $< -o $@

$(build)/%.bin: $(loader)/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f bin $< -o $@

$(build)/%.o: $(init)/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(cflags) -c $< -o $@

$(build)/%.o: $(debug)/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(cflags) -c $< -o $@

$(build)/%.o: $(lib)/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(cflags) -c $< -o $@

$(build)/%.o: $(drv)/vbe/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(cflags) -c $< -o $@

$(fd)/%.o: $(fd)/main/%.c
	$(shell mkdir -p $(dir $@))
	gcc -I$(fd_include) -c $< -o $@

$(kernel_elf): $(kernel_o)
	ld -m elf_i386 $^ -o $@ -Ttext $(kernel_start) -e kernel_main

$(kernel_bin): $(kernel_elf)
	objcopy -O binary $^ $@

$(fd_out): $(fd_o)
	gcc $^ -o $@

# 清理
clean:
	rm -rf $(build)

# 构建
make_vhd:
	$(shell mkdir -p $(dir $(vhd)))
	qemu-img create -f vpc -o subformat=fixed "$(vhd)" 32M

write_vhd: $(boot_bin) $(loader_bin) $(kernel_bin)
	$(fd_out) $(build)/boot.bin -o $(vhd) -c -w 0
	$(fd_out) $(build)/loader.bin -o $(vhd) -c -w 1
	$(fd_out) $(kernel_bin) -o $(vhd) -c -wfs -p 0 -d 0

build: clean $(fd_out) make_vhd write_vhd

run:
	qemu-system-i386 -m 128M -audiodev sdl,id=audio0 -device sb16,audiodev=audio0 -hda $(vhd)

run_g:
	qemu-system-i386 -m 128M -audiodev sdl,id=audio0 -device sb16,audiodev=audio0 -hda $(vhd) -s -S

run_b:
	bochs -f ./bochs/bochsrc -q -unlock