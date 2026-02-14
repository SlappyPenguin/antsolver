all: sets strengths

sets:
	g++ sets.cpp \
	-std=c++20 \
	-Wall -O3 -march=native -flto -funroll-loops -pthread \
	-o ../../build/sets

strengths:
	g++ strengths.cpp ../evaluation/rank.cpp \
	-std=c++20 \
	-Wall -O3 -march=native -flto -funroll-loops -pthread \
	-o ../../build/strengths