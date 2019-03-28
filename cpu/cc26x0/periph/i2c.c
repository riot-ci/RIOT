/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc26x0
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "mutex.h"

#include "cpu.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if ENABLE_DEBUG
#define PREG(x) DEBUG("%s=0x%08x\n", #x, (unsigned)x);
#else
#define PREG(x)
#endif

/**
 * @brief Array holding one pre-initialized mutex for each I2C device
 */
static mutex_t _locks[I2C_NUMOF];

static int _check_errors(void);

void i2c_init(i2c_t devnum)
{
    (void)devnum;
    assert(devnum < I2C_NUMOF);

    /* enable SERIAL power domain */
    PRCM->PDCTL0SERIAL = 1;
    while (!(PRCM->PDSTAT0 & PDSTAT0_SERIAL_ON)) {}

    /* enable i2c clock in run mode */
    PRCM->I2CCLKGR = 1;
    PRCM->CLKLOADCTL |= CLKLOADCTL_LOAD;
    while (!(PRCM->CLKLOADCTL & CLKLOADCTL_LOADDONE)) {}

    /* configure pins */
    IOC->CFG[I2C_SDA_PIN] = (IOCFG_PORTID_I2C_MSSDA
                             | IOCFG_INPUT_ENABLE
                             | IOCFG_IOMODE_OPEN_DRAIN
                             | IOCFG_PULLCTL_UP);
    IOC->CFG[I2C_SCL_PIN] = (IOCFG_PORTID_I2C_MSSCL
                             | IOCFG_INPUT_ENABLE
                             | IOCFG_IOMODE_OPEN_DRAIN
                             | IOCFG_PULLCTL_UP);

    /* initialize I2C master */
    I2C->MCR = MCR_MFE;

    /* configure clock speed */
    /*{PERDMACLK / [2 × (SCL_LP + SCL_HP) × SCL_CLK]} – 1*/
    /* with SCL_LP==6 && SCL_HP==4 use 0x17 for 100kHZ with 48MHZ CPU clock */
    I2C->MTPR = MTPR_TPR_100KHZ;
}

int i2c_acquire(i2c_t dev)
{
    assert(dev < I2C_NUMOF);
    mutex_lock(&_locks[dev]);
    return 0;
}

int i2c_release(i2c_t dev)
{
    assert(dev < I2C_NUMOF);
    mutex_unlock(&_locks[dev]);
    return 0;
}

int i2c_read_bytes(i2c_t dev, uint16_t addr,
                   void *data, size_t len, uint8_t flags)
{
    (void)dev;
    int ret = 0;
    DEBUG("i2c_read_bytes() %u\n", len);
    assert(dev < I2C_NUMOF);

    /* Check for unsupported operations */
    if (flags & I2C_ADDR10) {
        return -EOPNOTSUPP;
    }

    /* Check for wrong arguments given */
    if (data == NULL || len == 0) {
        return -EINVAL;
    }

    char *bufpos = data;

    /* Sequence may be omitted in a single master system */
    while (I2C->MSTAT & MSTAT_BUSY);

    I2C->MSA = ((uint32_t)addr << 1) | MSA_RS;

    while (len--) {
        DEBUG("LOOP %u\n", len);
        /* setup transfer */
        uint32_t mctrl = MCTRL_RUN;
        if (!(flags & I2C_NOSTART)) {
            DEBUG("START\n");
            mctrl |= MCTRL_START;

            /* make note not to generate START from second byte onwards */
            flags |= I2C_NOSTART;
        }

        /* after last byte, generate STOP unless told not to */
        if (!len && !(flags & I2C_NOSTOP)) {
            DEBUG("STOP\n");
            mctrl |= MCTRL_STOP;
        }
        else {
            DEBUG("ACK\n");
            mctrl |= MCTRL_ACK;
        }

        /* initiate transfer */
        I2C->MCTRL = mctrl;

        /* wait for transfer to be complete */
        while (I2C->MSTAT & MSTAT_IDLE);
        while (I2C->MSTAT & MSTAT_BUSY);

        /* check if there was an error */
        ret = _check_errors();
        if (ret != 0) {
            return ret;
        }
        /* copy next byte from I2C data register */
        DEBUG("IN=0x%02x\n", (unsigned)I2C->MDR);
        *bufpos++ = I2C->MDR;
    }

    return ret;
}

int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data, size_t len,
                    uint8_t flags)
{
    (void)dev;
    int ret = 0;
    DEBUG("i2c_write_bytes() %u\n", len);
    assert(dev < I2C_NUMOF);

    const unsigned char *bufpos = data;

    /* Check for unsupported operations */
    if (flags & I2C_ADDR10) {
        return -EOPNOTSUPP;
    }

    /* Check for wrong arguments given */
    if (data == NULL || len == 0) {
        return -EINVAL;
    }

    PREG(I2C->MSTAT);

    /* Since write is 0 we just need shift the address in */
    I2C->MSA = (uint32_t)addr << 1;

    while (len--) {
        DEBUG("LOOP %u 0x%2x\n", len, (unsigned)*bufpos);
        /* copy next byte into I2C data register */
        I2C->MDR = *bufpos++;

        /* Sequence may be omitted in a single master system. */
        while (I2C->MSTAT & MSTAT_BUSY);

        /* setup transfer */
        uint32_t mctrl = MCTRL_RUN;
        if (!(flags & I2C_NOSTART)) {
            DEBUG("START\n");
            mctrl |= MCTRL_START;

            /* make note not to generate START from second byte onwards */
            flags |= I2C_NOSTART;
        }

        /* after last byte, generate STOP unless told not to */
        if (!len && !(flags & I2C_NOSTOP)) {
            DEBUG("STOP\n");
            mctrl |= MCTRL_STOP;
        }

        /* initiate transfer */
        I2C->MCTRL = mctrl;

        /* The reference manual (SWCU117H) is ambiguous on how to wait:
         *
         * 1. 21.4 8. says "wait until BUSBUSY is cleared"
         * 2. command flow diagrams (e.g., 21.3.5.1) indicate to wait while
         *    BUSY is set
         *
         * (3. 21.5.1.10 says BUSY is only valid after 4 SYSBUS clock cycles)
         *
         * Waiting first for cleared IDLE and then for cleared BUSY works fine.
         */

        /* wait for transfer to be complete */
        while (I2C->MSTAT & MSTAT_IDLE);
        while (I2C->MSTAT & MSTAT_BUSY);

        /* check if there was an error */
        ret = _check_errors();
        if (ret != 0) {
            return ret;
        }
    }

    return ret;
}

static int _check_errors(void)
{
    /* check if there was an error */
    if (I2C->MSTAT & MSTAT_ERR) {
        PREG(I2C->MSTAT);
        if (I2C->MSTAT & MSTAT_ADRACK_N) {
            DEBUG("ADDRESS NACK\n");
            return -ENXIO;
        }
        else if (I2C->MSTAT & MSTAT_DATACK_N) {
            DEBUG("DATA NACK\n");
            return -EIO;
        }
        else if (I2C->MSTAT & MSTAT_DATACK_N) {
            DEBUG("ARBITRATION LOSS\n");
            return -EAGAIN;
        }
        else if (I2C->MSTAT & MSTAT_DATACK_N) {
            /* Accroding to data sheet we should send stop */
            I2C->MCTRL = MCTRL_STOP;
            DEBUG("ARBITRATION LOSS\n");
            return -EAGAIN;
        }
        return -ETIMEDOUT;
    }
    return 0;
}
