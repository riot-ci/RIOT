/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_saul
 * @{
 *
 * @file
 * @brief       SAUL string functions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stddef.h>
#include <stdint.h>

#include "saul.h"

static const char *actuators[] = {
    [_SAUL_ACT_ANY]             = "ACT_ANY",
    [_SAUL_ACT_LED_RGB]         = "ACT_LED_RGB",
    [_SAUL_ACT_SERVO]           = "ACT_SERVO",
    [_SAUL_ACT_MOTOR]           = "ACT_MOTOR",
    [_SAUL_ACT_SWITCH]          = "ACT_SWITCH",
    [_SAUL_ACT_DIMMER]          = "ACT_DIMMER",
};

static const char *sensors[] = {
    [_SAUL_SENSE_ANY]           = "SENSE_ANY",
    [_SAUL_SENSE_BTN]           = "SENSE_BTN",
    [_SAUL_SENSE_TEMP]          = "SENSE_TEMP",
    [_SAUL_SENSE_HUM]           = "SENSE_HUM",
    [_SAUL_SENSE_LIGHT]         = "SENSE_LIGHT",
    [_SAUL_SENSE_ACCEL]         = "SENSE_ACCEL",
    [_SAUL_SENSE_MAG]           = "SENSE_MAG",
    [_SAUL_SENSE_GYRO]          = "SENSE_GYRO",
    [_SAUL_SENSE_COLOR]         = "SENSE_COLOR",
    [_SAUL_SENSE_PRESS]         = "SENSE_PRESS",
    [_SAUL_SENSE_ANALOG]        = "SENSE_ANALOG",
    [_SAUL_SENSE_UV]            = "SENSE_UV",
    [_SAUL_SENSE_OBJTEMP]       = "SENSE_OBJTEMP",
    [_SAUL_SENSE_COUNT]         = "SENSE_PULSE_COUNT",
    [_SAUL_SENSE_DISTANCE]      = "SENSE_DISTANCE",
    [_SAUL_SENSE_CO2]           = "SENSE_CO2",
    [_SAUL_SENSE_TVOC]          = "SENSE_TVOC",
    [_SAUL_SENSE_PROXIMITY]     = "SENSE_PROXIMITY",
    [_SAUL_SENSE_RSSI]          = "SENSE_RSSI",
    [_SAUL_SENSE_CHARGE]        = "SENSE_CHARGE",
    [_SAUL_SENSE_CURRENT]       = "SENSE_CURRENT",
    [_SAUL_SENSE_OCCUP]         = "SENSE_OCCUP",
    [_SAUL_SENSE_PM]            = "SENSE_PM",
    [_SAUL_SENSE_CAPACITANCE]   = "SENSE_CAPACITANCE",
    [_SAUL_SENSE_VOLTAGE]       = "SENSE_VOLTAGE",
    [_SAUL_SENSE_PH]            = "SENSE_PH",
};

const char *saul_class_to_str(const uint8_t class_id)
{
    const char *result = NULL;
    uint8_t id = class_id & SAUL_ID_MASK;
    uint8_t cat = class_id & SAUL_CAT_MASK;
    switch (cat) {
        case SAUL_CAT_UNDEF:
            return "CLASS_UNDEF";
        case SAUL_CAT_ACT:
            if (id < SAUL_ACT_NUMOF) {
                result = actuators[id];
            }
            break;
        case SAUL_CAT_SENSE:
            if (id < SAUL_SENSE_NUMOF) {
                result = sensors[id];
            }
            break;
        default:
            if (class_id == SAUL_CLASS_ANY) {
                return "CLASS_ANY";
            }
            break;
    }

    if (result == NULL) {
        result = "CLASS_UNKNOWN";
    }

    return result;
}
