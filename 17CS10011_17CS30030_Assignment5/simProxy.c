// Compile as gcc -std=gnu99 simProxy.c -o SimProxy

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define max(a, b) (((a) > (b)) ? (a) : (b)) // MAX FUNCTION
#define BUFF_SIZE 2048                      // BUFF_SIZE
#define max_connections 1000                // MAX CONNECTIONS

void set_nonblock(int fd);                                                                // Set File Descriptor fd to O_NONBLOCK
void setAll(int num, int *x, fd_set *read_set, fd_set *write_set, int *infd, int *outfd); // FSET ALL Connections before Checking FISSET for the same Conncections.
void DONTCNTRLC();                                                                        // WE WANT EXIT BY exit command

int main(int argc, char *argv[])
{
        // IF SIGPIPE BREAKDOWN WE IGNORED IT FOR NOW.
        signal(SIGPIPE, SIG_IGN);
        switch (argc)
        {
        case 4: // IF FOUR ARGS FINE!
                break;

        default:
                printf("Command Line Args:\n./SimProxy <listen port> <institute_proxy_IP> <institute_proxy_port>\n");
                return 0;
                break;
        }
        // TIMEOUT FOR SELECT
        struct timeval timeout;
        timeout.tv_sec = 0, timeout.tv_usec = 1000;
        // proxyaddr is sockaddr_in for INSTI SERVER
        struct sockaddr_in proxyaddr;
        bzero(&proxyaddr, sizeof(proxyaddr));
        // LOCAL SERVER FOR PROXY SERVER HERE
        struct sockaddr_in inpaddr;
        bzero(&inpaddr, sizeof(inpaddr));
        socklen_t len;
        socklen_t len_cli;
        // BUFF FOR SIZE 2048
        char *buff;
        buff = (char *)malloc(sizeof(char) * BUFF_SIZE);
        memset(buff, '\0', sizeof(buff));
        signal(SIGINT, DONTCNTRLC); // WE WANT A NON LEAKING PROCESS SO EXIT BY exit command on terminal please.
        int sock_in = socket(AF_INET, SOCK_STREAM, 0);
        switch (sock_in)
        {
        case 0:
                exit(1);
                break;

        default:
                break;
        }
        int on = 1;
        set_nonblock(sock_in);
        if (setsockopt(sock_in, SOL_SOCKET, SO_REUSEADDR, &on, 4))
        {
                exit(1);
        }

        inpaddr.sin_addr.s_addr = htonl(INADDR_ANY), inpaddr.sin_family = AF_INET, inpaddr.sin_port = htons(atoi(argv[1]));

        proxyaddr.sin_family = AF_INET, proxyaddr.sin_port = htons(atoi(argv[3]));

        // Error in NETWORK format error.
        if (inet_pton(AF_INET, argv[2], &proxyaddr.sin_addr) <= 0)
        {
                printf("binary network format error\n");
                perror("binary network format error\n");
                exit(1);
        }

        // Error in sock_in bind.
        if (bind(sock_in, (struct sockaddr *)&inpaddr, sizeof(inpaddr)) != 0)
        {
                printf("bind error\n");
                perror("bind error\n");
                exit(1);
        }
        // Error in listen.
        if (listen(sock_in, max_connections) < 0)
        {
                printf("listen error\n");
                perror("listen error\n");
                exit(1);
        }

        fd_set read_set;
        FD_ZERO(&read_set); // initate to 0
        fd_set write_set;
        FD_ZERO(&write_set); // initate to 0
        int *infd;
        infd = (int *)malloc(max_connections * sizeof(int));
        int *outfd;
        outfd = (int *)malloc(max_connections * sizeof(int));
        int num_connections = 0;
        printf("Proxy running on port %s. Forwarding all connections to %s:%s\n", argv[1], argv[2], argv[3]);
        for (;;)
        {
                FD_ZERO(&read_set);
                FD_SET(0, &read_set);
                FD_SET(sock_in, &read_set);
                FD_ZERO(&write_set);
                int maxfd = -1;
                setAll(num_connections, &maxfd, &read_set, &write_set, infd, outfd);
                maxfd = max((maxfd + 1), (sock_in + 1));

                if (select(maxfd, &read_set, &write_set, NULL, &timeout) > 0) // select out of all maxfd
                {
                        // IF REQUEST GOES TO OUR STDIN
                        if (FD_ISSET(0, &read_set))
                        {
                                char *isExit;
                                isExit = (char *)malloc(sizeof(char) * 56);
                                read(0, (void *)isExit, 54);
                                printf("EXIT == %s\n", isExit);
                                if (strncmp(isExit, "exit", 4) == 0 && strlen(isExit) == 5)
                                {
                                        int i = 0;
                                        while (i < num_connections)
                                        {
                                                // Closing sockets now.
                                                close(infd[i]);
                                                close(outfd[i]);
                                                i -= (-1);
                                        }
                                        // NO MEMORY LEAKAGE
                                        free(buff);
                                        free(infd);
                                        free(outfd);
                                        close(sock_in);
                                        free(isExit);
                                        return 0;
                                }
                        }
                        else if (FD_ISSET(sock_in, &read_set))
                        {
                                struct sockaddr_in cliaddr;
                                if (num_connections < max_connections)
                                {
                                        // ACCEPT CONNECTIONS at infd[num_connections]
                                        infd[num_connections] = accept(sock_in, (struct sockaddr *)&cliaddr, &len_cli);
                                        if (infd[num_connections] >= 0)
                                        {
                                                char *str;
                                                str = (char *)(malloc(sizeof(char) * 100));
                                                inet_ntop(AF_INET, &(cliaddr.sin_addr), str, sizeof(cliaddr));
                                                int clientServerPort = ntohs(cliaddr.sin_port);
                                                printf("Connection accepted from %s:%d\n", str, clientServerPort);
                                                // socket to connect with INSTI SERVER
                                                outfd[num_connections] = socket(AF_INET, SOCK_STREAM, 0);
                                                if (outfd[num_connections] == -1)
                                                {
                                                        exit(1);
                                                }
                                                set_nonblock(outfd[num_connections]);
                                                connect(outfd[num_connections], (struct sockaddr *)&proxyaddr, sizeof(proxyaddr));
                                                num_connections += 1;
                                                // NO MEMORY LEAK
                                                free(str);
                                        }
                                }
                        }
                        int i = 0;
                        for (; i < num_connections; i -= (-1))
                        {
                                if (FD_ISSET(infd[i], &read_set))
                                {
                                        if (FD_ISSET(outfd[i], &write_set))
                                        {
                                                // IF READ FOR infd[i] is set and WRITE for outfd[i] is set do the same.
                                                int a;
                                                memset(buff, '\0', sizeof(buff));
                                                if ((a = read(infd[i], buff, BUFF_SIZE)) == 0)
                                                {
                                                        continue;
                                                }
                                                // SEND NUM OF BYTES READ
                                                send(outfd[i], buff, a, 0);
                                                if (errno == EPIPE)
                                                {
                                                        continue;
                                                }
                                        }
                                }

                                if (FD_ISSET(outfd[i], &read_set))
                                {
                                        if (FD_ISSET(infd[i], &write_set))
                                        {
                                                // IF READ FOR outfd[i] is set and WRITE for infd[i] is set do the same.
                                                int a;
                                                memset(buff, '\0', sizeof(buff));
                                                if ((a = read(outfd[i], buff, BUFF_SIZE)) == 0)
                                                {
                                                        continue;
                                                }
                                                // SEND NUM OF BYTES READ
                                                send(infd[i], buff, a, 0);
                                                if (errno == EPIPE)
                                                {
                                                        continue;
                                                }
                                        }
                                }
                        }
                }
        }

        return 0;
}

void set_nonblock(int fd)
{
        int fl, x;
        // GET THE PROPERTIES
        fl = fcntl(fd, F_GETFL, 0);
        if (fl < 0)
        {
                printf("fcntl GET ERROR\n");
                // SEND ERROR CODE 1
                exit(1);
        }
        // SET INITIAL AND THEN NONBLOCK
        x = fcntl(fd, F_SETFL, fl | O_NONBLOCK);
        if (x < 0)
        {
                printf("fcntl SET ERROR\n");
                // SEND ERROR CODE 1
                exit(1);
        }
}

void setAll(int num, int *x, fd_set *read_set, fd_set *write_set, int *infd, int *outfd)
{
        int i = 0;
        while (i < num)
        {
                // infd[i] gets read_set instr
                FD_SET(infd[i], read_set);
                // infd[i] gets write_set instr
                FD_SET(infd[i], write_set);
                if ((*x) < infd[i])
                        (*x) = infd[i];
                // outfd[i] gets read_set instr
                FD_SET(outfd[i], read_set);
                // outfd[i] gets write_set instr
                FD_SET(outfd[i], write_set);
                if ((*x) < outfd[i])
                        (*x) = outfd[i];
                i -= (-1);
        }
}

void DONTCNTRLC()
{
        printf("**************** Please Exit By Typing exit in Command Line ********************\n");
        printf("******************** This Step is Taken as we Don't want Memory Leakages from Our Code ********************\n");
}