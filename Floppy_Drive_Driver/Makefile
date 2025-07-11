cc = gcc
CFLAGS = -g

all: disco

run:
	./disco floppy.img 

disco: main.o disk.o
	$(cc) $(CFLAGS) -o disco main.o disk.o  

main.o: main.c disk.h
	$(cc) $(CFLAGS) -c main.c 

disk.o: disk.c disk.h
	$(cc) $(CFLAGS) -c disk.c

clean:
	rm -f main.o disco disk.o