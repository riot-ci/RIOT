/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_periph_init
 * @{
 *
 * @file
 * @brief       Common static peripheral driver initialization implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#define USB_H_USER_IS_RIOT_INTERNAL

#ifdef MODULE_AUTO_INIT_PERIPH
#ifdef MODULE_AUTO_INIT_PERIPH_I2C
#include "periph/i2c.h"
#endif
#ifdef MODULE_AUTO_INIT_PERIPH_SPI
#include "periph/spi.h"
#endif
#ifdef MODULE_AUTO_INIT_PERIPH_RTC
#include "periph/rtc.h"
#endif
#ifdef MODULE_AUTO_INIT_PERIPH_RTT
#include "periph/rtt.h"
#endif
#ifdef MODULE_AUTO_INIT_PERIPH_HWRNG
#include "periph/hwrng.h"
#endif
#ifdef MODULE_AUTO_INIT_PERIPH_USBDEV
#include "periph/usbdev.h"
#endif
#ifdef MODULE_AUTO_INIT_PERIPH_WDT
#include "periph/wdt.h"
#endif
#endif /* MODULE_AUTO_INIT_PERIPH */

void periph_init(void)
{
#ifdef MODULE_AUTO_INIT_PERIPH
    /* initialize configured I2C devices */
#ifdef MODULE_AUTO_INIT_PERIPH_I2C
    for (unsigned i = 0; i < I2C_NUMOF; i++) {
        i2c_init(I2C_DEV(i));
    }
#endif

    /* initialize configured SPI devices */
#ifdef MODULE_AUTO_INIT_PERIPH_SPI
    for (unsigned i = 0; i < SPI_NUMOF; i++) {
        spi_init(SPI_DEV(i));
    }
#endif

    /* Initialize RTC */
#ifdef MODULE_AUTO_INIT_PERIPH_RTC
    rtc_init();
#endif

    /* Initialize RTT */
#ifdef MODULE_AUTO_INIT_PERIPH_RTT
    #error
    rtt_init();
#endif

#ifdef MODULE_AUTO_INIT_PERIPH_HWRNG
    hwrng_init();
#endif

#ifdef MODULE_AUTO_INIT_PERIPH_USBDEV
    usbdev_init_lowlevel();
#endif

#if defined(MODULE_AUTO_INIT_PERIPH_WDT) && WDT_HAS_INIT
    wdt_init();
#endif

#endif /* MODULE_AUTO_INIT_PERIPH */
}
