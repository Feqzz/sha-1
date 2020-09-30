PROG = main
NAME = sha-1
CC = g++

all: build run

build:
	$(CC) $(PROG).cpp -o $(NAME)

run:
	./$(NAME) abc