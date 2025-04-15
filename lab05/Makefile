CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_SOURCE
TARGETS = zad1 sender catcher

all: $(TARGETS)

zad1: zad1.c
	$(CC) $(CFLAGS) -o zad1 zad1.c

sender: sender.c catcher.c
	$(CC) $(CFLAGS) -o sender sender.c

catcher: catcher.c
	$(CC) $(CFLAGS) -o catcher catcher.c

clean:
	rm -f $(TARGETS)

.PHONY: all clean