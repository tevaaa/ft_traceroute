NAME = ft_traceroute

CC = cc
CFLAGS = -Wall -Wextra -Werror

SRC = src/main.c src/args_parsing.c src/init.c src/traceroute.c src/socket.c

all: $(NAME)

$(NAME): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(NAME) 

clean:
	rm -f $(NAME)

re: clean all

.PHONY: all clean re