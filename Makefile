INCLUDE = /usr/X11R6/include
LIB = /usr/X11R6/lib

CC = gcc

all: sample_v5

sample_v5: sample_v5.o
	$(CC) sample_v5.o -o sample_v5 -lgpg3100 -lpthread

sample_v5.o: sample_v5.c
	$(CC) -Wall -c sample_v5.c -o sample_v5.o

clean:
	rm -f *.o ¥#* *~ sample_v5
