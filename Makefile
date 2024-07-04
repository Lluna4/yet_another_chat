CC = gcc

COMPILER_FLAGS = -pthread -lssl lib/user.a

all : main.c
	$(CC) main.c -o sv.out $(COMPILER_FLAGS)
	$(CC) cli.c -o cli.out -lssl

clean:
	rm -rf *.out

re : clean all

.PHONY: all clean re
