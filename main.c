#include <stdlib.h>
#include <sys/socket.h>	
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

int connected_clients[1024] = {0}; //make auto scaling array
int connected_index = 0;

void *accept_thread(void *arg)
{
	int socketfd = *((int *)(arg));
	printf("Listening incomming connections at socket %i\n", socketfd);
	while (1)
	{
		int new_fd = accept(socketfd, NULL, NULL);
		connected_clients[connected_index] = new_fd;
		connected_index++;
		printf("Client with socket %i connected to the server", new_fd);
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
	}

	if (listen(socketfd, 25) == -1)
	{
		printf("Listen failed %s\n", strerror(errno));
	}
	int *i = malloc(sizeof(*i));
	*i = socketfd;
	printf("Thread cration returned %i\n", pthread_create(&thread, NULL, accept_thread, (void *) i));
	
	//main loop WIP
	int a = 0;
	while (1)
	{
		a++;
	}
	pthread_join(thread,NULL);
}
