CC=gcc
CFLAGS=-g -DDEBUG
NAME=api20

$(NAME): $(NAME).c
	$(CC) $(CFLAGS) -o $@ $<
