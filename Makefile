INCLUDE = /usr/X11R6/include
LIB = /usr/X11R6/lib

CC = gcc

all: Ammeter_v1

Ammeter_v1: Ammeter_v1.o
	$(CC) Ammeter_v1.o -o Ammeter_v1 -lgpg3100 -lpthread

Ammeter_v1.o: Ammeter_v1.c
	$(CC) -std=gnu99 -Wall -c Ammeter_v1.c -o Ammeter_v1.o

clean:
	rm -f *.o ¥#* *~ Ammeter_v1
