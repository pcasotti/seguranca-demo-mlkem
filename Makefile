CC = gcc
CFLAGS ?= -Wall -Wextra -O2 -std=c11 -D_POSIX_C_SOURCE=200809L
CPPFLAGS += -Isrc $(shell pkg-config --cflags liboqs 2>/dev/null)
LDLIBS += $(shell pkg-config --libs liboqs 2>/dev/null)
LDLIBS += -loqs

SRC = src/main.c src/kem_demo.c src/utils.c
OBJ = $(SRC:.c=.o)
TARGET = kem_experiment

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDLIBS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

benchmark: $(TARGET)
	./$(TARGET) --benchmark 30

clean:
	rm -f src/*.o $(TARGET)

.PHONY: all run benchmark clean
