Boot=Bootstrap
Ker_32=kernel_32bit
Ker_64=kernel_64bit

all:Boot Ker_32 Ker_64 Sios.img

Boot:
	make -C Bootstrap
Ker_32:
	make -C kernel_32bit
Ker_64:
	make -C kernel_64bit

Sios.img: $(Boot)/Boot.img $(Ker_32)/kernel_32.img $(Ker_64)/kernel_64.img 
	cat $^ > $@

clean:
	make -C $(Boot) clean
	make -C $(Ker_32) clean
	make -C $(Ker_64) clean
	rm -f Sios.img
