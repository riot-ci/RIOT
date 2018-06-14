/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       SAUL registry shell commands
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "saul_reg.h"

/* this function does not check, if the given device is valid */
static void probe(int num, saul_reg_t *dev, int ctxt)
{
    int dim;
    phydat_t res;

    dim = saul_reg_read(dev, ctxt, &res);
    if (dim <= 0) {
        printf("error: failed to read from device #%i\n", num);
        return;
    }

    /* print results */
    printf("Reading from #%i (", num);

    /* replace context in name */
    printf(dev->name, ctxt);

    printf("|%s)\n", saul_class_to_str(dev->driver->type));

    phydat_dump(&res, dim);
}

static void probe_all(void)
{
    saul_reg_t *dev = saul_reg;
    unsigned int i = 0;

    while (dev) {
        /* if ctxtlist is empty, ignore it */
        if (dev->ctxtlist == 0) {
            probe(i++, dev, 0);
            puts("");
            dev = dev->next;
            continue;
        }

        for (unsigned j = 0; j <= sizeof(dev->ctxtlist); j++) {
            /* check to see if this context bit is enabled */
            if ((dev->ctxtlist >> j) & 0x1) {
                probe(i++, dev, j);
                puts("");
            }
        }

        dev = dev->next;
    }
}

static void list(void)
{
    saul_reg_t *dev = saul_reg;
    int i = 0;

    if (dev) {
        puts("ID\tClass\t\tName");
    }
    else {
        puts("No devices found");
    }

    while (dev) {
        /* if ctxtlist is empty, ignore it */
        if (dev->ctxtlist == 0) {
            printf("#%i\t%s\t%s\n",
                   i++, saul_class_to_str(dev->driver->type), dev->name);
            dev = dev->next;
            continue;
        }

        for (unsigned j = 0; j <= sizeof(dev->ctxtlist); j++) {
            /* check to see if this context bit is enabled */
            if ((dev->ctxtlist >> j) & 0x1) {
                printf("#%u\t%s\t", i, saul_class_to_str(dev->driver->type));

                /* replace context in name */
                printf(dev->name, j);

                /* print newline */
                puts("");

                /* increment list counter, as if context is real list item */
                i++;
            }
        }

        dev = dev->next;
    }
}

static void read(int argc, char **argv)
{
    int num;
    saul_ctxt_ptr_t dev;

    if (argc < 3) {
        printf("usage: %s %s <device id>|all\n", argv[0], argv[1]);
        return;
    }
    if (strcmp(argv[2], "all") == 0) {
        probe_all();
        return;
    }
    /* get device id */
    num = atoi(argv[2]);
    dev = saul_reg_find_nth(num);
    if (dev.reg == NULL) {
        puts("error: undefined device id given");
        return;
    }
    probe(num, dev.reg, dev.ctxt);
}

static void write(int argc, char **argv)
{
    int num, dim;
    saul_ctxt_ptr_t dev;
    phydat_t data;

    if (argc < 4) {
        printf("usage: %s %s <device id> <value 0> [<value 1> [<value 2]]\n",
               argv[0], argv[1]);
        return;
    }
    num = atoi(argv[2]);
    dev = saul_reg_find_nth(num);
    if (dev.reg == NULL) {
        puts("error: undefined device given");
        return;
    }
    /* parse value(s) */
    memset(&data, 0, sizeof(data));
    dim = ((argc - 3) > (int)PHYDAT_DIM) ? (int)PHYDAT_DIM : (argc - 3);
    for (int i = 0; i < dim; i++) {
        data.val[i] = atoi(argv[i + 3]);
    }

    /* print values before writing */
    printf("Writing to device #%i - ", num);
    /* replace context in name */
    printf(dev.reg->name, dev.ctxt);
    puts("");

    phydat_dump(&data, dim);
    /* write values to device */
    dim = saul_reg_write(dev.reg, dev.ctxt, &data);
    if (dim <= 0) {
        if (dim == -ENOTSUP) {
            printf("error: device #%i is not writable\n", num);
        }
        else {
            printf("error: failure to write to device #%i\n", num);
        }
        return;
    }
    printf("data successfully written to device #%i\n", num);
}

int _saul(int argc, char **argv)
{
    if (argc < 2) {
        list();
    }
    else {
        if (strcmp(argv[1], "read") == 0) {
            read(argc, argv);
        }
        else if (strcmp(argv[1], "write") == 0) {
            write(argc, argv);
        }
        else {
            printf("usage: %s read|write\n", argv[0]);
        }
    }
    return 0;
}
