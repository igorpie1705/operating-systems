CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_SOURCE
TARGETS = zad1 zad2 prog1 prog2 to_worker from_worker

all: $(TARGETS)

zad1: zad1.c
	$(CC) $(CFLAGS) -o zad1 zad1.c

zad2: prog1.c prog2.c
	mkfifo to_worker
	mkfifo from_worker
	$(CC) $(CFLAGS) -o prog1 prog1.c
	$(CC) $(CFLAGS) -o prog2 prog2.c

clean:
	rm -f $(TARGETS)

.PHONY: all clean zad2