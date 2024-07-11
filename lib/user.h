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

struct color_gradient_preset
{
    struct colors color1;
    struct colors color2;
    struct colors color3;
};

struct user
{
	char *name;
	char *pronouns;
	int socket;
	SSL *ssl;
	struct colors col;
	struct color_gradient_preset colors;
	int has_gradient;
	int has_3color_gradient;
};

struct users
{
	struct user *users;
	int size;
};

struct colors random_color();
char *color_string(struct colors color, char *str);
char *color_string_gradient(struct colors color, struct colors color2,char *str);
char *multicolor_string(struct colors *color_arr, char *str, size_t size);
void add_user(struct user u, struct users *us);
struct users fix_array(int start, struct users a);
void remove_user(int socket, struct users *us);
struct user *get_user(int socket, struct users *us);

#endif
