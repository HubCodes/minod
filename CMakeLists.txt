CMAKE_MINIMUM_REQUIRED(VERSION 3.15)
PROJECT(minod C)

SET(CMAKE_C_STANDARD 11)

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_SEARCH_MODULE(LIBUV REQUIRED libuv)

LINK_LIBRARIES(${LIBUV_LIBRARIES})

ADD_EXECUTABLE(minod main.c hashmap.c minod.h hashmap.h server.c server.h alloc.c alloc.h processor.h util.h buf_queue.c buf_queue.h)
