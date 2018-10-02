/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 */

#include <stdio.h>
#include <string.h>

int genhdr(int argc, char *argv[]);
extern const char generate_usage[];

int main(int argc, char *argv[])
{
    if (argc < 2) {
        goto usage;
    }
    else if (!strcmp(argv[1], "generate")) {
        return genhdr(argc - 1, &argv[1]);
    }

usage:
    fprintf(stderr, "usage: %s\n", generate_usage);
    return 1;
}
