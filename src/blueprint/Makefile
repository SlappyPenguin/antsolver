all: tree generator trainer reformat play_with_blueprint

tree: 
	g++ tree.cpp \
	-std=c++20 \
	-Wall -O3 -march=native -flto -funroll-loops -pthread \
	-o ../../build/tree

generator:
	g++ generator.cpp ../helper/convert.cpp ../evaluation/rank.cpp ../helper/init.cpp \
	-std=c++20 \
	-Wall -O3 -march=native -flto -funroll-loops -pthread \
	-o ../../build/generator

trainer:
	g++ trainer.cpp ../helper/init.cpp \
	-std=c++20 \
	-Wall -O3 -march=native -flto -funroll-loops -pthread \
	-o ../../build/trainer

reformat:
	g++ reformat.cpp ../helper/init.cpp \
	-std=c++20 \
	-Wall -O3 -march=native -flto -funroll-loops -pthread \
	-o ../../build/reformat

play_with_blueprint:
	g++ play_with_blueprint.cpp ../helper/convert.cpp ../helper/init.cpp \
	-std=c++20 \
	-Wall -O3 -march=native -flto -funroll-loops -pthread \
	-o ../../build/play_with_blueprint