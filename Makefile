OBJECTS = main.o sun.o
CC = gcc
LIBS = -lm
CFLAGS = -m64 -std=c99 -pedantic -Wall -Wextra -Werror -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes

adjust-backlight: $(OBJECTS)
	$(CC) -o adjust-backlight $(OBJECTS) $(LIBS) $(CFLAGS)

main.o: main.c
	$(CC) -c main.c $(CFLAGS)

sun.o: sun.c sun.h
	$(CC) -c sun.c $(CFLAGS)

clean:
	rm adjust-backlight main.o sun.o
