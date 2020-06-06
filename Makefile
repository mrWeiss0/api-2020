CC=gcc
CFLAGS=-g -DDEBUG -Wall -Wextra
NAME=api20

$(NAME): $(NAME).c
	$(CC) $(CFLAGS) -o $@ $<
