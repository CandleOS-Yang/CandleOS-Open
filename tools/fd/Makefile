vhd := ./image/CandleOS-X.vhd

fd_dirs := ./main
fd_c := $(foreach dir,$(fd_dirs),$(wildcard $(dir)/*.c))
fd_o := $(patsubst %.c,./%.o,$(notdir $(fd_c)))
		
fd_out := ./fd.out
fd_inc := ./include/

infile := ./file/test.txt

clean:
	rm -rf $(vhd)
	rm -rf $(fd_o)
	rm -rf $(fd_out)

make_vhd:
	qemu-img create -f vpc -o subformat=fixed "$(vhd)" 32M

%.o: $(fd_dirs)/%.c
	gcc -c -I$(fd_inc) $< -o $@

make_fd: $(fd_o)
	gcc $^ -o $(fd_out)

write_mbr:
	$(fd_out) ./file/boot.bin -o $(vhd) -c -w 0
	$(fd_out) ./file/loader.bin -o $(vhd) -c -w 1

add_file:
	$(fd_out) $(infile) -o $(vhd) -f

build: clean make_vhd make_fd write_mbr