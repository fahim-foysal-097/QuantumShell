CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Iinclude -g
SRCS = src/main.c src/parse.c src/read.c src/alias.c src/execute.c src/builtins.c src/config.c src/prompt.c src/vars.c
OBJ = $(SRCS:.c=.o)
TARGET = qsh
PREFIX = /usr/local

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(OBJ)

install: $(TARGET)
	install -Dm755 $(TARGET) $(PREFIX)/bin/$(TARGET)
	@echo "Installed $(TARGET) to $(PREFIX)/bin/"

uninstall:
	rm -f $(PREFIX)/bin/$(TARGET)
	@echo "Removed $(TARGET) from $(PREFIX)/bin/"
