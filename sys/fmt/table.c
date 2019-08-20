/*
 * Copyright 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_fmt_table
 * @{
 *
 * @file
 * @brief       Implementation of the table extensions of the string formatting
 *              library
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include "fmt.h"

static const char fmt_table_spaces[16] = "                ";

void print_pattern(const char *pat, size_t pat_size, size_t fill_size)
{
    while (fill_size > pat_size) {
        print(pat, pat_size);
    }

    print(pat, fill_size);
}

void print_col_u32_dec(uint32_t number, size_t width)
{
    char sbuf[10]; /* "4294967295" */
    size_t slen;

    slen = fmt_u32_dec(sbuf, number);
    if (width > slen) {
        print_pattern(fmt_table_spaces, sizeof(fmt_table_spaces), width - slen);
    }
    print(sbuf, slen);
}

void print_col_s32_dec(int32_t number, size_t width)
{
    char sbuf[11]; /* "-2147483648" */
    size_t slen;

    slen = fmt_s32_dec(sbuf, number);
    if (width > slen) {
        print_pattern(fmt_table_spaces, sizeof(fmt_table_spaces), width - slen);
    }
    print(sbuf, slen);
}
