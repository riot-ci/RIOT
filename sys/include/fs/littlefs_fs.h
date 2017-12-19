/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_littlefs  littlefs integration
 * @ingroup     pkg_littlefs
 * @brief       RIOT integration of littlefs
 *
 * Integration of littlefs into RIOT
 *
 * @{
 *
 * @file
 * @brief       littlefs integration with vfs
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#ifndef FS_LITTLEFS_FS_H
#define FS_LITTLEFS_FS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vfs.h"
#include "lfs.h"
#include "mtd.h"
#include "mutex.h"

#if VFS_FILE_BUFFER_SIZE < 52
#error "VFS_FILE_BUFFER_SIZE is too small, at least 52 bytes is required"
#endif

#if VFS_DIR_BUFFER_SIZE < 44
#error "VFS_DIR_BUFFER_SIZE is too small, at least 44 bytes is required"
#endif

/**
 * @name    littlefs configuration
 * @{
 */
#ifndef LITTLEFS_LOOKAHED_SIZE
/** Default lookahed size */
#define LITTLEFS_LOOKAHED_SIZE      (128)
#endif

#ifndef LITTLEFS_FILE_BUFFER_SIZE
/** File buffer size, if 0, dynamic allocation is used.
 * If set, only one file can be used at a time, must be program size (mtd page size
 * is used internally as program size) */
#define LITTLEFS_FILE_BUFFER_SIZE   (0)
#endif

#ifndef LITTLEFS_READ_BUFFER_SIZE
/** Read buffer size, if 0, dynamic allocation is used.
 * If set, it must be read size (mtd page size is used internally as read size) */
#define LITTLEFS_READ_BUFFER_SIZE   (0)
#endif

#ifndef LITTLEFS_PROG_BUFFER_SIZE
/** Prog buffer size, if 0, dynamic allocation is used.
 * If set, it must be program size */
#define LITTLEFS_PROG_BUFFER_SIZE   (0)
#endif
/** @} */

/**
 * @brief   littlefs descriptor for vfs integration
 */
typedef struct {
    lfs_t fs;                   /**< littlefs descriptor */
    struct lfs_config config;   /**< littlefs config */
    mtd_dev_t *dev;             /**< mtd device to use */
    mutex_t lock;               /**< mutex */
#if LITTLEFS_FILE_BUFFER_SIZE || DOXYGEN
    /** file buffer to use internally if LITTLEFS_FILE_BUFFER_SIZE is set */
    uint8_t file_buf[LITTLEFS_FILE_BUFFER_SIZE];
#endif
#if LITTLEFS_READ_BUFFER_SIZE || DOXYGEN
    /** read buffer to use internally if LITTLEFS_READ_BUFFER_SIZE is set */
    uint8_t read_buf[LITTLEFS_READ_BUFFER_SIZE];
#endif
#if LITTLEFS_PROG_BUFFER_SIZE || DOXYGEN
    /** prog buffer to use internally if LITTLEFS_PROG_BUFFER_SIZE is set */
    uint8_t prog_buf[LITTLEFS_PROG_BUFFER_SIZE];
#endif
    /** lookahead buffer to use internally */
    uint8_t lookahed_buf[LITTLEFS_LOOKAHED_SIZE / 8];
} littlefs_desc_t;

/** The littlefs vfs driver */
extern const vfs_file_system_t littlefs_file_system;

#ifdef __cplusplus
}
#endif

#endif /* FS_LITTLEFS_FS_H */
/** @} */
