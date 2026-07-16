CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -g -Iinclude

TARGET := motor_monitor
SOURCES := src/main.c src/motor.c
OBJECTS := $(SOURCES:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

src/%.o: src/%.c include/motor.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)