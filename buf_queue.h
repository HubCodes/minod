#ifndef MINOD_BUF_QUEUE_H
#define MINOD_BUF_QUEUE_H

#include <uv.h>

typedef struct buf_queue buf_queue;

buf_queue* buf_queue_new(void);
void buf_queue_append(buf_queue* self, const uv_buf_t* buf);

#endif // MINOD_BUF_QUEUE_H
