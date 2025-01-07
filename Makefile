# Compiler and flags
CC = gcc
CFLAGS = -Wall -O2 -I/usr/include/opus -I/usr/include/aarch64-linux-gnu -I/usr/include/SDL2 -D_REENTRANT
LDFLAGS = -lpthread -lsndfile -lfftw3 -lSDL2 -lm

# Executable name
TARGET = beatvisualizer

# Source and object files
SRCDIR = src
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(SRCS:.c=.o)

# Default target: build the executable
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

# Rule to build object files
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Clean rule to remove build artifacts
clean:
	rm -f $(OBJS) $(TARGET)
