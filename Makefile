OBJECTS = main.o
CC = gcc
CFLAGS = -m64 -std=c99 -pedantic -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wdeclaration-after-statement

adjust-backlight: $(OBJECTS)
	$(CC) -o adjust-backlight $(OBJECTS) $(CFLAGS)

main.o: main.c
	$(CC) -c main.c $(CFLAGS)

clean:
	rm adjust-backlight main.o
