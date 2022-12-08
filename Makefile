INCLUDE = /usr/X11R6/include
LIB = /usr/X11R6/lib

CC = gcc

all: Ammeter_v2

Ammeter_v2: Ammeter_v2.o
	$(CC) Ammeter_v2.o -o Ammeter_v2 -lgpg3100 -lpthread

Ammeter_v2.o: Ammeter_v2.c
	$(CC) -std=gnu99 -Wall -c Ammeter_v2.c -o Ammeter_v2.o

clean:
	rm -f *.o ¥#* *~ Ammeter_v2
