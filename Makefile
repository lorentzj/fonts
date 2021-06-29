CC = gcc
CCFLAGS = -Wall -Werror
CCINCLUDE = -I /usr/local/include/
CCLIBS = -lGL -lGLEW -lSDL2 -lSDL2_image

.PHONY: all, clean

all:
	@$(CC) $(CCFLAGS) $(CCINCLUDE) src/*.c $(CCLIBS) -o viewfont 

clean:
	@rm viewfont 
