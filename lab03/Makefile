CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
INPUT_DIR = art/
OUTPUT_DIR = myart/

all: flipper

flipper: flipper.o
	$(CC) $(CFLAGS) flipper.o -o flipper

flipper.o: flipper.c
	$(CC) $(CFLAGS) -c flipper.c -o flipper.o

run: all
	./flipper $(INPUT_DIR) $(OUTPUT_DIR)

clean:
	rm -f flipper.o flipper

clean_output:
	rm -rf $(OUTPUT_DIR)

clean_all: clean clean_output

.PHONY: all clean run clean_output clean_all