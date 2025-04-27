CC = gcc
CFLAGS = -Wall -g
LIBS = `sdl-config --libs` -lSDL_image
INCLUDES = `sdl-config --cflags`
TARGET = simple_platformer
SRC = main.c
OBJ = main.o

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

