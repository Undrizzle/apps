CFLAGS = -g -O2

mkweb:mkweb.o md5-little-endian.o
	gcc md5-little-endian.o mkweb.o -o mkweb
md5-little-endian.o:
	gcc -c md5-little-endian.c $(CFLAGS)
mkweb.o:
	gcc -c mkweb.c $(CFLAGS)

.PHONY:clean
clean:
	rm *.o mkweb -f