/*
 * Copyright (C) 2019  Javier FILEIV <javier.fileiv@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */
/**
 * @ingroup     pkg_mqtt-paho
 * @{
 *
 * @file
 * @brief       Network MQTT interface definitions
 *
 * @author      Javier FILEIV <javier.fileiv@gmail.com>
 */
#ifndef MQTT_H
#define MQTT_H

#include <mutex.h>
#include <xtimer.h>
#include <thread.h>
#include <net/sock/tcp.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief  struct to get time references within mqtt paho
 *
 */
typedef struct Timer
{
    xtimer_ticks64_t timeout;   /**< timeout ticks*/
} Timer;

/**
 * @brief  Initialize timer struct
 *
 * @param timer timer to init
 */
void TimerInit(Timer* timer);

/**
 * @brief  is timer expired?
 *
 * @param   timer   timer to check
 *
 * @return 1 if timer expired, 0 otherwise
 */
char TimerIsExpired(Timer* timer);

/**
 * @brief  start timer set to milli seconds
 *
 * @param   timer   timer to start
 * @param   msecs   time to set in msecs
 */
void TimerCountdownMS(Timer* timer, unsigned int msecs);

/**
 * @brief  start timer set to seconds
 *
 * @param   timer   timer to start
 * @param   secs   time to set in secs
 */
void TimerCountdown(Timer* timer, unsigned int secs);

/**
 * @brief  Returns millisecs left in timer
 *
 * @param   timer   timer to check
 *
 * @return  msecs left
 */
int TimerLeftMS(Timer* timer);

/**
 * @brief  Network struct within mqtt paho
 */
typedef struct Network
{
    sock_tcp_t sock;                                                    /**< socket number */
    int (*mqttread) (struct Network*, unsigned char*, int, int);        /**< read internal function */
    int (*mqttwrite) (struct Network*, unsigned char*, int, int);       /**< write internal function */
    int (*disconnect) (struct Network*);                                /**< disconnect internal function */

} Network;

/**
 * @brief  Initialize network struct
 *
 * @param   n   network struct
 */
void MQTTNetworkInit(Network* n);

/**
 * @brief  Connect network to host
 *
 * @param   n   network struct
 * @param   address_ip  IP address to connect to
 * @param   port_number port to connect to
 *
 * @return  0 if success, !=0 otherwise
 */
int MQTTNetworkConnect(Network* n, char* address_ip, int port_number);

/**
 * @brief  Disconnect network
 *
 * @param   n       network struct
 */
void MQTTNetworkDisconnect(Network* n);

/**
 * @brief  Mutex struct within mqtt paho
 */
typedef struct Mutex
{
    mutex_t lock;   /**< MQTT thread mutex*/
} Mutex;

/**
 * @brief  Initialize mutex struct
 *
 * @param  mutex pointer
 */
void MutexInit(Mutex* mutex);

/**
 * @brief  Locks mutex struct
 *
 * @param  mutex pointer
 *
 * @return  0 if success, !=0 otherwise
 */
int MutexLock(Mutex* mutex);

/**
 * @brief  Unlocks mutex struct
 *
 * @param  mutex pointer
 *
 * @return  0 if success, !=0 otherwise
 */
int MutexUnlock(Mutex* mutex);

/**
 * @brief  Thread struct within mqtt paho
 */
typedef struct Thread
{
    char stack[THREAD_STACKSIZE_MEDIUM];    /**< stack variable for MQTT thread*/
    kernel_pid_t pid;                       /**< MQTT thread pid*/
} Thread;

/**
 * @brief  Start new thread
 *
 * @param  thread to start
 * @param  fn  pointer function to execute
 * @param  arg arguments to pass to that fn
 *
 * @return  0 if success, !=0 otherwise
 */
int ThreadStart(Thread* thread, void *(*fn)(void*), void* arg);

#ifdef __cplusplus
}
#endif

#endif /* MQTT_H */
/** @} */
