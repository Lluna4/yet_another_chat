#include "user.h"

struct colors random_color()
{
	int r, g, b;
	r = random();
	g = random();
	b = random();
	struct colors a = {r, g, b};

	return a;
}

char *color_string(struct colors color, char *str)
{
	char *buf;
	//struct colors color = random_color();
	asprintf(&buf, "\033[38;2;%i;%i;%im%s%s", color.r, color.g, color.b, str, RESET);
	return buf;
}

char *color_string_gradient(struct colors color, struct colors color2,char *str)
{
	char *buf = "";
	//struct colors color = random_color();
	int grad_r = abs(color.r - color2.r)/strlen(str);
	int grad_g = abs(color.g - color2.g)/strlen(str);
	int grad_b = abs(color.b - color2.b)/strlen(str);
	if (color.r > color2.r)
		grad_r *= -1;
	if (color.g > color2.g)
		grad_g *= -1;
	if (color.b > color2.b)
		grad_b *= -1;
	int r = color.r, g = color.g, b = color.b;
	for(int i = 0; i < strlen(str);i++)
	{
		asprintf(&buf, "%s\033[38;2;%i;%i;%im%c%s", buf, r, g, b, str[i], RESET);
		r += grad_r;
		g += grad_g;
		b += grad_b;
	}	
	return buf;
}

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
