#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr = {.sin_addr = inet_addr("127.0.0.1"), .sin_port = htons(2907), .sin_family = AF_INET};
	int addr_size = sizeof(addr);
	char buffer[1024];
	char *read_buf = calloc(1024, sizeof(char));
	char *cmd_buffer;

	if (connect(sock, (struct sockaddr *)&addr, addr_size) == -1)
	{
		printf("Connect failed!");
		return -1;
	}

	printf("What is your name: ");
	fgets(buffer, 1023, stdin);
	buffer[strcspn(buffer, "\n")] = 0;
	asprintf(&cmd_buffer, "/name %s", buffer);
	send(sock, cmd_buffer, 1024, 0);
	free(cmd_buffer);
	memset(buffer, 0, 1024);
	printf("What are your pronouns: ");
	fgets(buffer, 1023, stdin);
	buffer[strcspn(buffer, "\n")] = 0;
	asprintf(&cmd_buffer, "/pronouns %s", buffer);
	send(sock, cmd_buffer, 1024, 0);
	free(cmd_buffer);
	memset(buffer, 0, 1024);

	while(1)
	{
		fgets(buffer, 1023, stdin);
		buffer[strlen(buffer)-1] = '\0';
		send(sock, buffer, 1024, 0);
		if (buffer[0] != '/')
		{
			recv(sock, read_buf, 1024, 0);
			printf("s\n", read_buf);
			memset(read_buf, 0, 1024);
		}
	}
}
