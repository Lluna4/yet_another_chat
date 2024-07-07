#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/ssl.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>
#define MAX_EVENTS 1024

SSL *ssl;
int epfd;

char *prompt(char *question, size_t size)
{
    char *buffer = calloc(size, sizeof(char));
    if (buffer == NULL)
        return NULL;
    printf("%s", question);
    fgets(buffer, size, stdin);
    size_t size2 = strcspn(buffer, "\n");
    if (size2 > size)
        buffer[strlen(buffer)-1] = 0;
    else
        buffer[size2] = 0;
    return buffer;
}

void prompt_send(char *question, char *cmd)
{
    char *cmd_buffer;
    char *buffer = calloc(1024, sizeof(char));
    if (buffer == NULL)
        return;
    printf("%s", question);
    fgets(buffer, 1023, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    asprintf(&cmd_buffer, "%s %s", cmd, buffer);
    SSL_write(ssl, cmd_buffer, strlen(cmd_buffer));
    free(cmd_buffer);
    free(buffer);
}

void *read_sock(void *arg)
{
    char *buffer = calloc(1024, sizeof(char));
    if (buffer == NULL)
        return NULL;
    int pipefd = *((int *)(arg));
    while (1)
    {
        SSL_read(ssl, buffer, 1023);
	if (strlen(buffer) > 4)
		write(pipefd, buffer, 1023);
	memset(buffer, 0, 1024);
    }
}

void *input(void *arg)
{
    char *buffer = calloc(1024, sizeof(char));
    int pipefd = *((int *)(arg));
    while(1)
    {
        fgets(buffer, 1023, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
	write(pipefd, buffer, 1023);
    }
}

void add_to_list(int fd, int epfd)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
}

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    pthread_t thread_recv;
    pthread_t thread_send;
    char *buffer = calloc(1024, sizeof(char));
    if (buffer == NULL)
        return -1;
    int fildes_recv[2];
    int fildes_send[2];


    char *address = prompt("Address: ", 12);
    if (address == NULL)
        return -1;
    char *port_str = prompt("Port: ", 6);
    if (port_str == NULL)
        return -1;
    struct sockaddr_in addr = {.sin_family = AF_INET, .sin_port = htons(atoi(port_str)), .sin_addr = inet_addr(address)};
    size_t addr_size = sizeof(addr);
    if (connect(sock, (struct sockaddr *)&addr, addr_size) == -1)
    {
        printf("connect failed: %s\n", strerror(errno));
        return -1;
    }

    SSL_CTX *ctx = SSL_CTX_new(TLS_method());
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);
    SSL_connect(ssl);
	
    epfd = epoll_create1(0);
    pipe(fildes_recv);
    pipe(fildes_send);
    int *pipefd_recv = malloc(sizeof(int));
    int *pipefd_send = malloc(sizeof(int));
    *pipefd_recv = fildes_recv[1];
    *pipefd_send = fildes_send[1];

    add_to_list(fildes_recv[0], epfd);
    add_to_list(fildes_send[0], epfd);


    
    prompt_send("What is your name? ", "/name");
    prompt_send("What are your pronouns? ", "/pronouns");

    pthread_create(&thread_recv, NULL, read_sock, (void *)pipefd_recv);
    pthread_create(&thread_send, NULL, input, (void *)pipefd_send);
    
    struct epoll_event events[MAX_EVENTS];
    int events_ready = 0;

    while(1)
    {
	events_ready = epoll_wait(epfd, events, MAX_EVENTS, -1);

	for (int i = 0; i < events_ready;i++)
	{
		if (events[i].data.fd == fildes_recv[0])
		{
			read(events[i].data.fd, buffer, 1023);
			printf("%s\n", buffer);
			memset(buffer, 0, 1024);
		}
		else if(events[i].data.fd == fildes_send[0])
		{
			read(events[i].data.fd, buffer, 1023);
			SSL_write(ssl, buffer, 1023);
			memset(buffer, 0, 1024);
			printf("\033[A");
		}
	}
    }

    free(buffer);
    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);
    return 0;
}

