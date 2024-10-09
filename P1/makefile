CC = gcc
CFLAGS = -Wall -g
TARGET = memgrind

# List of object files to create
OBJS = mymalloc.o memgrind.o memtest.o

# Default target to build the program
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile mymalloc.c to object file
mymalloc.o: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -c mymalloc.c

# Compile memtest.c to object file
memtest.o: memtest.c mymalloc.h
	$(CC) $(CFLAGS) -c memtest.c

# Compile memgrind.c to object file
memgrind.o: memgrind.c mymalloc.h
	$(CC) $(CFLAGS) -c memgrind.c

# Clean up object files and executable
clean:
	rm -f *.o $(TARGET)
	