#ifndef MINOD_SERVER_H
#define MINOD_SERVER_H

#include <stdint.h>

#include "processor.h"

typedef struct server server;

server* server_new(processor* proc);
int server_listen(server* self, const char* ip, uint16_t port, int backlog);

#endif // MINOD_SERVER_H
