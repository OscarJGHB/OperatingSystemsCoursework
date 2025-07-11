CC=gcc
CFLAGS=-c -Wall -g
TARNAME=hw1

all: hw1

hw1: utils.o hw1.o
	$(CC) -g utils.o hw1.o -o hw1 -lm

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) utils.c

hw1.o: hw1.c utils.h
	$(CC) $(CFLAGS) hw1.c

clean:
	/bin/rm -f hw1 utils.o hw1.o $(TARNAME).tar.gz

run:
	./hw1 lines.txt

tarball:
	tar -czvf $(TARNAME).tar.gz hw1.c utils.c utils.h Makefile
