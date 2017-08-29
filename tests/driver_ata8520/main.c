/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the ATA8520 SigFox module
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "xtimer.h"

#include "ata8520_params.h"
#include "ata8520.h"

static ata8520_t dev;

int main(void)
{
    puts("ATA8520 test application\n");

    printf("+------------Initializing------------+\n");
    switch(ata8520_init(&dev, &ata8520_params[0])) {
    case -ATA8520_ERR_SPI:
        puts("[Error] An error occurred when initializing SPI bus.");
        return 1;
    case -ATA8520_ERR_GPIO_INT:
        puts("[Error] An error occurred when initializing interrupt pin.");
        return 1;
    case -ATA8520_ERR_GPIO_SLEEP:
        puts("[Error] An error occurred when initializing poweron pin.");
        return 1;
    case -ATA8520_ERR_GPIO_RESET:
        puts("[Error] An error occurred when initializing reset- pin.");
        return 1;
    }
    printf("Initialization successful\n\n");
    uint8_t atmel_version[2];
    uint8_t sigfox_version[11];
    uint8_t sigfox_pac[SIGFOX_PAC_LENGTH + 1];
    uint8_t sigfox_id[SIGFOX_ID_LENGTH + 1];

    ata8520_atmel_version(&dev, atmel_version);
    ata8520_sigfox_version(&dev, sigfox_version);
    ata8520_id(&dev, sigfox_id);
    ata8520_pac(&dev, sigfox_pac);

    printf("Atmel version : %d:%d\n", atmel_version[0], atmel_version[1]);
    printf("SigFox version: %d:%d\n", sigfox_version[0], sigfox_version[1]);
    printf("SigFox ID: %s\n", (char*)sigfox_id);
    printf("SigFox PAC: %s\n", (char*)sigfox_pac);

    while(1) {
        puts("Sending message 'This is RIOT'");
        ata8520_send_msg(&dev, (uint8_t*)"This is RIOT");
        xtimer_sleep(15 * SEC_PER_MIN);
    }
    return 0;
}
