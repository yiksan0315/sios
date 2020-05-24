all:Boot.img

Boot.img: Source/Bootstrap.asm
	nasm -f bin -o $@ $^

clean:
	rm -f Boot.img


