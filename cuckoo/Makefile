BIN_FILE=cuckoo

CPP_FILES=$(wildcard *.cpp)
H_FILES=$(wildcard *.h)

CC=g++
RM=rm -rf
CFLAGS=-Wall -std=c++11 -ggdb

INCLUDE_PATH=../gtest/include
LIB_PATH=../gtest
LIBS=-lgtest_main -lpthread

test: $(BIN_FILE)

$(BIN_FILE): $(CPP_FILES) $(H_FILES)
	$(CC) $(CFLAGS) -I $(INCLUDE_PATH) -L $(LIB_PATH) $(CPP_FILES) $(LIBS) -o $(BIN_FILE)

clean:
	$(RM) $(BIN_FILE)
	find . -name "*.o" -type f -delete 
