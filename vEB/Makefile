BIN_FILE=veb

CC=g++
RM=rm -rf
CFLAGS=-Wall -std=c++11

all: $(BIN_FILE)

$(BIN_FILE): main.cpp veb.cpp veb.h
	$(CC) $(CFLAGS) main.cpp veb.cpp -o $(BIN_FILE)

clean:
	$(RM) $(BIN_FILE)
	find . -name "*.o" -type f -delete 
