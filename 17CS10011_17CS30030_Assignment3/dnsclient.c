#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8182
#define MAX_LINE 1024
#define MAX_CHAR 100

int main()
{
	int sockfd;
	struct sockaddr_in serv_addr;
	char hostname[MAX_CHAR];
	char buffer[MAX_CHAR];
	int n;
	socklen_t len;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
		sockfd = -1;
	switch (sockfd)
	{
	case -1:
		perror("[ERROR} Unable to create the socket\n");
		exit(1);
		break;

	default:
		printf("Enter the hostname\n");
		memset(&serv_addr, 0, sizeof(serv_addr));
		/*set 
		*/
		int set = 1;
		if (set == 1)
		{
			serv_addr.sin_addr.s_addr = INADDR_ANY;
			serv_addr.sin_family = AF_INET;
		}
		serv_addr.sin_port = htons(PORT);
		scanf("%s", hostname);
		sendto(sockfd, (const char *)hostname, strlen(hostname) + 1, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
		len = sizeof(serv_addr);
		n = recvfrom(sockfd, (char *)buffer, MAX_LINE, 0, (struct sockaddr *)&serv_addr, &len);
		if (n < 0)
		{
			printf("ERROR\n");
		}
		else
		{
			buffer[n] = '\0';
			printf("IP for %s : %s\n", hostname, buffer);
		}
		break;
	}
	return 0;
}
