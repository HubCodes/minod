#ifndef MINOD_UTIL_H
#define MINOD_UTIL_H

#define PANIC "[PANIC] "
#define WARN "[WARN] "
#define INFO "[INFO] "

#define PANICF(...)                         \
    do {                                    \
        fprintf(stderr, PANIC __VA_ARGS__); \
        exit(EXIT_FAILURE);                 \
    } while(0)

#define WARNF(...)                          \
    do {                                    \
        fprintf(stderr, WARN __VA_ARGS__);  \
   } while (0)

#define INFOF(...)                          \
    do {                                    \
        fprintf(stderr, INFO __VA_ARGS__);  \
    } while (0)

#endif // MINOD_UTIL_H
