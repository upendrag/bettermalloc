
all:
	gcc -Wall -fPIC -shared -Wl,-soname,libmem.so.1 -o libmem.so.1.0 mem.c
	ln -sf libmem.so.1.0 libmem.so
	ln -sf libmem.so.1.0 libmem.so.1

test: all
	gcc -Wall -L. test.c -lmem -o test.o

clean:
	rm -f libmem.* *.o

.PHONY: clean
