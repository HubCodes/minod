#ifndef MINOD_ALLOC_H
#define MINOD_ALLOC_H

#include <uv.h>

void uv_allocator(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);

#endif // MINOD_ALLOC_H
