EXEC_PATH = out
SRCS = ./src/cegex-pp.cpp 
TEST_SRCS = ./src/test.cpp
CXX = g++
CXX_FLAGS = -Wall -Wextra -std=c++17
INCLUDE = -I./include/
OBJ_BUILD_PATH = ./build/libcegex-pp.o
SHARED_LIB_BUILD_PATH = ./build/libcegex-pp.so
STATIC_LIB_BUILD_PATH = ./build/libcegex-pp.a

.PHONY: build test clean

all: build test

build:
	$(CXX) -fPIC -c $(SRCS) $(CXX_FLAGS) $(INCLUDE) -o $(OBJ_BUILD_PATH)
	$(CXX) -shared -o $(SHARED_LIB_BUILD_PATH) $(OBJ_BUILD_PATH)
	ar rcs $(STATIC_LIB_BUILD_PATH) $(OBJ_BUILD_PATH)

test:
	$(CXX) $(TEST_SRCS) $(STATIC_LIB_BUILD_PATH) $(CXX_FLAGS) $(INCLUDE) -o $(EXEC_PATH)
	@./$(EXEC_PATH)

clean:
	@rm $(BINARY_NAME)
	@rm $(OBJ_BUILD_PATH) $(SHARED_LIB_BUILD_PATH) $(STATIC_LIB_BUILD_PATH) $(EXEC_PATH)