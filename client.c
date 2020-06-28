#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

enum cmd_type {
    GET,
    SET,
};

struct cmd {
    enum cmd_type cmd_type;
    int key_size, value_size;
    char buf[];
};

int main(void) {
    int serverfd;
    struct sockaddr_in addr;
    char buffer[] = "Hello\0World";
    char recvbuf[256] = { 0, };
    struct cmd* cmd;

    cmd = malloc(sizeof(*cmd) + 12);

    serverfd = socket(PF_INET, SOCK_STREAM, 0);
    if (serverfd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons((short)7070);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    if (connect(serverfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        return EXIT_FAILURE;
    }

    cmd->cmd_type = SET;
    cmd->key_size = 5;
    cmd->value_size = 5;
    memcpy(cmd->buf, buffer, sizeof(buffer));
    
    printf("key: %s, value: %s\n", cmd->buf, cmd->buf + 6);

    printf("send\n");

    if (send(serverfd, (void*)cmd, sizeof(*cmd) + 12, 0) < 0) {
        perror("send");
        return EXIT_FAILURE;
    }

    printf("recv\n");

    if (recv(serverfd, NULL, 0, 0) < 0) {
        perror("recv");
        return EXIT_FAILURE;
    }

    cmd->cmd_type = GET;
    printf("cmd type is get\n");

    if (send(serverfd, (void*)cmd, sizeof(*cmd) + 12, 0) < 0) {
        perror("send2");
        return EXIT_FAILURE;
    }

    printf("send2 complet\n");

    if (recv(serverfd, (void*)recvbuf, sizeof(recvbuf), 0) < 0) {
        perror("recv2");
        return EXIT_FAILURE;
    }

    printf("result: %s\n", recvbuf);

    close(serverfd);

    return EXIT_SUCCESS;
}

