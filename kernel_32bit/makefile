NASM=nasm
CC=x86_64-linux-gnu-gcc -c -m32 -ffreestanding
LD=x86_64-linux-gnu-ld -melf_i386 -e main -Ttext 0x10200 -nostdlib
OBJCOPY=x86_64-linux-gnu-objcopy -j .text -j .data -j .rodata -j .bss -S -O binary
RESIZEIMAGE=../Utility/ResizeImage.out

TEMPDIR=Temp
SOURCEDIR=Source

all:$(TEMPDIR)/GDT.img kernel_32.img 

kernel_32.img:$(TEMPDIR)/kernel_32.bin
	$(OBJCOPY) $^ $(TEMPDIR)/Tempimg.img
	cat $(TEMPDIR)/GDT.img $(TEMPDIR)/Tempimg.img> $@
	$(RESIZEIMAGE) $@
	cat $(TEMPDIR)/ResizeImage.img >> $@

$(TEMPDIR)/kernel_32.bin:$(TEMPDIR)/kernel.o $(TEMPDIR)/IO.o $(TEMPDIR)/paging.o $(TEMPDIR)/Entry_64bit.o 
	$(LD) -o $@ $^

$(TEMPDIR)/paging.o:$(SOURCEDIR)/paging.c
	$(CC) -o $@ $^

$(TEMPDIR)/kernel.o:$(SOURCEDIR)/kernel.c
	$(CC) -o $@ $^

$(TEMPDIR)/IO.o:$(SOURCEDIR)/IO.c
	$(CC) -o $@ $^

$(TEMPDIR)/Entry_64bit.o:$(SOURCEDIR)/Entry_64bit.asm
	$(NASM) -f elf -o $@ $^

$(TEMPDIR)/GDT.img:$(SOURCEDIR)/GDT.asm
	$(NASM) -f bin -o $@ $^

clean:
	rm -f *.img
	rm -f $(TEMPDIR)/*.*


