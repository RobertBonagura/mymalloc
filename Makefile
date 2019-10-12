all: test.c mymalloc.o
	gcc test.c mymalloc.o
mymalloc.o: mymalloc.c
	gcc -c mymalloc.c
clean:
	rm a.out; rm mymalloc.o
test0:
	make clean; make all
