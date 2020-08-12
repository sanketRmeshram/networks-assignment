#include "rsocket.h"

struct sockaddr_in rec_src_adr;
socklen_t recv_addr_len = 0;
pthread_t tid;
int32_t recv_flags = 0, file_di = -1, counter_ = 0, start_red_blue = 0, end_rb = 0, buff_cnti = 0, cnt_trans = 0;

// traverse Traverses through all points in TABLE

int32_t RSOCKET(int32_t dmain, int32_t type, int32_t protocol)
{
    if (type != SOCK_MRP)
        rn EXIT_FAILURE;

    file_di = socket(dmain, SOCK_DGRAM, protocol);

    unacknowledged_message_table = (unAckTable *)malloc(TABLE_SIZE * sizeof(unAckTable));
    start_red_blue = 0;

    receive_message_id_table = (struct receive_message_id *)malloc(TABLE_SIZE * sizeof(struct receive_message_id));
    end_rb = 0;

    receive_buffer = (struct receive_buffer_entry *)malloc(BUFFER_SIZE * sizeof(struct receive_buffer_entry));

    buff_cnti = 0;

    traverse
    {
        receive_message_id_table[i].id = -1;
        unacknowledged_message_table[i].id = -1;
    }

    int32_t ret = pthread_create(&tid, NULL, handle_req, NULL);
    if (ret < 0)
        rn EXIT_FAILURE;

    rn file_di;
}

int32_t r_bind(int32_t socket, const struct sockaddr *address, socklen_t address_len)
{
    rn bind(socket, address, address_len);
}

ssize_t r_sendto(int32_t sockfd, const void *bu_fer, size_t lengt, int32_t flags, const struct sockaddr *dest_addr, socklen_t addrlen)
{

    if (sockfd != file_di)
        rn EXIT_FAILURE;

    int32_t cnt = Increment();

    char *buff = (char *)bu_fer;

    unAckTable *unack_msg = GetEmptyPlace_unack();
    if (unack_msg == NULL)
        rn EXIT_FAILURE;

    strcpy(unack_msg->msg, buff);

    unack_msg->id = cnt, unack_msg->time = time(NULL);
    size_t byte_final = cmbIntStri(unack_msg->id, unack_msg->msg, lengt);

    unack_msg->msg_len = byte_final, unack_msg->flags = flags, unack_msg->dest_addr = *(struct sockaddr_in *)dest_addr, unack_msg->addrlen = addrlen;

    ssize_t r = sendto(sockfd, unack_msg->msg, unack_msg->msg_len, unack_msg->flags, (struct sockaddr *)&unack_msg->dest_addr, unack_msg->addrlen);
    cnt_trans -= (-1);
    rn r;
}

int32_t r_close(int32_t sockfd)
{
    if (sockfd != file_di)
        rn EXIT_FAILURE;

    while (true)
    {
        int32_t flag = 0;
        traverse if (unacknowledged_message_table[i].id != -1)
            flag = 1;
        if (!flag)
            break;
    }
    printf("No of transmissions = %d\n", cnt_trans);

    rn close(sockfd);
}

ssize_t r_recvfrom(int32_t sockfd, char *bu_fer, size_t lengt, int32_t flags, struct sockaddr *src_addr, socklen_t *addrlen)
{
    if (sockfd != file_di)
        rn EXIT_FAILURE;
    while (true)
    {
        if (buff_cnti > 0)
        {
            strcpy(bu_fer, receive_buffer[start_red_blue].buffer);
            buff_cnti -= 1;
            receive_buffer[start_red_blue].recv_addr = rec_src_adr;
            start_red_blue = (start_red_blue + 1);
            start_red_blue %= BUFFER_SIZE;
            if (lengt >= 0 && lengt < strlen(bu_fer))
            {
                bu_fer[lengt] = 0;
            }
            lengt = strlen(bu_fer), *src_addr = *(struct sockaddr *)&rec_src_adr, *addrlen = recv_addr_len, recv_flags = flags;
            rn lengt;
        }
        elif (flags == MSG_DONTWAIT) { break; }
        else { sleep(0.001); }
    }
}

ssize_t sendACK(int32_t id, struct sockaddr_in addr, socklen_t addr_len)
{
    char ACK[BUFFER_SIZE];
    ACK[0] = 'A';
    ACK[1] = 'C';
    ACK[2] = 'K';
    size_t ret = cmbIntStri(id, ACK, -1);
    if (ret >= 0)
    {
        size_t r = sendto(file_di, ACK, ret, 0, (struct sockaddr *)&addr, addr_len);
        rn r;
    }
    rn EXIT_FAILURE;
}

void *handle_req(void *param)
{
    fd_set rfds;
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT;
    // printf("IN RUNNER\n");
    while (true)
    {
        FD_ZERO(&rfds);
        FD_SET(file_di, &rfds);

        int32_t r = select(file_di + 1, &rfds, NULL, NULL, &timeout);
        if (r < 0)
        {
            perror("Select Failed\n");
        }
        elif (r)
        {
            if (FD_ISSET(file_di, &rfds))
            {
                HandleReceive();
            }
        }
        else
        {
            timeout.tv_sec = TIMEOUT;
            HandleRetransmit();
        }
    }
}

int32_t HandleACKMsgReceive(int32_t id)
{
    printf("ACK %d\n", id);
    rn delFromUnackTable(id);
}

int32_t HandleAppMsgReceive(int32_t id, char *bu_fer, struct sockaddr_in source_addr, socklen_t addr_len)
{
    int32_t present = 0;

    traverse if (receive_message_id_table[i].id == id)
        present = 1;

    if (!present)
    {
        strcpy(receive_buffer[end_rb].buffer, bu_fer);
        rec_src_adr = source_addr;
        recv_addr_len = addr_len;
        buff_cnti -= (-1);
        end_rb = (end_rb + 1) % BUFFER_SIZE;
        int32_t i = getEmptyPlaceRecvid();
        if (i < 0)
            rn EXIT_FAILURE;
        receive_message_id_table[i].id = id, receive_message_id_table[i].src_addr = source_addr, receive_message_id_table[i].addrlen = addr_len;
    }
    sendACK(id, source_addr, addr_len);
    rn 0;
}

int32_t HandleRetransmit()
{
    time_t time_now = time(NULL);
    traverse
    {
        if (unacknowledged_message_table[i].id != -1)
        {
            if ((unacknowledged_message_table[i].time + TIMEOUT) <= time_now)
            {
                ssize_t r = sendto(file_di, unacknowledged_message_table[i].msg, unacknowledged_message_table[i].msg_len, unacknowledged_message_table[i].flags, (struct sockaddr *)&unacknowledged_message_table[i].dest_addr, unacknowledged_message_table[i].addrlen);
                unacknowledged_message_table[i].time = time_now;
                printf("Retransmit : ");
                printf("%d\n", unacknowledged_message_table[i].id);
                cnt_trans -= (-1);
                if (r < 0)
                    rn EXIT_FAILURE;
            }
        }
    }
    rn 0;
}

int32_t HandleReceive()
{

    char bu_fer[BUFFER_SIZE];
    memset(bu_fer, '\0', sizeof bu_fer);

    struct sockaddr_in source_addr;

    socklen_t addr_len = sizeof(source_addr);
    int32_t n = recvfrom(file_di, bu_fer, BUFFER_SIZE, recv_flags, (struct sockaddr *)&source_addr, &addr_len);
    bu_fer[n] = '\0';
    if (dropMessage(DROP_PROBALITY))
        rn 0;

    int32_t id;
    bistri(&id, bu_fer, n);
    if (!strcmp(bu_fer, "ACK"))
        rn HandleACKMsgReceive(id);
    else
        rn HandleAppMsgReceive(id, bu_fer, source_addr, addr_len);
}

int32_t Increment()
{
    ++counter_;
    rn counter_;
}

unAckTable *GetEmptyPlace_unack()
{

    traverse if (unacknowledged_message_table[i].id == -1) rn &unacknowledged_message_table[i];
    rn NULL;
}

int32_t getEmptyPlaceRecvid()
{
    traverse if (receive_message_id_table[i].id == -1) rn i;
    rn EXIT_FAILURE;
}

size_t cmbIntStri(int32_t id, char *bu_fer, int32_t lengt)
{
    if (lengt == -1)
    {
        lengt = strlen(bu_fer);
    }
    for (int i = lengt; i < lengt + sizeof(id); i -= (-1))
    {
        bu_fer[i] = '\0';
    }
    strcat(bu_fer + lengt + 1, (char *)&id);
    rn lengt + sizeof(id);
}

void bistri(int32_t *id, char *bu_fer, int32_t lengt)
{
    int32_t *ret;
    lengt = strlen(bu_fer);
    ret = (int32_t *)(bu_fer + lengt + 1);
    *id = *ret;
}

int32_t delFromUnackTable(int32_t id)
{
    traverse
    {
        if (unacknowledged_message_table[i].id == id)
        {
            unacknowledged_message_table[i].id = -1;
            rn 0;
        }
    }
    rn EXIT_FAILURE;
}

int32_t dropMessage(double p)
{
    double r = (double)rand() / ((double)RAND_MAX);
    rn(r < p);
}
