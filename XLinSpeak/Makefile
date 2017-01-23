.PHONY : clean all test test32 test64

all : lin32.xpl lin64.xpl

lin32.xpl : xpl.c hook.c hook.h sec.c sec.h len.c len.h utils.c utils.h hook_asm32.o
	gcc -g -shared -fvisibility=hidden -fPIC -o $@ \
	    -Wall -Wextra -m32 \
            -I SDK/CHeaders/XPLM $^ -L. -lspeechd

lin64.xpl : xpl.c hook.c hook.h sec.c sec.h len64.c len.h utils.c utils.h hook_asm64.o
	gcc -g -shared -fvisibility=hidden -fPIC -o $@ \
	    -Wall -Wextra \
            -I SDK/CHeaders/XPLM $^ -lspeechd

hook_asm32.o : hook_asm32.asm
	nasm -f elf32 -o $@ $^

hook_asm64.o : hook_asm64.asm
	nasm -f elf64 -o $@ $^

test : test64 test32

test64 : asm64.ref len64
	./len64 asm64.bin > dis64.ref
	sed '/^[[:blank:]]/d;s/[[:blank:]].*$$//' dis64.ref > dis64.addr
	sed '/^[[:blank:]]/d;s/[[:blank:]].*$$//' asm64.ref > asm64.addr
	diff dis64.addr asm64.addr

asm64.bin : asm64.asm
	nasm -f bin -o $@ $^

asm64.ref : asm64.bin
	ndisasm -b64 $^ > $@

len64 : len64.c
	gcc -g -Wall -Wextra -o $@ -DTEST_LEN $^

test32 : asm32.ref len32
	./len32 asm32.bin > dis32.ref
	sed '/^[[:blank:]]/d;s/[[:blank:]].*$$//' dis32.ref > dis32.addr
	sed '/^[[:blank:]]/d;s/[[:blank:]].*$$//' asm32.ref > asm32.addr
	diff dis32.addr asm32.addr

asm32.bin : asm32.asm
	nasm -f bin -o $@ $^

asm32.ref : asm32.bin
	ndisasm -b32 $^ > $@

len32 : len.c
	gcc -g -Wall -Wextra -o $@ -DTEST_LEN $^

clean :
	rm -f *.o lin*.xpl asm*.addr asm*.bin asm*.ref dis*.addr dis*.ref len32 len64

