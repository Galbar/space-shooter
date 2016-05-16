CC = g++
CFLAGS = -Wall -std=c++11 -O3
INC    := -I../hummingbird/include -I../MOGL/include -I../glm
LIB_DIR   := -L../hummingbird/lib/ -L../MOGL/lib/

all: main.cpp
	$(CC) -o game main.cpp $(CFLAGS) $(INC) $(LIB_DIR) -lhum -lmogl -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system -lGL -lGLEW
