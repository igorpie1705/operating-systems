CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

all: countdown

countdown: countdown.o
	$(CC) $(CFLAGS) countdown.o -o countdown

countdown.o: countdown.c
	$(CC) $(CFLAGS) -c countdown.c -o countdown.o

clean:
	rm -f countdown countdown.o

.PHONY: all clean
