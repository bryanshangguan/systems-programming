CC = gcc
CFLAGS = -Wall -Wextra -std=c99

words: words.o
    $(CC) $(CFLAGS) -o words words.o

words.o: words.c
    $(CC) $(CFLAGS) -c words.c

clean:
    rm -f words words.o
