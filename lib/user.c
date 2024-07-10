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
	int lenght = (strlen("\033[38;2;%ii;%ii;%iim") + strlen(RESET)) * strlen(str);
	char *buf = calloc(lenght + 1, sizeof(char));
	char *temp;
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
		asprintf(&temp, "\033[38;2;%i;%i;%im%c%s", r, g, b, str[i], RESET);
		strcat(buf, temp);
		free(temp);
		r += grad_r;
		g += grad_g;
		b += grad_b;
	}	
	return buf;
}

char *color_string_gradient3(struct colors color, struct colors color2, struct colors color3,char *str)
{
    int lenght = (strlen("\033[38;2;%ii;%ii;%iim") + strlen(RESET)) * strlen(str);
	char *buf = calloc(lenght + 1, sizeof(char));
	char *temp;
	int half_lenght = strlen(str)/2;
	int rest_lenght = strlen(str) - half_lenght;
	//struct colors color = random_color();
	int grad_r_half = abs(color.r - color2.r)/half_lenght;
	int grad_g_half = abs(color.g - color2.g)/half_lenght;
	int grad_b_half = abs(color.b - color2.b)/half_lenght;
	
	int grad_r_rest = abs(color2.r - color3.r)/rest_lenght;
	int grad_g_rest= abs(color2.g - color3.g)/rest_lenght;
	int grad_b_rest = abs(color2.b - color3.b)/rest_lenght;
	if (color.r > color2.r)
		grad_r_half *= -1;
	if (color.g > color2.g)
		grad_g_half *= -1;
	if (color.b > color2.b)
		grad_b_half *= -1;
	if (color2.r > color3.r)
		grad_r_rest *= -1;
	if (color2.g > color3.g)
		grad_g_rest *= -1;
	if (color2.b > color3.b)
		grad_b_rest *= -1;
	int r_half = color.r, g_half = color.g, b_half = color.b;
	int r_rest = color2.r, g_rest = color2.g, b_rest = color2.b;
	for(int i = 0; i < half_lenght;i++)
	{
		asprintf(&temp, "\033[38;2;%i;%i;%im%c%s", r_half, g_half, b_half, str[i], RESET);
		strcat(buf, temp);
		free(temp);
		r_half += grad_r_half;
		g_half += grad_g_half;
		b_half += grad_b_half;
	}
	for(int i = half_lenght; i < strlen(str);i++)
	{
		asprintf(&temp, "\033[38;2;%i;%i;%im%c%s", r_rest, g_rest, b_rest, str[i], RESET);
		strcat(buf, temp);
		free(temp);
		r_rest += grad_r_rest;
		g_rest += grad_g_rest;
		b_rest += grad_b_rest;
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
