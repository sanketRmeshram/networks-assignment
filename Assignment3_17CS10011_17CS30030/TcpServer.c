#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define MAX 100
#define PORT 8081
#define SA struct sockaddr
#define CHUNK_SERVER_SIZE 10

void fileTransfer(int sockfd)
{
        char buff[MAX];
        int n = read(sockfd, buff, sizeof(buff));
        buff[n] = '\0';
        int fileDesc = open(buff, O_RDONLY);
        if (fileDesc < 0)
        {
                printf("Closing connection as File %s NotFound\n", buff);
                return;
        }
        printf("Sending File: %s's Contents\n", buff);
        int sz = 1;

        while (sz != 0)
        {
                sz = read(fileDesc, buff, CHUNK_SERVER_SIZE);
                buff[sz] = '\0';
                send(sockfd, buff, sz, 1);
        }
        return;
}

int main()
{
        int sockfd, connfd, len;
        struct sockaddr_in servaddr, cli;

        // socket create and verification
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1)
        {
                perror("socket creation failed...\n");
                exit(0);
        }
        else
        {
                printf("\nServer Running../ \n");
        }
        bzero(&servaddr, sizeof(servaddr));

        // assign IP, PORT
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(PORT);

        // Binding newly created socket to given IP and verification
        if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
        {
                printf("socket bind failed...\n");
                exit(0);
        }
        else
        {
                printf("Socket successfully binded..\n");
        }

        // Now server is ready to listen and verification
        if ((listen(sockfd, 5)) != 0)
        {
                printf("Listen failed...\n");
                exit(0);
        }
        else
        {
                printf("Server listening..\n");
        }
        len = sizeof(cli);

        while (1)
        {
                connfd = accept(sockfd, (SA *)&cli, &len);
                if (connfd < 0)
                {
                        printf("server acccept failed...\n");
                        exit(0);
                }
                else
                {
                        printf("server acccept the client...\n");
                }
                fileTransfer(connfd);
                close(connfd); // Sends EOF Notification to Client as we it's work is Done.
        }
        return 0;
}