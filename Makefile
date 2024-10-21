CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SRCS = main.c builtin.c parse.c
TARGET = cshell

.PHONY: build clean run

build:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

run:
	./$(TARGET)

test:
	gcc -o test_main tests/test_main.c builtin.c -I/opt/homebrew/opt/cunit/include -L/opt/homebrew/opt/cunit/lib -lcunit && ./test_main && rm test_main