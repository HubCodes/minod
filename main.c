#include "hashmap.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

enum {
    IP_PROTO = 0,
    LISTEN_PORT = 7070,
    TCP_BACKLOG = 65535,
};

enum cmd_type {
    GET,
    SET,
};

struct cmd {
    enum cmd_type cmd_type;
    int key_size, value_size;
    char buf[];
};

int main(int argc, char** argv) {
    int sockfd, clientfd;
    struct sockaddr clientaddr;
    socklen_t clientaddr_len;
    struct sockaddr_in addr;
    char buffer[256] = { 0, };
    Map* map;

    map = Map_new();

    sockfd = socket(PF_INET, SOCK_STREAM, IP_PROTO);
    if (sockfd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons((short)LISTEN_PORT);
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return EXIT_FAILURE;
    }

    if (listen(sockfd, TCP_BACKLOG) < 0) {
        perror("listen");
        return EXIT_FAILURE;
    }

    if ((clientfd = accept(sockfd, &clientaddr, &clientaddr_len)) < 0) {
        perror("accept");
        return EXIT_FAILURE;
    }

    for (;;) {
        struct cmd* cmd;
        int valuesize;
        void* value;

        if (recv(clientfd, (void*)buffer, 256, 0) < 0) {
            perror("recv");
            return EXIT_FAILURE;
        }
        cmd = (struct cmd*)buffer;
        if (cmd->cmd_type == GET) {
            printf("getting\n");
            printf("getting key: %s\n", cmd->buf);
            value = Map_get(map, cmd->buf, &valuesize);
            if (value == NULL) {
                printf("null!\n");
                send(clientfd, "", 0, 0);
            } else {
                send(clientfd, value, valuesize, 0);
            }
            close(clientfd);
            break;
        } else if (cmd->cmd_type == SET) {
            printf("setting\n");
            printf("key size: %d, value_size: %d\n", cmd->key_size, cmd->value_size);
            printf("key: %s\n", cmd->buf);
            Map_set(map, cmd->buf, cmd->buf + cmd->key_size + 1, cmd->value_size);
            send(clientfd, "", 0, 0);
        }
        memset(buffer, 0, sizeof(buffer));
    }

    return EXIT_SUCCESS;
}

