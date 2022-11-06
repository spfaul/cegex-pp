BINARY_NAME = out
SRCS = ./src/cegex-pp.cpp 
TEST_SRCS = ./src/test.cpp
CXX = g++
CXX_FLAGS = -Wall -Wextra -std=c++17
INCLUDE = -I./include/
OBJ_BUILD_PATH = ./build/libcegex-pp.o
SHARED_LIB_BUILD_PATH = ./build/libcegex-pp.so

.PHONY: build test clean

all: build test clean

build:
	$(CXX) -fPIC -c $(SRCS) $(CXX_FLAGS) $(INCLUDE) -o $(OBJ_BUILD_PATH)
	gcc -shared -o $(SHARED_LIB_BUILD_PATH) $(OBJ_BUILD_PATH)	

test:
	$(CXX) $(TEST_SRCS) $(CXX_FLAGS) $(INCLUDE) -Lbuild/ -lcegex-pp -o $(BINARY_NAME) 
	@./$(BINARY_NAME)

clean:
	@rm $(BINARY_NAME)