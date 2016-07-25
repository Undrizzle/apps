CFLAGS = -g -O2

mkenv:mkenv.o
	gcc mkenv.o -o mkenv
mkenv.o:
	gcc -c mkenv.c $(CFLAGS)

.PHONY:clean
clean:
	rm *.o mkenv -f