/*
 * Copyright (C) Baptiste Clenet
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       Implementation of OpenThread platform config
 *
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 * @}
 */

#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H

/**
 * @def OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS
 *
 * The number of message buffers in buffer pool
 */
#ifdef OPENTHREAD_ENABLE_NCP_UART
#define OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS       5
#elif defined(OPENTHREAD_FTD) || defined(OPENTHREAD_MTD)
#define OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS       15
#endif

/**
  * @def OPENTHREAD_CONFIG_LEGACY_TRANSMIT_DONE
  *
  * Define to 1 if you want use legacy transmit done.
  *
  */
#define OPENTHREAD_CONFIG_LEGACY_TRANSMIT_DONE 1

#endif /* PLATFORM_CONFIG_H */
