#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_CHAR 100

typedef char string[MAX_CHAR + 1];

#define MAX_WORDS 100
#define MAX_LINE 1024
#define PORT 8182

int max(int a, int b)
{
	if (a > b)
		return a;
	else if (b >= a)
		return b;
}

void send_image(int socket, char *pic)
{
	FILE *picture = fopen(pic, "r");
	if (picture == NULL)
	{
		printf("ERROR\n");
		return;
	}
	char endwith[] = "&&&";
	send(socket, endwith, 4, 0);
	fseek(picture, 0, SEEK_END);
	int size = ftell(picture);
	fseek(picture, 0, SEEK_SET);
	int converted_number = htonl(size);
	write(socket, &converted_number, sizeof(int));
	char send_buffer[2048];
	int read_size;
	int stat;
	int ssz = 0;
	while (!feof(picture))
	{
		read_size = fread(send_buffer, 1, sizeof(send_buffer) - 1, picture);
		int ssi = write(socket, send_buffer, read_size);
		bzero(send_buffer, sizeof(send_buffer));
	}
}

void udpreceive(int sockfd, char buffer[], struct sockaddr_in *cliaddr)
{
	socklen_t len;
	int n;
	len = sizeof(*cliaddr);
	n = recvfrom(sockfd, (char *)buffer, MAX_LINE, 0, (struct sockaddr *)cliaddr, &len);
	buffer[n] = '\0';
}

int hostname_to_ip(char *hostname, char *ip)
{
	struct hostent *he;
	struct in_addr **addr_list;
	int i;

	while ((he = gethostbyname(hostname)) == NULL)
	{
		herror("gethostbyname");
		return 1;
	}

	addr_list = (struct in_addr **)he->h_addr_list;
	i = 0;
	while (addr_list[i] != NULL)
	{
		strcat(ip, inet_ntoa(*addr_list[i]));
		strcat(ip, ", ");
		i++;
	}

	return 1;
}
int main()
{
	int sockfd1, sockfd2, newsockfd, nfds;
	socklen_t clilen1, clilen2;
	struct sockaddr_in cli_add1, cli_add2, serv_addr;
	fd_set readSockSet;
	// struct timeval timeout;
	//creating socket 1
	sockfd1 = socket(AF_INET, SOCK_STREAM, 0);
	if ((sockfd1) < 0)
	{
		perror("[ERROR] Unable to create the TCP socket\n");
		exit(1);
	}

	// creating socket 2
	sockfd2 = socket(AF_INET, SOCK_DGRAM, 0);
	if ((sockfd2) < 0)
	{
		sockfd2 = -1;
	}
	switch (sockfd2)
	{
	case -1:
		perror("[ERROR] Unable to create the UDP socket\n");
		exit(1);
		break;

	default:
		memset(&serv_addr, 0, sizeof(serv_addr));
		memset(&cli_add1, 0, sizeof(cli_add1));
		memset(&cli_add2, 0, sizeof(cli_add2));

		// setting server
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(PORT);

		int z = bind(sockfd1, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
		if (z < 0)
		{
			perror("[ERROR] Unable to bind the TCP socket\n");
			exit(1);
		}
		else
		{
			z = bind(sockfd2, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
			if (z < 0)
			{
				perror("[ERROR] Unable to bind the UDP socket\n");
				exit(1);
			}
			else
			{
				listen(sockfd1, 5);
				for (; 1;)
				{
					clilen1 = sizeof(cli_add1);
					clilen2 = sizeof(cli_add2);
					if (1)
					{
						FD_ZERO(&readSockSet);
						FD_SET(sockfd1, &readSockSet);
						FD_SET(sockfd2, &readSockSet);
					}
					nfds = max(sockfd1, sockfd2) + 1;
					int ret = select(nfds, &readSockSet, 0, 0, 0);
					if (ret < 0)
						ret = -1;
					switch (ret)
					{
					case -1:
						perror("[ERROR] Unable to make select call\n");
						exit(1);
						break;

					default:
						printf("\nSelected\n");
						if (FD_ISSET(sockfd1, &readSockSet))
						{
							printf("TCP socket is chosen\n");
							pid_t child;
							int i; // iterator
							if ((child = fork()) == 0)
							{
								newsockfd = accept(sockfd1, (struct sockaddr *)&cli_add1, &clilen1);
								if ((newsockfd) < 0)
								{
									perror("[ERROR] Can't connect to TCP\n");
									exit(1);
								}
								else
								{
									char filename[MAX_CHAR], temp_buf[MAX_CHAR];
									for (i = 0; i < MAX_CHAR; i++)
									{
										filename[i] = temp_buf[i] = '\0';
									}
									int sz = 0, total_sz = 0;
									total_sz = sz = read(newsockfd, filename, MAX_CHAR);
									printf("%s\n", filename);
									char x[] = "images/";
									strcat(x, filename);
									printf("%s\n", x);
									DIR *d;
									struct dirent *dir;
									d = opendir(x);
									if (d)
									{
										printf("FOUND\n");
										while ((dir = readdir(d)) != NULL)
										{
											int is = 0;
											for (i = 3; i < strlen(dir->d_name); i++)
											{
												if (dir->d_name[i - 3] == '.' && dir->d_name[i - 2] == 'j' && dir->d_name[i - 1] == 'p' && dir->d_name[i] == 'g')
												{
													is = 1;
												}
											}
											if (is == 0)
												continue;
											char pic[100];
											for (i = 0; x[i] != '\0'; i++)
											{
												pic[i] = x[i];
											}
											pic[i] = '/';
											i++;
											int j;
											for (j = 0; j < strlen(dir->d_name); j++)
											{
												pic[i + j] = dir->d_name[j];
											}
											pic[i + j] = '\0';
											printf("Sending : %s\n", pic);
											send_image(newsockfd, pic);
										}
										// Send End Message
										char ENDOFSERVICE[] = "END";
										send(newsockfd, ENDOFSERVICE, 4, 0);
										closedir(d);
									}
									else
									{
										printf("No such Directory\n");
									}
								}
							}
						}
						if (FD_ISSET(sockfd2, &readSockSet))
						{
							int set = 1;
							char hostname[MAX_LINE];
							if (set == 1)
							{
								printf("UDP socket is chosen\n");
								udpreceive(sockfd2, hostname, &cli_add2);
							}
							char ip[1000];
							int i = 0;
							while (i < 1000)
							{
								ip[i] = '\0';
								i++;
							}
							hostname_to_ip(hostname, ip);
							sendto(sockfd2, (const char *)ip, strlen(ip) + 1, 0, (const struct sockaddr *)&cli_add2, clilen2);
							printf("[SUCCESS] IP(s) Send\n");
						}
						break;
					}
				}
			}
		}
		break;
	}

	return 0;
}