BIN=code 
CC=gcc
LFLAGS=-c
FLAGS=-o
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)
RM=rm -rf 

#链接形成可编译文件
$(BIN):$(OBJ)
	@$(CC) $(FLAGS) $@ $^

#编译
%.o:%.c 
	@$(CC) $(LFLAGS) $< -g

.PHONY:clean 
clean:
	@$(RM) $(BIN) $(OBJ)

