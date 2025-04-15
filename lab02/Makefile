CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS = -ldl

all: static shared dynamic

static: libcollatz.a client_static

shared: libcollatz.so client_shared

dynamic: libcollatz.so client_dynamic

libcollatz.a: collatz.o
	ar rcs libcollatz.a collatz.o

libcollatz.so: collatz.o
	$(CC) -shared -o libcollatz.so collatz.o

collatz.o: collatz.c collatz.h
	$(CC) $(CFLAGS) -c -fPIC collatz.c

client_static: main.c libcollatz.a
	$(CC) $(CFLAGS) -o client_static main.c libcollatz.a

client_shared: main.c libcollatz.so
	$(CC) $(CFLAGS) -o client_shared main.c -L. -Wl,-rpath,. -lcollatz

client_dynamic: main.c libcollatz.so
	$(CC) $(CFLAGS) -DDYNAMIC_LOAD -o client_dynamic main.c $(LDFLAGS)

clean:
	rm -f *.o *.a *.so client_static client_shared client_dynamic