/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       test application for including files as blobs
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "fmt.h"
#include "blob/blobtest.txt.h"
#include "blob/blob_subdir/blobtest_subdir.txt.h"
#include "blob/blobtest.bin.h"

int main(void)
{
    print((char *)blobtest_txt, blobtest_txt_len);
    print("\n", 1);

    print((char *)blobtest_subdir_txt, blobtest_subdir_txt_len);
    print("\n", 1);

    for (size_t n = 0; n < blobtest_bin_len; n++) {
        print("0x", 2);
        print_byte_hex(blobtest_bin[n]);
        print("\n", 1);
    }

    return 0;
}
