all: build run

build:
	g++ ./src/cegex-pp.cpp -Wall -o out

run: build
	./out