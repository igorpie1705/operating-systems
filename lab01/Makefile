CXX = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

all: countdown

countdown: countdown.c
	$(CXX) $(CFLAGS) countdown.c -o countdown

clean:
	rm -f countdown

.PHONY: all countdown clean