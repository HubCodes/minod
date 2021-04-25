#include "server.h"

#include "alloc.h"
#include "buf_queue.h"
#include "util.h"

#include <stdlib.h>
#include <uv.h>

struct server {
    uv_loop_t* loop;
    uv_tcp_t* tcp;
    processor* proc;
};

typedef struct session {
    uv_stream_t* client;
    buf_queue* bufq;
} session;

static void on_new_connection(uv_stream_t* server, int status);
static void on_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
static void on_client_closed(uv_handle_t* client);

server* server_new(processor* proc) {
    server* self;

    self = malloc(sizeof(*self));
    self->proc = proc;
    self->loop = uv_loop_new();
    self->tcp = malloc(sizeof(*self->tcp));
    uv_tcp_init(self->loop, self->tcp);

    return self;
}

int server_listen(server* self, const char* ip, uint16_t port, int backlog) {
    int res;
    struct sockaddr_in s_addr;

    uv_ip4_addr(ip, port, &s_addr);
    uv_tcp_bind(self->tcp, (const struct sockaddr*) &s_addr, 0);

    res = uv_listen((uv_stream_t*) self->tcp, backlog, on_new_connection);
    if (res != 0) {
        PANICF("listen error: %s\n", uv_strerror(res));
    }

    return uv_run(self->loop, UV_RUN_DEFAULT);
}

static void on_new_connection(uv_stream_t* server, int status) {
    int res;

    if (status < 0) {
        WARNF("new connection error: %s\n", uv_strerror(status));
        return;
    }

    uv_tcp_t* client = malloc(sizeof(*client));
    uv_tcp_init(server->loop, client);

    res = uv_accept(server, (uv_stream_t*) client);
    if (res == 0) {
        session* sess = malloc(sizeof(*sess));
        sess->client = (uv_stream_t*) client;
        sess->bufq = buf_queue_new();

        uv_handle_set_data((uv_handle_t*) client, sess);
        uv_read_start((uv_stream_t*) client, uv_allocator, on_read);
    }
}

static void on_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) {
    if (nread < 0) {
        if (nread != UV_EOF) {
            WARNF("read error: %s\n", uv_strerror(nread));
        }
        uv_close((uv_handle_t*) client, on_client_closed);
    } else if (nread == 0) {
        // EAGIAN | EWOULDBLOCK
    } else {
        session* sess = uv_handle_get_data((uv_handle_t*) client);
        buf_queue* bufq = sess->bufq;

        buf_queue_append(bufq, buf);
        // TODO: process_handle
    }
}
