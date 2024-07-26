CC = gcc

CFLAGS = -g -Wall

LDFLAGS =

LDLIBS =

daedalus: daedalus.o map.o

daedalus.o: daedalus.c

map.o: map.c map.h

.PHONY: clean
clean:
	rm -f *.o a.out core daedalus

.PHONY: all
all: clean daedalus