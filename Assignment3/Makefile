# Compiler and flags
CC = gcc
CFLAGS = -lm

# Source and output
SRC = main.c
EXEC = vader

# Default target (build the program)
all: $(EXEC)

# Rule to build the executable
$(EXEC): $(SRC)
	$(CC) $(SRC) -o $(EXEC) $(CFLAGS)

# Clean rule to remove the executable
clean:
	rm -f $(EXEC)
