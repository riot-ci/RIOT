/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_saul_reg
 * @{
 *
 * @file
 * @brief       SAUL registry implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "saul_reg.h"

/**
 * @brief   Keep the head of the device list as global variable
 */
saul_reg_t *saul_reg = NULL;


int saul_reg_add(saul_reg_t *dev)
{
    saul_reg_t *tmp = saul_reg;

    if (dev == NULL) {
        return -ENODEV;
    }

    /* prepare new entry */
    dev->next = NULL;
    /* add to registry */
    if (saul_reg == NULL) {
        saul_reg = dev;
    }
    else {
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = dev;
    }
    return 0;
}

int saul_reg_rm(saul_reg_t *dev)
{
    saul_reg_t *tmp = saul_reg;

    if (saul_reg == NULL || dev == NULL) {
        return -ENODEV;
    }
    if (saul_reg == dev) {
        saul_reg = dev->next;
        return 0;
    }
    while (tmp->next && (tmp->next != dev)) {
        tmp = tmp->next;
    }
    if (tmp->next == dev) {
        tmp->next = dev->next;
    }
    else {
        return -ENODEV;
    }
    return 0;
}

saul_ctxt_ptr_t saul_reg_find_nth(int pos)
{
    saul_ctxt_ptr_t ret;
    saul_reg_t *tmp = saul_reg;

    ret.ctxt = 0;

    for (int i = 0; tmp != NULL; i++) {
        /* if ctxtlist is empty, ignore it */
        if (tmp->ctxtlist == 0) {
            if (i == pos) {
                break;
            }
            tmp = tmp->next;
            continue;
        }

        /* if ctxtlist is not empty, examine it bitwise */
        for (unsigned j = 0; j < sizeof(tmp->ctxtlist); j++) {
            /* check to see if this context bit is enabled */
            if ((tmp->ctxtlist >> j) & 0x1) {
                if (i == pos) {
                    ret.ctxt = j;
                }

                /* increment list counter, as if context is real list item */
                i++;
            }
        }
    }

    ret.reg = tmp;
    return ret;
}

saul_reg_t *saul_reg_find_type(uint8_t type)
{
    saul_reg_t *tmp = saul_reg;

    while (tmp) {
        if (tmp->driver->type == type) {
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

saul_reg_t *saul_reg_find_name(const char *name)
{
    saul_reg_t *tmp = saul_reg;

    while (tmp) {
        if (strcmp(tmp->name, name) == 0) {
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

int saul_reg_read(saul_reg_t *dev, uint8_t ctxt, phydat_t *res)
{
    if (dev == NULL) {
        return -ENODEV;
    }
    return dev->driver->read(dev->dev, ctxt, res);
}

int saul_reg_write(saul_reg_t *dev, uint8_t ctxt, phydat_t *data)
{
    if (dev == NULL) {
        return -ENODEV;
    }
    return dev->driver->write(dev->dev, ctxt, data);
}
