BIN_FILE=rmq

CC=g++
RM=rm -rf
CFLAGS=-Wall -std=c++11

all: $(BIN_FILE)

$(BIN_FILE): main.cpp rmq.h
	$(CC) $(CFLAGS) main.cpp -o $(BIN_FILE)

clean:
	$(RM) $(BIN_FILE)
	find . -name "*.o" -type f -delete 
