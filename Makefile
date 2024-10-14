CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SRCS = main.c builtin.c external.c parse.c
TARGET = cshell

build:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

run:
	./$(TARGET)

.PHONY: build clean run