#include <stdlib.h>
#include <sys/socket.h>	
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/epoll.h>
#define MAX_EVENTS 1024

int connected_clients[1024] = {0}; //make auto scaling array
int connected_index = 0;
int epfd = 0;

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
		printf("Client with socket %i connected to the server\n", new_fd);
	}
}

int main()
{
	setvbuf (stdout, NULL, _IONBF, 0);
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
	
	while (1)
	{
		events_ready = epoll_wait(epfd, events, MAX_EVENTS, -1);
		for (int i = 0; i < events_ready;i++)
		{
			status = recv(events[i].data.fd, buffer, 4095, 0);
			if (status == 0 || status == -1)
			{
				remove_from_list(events[i].data.fd, epfd);
				continue;
			}
			printf("message received %s\n", buffer);
			memset(buffer, 0, 4096);
		}
	}
	pthread_join(thread,NULL);
}
