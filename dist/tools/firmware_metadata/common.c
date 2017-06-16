#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "hashes/sha256.h"

off_t fsize(const char *filename)
{
    struct stat st;

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1;
}

int to_file(const char *filename, void *buf, size_t len)
{
    int fd = open(filename, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    if (fd > 0) {
        ssize_t res = write(fd, buf, len);
        close(fd);
        return res == (ssize_t)len;
    } else {
        return fd;
    }
}

int from_file(const char *filename, void *buf, size_t len)
{
    int fd = open(filename, O_RDONLY);
    if (fd > 0) {
        ssize_t res = read(fd, buf, len);
        close(fd);
        return res == (ssize_t)len;
    } else {
        return fd;
    }
}

ssize_t do_sha256(const char *filename, void *tgt, off_t offset)
{
    sha256_context_t sha256;
    sha256_init(&sha256);

    ssize_t bytes_read;
    ssize_t total = 0;
    char buf[1024];

    int fd = open(filename, O_RDONLY);
    if (!fd) {
        return -1;
    }

    if (offset) {
        if (lseek(fd, offset, SEEK_SET) == -1) {
            return -1;
        }
    }

    while ((bytes_read = read(fd, buf, sizeof(buf)))) {
        sha256_update(&sha256, buf, bytes_read);
        total += bytes_read;
    }

    sha256_final(&sha256, tgt);

    close(fd);

    return total;
}
