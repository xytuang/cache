CC = gcc

CFLAGS = -g -Wall

LDFLAGS =

LDLIBS =

daedalus: daedalus.o

daedalus.o: daedalus.c

.PHONY: clean
clean:
	rm -f *.o a.out core daedalus

.PHONY: all
all: clean daedalus