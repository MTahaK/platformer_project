CC = g++
CFLAGS = -std=c++17 -Wall -Iinclude
LDFLAGS = -lglfw -ldl -lGL

SRC = src/main.cpp src/glad.c
OBJ = $(SRC:.cpp=.o)

TARGET = game

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET) *.o