#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8182
#define MAX_CHAR 10000

int min(int a, int b)
{
	if (a > b)
		return b;
	return a;
}

int main()
{
	struct sockaddr_in serv_addr;
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if ((sockfd) < 0)
	{
		sockfd = -1;
	}

	switch (sockfd)
	{
	case -1:
		perror("[ERROR] Unable to create the socket\n");
		exit(0);
		break;

	default:
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(PORT);
		// making connection to server
		if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		{
			perror("[ERROR] Unable to connect to server\n");
			close(sockfd);
			exit(0);
		}
		printf("[SUCCESS] Connected to Server \n");
		printf("Enter the filename : ");
		char filename[MAX_CHAR];
		scanf("%s", filename);
		int leng = strlen(filename);
		send(sockfd, filename, strlen(filename) + 1, 0);
		printf("[SUCCESS] Filename send\n");

		// READ IMAGES SENT
		char imagearray[2048];
		int counter = 0, size, recv_size = 0, stat, read_size;
		while (1)
		{
			int x = read(sockfd, filename, 4);
			if (strcmp(filename, "END") == 0)
			{
				printf("%d Number of Images were Read in the Service\n", counter);
				close(sockfd);
				return 0;
			}
			counter++;
			stat = 0;
			read_size = 0;
			size = 0;
			recv_size = 0;
			stat = read(sockfd, &size, sizeof(size));
			size = ntohl(size);
			char snum[100];
			sprintf(snum, "%d", counter);
			strcat(snum, "_.jpg");
			FILE *image = fopen(snum, "w");
			while (recv_size < size)
			{
				read_size = read(sockfd, imagearray, min((int)1024, size - recv_size));
				fwrite(imagearray, 1, read_size, image);
				recv_size += read_size;
			}
			fclose(image);
		}
		break;
	}
	return 0;
}