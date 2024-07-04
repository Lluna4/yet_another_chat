#ifndef USER_H
# define USER_H

#include <string.h>
#include <stdlib.h>
#include <openssl/ssl.h>
struct user
{
	char *name;
	char *pronouns;
	int socket;
	SSL *ssl;
};

struct users
{
	struct user *users;
	int size;
};


void add_user(struct user u, struct users *us);
struct users fix_array(int start, struct users a);
void remove_user(int socket, struct users *us);
struct user *get_user(int socket, struct users *us);

#endif
