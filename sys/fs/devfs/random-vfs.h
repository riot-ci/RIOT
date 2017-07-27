
#ifndef RANDOM_VFS_H
#define RANDOM_VFS_H

#include "vfs.h"

#if defined(FEATURE_PERIPH_HWRNG) || defined(DOXYGEN)
/**
 * @brief hwrng driver for vfs
 */
extern const vfs_file_ops_t hwrng_vfs_ops;
#endif

#if defined(MODULE_RANDOM)
extern const vfs_file_ops_t random_vfs_ops;
#endif

#endif /* RANDOM_VFS_H */
