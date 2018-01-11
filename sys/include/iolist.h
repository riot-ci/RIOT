#ifndef IOLIST_H
#define IOLIST_H

#include <unistd.h>

typedef struct iolist iolist_t;

struct iolist {
    iolist_t *iol_next;
    void *iol_base;
    size_t iol_len;
};

unsigned iolist_count(const iolist_t *iolist);

struct iovec;
size_t iolist_to_iovec(const iolist_t *iolist, struct iovec *iov, unsigned *count);

#endif /* IOLIST_H */
