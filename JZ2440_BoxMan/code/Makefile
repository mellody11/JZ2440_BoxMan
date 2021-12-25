
CC      = arm-linux-gcc
LD      = arm-linux-ld
AR      = arm-linux-ar
OBJCOPY = arm-linux-objcopy
OBJDUMP = arm-linux-objdump

INCLUDEDIR 	:= $(shell pwd)/include
CFLAGS 		:= -Wall -O2
CPPFLAGS   	:= -nostdinc -I$(INCLUDEDIR)
LDFLASG         := -L $(shell dirname `$(CC) $(CFLAGS) -print-libgcc-file-name`) -lgcc


export 	CC LD AR OBJCOPY OBJDUMP INCLUDEDIR CFLAGS CPPFLAGS

objs := head.o init.o nand.o interrupt.o serial.o lcddrv.o framebuffer.o lcdlib.o lxx.o GPIO_Key.o adc_ts.o lib/libc.a

lxx.bin: $(objs)
	${LD} -Tlxx.lds -o lxx_elf $^ ${LDFLASG}
	${OBJCOPY} -O binary -S lxx_elf $@
	${OBJDUMP} -D -m arm lxx_elf > lxx.dis

.PHONY : lib/libc.a
lib/libc.a:
	cd lib; make; cd ..
	
%.o:%.c
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o:%.S
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	make  clean -C lib
	rm -f lxx.bin lxx_elf lxx.dis *.o
	
