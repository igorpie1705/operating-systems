CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_SOURCE
TARGETS = zad1 zad2

all: $(TARGETS)

zad1: zad1.c
	$(CC) $(CFLAGS) -o zad1 zad1.c

zad2: zad2.c
	$(CC) $(CFLAGS) -o zad2 zad2.c

clean:
	rm -f $(TARGETS)

.PHONY: all clean