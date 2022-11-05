BINARY_NAME = out
SRCS = ./src/cegex-pp.cpp ./src/test.cpp
CXX = g++
CXX_FLAGS = -Wall -Wextra

all: build run

build:
	g++ $(SRCS) $(CXX_FLAGS) -I./include/ -o $(BINARY_NAME)

run: build
	./$(BINARY_NAME)

clean:
	rm $(BINARY_NAME)