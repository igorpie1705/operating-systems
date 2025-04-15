CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGETS = zad1 zad2

all: $(TARGETS)

zad1: zad1.c
	$(CC) $(CFLAGS) -o zad1 zad1.c

zad2: zad2.c
	$(CC) $(CFLAGS) -o zad2 zad2.c

run1: zad1
	./zad1 $(ARG)

run2: zad2
	./zad2 $(ARG)

clean:
	rm -f $(TARGETS)

.PHONY: all clean run1 run2