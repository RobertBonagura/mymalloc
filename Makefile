all: test.c memgrind.c mymalloc.o
	gcc -o test test.c mymalloc.o; gcc -g -o memgrind memgrind.c mymalloc.o
mymalloc.o: mymalloc.c
	gcc -c mymalloc.c
clean:
	rm test; rm memgrind; rm mymalloc.o
test0:
	make clean; make all
