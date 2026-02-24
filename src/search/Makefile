all: play_with_search play_against_search init_visualiser

play_with_search:
	g++ play_with_search.cpp retrainer.cpp search.cpp ../helper/convert.cpp ../evaluation/rank.cpp ../helper/init.cpp \
	-std=c++20 \
	-Wall -O3 -march=native -flto -funroll-loops -pthread \
	-o ../../build/play_with_search

play_against_search:
	g++ play_against_search.cpp retrainer.cpp search.cpp ../helper/convert.cpp ../evaluation/rank.cpp ../helper/init.cpp \
	-std=c++20 \
	-Wall -O3 -march=native -flto -funroll-loops -pthread \
	-o ../../build/play_against_search

init_visualiser:
	g++ init_visualiser.cpp retrainer.cpp search.cpp ../helper/convert.cpp ../evaluation/rank.cpp ../helper/init.cpp \
	-std=c++20 \
	-Wall -O3 -march=native -flto -funroll-loops -pthread \
	-o ../../build/init_visualiser