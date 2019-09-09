/*
 * Copyright (C) 2019 Gunar Schorcht
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
 * @brief    Demonstrates the use of an Arduino library imported as package
 *
 * @author   Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <stdint.h>
#include <TalkingLED.h>

#include "board.h"

TalkingLED tled;

uint16_t sequence1[] = {500, 500, 500, 500, 0};
uint16_t Sequence2[] = {250, 250, 250, 250, 0};

int main(void)
{
    tled.begin(LED0_PIN);

    while (1) {
        tled.sequence(sequence1);
        tled.sequence(Sequence2);
    }

    return 0;
}
