#include "rsocket.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#define MAXLINE 100
#define ROLLNO 10011

int main()
{

    struct sockaddr_in m1_addr;
    struct sockaddr_in m2_addr;
    char buffer[MAXLINE];
    socklen_t lengt;
    memset(&m1_addr, 0, sizeof(m1_addr));
    memset(&m2_addr, 0, sizeof(m2_addr));
    int sockfd = RSOCKET(AF_INET, SOCK_MRP, 0);
    m2_addr.sin_family = AF_INET, m2_addr.sin_port = htons(5000 + 2 * ROLLNO), m2_addr.sin_addr.s_addr = INADDR_ANY;
    r_bind(sockfd, (const struct sockaddr *)&m2_addr, sizeof(m2_addr));
    lengt = sizeof(m1_addr);
    int i = 0;
    while (i < MAXLINE)
    {
        r_recvfrom(sockfd, (char *)buffer, MAXLINE, 0, (struct sockaddr *)&m1_addr, &lengt);
        printf("%s\n", buffer);
        i++;
    }
    printf("\n");
    return 0;
}
