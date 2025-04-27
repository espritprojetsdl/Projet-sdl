CC = gcc
CFLAGS = -Wall -g `sdl-config --cflags`
LDFLAGS = `sdl-config --libs` -lSDL_image -lSDL_gfx -lSDL_ttf
SRC = main.c fonctions.c
OBJ = $(SRC:.c=.o)
TARGET = monjeu

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c fonctions.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

