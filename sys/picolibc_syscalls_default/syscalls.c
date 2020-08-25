/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_picolibc PicoLibc system call
 * @ingroup     sys
 * @brief       PicoLibc system call
 * @{
 *
 * @file
 * @brief       PicoLibc system call implementations
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>

#include "log.h"
#include "periph/pm.h"
#include "stdio_base.h"

#define VFS_FD_OFFSET   (STDERR_FILENO + 1)

/**
 * @brief Exit a program without cleaning up files
 *
 * If your system doesn't provide this, it is best to avoid linking with subroutines that
 * require it (exit, system).
 *
 * @param n     the exit code, 0 for all OK, >0 for not OK
 */
void __attribute__((__noreturn__))
_exit(int n)
{
    LOG_INFO("#! exit %i: powering off\n", n);
    pm_off();
    while(1);
}

/**
 * @brief Send a signal to a thread
 *
 * @param[in] pid the pid to send to
 * @param[in] sig the signal to send
 *
 * @return    always returns -1 to signal error
 */
__attribute__ ((weak))
int kill(pid_t pid, int sig)
{
    (void)pid;
    (void)sig;
    errno = ESRCH;                         /* not implemented yet */
    return -1;
}

#include "mutex.h"

static mutex_t picolibc_put_mutex = MUTEX_INIT;

#define PICOLIBC_STDOUT_BUFSIZE 64

static char picolibc_stdout[PICOLIBC_STDOUT_BUFSIZE];
static int picolibc_stdout_queued;

static void _picolibc_flush(void)
{
    if (picolibc_stdout_queued) {
        stdio_write(picolibc_stdout, picolibc_stdout_queued);
        picolibc_stdout_queued = 0;
    }
}

static int picolibc_put(char c, FILE *file)
{
    (void)file;
    mutex_lock(&picolibc_put_mutex);
    picolibc_stdout[picolibc_stdout_queued++] = c;
    if (picolibc_stdout_queued == PICOLIBC_STDOUT_BUFSIZE || c == '\n')
        _picolibc_flush();
    mutex_unlock(&picolibc_put_mutex);
    return 1;
}

static int picolibc_flush(FILE *file)
{
    (void)file;
    mutex_lock(&picolibc_put_mutex);
    _picolibc_flush();
    mutex_unlock(&picolibc_put_mutex);
    return 0;
}

static int picolibc_get(FILE *file)
{
    (void)file;
    picolibc_flush(NULL);
    char c = 0;
    stdio_read(&c, 1);
    return c;
}

FILE picolibc_stdio =
    FDEV_SETUP_STREAM(picolibc_put, picolibc_get, picolibc_flush, _FDEV_SETUP_RW);

FILE *const __iob[] = {
    &picolibc_stdio,    /* stdin  */
    &picolibc_stdio,    /* stdout */
    &picolibc_stdio,    /* stderr */
};

#include <thread.h>
/**
 * @brief Get the process-ID of the current thread
 *
 * @return      the process ID of the current thread
 */
pid_t getpid(void)
{
    return thread_getpid();
}

#if MODULE_VFS
#include "vfs.h"
#else
#include <sys/stat.h>
#endif

/**
 * @brief Open a file
 *
 * This is a wrapper around @c vfs_open
 *
 * @param name  file name to open
 * @param flags flags, see man 3p open
 * @param mode  mode, file creation mode if the file is created when opening
 *
 * @return      fd number (>= 0) on success
 * @return      -1 on error, @c errno set to a constant from errno.h to indicate the error
 */
int open(const char *name, int flags, int mode)
{
#ifdef MODULE_VFS
    int fd = vfs_open(name, flags, mode);
    if (fd < 0) {
        /* vfs returns negative error codes */
        errno = -fd;
        return -1;
    }
    return fd + VFS_FS_OFFSET;
#else
    (void)name;
    (void)flags;
    (void)mode;
    errno = ENODEV;
    return -1;
#endif
}

/**
 * @brief Read bytes from an open file
 *
 * This is a wrapper around @c vfs_read
 *
 * @param[in]  fd     open file descriptor obtained from @c open()
 * @param[out] dest   destination buffer
 * @param[in]  count  maximum number of bytes to read
 *
 * @return       number of bytes read on success
 * @return       -1 on error, @c errno set to a constant from errno.h to indicate the error
 */
_READ_WRITE_RETURN_TYPE read(int fd, void *dest, size_t count)
{
    if (fd == STDIN_FILENO) {
        return stdio_read(dest, count);
    } else if ((fd < VFS_FD_OFFSET) || !IS_USED(MODULE_VFS)) {
        errno = ENOTSUP;
        return -1;
    }

#ifdef MODULE_VFS
    res = vfs_read(fd - VFS_FD_OFFSET, dest, count);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
#endif
}

/**
 * @brief Write bytes to an open file
 *
 * This is a wrapper around @c vfs_write
 *
 * @param[in]  fd     open file descriptor obtained from @c open()
 * @param[in]  src    source data buffer
 * @param[in]  count  maximum number of bytes to write
 *
 * @return       number of bytes written on success
 * @return       -1 on error, @c errno set to a constant from errno.h to indicate the error
 */
_READ_WRITE_RETURN_TYPE write(int fd, const void *src, size_t count)
{
    if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        return stdio_write(src, count);
    } else if ((fd < VFS_FD_OFFSET) || !IS_USED(MODULE_VFS)) {
        errno = ENOTSUP;
        return -1;
    }

#ifdef MODULE_VFS
    int res = vfs_write(fd - VFS_FD_OFFSET, src, count);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
#endif
}

/**
 * @brief Close an open file
 *
 * This is a wrapper around @c vfs_close
 *
 * If this call returns an error, the fd should still be considered invalid and
 * no further attempt to use it shall be made, not even to retry @c close()
 *
 * @param[in]  fd     open file descriptor obtained from @c open()
 *
 * @return       0 on success
 * @return       -1 on error, @c errno set to a constant from errno.h to indicate the error
 */
int close(int fd)
{
    if (fd < VFS_FD_OFFSET || !IS_USED(MODULE_VFS)) {
        errno = ENOTSUP;
        return -1;
    }

#ifdef MODULE_VFS
    int res = vfs_close(fd - VFS_FD_OFFSET);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
#endif
}

/**
 * Current process times (not implemented).
 *
 * @param[out]  ptms    Not modified.
 *
 * @return  -1, this function always fails. errno is set to ENOSYS.
 */
clock_t times(struct tms *ptms)
{
    (void)ptms;
    errno = ENOSYS;

    return (-1);
}

/**
 * @brief Query or set options on an open file
 *
 * This is a wrapper around @c vfs_fcntl
 *
 * @param[in]  fd     open file descriptor obtained from @c open()
 * @param[in]  cmd    fcntl command, see man 3p fcntl
 * @param[in]  arg    argument to fcntl command, see man 3p fcntl
 *
 * @return       0 on success
 * @return       -1 on error, @c errno set to a constant from errno.h to indicate the error
 */
int fcntl(int fd, int cmd, int arg)
{
    if (fd < VFS_FD_OFFSET || !IS_USED(MODULE_VFS)) {
        errno = ENOTSUP;
        return -1;
    }

#ifdef MODULE_VFS
    int res = vfs_fcntl(fd - VFS_FD_OFFSET, cmd, arg);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
#else
    (void)cmd;
    (void)arg;
#endif
}

/**
 * @brief Seek to position in file
 *
 * This is a wrapper around @c vfs_lseek
 *
 * @p whence determines the function of the seek and should be set to one of
 * the following values:
 *
 *  - @c SEEK_SET: Seek to absolute offset @p off
 *  - @c SEEK_CUR: Seek to current location + @p off
 *  - @c SEEK_END: Seek to end of file + @p off
 *
 * @param[in]  fd       open file descriptor obtained from @c open()
 * @param[in]  off      seek offset
 * @param[in]  whence   determines the seek method, see detailed description
 *
 * @return the new seek location in the file on success
 * @return -1 on error, @c errno set to a constant from errno.h to indicate the error
 */
off_t lseek(int fd, _off_t off, int whence)
{
    if (fd < VFS_FD_OFFSET || !IS_USED(MODULE_VFS)) {
        errno = ENOTSUP;
        return -1;
    }

#ifdef MODULE_VFS
    int res = vfs_lseek(fd - VFS_FD_OFFSET, off, whence);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
#else
    (void)off;
    (void)whence;
#endif
}

/**
 * @brief Get status of an open file
 *
 * This is a wrapper around @c vfs_fstat
 *
 * @param[in]  fd       open file descriptor obtained from @c open()
 * @param[out] buf      pointer to stat struct to fill
 *
 * @return 0 on success
 * @return -1 on error, @c errno set to a constant from errno.h to indicate the error
 */
int fstat(int fd, struct stat *buf)
{
    if (fd < VFS_FD_OFFSET || !IS_USED(MODULE_VFS)) {
        errno = ENOTSUP;
        return -1;
    }

#ifdef MODULE_VFS
    int res = vfs_fstat(fd - VFS_FD_OFFSET, buf);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return 0;
#else
    (void)buf;
#endif
}

/**
 * @brief Status of a file (by name)
 *
 * This is a wrapper around @c vfs_fstat
 *
 * @param[in]  name     path to file
 * @param[out] buf      pointer to stat struct to fill
 *
 * @return 0 on success
 * @return -1 on error, @c errno set to a constant from errno.h to indicate the error
 */
int stat(const char *name, struct stat *st)
{
#ifdef MODULE_VFS
    int res = vfs_stat(name, st);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return 0;
#else
    (void)name;
    (void)st;
    errno = ENODEV;
    return -1;
#endif
}

/**
 * @brief  Unlink (delete) a file
 *
 * @param[in]  path     path to file to be deleted
 *
 * @return 0 on success
 * @return -1 on error, @c errno set to a constant from errno.h to indicate the error
 */
int unlink(const char *path)
{
#ifdef MODULE_VFS
    int res = vfs_unlink(path);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return 0;
#else
    (void)path;
    errno = ENODEV;
    return -1;
#endif
}
