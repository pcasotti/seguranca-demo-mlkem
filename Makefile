CC = gcc
CFLAGS = -Wall -Wextra -O3 -Iinclude
LDFLAGS = -loqs

SRC = src/main.c src/kem_demo.c src/utils.c
OBJ = $(SRC:.c=.o)
TARGET = kem_experiment

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

.PHONY: all clean
