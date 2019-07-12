/*
 * Copyright 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_fmt_table Table extension of the string formatting API
 *                            (fmt_table)
 * @ingroup     sys_fmt
 * @brief       Provides utilities to print tables.
 *
 * \note The print functions in this library do not buffer any output.
 * Mixing calls to standard @c printf from stdio.h with the @c print_xxx
 * functions in fmt, especially on the same output line, may cause garbled
 * output.
 *
 * @{
 *
 * @file
 * @brief       Table extension of the string formatting API
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef FMT_TABLE_H
#define FMT_TABLE_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   16 bytes of read-only spaces, useful for @ref print_pattern
 */
extern const char fmt_table_spaces[16];

/**
 * @brief Prints @p fill_size bytes of the given pattern, repeating the
 *        pattern if needed
 * @param pat       Pattern to print
 * @param pat_size  Size of the pattern in bytes
 * @param fill_size Number of bytes to print (if bigger than @p pat_size, the
 *                  pattern will be repeated)
 *
 * E.g. `print_pattern("ab", 2, 5);` will print `ababa` to the console.
 * This can be used to fill table columns with spaces, draw lines, etc.
 */
void print_pattern(const char *pat, size_t pat_size, size_t fill_size);

/**
 * @brief Print a table column with the given number as decimal
 * @param number    Number to print in the column
 * @param width     Width of the column
 */
void print_col_u32_dec(uint32_t number, size_t width);

/**
 * @brief Print a table column with the given number as decimal
 * @param number    Number to print in the column
 * @param width     Width of the column
 */
void print_col_s32_dec(int32_t number, size_t width);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* FMT_TABLE_H */
