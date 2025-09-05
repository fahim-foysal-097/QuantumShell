CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Ilib -g
SRCS = src/main.c src/parse.c src/read.c src/alias.c src/execute.c src/builtins.c src/config.c
OBJ = $(SRCS:.c=.o)
TARGET = qsh

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(OBJ)
