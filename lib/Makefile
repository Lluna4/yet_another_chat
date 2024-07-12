HEADER = user.h

NAME = user.a

FLAGS = -Wall -Wextra -Werror -lssl

SRC = user.c

OBJ = ${SRC:.c=.o} 

%.o: %.c $(HEADER)
	$(CC) $(FLAGS) -c $< -o $@


$(NAME): ${OBJ} ${HEADER}
	ar -rcs $(NAME) ${OBJ}

all: ${NAME}

clean:
	rm -rf *.o

fclean: clean
	rm -rf ${NAME}

re: fclean all

.PHONY: all clean fclean re