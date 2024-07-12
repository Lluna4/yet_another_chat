CC = gcc

COMPILER_FLAGS = -pthread -lssl lib/user.a

all : src/main.c
	$(CC) src/main.c -o build/sv.out $(COMPILER_FLAGS)
	$(CC) src/cli.c -o build/cli.out $(COMPILER_FLAGS)

clean:
	rm -rf *.out

re : clean all

.PHONY: all clean re
