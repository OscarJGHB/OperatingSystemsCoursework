cc = gcc
CFLAGS = -O0 -g


all: test

run:
	./test 32 1000 1000

test: hw6.o access_test.o measure_tools.o
	$(cc) $(CFLAGS) -o test hw6.o access_test.o  measure_tools.o -lm

access_test.o: access_test.c access_test.h
	$(cc) $(CFLAGS) -c access_test.c 

hw6.o: hw6.c access_test.h
	$(cc) $(CFLAGS) -c hw6.c

measure_tools.o: measure_tools.c measure_tools.h
	$(cc) $(CFLAGS) -c measure_tools.c 

asm: hw6.c
	gcc -O0 -S hw6.c -o hw6.s

clean:
	rm -f hw6.o access_test.o access_test hw6.s measure_tools.o test
	
