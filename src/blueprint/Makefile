all: tree generator

tree: 
	g++ tree.cpp \
	-std=c++20 \
	-Wall -O3 -march=native -flto -funroll-loops -pthread \
	-o ../../build/tree

generator:
	g++ generator.cpp ../convert/convert.cpp ../evaluation/rank.cpp \
	-std=c++20 \
	-Wall -O3 -march=native -flto -funroll-loops -pthread \
	-o ../../build/generator