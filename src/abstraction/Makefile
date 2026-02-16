all: sets strengths distributions

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

distributions:
	g++ distributions.cpp ../convert/convert.cpp \
	-std=c++20 \
	-Wall -O3 -march=native -flto -funroll-loops -pthread \
	-o ../../build/distributions