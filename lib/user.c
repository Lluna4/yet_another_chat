#include "user.h"
#include <string.h>
#include <stdlib.h>
#include <openssl/ssl.h>

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

char *multicolor_string(struct colors *color_arr, char *str, size_t size)
{
    if (size > strlen(str))
    {
        return str;
    }
    int lenght = (strlen("\033[38;2;%ii;%ii;%iim") + strlen(RESET)) * strlen(str);
	char *buf = calloc(lenght + 1, sizeof(char));
	char *temp;
	int **grad_steps = calloc(size + 1, sizeof(int *));
	float letter_step = (float)strlen(str)/size;
	int string_index = 0;
	for(int i = 0; i < size;i++)
	{
	    grad_steps[i] = calloc(4, sizeof(int));
		grad_steps[i][0] = abs(color_arr[i].r - color_arr[i+1].r)/letter_step;
		grad_steps[i][1] = abs(color_arr[i].g - color_arr[i+1].g)/letter_step;
		grad_steps[i][2] = abs(color_arr[i].b - color_arr[i+1].b)/letter_step;
		if (color_arr[i].r > color_arr[i+1].r)
			grad_steps[i][0] *= -1;
		if (color_arr[i].g > color_arr[i+1].g)
			grad_steps[i][1] *= -1;
		if (color_arr[i].b> color_arr[i+1].b)
			grad_steps[i][2] *= -1;
	}
	
	for (int i = 0; i< size;i++)
	{
	    int r = color_arr[i].r, g = color_arr[i].g, b = color_arr[i].b;
		if (i == size -1)
		    letter_step = strlen(str) - string_index;
		for (int x = 0; x < letter_step;x++)
		{
    		asprintf(&temp, "\033[38;2;%i;%i;%im%c%s", r, g, b, str[string_index], RESET);
    		strcat(buf, temp);
    		free(temp);
    		r += grad_steps[i][0];
    		g += grad_steps[i][1];
    		b += grad_steps[i][2];
            string_index++;
		}
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
