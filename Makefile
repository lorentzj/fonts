CC = gcc
CCFLAGS = -Wall -Werror
CCINCLUDE = -I /usr/local/include/
CCLIBS = -lGL -lGLEW -lSDL2 -lSDL2_image

.PHONY: all, clean

all:
	@mkdir -p bin
	@$(CC) $(CCFLAGS) $(CCINCLUDE) src/*.c $(CCLIBS) -o bin/stellq

clean:
	@rm -rf bin