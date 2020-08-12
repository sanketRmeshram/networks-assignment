#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define MAX 100
#define PORT 8081
#define SA struct sockaddr
#define _SIZE 1000000
#define READ_BUFF_SIZE 5

int checkConn(int sockfd, int *ptr, char *str)
{
        char buff[MAX];
        bzero(buff, sizeof(buff));
        int szR = read(sockfd, buff, READ_BUFF_SIZE);
        // Basically Close from TCPServer should send EOF for FileNotFound Case
        if (szR == EOF)
        {
                printf("File Not Found\n");
                return 0;
        }
        int i = (*ptr);
        int fd = open("client.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        write(fd, buff, szR);
        int iz = 0;
        for (; iz < szR; iz++)
                str[iz] = buff[iz];
        (*ptr) += szR;
        return fd;
}

int countWords(char *buff, int size)
{
        // Replacing All sepreaters by ' '
        int i;
        for (i = 0; i < size; i++)
        {
                if (buff[i] == ',')
                {
                        buff[i] = ' ';
                }
                if (buff[i] == ';')
                {
                        buff[i] = ' ';
                }
                if (buff[i] == ':')
                {
                        buff[i] = ' ';
                }
                if (buff[i] == '.')
                {
                        buff[i] = ' ';
                }
                if (buff[i] == '\n')
                {
                        buff[i] = ' ';
                }
                if (buff[i] == '\t')
                {
                        buff[i] = ' ';
                }
        }
        int idx = 0;
        int ans = 0;
        while (idx < size)
        {
                while (idx < size && (buff[idx] == ' '))
                        idx++;
                if (idx == size)
                        break;
                ans++;
                while (idx < size && buff[idx] != ' ')
                        idx++;
        }
        return ans;
}

int main()
{
        int sockfd, connfd;
        struct sockaddr_in servaddr, cli;

        // socket create and varification
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1)
        {
                printf("socket creation failed...\n");
                exit(0);
        }
        else
        {
                printf("Socket successfully created..\n");
        }
        bzero(&servaddr, sizeof(servaddr));

        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = INADDR_ANY;
        servaddr.sin_port = htons(PORT);
        // connect the client socket to server socket
        if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
        {
                printf("connection with the server failed...\n");
                exit(0);
        }
        else
        {
                printf("connected to the server..\n");
        }

        // Actual Problem Statement Starts

        char buff[MAX];
        char store[_SIZE];
        int n;
        bzero(buff, sizeof(buff));
        printf("Enter the File Name : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
                ;
        buff[n - 1] = '\0';
        send(sockfd, buff, n, 1); // Send the file Name

        int ptr = 0;
        int fd = checkConn(sockfd, &ptr, store); // Checking if it is still Connected
        if (fd == 0)
        {
                return 0;
        }
        while (1)
        {
                bzero(buff, sizeof(buff));
                int sz = read(sockfd, buff, READ_BUFF_SIZE);
                if (sz <= 0)
                {
                        break;
                }
                int i = ptr;
                write(fd, buff, sz);
                for (int i = 0; i < sz; i++)
                        store[i + ptr] = buff[i];
                ptr += sz;
        }
        printf("File Written Successfully\nNo of Bytes received is : %d,", ptr);
        store[ptr] = '\0';
        int ans = countWords(store, ptr);
        printf("No of Words : %d\n", ans);
        close(sockfd);
        return 0;
}