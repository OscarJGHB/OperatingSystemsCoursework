cc = gcc
CFLAGS = -g

all: mem_sim

mem_sim: main.o mem.o
	$(cc) $(CFLAGS) -o mem_sim main.o mem.o 

mem.o: mem.c mem.h
	$(cc) $(CFLAGS) -c mem.c

main.o: main.c mem.h
	$(cc) $(CFLAGS) -c main.c

clean:
	rm -f main.o mem.o mem_sim

	