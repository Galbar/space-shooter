CC = g++
CFLAGS = -Wall -std=c++11 -O3
INC    := -I../Hum2D/include -I../SFML/include

all: main.cpp
	$(CC) -o game main.cpp $(CFLAGS) $(INC) -L../Hum2D/lib -L../SFML/lib -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system -lhum2d -lhum2d-sfml
