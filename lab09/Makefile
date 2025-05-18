CC = gcc
CFLAGS = -Wall -Wextra -O3 -pthread -lm
TARGET = integral
SOURCE = integral.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

run: $(TARGET)
	./$(TARGET) 0.000001 4  # Przykładowe parametry: dx=0.000001, 4 wątki

clean:
	rm -f $(TARGET)

.PHONY: all run clean