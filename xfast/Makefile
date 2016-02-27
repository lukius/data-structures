BIN_FILE=xfast

CPP_FILES=$(wildcard *.cpp)
H_FILES=$(wildcard *.h)

CUCKOO_CPP=../cuckoo/hasher.cpp
CUCKOO_H=$(wildcard ../cuckoo/*.h)

CC=g++
RM=rm -rf
CFLAGS=-Wall -std=c++11 -ggdb

LIB_PATH=../gtest
INCLUDES=-I../gtest/include -I../cuckoo
LIBS=-lgtest_main -lpthread

test: $(BIN_FILE)

$(BIN_FILE): $(CPP_FILES) $(H_FILES) $(CUCKOO_CPP) $(CUCKOO_H)
	$(CC) $(CFLAGS) $(INCLUDES) -L $(LIB_PATH) $(CPP_FILES) $(CUCKOO_CPP) $(CUCKOO_H) $(LIBS) -o $(BIN_FILE)

clean:
	$(RM) $(BIN_FILE)
	find . -name "*.o" -type f -delete 
