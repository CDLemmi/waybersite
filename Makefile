
CC = gcc

CFLAGS =  -g -Isrc -fsanitize=address

TARGET = main

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:src/%.c=build/%.o)

LIBS = -lssl -lcrypto -lsodium -lsqlite3 -fsanitize=address -lm



all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir build

clean:
	rm -rf build/
	rm -f $(TARGET)
