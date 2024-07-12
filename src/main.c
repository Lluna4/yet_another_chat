#include <stdlib.h>
#include <sys/socket.h>	
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <unistd.h>
#include <time.h>
#include "../lib/user.h"
#define MAX_EVENTS 1024

int epfd = 0;
struct users connected = {0};
SSL_CTX *ctx;

//color gradient presets
struct color_gradient_preset bi_flag = {{214, 2, 112}, {0, 56, 168}, {0}};
struct color_gradient_preset trans_flag = {{245, 169, 184}, {255, 255, 255}, {91, 206, 250}};
struct color_gradient_preset non_binary_flag = {{252, 244, 52}, {156, 89, 209}, {0, 0, 0}};
struct color_gradient_preset lesbian_flag = {{213, 45, 0}, {255, 255, 255},{163, 2, 98}};
struct color_gradient_preset gay_flag = {{7, 141, 112}, {255, 255, 255},{61, 26, 120}};

size_t character_times(char *buf, char c) //counts how many times c appears in buf
{
	int max_size = strlen(buf);
	int ret = 0;

	for (int i = 0; i < max_size;i++)
	{
		if (buf[i] == c)
			ret++;
	}
	return ret;
}

int startswith(char *buf, char *str)
{
	int index = 0;
	if (*buf == '\0' && *buf == '\0')
		return 0;
	while (buf[index] != '\0' && str[index] != '\0')
	{
		if (buf[index] != str[index])
			return 0;
		index++;
	}
	return 1;
}

void add_to_list(int fd, int epfd)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
}

void remove_from_list(int fd, int epfd)
{
    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
}

void *accept_thread(void *arg)
{
	int socketfd = *((int *)(arg));
	printf("Listening incomming connections at socket %i\n", socketfd);
	while (1)
	{
		int new_fd = accept(socketfd, NULL, NULL);
		
		
		struct user new_user = {.socket = new_fd, .name = NULL, .pronouns = NULL, .ssl = SSL_new(ctx), .col = random_color(), .colors = {0}, .has_gradient = 0, .has_3color_gradient = 0};
		SSL_set_fd(new_user.ssl, new_fd);
		SSL_use_certificate_file(new_user.ssl, "cert.pem", SSL_FILETYPE_PEM);
		SSL_use_PrivateKey_file(new_user.ssl, "key.pem", SSL_FILETYPE_PEM);
		int res = SSL_accept(new_user.ssl);

		if (res != 1)
		{
			printf("SSL error, %i\n", SSL_get_error(new_user.ssl ,res));
			close(new_fd);
			continue;
		}
		add_user(new_user, &connected);
		printf("Client with socket %i connected to the server\n", new_fd);
		add_to_list(new_fd, epfd);
	}
}

int main()
{
	setvbuf(stdout, NULL, _IONBF, 0);
	srandom(time(NULL));
	connected.size = 0;
	connected.users = malloc(sizeof(struct user));
	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	pthread_t thread;
	struct sockaddr_in listen_addr = {.sin_addr = 0, .sin_port = htons(2907), .sin_family = AF_INET};
	size_t size_addr = sizeof(listen_addr);
	ctx = SSL_CTX_new(TLS_server_method());
	
	if (bind(socketfd, (struct sockaddr *)&listen_addr, size_addr) == -1)
	{
		printf("Bind failed %s\n", strerror(errno));
		return errno;
	}

	if (listen(socketfd, 25) == -1)
	{
		printf("Listen failed %s\n", strerror(errno));
		return errno;
	}
	int *i = malloc(sizeof(*i));
	*i = socketfd;
	printf("Thread cration returned %i\n", pthread_create(&thread, NULL, accept_thread, (void *) i));
	
	epfd = epoll_create1(0);

	//main loop WIP
	struct epoll_event events[MAX_EVENTS];
	int events_ready = 0;
	char *buffer = calloc(4096, sizeof(char));
	int status = 0;
	char *send_buffer;
	//struct colors color1 = {214, 2, 112};
	//struct colors color2 = {0, 56, 168};
	
	while (1)
	{
		events_ready = epoll_wait(epfd, events, MAX_EVENTS, -1);
		if (events_ready == -1)
		{
			printf("Fail %s\n", strerror(errno));
			return errno;
		}
		for (int i = 0; i < events_ready;i++)
		{
			struct user *a = get_user(events[i].data.fd, &connected);
			status = SSL_read(a->ssl, buffer, 4095);
			if (status == 0 || status == -1)
			{
				remove_from_list(events[i].data.fd, epfd);
				remove_user(events[i].data.fd, &connected);
				printf("User %i disconnected\n", events[i].data.fd);
				continue;
			}
			
			//printf("message received %s\n", buffer);
			if (startswith(buffer, "/name") == 1)
			{
				buffer += strlen("/name ");
				a->name = strdup(buffer);
				memset(buffer, 0, 4096);
				continue;
			}
			else if (startswith(buffer, "/pronouns") == 1)
			{
				buffer += strlen("/pronouns ");
				a->pronouns = strdup(buffer);
				memset(buffer, 0, 4096);
				continue;
			}
			else if (startswith(buffer, "/preset"))
			{
			    buffer += strlen("/preset ");
				if (strcmp(buffer, "bi") == 0)
				{
				    a->colors = bi_flag;
					a->has_gradient = 1;
					a->has_3color_gradient = 0;
				}
				if (strcmp(buffer, "trans") == 0)
				{
				    a->colors = trans_flag;
					a->has_gradient = 1;
					a->has_3color_gradient = 1;
				}
				if (strcmp(buffer, "lesbian") == 0)
				{
				    a->colors = lesbian_flag;
					a->has_gradient = 1;
					a->has_3color_gradient = 1;
				}
				if (strcmp(buffer, "gay") == 0)
				{
				    a->colors = gay_flag;
					a->has_gradient = 1;
					a->has_3color_gradient = 1;
				}
				if (strcmp(buffer, "nb") == 0)
				{
				    a->colors = non_binary_flag;
					a->has_gradient = 1;
					a->has_3color_gradient = 1;
				}
				continue;
			}
			if (a->has_gradient == 0 && a->has_3color_gradient == 0)
			{
			    char *colored_string = color_string(a->col, a->name);
			    asprintf(&send_buffer, "%s [%s]: %s", colored_string, a->pronouns, buffer);
				free(colored_string);
			}
			else if (a->has_gradient == 1 && a->has_3color_gradient == 0)
			{
			    char *colored_string = color_string_gradient(a->colors.color1,a->colors.color2, a->name);
			    asprintf(&send_buffer, "%s%s [%s]: %s", colored_string, RESET, a->pronouns, buffer);
				free(colored_string);
			}
			else if (a->has_3color_gradient == 1 && a->has_gradient == 1)
			{
			    struct colors b[3] = {a->colors.color1, a->colors.color2, a->colors.color3};
				char *colored_string = multicolor_string(b, a->name, 3);
			    asprintf(&send_buffer, "%s%s [%s]: %s", colored_string, RESET, a->pronouns, buffer);
				free(colored_string);
			}
			printf("%s\n", send_buffer);
			
			for (int i = 0; i < connected.size; i++)
			{
				SSL_write(connected.users[i].ssl, send_buffer, 1024);
			}
			memset(buffer, 0, 4095);
			free(send_buffer);
			send_buffer = NULL;
		}
	}
	pthread_join(thread,NULL);
	free(connected.users);
	free(buffer);
	free(i);
	return 0;
}
