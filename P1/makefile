CC = gcc
CFLAGS = -Wall -g

# Define two targets: memgrind and memtest
TARGET1 = memgrind
TARGET2 = memtest

# List of object files for each target
OBJS1 = mymalloc.o memgrind.o
OBJS2 = mymalloc.o memtest.o

# Default target: build both programs
all: $(TARGET1) $(TARGET2)

# Build the memgrind executable
$(TARGET1): $(OBJS1)
	$(CC) $(CFLAGS) -o $(TARGET1) $(OBJS1)

# Build the memtest executable
$(TARGET2): $(OBJS2)
	$(CC) $(CFLAGS) -o $(TARGET2) $(OBJS2)

# Compile mymalloc.c to object file
mymalloc.o: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -c mymalloc.c

# Compile memtest.c to object file
memtest.o: memtest.c mymalloc.h
	$(CC) $(CFLAGS) -c memtest.c

# Compile memgrind.c to object file
memgrind.o: memgrind.c mymalloc.h
	$(CC) $(CFLAGS) -c memgrind.c

# Clean up object files and executables
clean:
	rm -f *.o $(TARGET1) $(TARGET2)
