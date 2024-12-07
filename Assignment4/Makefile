# Variables
CC = gcc
CFLAGS = -lm
TARGET = pso
SRC = main.c PSO.c OF.c

# Build target
$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS)

# Clean up
clean:
	rm -f $(TARGET)

