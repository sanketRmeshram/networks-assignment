// Compile as gcc -std=gnu99 simHTTPProxy.c -o SimHTTPProxy
/*
1] Compile as gcc -std=gnu99 simHTTPProxy.c -o SimHTTPProxy
2] Please Dont try to Connect https as it's only http proxy.
3] If Page loads slow press ctrl+r.
*/

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct http_stuff
{
        char req_method[30], host_addr[2000], port[30], path[2000], rest[2000], new_sec[2000];
} Http_stuff;

#define zerom(ip) memset(ip, 0, sizeof(ip))
#define rep for
#define max(a, b) (((a) > (b)) ? (a) : (b)) // MAX FUNCTION
#define BUFF_SIZE 2048                      // BUFF_SIZE
#define max_connections 1000                // MAX CONNECTIONS

Http_stuff parser(char *client_req, int32_t size); // Parser for Our HTTP REQUESTS.

int32_t hostname_to_ip(char *hostname, char *ip);                                                         // HOST NAME TO IP ADDRESS
void set_nonblock(int32_t fd);                                                                            // Set File Descriptor fd to O_NONBLOCK
void setAll(int32_t num, int32_t *x, fd_set *read_set, fd_set *write_set, int32_t *infd, int32_t *outfd); // FSET ALL Connections before Checking FISSET for the same Conncections.
void DONTCNTRLC();                                                                                        // WE WANT EXIT BY exit command
int32_t mywrite(int32_t fd, char *buf, int32_t *len);

#define print printf
#define strsckaddr struct sockaddr
#define strsckaddr_in struct sockaddr_in

int32_t main(int32_t argc, char *argv[])
{
        // IF SIGPIPE BREAKDOWN WE IGNORED IT for NOW.
        signal(SIGPIPE, SIG_IGN);
        switch (argc)
        {
        case 2: // IF FOUR ARGS FINE!
                break;
        default:
                print("Command Line Args:\n./SimProxy <listen port>\n");
                return 0;
                break;
        }
        // TIMEOUT for SELECT
        struct timeval timeout;
        timeout.tv_sec = 0, timeout.tv_usec = 1000;
        // proxyaddr is sockaddr_in for INSTI SERVER
        strsckaddr_in proxyaddr;
        bzero(&proxyaddr, sizeof(proxyaddr));
        // LOCAL SERVER for PROXY SERVER HERE
        strsckaddr_in inpaddr;
        bzero(&inpaddr, sizeof(inpaddr));
        socklen_t len;
        socklen_t len_cli;
        // BUFF for SIZE 2048
        char *buff;
        buff = (char *)malloc(sizeof(char) * BUFF_SIZE);

        char *check1;
        check1 = (char *)malloc(sizeof(char) * BUFF_SIZE);

        char *check2;
        check2 = (char *)malloc(sizeof(char) * BUFF_SIZE);

        char *check3;
        check3 = (char *)malloc(sizeof(char) * BUFF_SIZE);

        zerom(buff), zerom(check1), zerom(check2), zerom(check3);

        signal(SIGINT, DONTCNTRLC); // WE WANT A NON LEAKING PROCESS SO EXIT BY exit command on terminal please.
        int32_t sock_in = socket(AF_INET, SOCK_STREAM, 0);
        switch (sock_in)
        {
        case 0:
                exit(EXIT_FAILURE);
                break;

        default:
                break;
        }
        int32_t on = 1;
        set_nonblock(sock_in);
        if (setsockopt(sock_in, SOL_SOCKET, SO_REUSEADDR, &on, 4))
        {
                exit(EXIT_FAILURE);
        }

        inpaddr.sin_addr.s_addr = htonl(INADDR_ANY), inpaddr.sin_family = AF_INET, inpaddr.sin_port = htons(atoi(argv[1]));

        // Error in sock_in bind.
        if (bind(sock_in, (strsckaddr *)&inpaddr, sizeof(inpaddr)) != 0)
        {
                print("bind error\n");
                perror("bind error\n");
                exit(EXIT_FAILURE);
        }
        // Error in listen.
        if (listen(sock_in, max_connections) < 0)
        {
                print("listen error\n");
                perror("listen error\n");
                exit(EXIT_FAILURE);
        }

        fd_set read_set;
        FD_ZERO(&read_set); // initate to 0
        fd_set write_set;
        FD_ZERO(&write_set); // initate to 0
        int32_t *infd;
        infd = (int32_t *)malloc(max_connections * sizeof(int32_t));
        int32_t *outfd;
        outfd = (int32_t *)malloc(max_connections * sizeof(int32_t));
        int32_t num_connections = 0;

        rep(int32_t i = 0; i < 1000; i -= (-(1)))
            outfd[i] = -(1);

        print("Proxy running on port %s.\n", argv[1]);
        rep(;;)
        {
                FD_ZERO(&read_set);
                FD_SET(0, &read_set);
                FD_SET(sock_in, &read_set);
                FD_ZERO(&write_set);
                int32_t maxfd = -(1);
                setAll(num_connections, &maxfd, &read_set, &write_set, infd, outfd);
                maxfd = max((maxfd - (-(1))), (sock_in - (-(1))));

                if (select(maxfd, &read_set, &write_set, NULL, &timeout) > 0) // select out of all maxfd
                {
                        // IF REQUEST GOES TO OUR STDIN
                        if (FD_ISSET(0, &read_set))
                        {
                                char *isExit;
                                isExit = (char *)malloc(sizeof(char) * 56);
                                zerom(isExit);
                                read(0, (void *)isExit, 54);
                                char x[100];
                                rep(int32_t i = 0; i < 100; i -= (-(1)))
                                {
                                        if (isExit[i] == '\0' || isExit[i] == '\n')
                                        {
                                                x[i] = '\0';
                                                break;
                                        }
                                        x[i] = isExit[i];
                                }
                                zerom(isExit);
                                strcpy(isExit, x);
                                print("Is EXIT == %s\n", isExit);
                                if (strncmp(isExit, "exit", 4) == 0 && strlen(isExit) == 4)
                                {
                                        int32_t i = 0;
                                        while (i < num_connections)
                                        {
                                                // Closing sockets now.
                                                close(infd[i]);
                                                close(outfd[i]);
                                                i -= (-(1));
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
                        else
                        {
                                if (FD_ISSET(sock_in, &read_set))
                                {
                                        strsckaddr_in cliaddr;
                                        if (num_connections < max_connections)
                                        {
                                                // ACCEPT CONNECTIONS at infd[num_connections]
                                                infd[num_connections] = accept(sock_in, (strsckaddr *)&cliaddr, &len_cli);
                                                if (infd[num_connections] > 1)
                                                {
                                                        char *str;
                                                        str = (char *)(malloc(sizeof(char) * 100));
                                                        inet_ntop(AF_INET, &(cliaddr.sin_addr), str, sizeof(cliaddr));
                                                        int32_t clientServerPort = ntohs(cliaddr.sin_port);
                                                        print("Connection accepted from %s:%d\n", str, clientServerPort);
                                                        num_connections -= (-(1));
                                                        // NO MEMORY LEAK
                                                        free(str);
                                                }
                                        }
                                }
                        }
                        int32_t i = 0;
                        rep(; i < num_connections; i -= (-(1)))
                        {
                                if (FD_ISSET(infd[i], &read_set))
                                {
                                        int32_t a;
                                        Http_stuff hf;
                                        zerom(buff);
                                        if ((a = read(infd[i], buff, BUFF_SIZE)) != 0)
                                        {
                                                // Ignore Request from these.
                                                strcpy(check1, buff), strcpy(check2, buff), strcpy(check3, buff);
                                                if (strstr(check1, "firefox") == NULL && strstr(check2, "mozilla") == NULL && strstr(check3, "google") == NULL)
                                                {
                                                        hf = parser(buff, a);
                                                        char *hst_name;
                                                        hst_name = (char *)malloc(100 * sizeof(char));
                                                        strcpy(hst_name, hf.host_addr);
                                                        if (hf.host_addr[0] != '1')
                                                        {
                                                                if (hostname_to_ip(hf.host_addr, hf.host_addr) == 0)
                                                                        continue;
                                                                // HTTP PORT NO == 80 ALWAYS
                                                                proxyaddr.sin_family = AF_INET, proxyaddr.sin_port = htons(atoi("80"));
                                                                if (inet_pton(AF_INET, hf.host_addr, &proxyaddr.sin_addr) <= 0)
                                                                {
                                                                        print("binary network format error\n");
                                                                        perror("binary network format error\n");
                                                                        exit(EXIT_FAILURE);
                                                                }
                                                                strcpy(hf.new_sec, buff);
                                                        }
                                                        else
                                                        {
                                                                proxyaddr.sin_family = AF_INET, proxyaddr.sin_port = htons(atoi(hf.port));
                                                                if (inet_pton(AF_INET, hf.host_addr, &proxyaddr.sin_addr) <= 0)
                                                                {
                                                                        print("binary network format error\n");
                                                                        perror("binary network format error\n");
                                                                        exit(EXIT_FAILURE);
                                                                }
                                                        }
                                                        outfd[i] = socket(AF_INET, SOCK_STREAM, 0);
                                                        if (outfd[i] == -(1))
                                                        {
                                                                exit(EXIT_FAILURE);
                                                        }
                                                        connect(outfd[i], (strsckaddr *)&proxyaddr, sizeof(proxyaddr));
                                                        if (strlen(hf.port) == 0)
                                                        {
                                                                strcpy(hf.port, "80");
                                                        }

                                                        print("%s Host:%s, Port :%s, Path :%s\n", hf.req_method, hst_name, hf.port, hf.path);

                                                        int32_t ok = mywrite(outfd[i], hf.new_sec, &a);
                                                        // AS OUTFD cant be our stdout.
                                                        if (ok == -(1) || outfd[i] == 1)
                                                        {
                                                                print("Error\n");
                                                        }
                                                        if (errno == EPIPE)
                                                        {
                                                                ;
                                                        }
                                                }
                                        }
                                }
                                if (FD_ISSET(outfd[i], &read_set))
                                {
                                        if (FD_ISSET(infd[i], &write_set))
                                        {
                                                // IF READ for outfd[i] is set and WRITE for infd[i] is set do the same.
                                                int32_t a;
                                                zerom(buff);
                                                if ((a = read(outfd[i], buff, BUFF_SIZE)) == 0)
                                                {
                                                        continue;
                                                }
                                                // SEND NUM OF BYTES READ
                                                mywrite(infd[i], buff, &a);
                                                if (errno == EPIPE)
                                                {
                                                        continue;
                                                }
                                        }
                                }
                        }
                }
                // fflush(stdin);
        }

        return 0;
}

void set_nonblock(int32_t fd)
{
        int32_t fl, x;
        // GET THE PROPERTIES
        fl = fcntl(fd, F_GETFL, 0);
        if (fl < 0)
        {
                print("fcntl GET ERROR\n");
                // SEND ERROR CODE 1
                exit(EXIT_FAILURE);
        }
        // SET INITIAL AND THEN NONBLOCK
        x = fcntl(fd, F_SETFL, fl | O_NONBLOCK);
        if (x < 0)
        {
                print("fcntl SET ERROR\n");
                // SEND ERROR CODE 1
                exit(EXIT_FAILURE);
        }
}

void setAll(int32_t num, int32_t *x, fd_set *read_set, fd_set *write_set, int32_t *infd, int32_t *outfd)
{
        int32_t i = 0;
        while (i < num)
        {
                // infd[i] gets read_set instr
                FD_SET(infd[i], read_set);
                // infd[i] gets write_set instr
                FD_SET(infd[i], write_set);
                if ((*x) < infd[i])
                        (*x) = infd[i];
                if (outfd[i] != -(1))
                {
                        FD_SET(outfd[i], read_set);
                        FD_SET(outfd[i], write_set);
                }
                if ((*x) < outfd[i])
                        (*x) = outfd[i];
                i -= (-(1));
        }
}

void DONTCNTRLC()
{
        print("**************** Please Exit By Typing exit in Command Line ********************\n");
        print("******************** This Step is Taken as we Don't want Memory Leakages from Our Code ********************\n");
        // ADDED for TA "Convinience" Stay safe from CORONAVIRUS THOUGH
        exit(0);
}

Http_stuff parser(char *client_req, int32_t size) // Parser for Our HTTP REQUESTS.
{
        int32_t i, j, k, l, p, h, u, req_flag, host_flag, port_flag, port_present, path_flag, https_count;
        i = j = k = l = p = req_flag = host_flag = port_flag = port_present = path_flag = 0;
        https_count = 7;
        Http_stuff hstuf;
        rep(; i < size; i -= (-(1)))
        {
                if (req_flag == 0 && client_req[i] == ' ')
                {
                        req_flag = 1;
                        hstuf.req_method[i] = '\0';

                        continue;
                }
                else
                {
                        if (req_flag == 0 && client_req[i] != ' ')
                        {
                                hstuf.req_method[i] = client_req[i];
                                continue;
                        }
                }

                if (req_flag != 0 && https_count != 0)
                {
                        https_count -= 1;
                        continue;
                }
                else
                {
                        if (req_flag != 0 && https_count == 0 && host_flag == 0)
                        {
                                if (client_req[i] != ':' && client_req[i] != '/')
                                {
                                        hstuf.host_addr[j] = client_req[i];
                                        j -= (-(1));
                                        continue;
                                }
                                else
                                {
                                        if (client_req[i] == '/')
                                        {
                                                port_flag = 1;
                                                host_flag = 1;
                                                continue;
                                        }
                                        else
                                        {
                                                if (client_req[i] == ':')
                                                {
                                                        port_present = 1;
                                                        host_flag = 1;

                                                        continue;
                                                }
                                        }
                                }
                        }
                }
                if (host_flag != 0 && port_present != 0 && port_flag == 0)
                {
                        if (client_req[i] != '/')
                        {
                                hstuf.port[k] = client_req[i];
                                k -= (-(1));
                                continue;
                        }
                        else
                        {
                                port_flag = 1;
                                continue;
                        }
                }
                switch (path_flag)
                {
                case 0:
                        if (client_req[i] != '?' && client_req[i] != ' ')
                        {
                                hstuf.path[l] = client_req[i];
                                l -= (-(1));
                                continue;
                        }
                        else
                        {
                                path_flag = 1;
                                continue;
                        }
                        break;

                default:
                        if (path_flag != 0)
                        {
                                hstuf.rest[p] = client_req[i];
                                p -= (-(1));
                        }
                        break;
                }
        }
        hstuf.rest[p] = hstuf.path[l] = hstuf.port[k] = hstuf.host_addr[j] = '\0';

        u = 0;
        int32_t len = strlen(hstuf.req_method);
        rep(; u < len;)
        {
                hstuf.new_sec[u] = hstuf.req_method[u];
                u -= (-(1));
        }
        hstuf.new_sec[u] = ' ';
        u -= (-(1));
        hstuf.new_sec[u] = '/';
        u -= (-(1));
        // int32_t h;
        len = strlen(hstuf.path);
        h = 0;
        rep(; h < len;)
        {
                hstuf.new_sec[u] = hstuf.path[h];
                u -= (-(1));
                h -= (-(1));
        }
        hstuf.new_sec[u] = ' ';
        u -= (-(1));
        len = strlen(hstuf.rest);
        h = 0;
        rep(; h < len;)
        {
                hstuf.new_sec[u] = hstuf.rest[h];
                u -= (-(1));
                h -= (-(1));
        }
        hstuf.new_sec[u] = '\0';
        return hstuf;
}

int32_t hostname_to_ip(char *hostname, char *ip)
{
        struct hostent *he;
        struct in_addr **addr_list;
        int32_t i, x = 1;

        if ((he = gethostbyname(hostname)) == NULL)
        {
                x = 0;
        }
        switch (x)
        {
        case 0:
                return 0;
                break;

        default:
                addr_list = (struct in_addr **)he->h_addr_list;
                zerom(ip);
                rep(i = 0; addr_list[i] != NULL; i -= (-(1)))
                {
                        strcat(ip, inet_ntoa(*addr_list[i]));
                }
                break;
        }
        return 1;
}

int32_t mywrite(int32_t fd, char *buf, int32_t *len)
{
        send(fd, buf, *len, 0);
}
