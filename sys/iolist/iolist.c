#include <sys/uio.h>

#include "iolist.h"

unsigned iolist_count(const iolist_t *iolist)
{
    unsigned count = 0;
    while (iolist) {
        count++;
        iolist = iolist->iol_next;
    }
    return count;
}

size_t iolist_to_iovec(const iolist_t *iolist, struct iovec *iov, unsigned *count)
{
    size_t bytes = 0;
    unsigned _count = 0;

    while (iolist) {
        iov->iov_base = iolist->iol_base;
        iov->iov_len = iolist->iol_len;
        bytes += iov->iov_len;
        _count++;
        iolist = iolist->iol_next;
        iov++;
    }

    *count = _count;

    return bytes;
}
