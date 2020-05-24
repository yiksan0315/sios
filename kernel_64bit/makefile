NASM=nasm -f elf64
CC=x86_64-linux-gnu-gcc -c -m64 -ffreestanding -masm=intel
LD=x86_64-linux-gnu-ld -melf_x86_64 -e main -Ttext 0x200000 -nostdlib -T linker.x
OBJCOPY=x86_64-linux-gnu-objcopy -j .text -j .data -j .rodata -j .bss -S -O binary
RESIZEIMAGE=../Utility/ResizeImage.out

TEMPDIR=Temp
SOURCEDIR=Source

ENTRYOBJECT=Switch_IA_32e.o
COBJECT=$(notdir $(patsubst %.c,%.o,$(wildcard $(SOURCEDIR)/*.c)))
ASMOBJECT=$(notdir $(patsubst %.asm,%.o,$(wildcard $(SOURCEDIR)/*.asm)))

all:kernel_64.img clean_Temp

clean_Temp:
	mv *.o $(TEMPDIR)
#	rm -f *.o

kernel_64.img:$(TEMPDIR)/kernel_64.bin
	$(OBJCOPY) $^ $@
	$(RESIZEIMAGE) $@
	cat $(TEMPDIR)/ResizeImage.img >> $@

$(TEMPDIR)/kernel_64.bin:$(ENTRYOBJECT) $(COBJECT) $(ASMOBJECT)
	$(LD) -o $@ $^

$(ENTRYOBJECT):$(SOURCEDIR)/Switch_IA_32e.s
	$(NASM) -o $@ $^

%.o:$(SOURCEDIR)/%.c
	$(CC) $<

%.o:$(SOURCEDIR)/%.asm
	$(NASM) -o $@ $<

clean:
	rm -f *.img
	rm -f *.o
	rm -f $(TEMPDIR)/*.*


