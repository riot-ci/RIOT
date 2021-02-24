/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for LIS2DH12 accelerometer driver
 *
 * @author      Jan Mohr <jan.mohr@ml-pa.com>
 *
 * @}
 */

#ifndef LIS2DH12_TEST_H
#define LIS2DH12_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief  lis2dh12 process kernel pid
 */
extern kernel_pid_t lis2dh12_process;

/**
 * @brief   Shell commands for test application
 *
 * @param[in] argc  number of arguments
 * @param[in] argv  arguments
 *
 * @return  1 on success and -1 on fault
 */
int shell_lis2dh12_cmd(int argc, char **argv);

/**
 * @brief   Function to initialize the module
 */
void lis2dh12_test_init(void);

/**
 * @brief   Function running in thread and interprets values caused of interrupt
 *
 * @param   unused arguments
 */
void* lis2dh12_test_process(void* arg);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* LIS2DH12_TEST_H */
