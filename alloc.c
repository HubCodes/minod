#include "alloc.h"

#include <stdlib.h>

static char* alloc_buffer(size_t size);

void uv_allocator(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
    (void) handle;

    buf->base = alloc_buffer(suggested_size);
    buf->len = suggested_size;
}

static char* alloc_buffer(size_t size) {
    return malloc(sizeof(char) * size);
}
