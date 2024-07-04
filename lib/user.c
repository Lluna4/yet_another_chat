#include "user.h"

void add_user(struct user u, struct users *us)
{
	us->users = realloc(us->users, (us->size + 1) * sizeof(struct user));
	us->users[us->size] = u;
	us->size++;
}

struct users fix_array(int start, struct users a)
{
	for (int i = start; i < a.size;i++)
	{
		a.users[i - 1] = a.users[i];
	}
	return a;
}

void remove_user(int socket, struct users *us)
{
	for (int i = 0; i < us->size;i++)
	{
		if (us->users[i].socket == socket)
		{
			SSL_free(us->users[i].ssl);
			free(us->users[i].name);
			free(us->users[i].pronouns);
			us->users[i] = (struct user){0};
			*us = fix_array(i + 1, *us);
			us->size--;
		}
	}
}

struct user *get_user(int socket, struct users *us)
{
	//struct user ret = {0};
	for (int i = 0; i < us->size;i++)
	{
		if (us->users[i].socket == socket)
			return &us->users[i];
	}
	return NULL;
}
