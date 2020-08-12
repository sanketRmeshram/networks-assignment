#include "rsocket.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#define MAXLINE 100
#define ROLLNO 10011

int main()
{

    struct sockaddr_in m2_addr;
    socklen_t lengt;
    char buffer[MAXLINE];

    int sockfd = RSOCKET(AF_INET, SOCK_MRP, 0);
    scanf("%s", buffer);
    memset(&m2_addr, 0, sizeof(m2_addr));

    m2_addr.sin_family = AF_INET, m2_addr.sin_port = htons(5000 + 2 * ROLLNO), m2_addr.sin_addr.s_addr = INADDR_ANY;

    int i = 0;
    while (i < strlen(buffer))
    {
        lengt = sizeof(m2_addr);
        r_sendto(sockfd, (const char *)(buffer + i), 1, 0, (const struct sockaddr *)&m2_addr, lengt);
        i++;
    }
    r_close(sockfd);
    return 0;
}
