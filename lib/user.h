#ifndef USER_H
# define USER_H

#include <string.h>
#include <stdlib.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#define RESET "\033[0m"


struct colors
{
	int r, g, b;
};

struct user
{
	char *name;
	char *pronouns;
	int socket;
	SSL *ssl;
	struct colors col;
};

struct users
{
	struct user *users;
	int size;
};

struct colors random_color();
char *color_string(struct colors color, char *str);
void add_user(struct user u, struct users *us);
struct users fix_array(int start, struct users a);
void remove_user(int socket, struct users *us);
struct user *get_user(int socket, struct users *us);

#endif
