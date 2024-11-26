CC = gcc
CFLAGS = -Wall -Wextra -g
OBJS = mysh.o command_parser.o builtins.o io.o wildcards.o utils.o

mysh: $(OBJS)
	$(CC) $(CFLAGS) -o mysh $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o mysh