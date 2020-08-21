/*
 * Copyright (C) 2015-2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_tcp
 *
 * @{
 *
 * @file
 * @brief       TCP event loop declarations.
 *
* @author       Simon Brummer <simon.brummer@posteo.de>
 */

#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Starts GNRC TCP's main processing thread.
 *
 * @retval  PID of processing thread on success
 * @retval  -EEXIST if processing thread was already started
 * @retval  see @ref thread_create() for more error cases.
 */
int _gnrc_tcp_event_loop_init(void);

#ifdef __cplusplus
}
#endif

#endif /* EVENTLOOP_H */
/** @} */
