#ifndef RSOCKET_H
#define RSOCKET_H
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#define _POSIX_C_SOURCE 200809L
#define MSG_SIZE 100
#define TABLE_SIZE 100
#define TIMEOUT 2
#define DROP_PROBALITY 0.2
#define SOCK_MRP 153
#define BUFFER_SIZE 100

int dropMessage(double p);

ssize_t r_recvfrom(int sockfd, char *bu_fer, size_t lengt, int flags, struct sockaddr *src_addr, socklen_t *addrlen);

int RSOCKET(int dmain, int type, int protocol);

void *handle_req(void *param);

int r_close(int fd);

int r_bind(int socket, const struct sockaddr *address, socklen_t address_len);

ssize_t r_sendto(int sockfd, const void *bu_fer, size_t lengt, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);

struct receive_buffer_entry
{
        char buffer[MSG_SIZE];
        struct sockaddr_in recv_addr;
} * receive_buffer;

struct receive_message_id
{
        int id;
        struct sockaddr_in src_addr;
        socklen_t addrlen;
} * receive_message_id_table;

typedef struct unacknowledged_message
{
        int id;
        char msg[MSG_SIZE];
        size_t msg_len;
        time_t time;
        int flags;
        struct sockaddr_in dest_addr;
        socklen_t addrlen;
} unAckTable;
unAckTable *unacknowledged_message_table;
int Increment();
int HandleRetransmit();
unAckTable *GetEmptyPlace_unack();

void bistri(int *id, char *bu_fer, int lengt);
int delFromUnackTable(int id);
int getEmptyPlaceRecvid();

int HandleACKMsgReceive(int id);
size_t cmbIntStri(int id, char *bu_fer, int lengt);

int HandleAppMsgReceive(int id, char *bu_fer, struct sockaddr_in source_addr, socklen_t addr_len);

int HandleReceive();

#endif
#define true 1
#define rn return
#define elif else if
#define traverse for (int i = 0; i < TABLE_SIZE; i++)
#define fill(x) memset(x, 0, sizeof(x));
