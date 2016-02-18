CC = g++
CFLAGS = -Wall -std=c++11 -O3
INC    := -I../Hum2D/include -I../SFML/include

all: main.cpp
	$(CC) -o game main.cpp $(CFLAGS) $(INC) ../Hum2D/lib/libhum2d.a ../SFML/lib/libhum2d-sfml.a -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
