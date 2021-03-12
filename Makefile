CC = gcc
CCFLAGS = -Wall -Werror

.PHONY: all, clean

all:
	@mkdir -p bin
	@$(CC) $(CCFLAGS) src/main.c -o bin/stellq

clean:
	@rm -rf bin