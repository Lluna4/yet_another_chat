#include <stdlib.h>
#include <sys/socket.h>	
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/epoll.h>
#include "lib/user.h"
#define MAX_EVENTS 1024

int epfd = 0;
struct users connected = {0};

int startswith(char *buf, char *str)
{
	int index = 0;

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
		add_to_list(new_fd, epfd);
		struct user new_user = {.socket = new_fd, .name = NULL, .pronouns = NULL};
		add_user(new_user, &connected);
		printf("Client with socket %i connected to the server\n", new_fd);
	}
}

int main()
{
	setvbuf (stdout, NULL, _IONBF, 0);
	connected.size = 0;
	connected.users = malloc(sizeof(struct user));
	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	pthread_t thread;
	struct sockaddr_in listen_addr = {.sin_addr = 0, .sin_port = htons(2907), .sin_family = AF_INET};
	size_t size_addr = sizeof(listen_addr);
	
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
	
	while (1)
	{
		events_ready = epoll_wait(epfd, events, MAX_EVENTS, -1);
		for (int i = 0; i < events_ready;i++)
		{
			status = recv(events[i].data.fd, buffer, 4095, 0);
			if (status == 0 || status == -1)
			{
				remove_from_list(events[i].data.fd, epfd);
				remove_user(events[i].data.fd, &connected);
				printf("User %i disconnected\n", events[i].data.fd);
				continue;
			}
			struct user *a = get_user(events[i].data.fd, &connected);
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
			printf("[%s [%s]] %s\n", a->name, a->pronouns, buffer);
			asprintf(&send_buffer, "[%s [%s]] %s", a->name, a->pronouns, buffer);
			for (int i = 0; i < connected.size; i++)
			{
				send(connected.users[i].socket, send_buffer, 1024, 0);
			}
			memset(buffer, 0, 4095);
			free(send_buffer);
			send_buffer = NULL;
		}
	}
	pthread_join(thread,NULL);
}
