/*
 * Copyright (C) 2019 Robert Olsson <roolss@kth.se>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_avr-rss2
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the rss2 AtMega256rfr2 board
 * @author      Robert Olsson <roolss@kth.se>
 *
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Clock configuration
 * @{
 */
#define CLOCK_CORECLOCK     (16000000UL)
/** @} */

/**
 * @name RTC configuration
 * @{
 */
#define RTC_NUMOF    (1U)
/** @} */

/**
 * @name RTT configuration
 * @{
 */
#define RTT_NUMOF        (1U)
#define RTT_MAX_VALUE    (0x00FFFFFF)    /* 24-bit timer */
#define RTT_FREQUENCY    (32U)
/** @} */

#ifdef __cplusplus
}
#endif

#include "periph_conf_atmega_common.h"

#endif /* PERIPH_CONF_H */
/** @} */
