#version2
BIN=code
CC=g++
#显示当前目录下所有.cpp文件
#SRC=$(shell ls *.cpp)
SRC=$(wildcard *.cpp)
#根据.cpp文件列表生成对应的.o文件名，不管这个文件是否存在
OBJ=$(SRC:.cpp=.o)
LFLAGS=-o
FLAGS=-c
RM=rm -rf 

$(BIN):$(OBJ)
	@$(CC) $(LFLAGS) $@ $^
	@echo "linking ... $^ to $@"

#获取所有的.cpp文件
%.o:%.cpp 
	@$(CC) $(FLAGS) $<
	@echo "compling ... $< to $@"

.PHONY:clean
clean:
	$(RM) $(BIN) $(OBJ)










#version1
#code:code.o main.o
#	g++ code.o main.o -o code 
#
#code.o:code.cpp
#	g++ -c code.cpp -o code.o
#
#main.o:main.cpp 
#	g++ -c main.cpp -o main.o
#
#.PHONY:clean
#clean:
#	rm -rf code code.o main.o







