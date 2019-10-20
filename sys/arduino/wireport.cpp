/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_arduino_api
 * @{
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 *
 * @brief       Definition of the Arduino 'Wire Library' for TwoWire interfaces
 *
 * This library is the implementation of the [Arduino Wire Library]
 * (https://www.arduino.cc/en/Reference/Wire) for the I2C peripheral
 * interfaces in RIOT. It supports only I2C master mode and the functions
 * that are documented in the official [Arduino Reference]
 * (https://www.arduino.cc/en/Reference/Wire) of this library.
 *
 * The implementation is an adoption of the original Arduino Wire Library
 * which is published under the following copyright:
 *
 * ```
 * TwoWire.h - TWI/I2C library for Arduino & Wiring
 * Copyright (c) 2006 Nicholas Zambetti. All right reserved.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
 * ```
 *
 * The documentation in this file is partially extracted from the original
 * [Arduino Reference](https://www.arduino.cc/en/Reference/Wire) of this
 * library which is published under the
 * [Creative Commons Attribution-ShareAlike 3.0 License]
 * (https://creativecommons.org/licenses/by-sa/3.0/).
 */

extern "C" {
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
}

#include "Wire.h"

#include "log.h"
#include "panic.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define WIRE_PORT_OK                    (0)
#define WIRE_PORT_ERROR_DATA_TO_LONG    (1)
#define WIRE_PORT_ERROR_ADDR_NACK       (2)
#define WIRE_PORT_ERROR_DATA_NACK       (3)
#define WIRE_PORT_ERROR_OTHER           (4)

/* Initialize Class Variables */

uint8_t TwoWire::rxBuffer[WIREPORT_BUFFER_LENGTH];
uint8_t TwoWire::rxBufferIndex = 0;
uint8_t TwoWire::rxBufferLength = 0;

uint8_t TwoWire::txAddress = 0;
uint8_t TwoWire::txBuffer[WIREPORT_BUFFER_LENGTH];
uint8_t TwoWire::txBufferIndex = 0;
uint8_t TwoWire::txBufferLength = 0;
uint8_t TwoWire::txError = 0;

uint8_t TwoWire::transmitting = 0;

/* Constructors */

TwoWire::TwoWire(void)
{
    DEBUG("[wire] %s\n", __func__);
}

/* Public Methods */

void TwoWire::begin(void)
{
    DEBUG("[wire] %s\n", __func__);

    rxBufferIndex = 0;
    rxBufferLength = 0;

    txBufferIndex = 0;
    txBufferLength = 0;

    i2c_init(ARDUINO_I2C_DEV);
}

void TwoWire::begin(uint8_t addr)
{
    (void)addr;
    DEBUG("[wire] %s\n", __func__);
    core_panic(PANIC_GENERAL_ERROR, "[wire] slave mode is not supported");
}

void TwoWire::setClock(uint32_t clk)
{
    (void)clk;
    /* not possible, bus speed is defined by the board definition */
    DEBUG("[wire] %s: clock is defined by board definition\n", __func__);
}

uint8_t TwoWire::requestFrom(uint8_t addr, uint8_t size, uint8_t stop)
{
    DEBUG("[wire] %s: addr %02x, size %d, stop %d\n", __func__, addr, size, stop);

    if (size > WIREPORT_BUFFER_LENGTH) {
        size = WIREPORT_BUFFER_LENGTH;
    }

    uint8_t read = 0;

    if (i2c_acquire(ARDUINO_I2C_DEV) == 0) {
        if (i2c_read_bytes(ARDUINO_I2C_DEV, addr, rxBuffer, size,
                           stop ? 0 : I2C_NOSTOP) == 0) {
            read = size;
        }
        i2c_release(ARDUINO_I2C_DEV);
    }

    rxBufferIndex = 0;
    rxBufferLength = read;

    return read;
}

uint8_t TwoWire::requestFrom(uint8_t addr, uint8_t size) {
    return requestFrom(addr, size, 1);
}

void TwoWire::beginTransmission(uint8_t addr)
{
    DEBUG("[wire] %s: addr %02x\n", __func__, addr);
    transmitting = 1;
    txAddress = addr;
    txBufferIndex = 0;
    txBufferLength = 0;
}

uint8_t TwoWire::endTransmission(uint8_t stop)
{
    DEBUG("[wire] %s: stop %d\n", __func__, stop);

    if (txError) {
        return txError;
    }

    if (i2c_acquire(ARDUINO_I2C_DEV) != 0) {
        return WIRE_PORT_ERROR_OTHER;
    }

    int res = i2c_write_bytes(ARDUINO_I2C_DEV,
                              txAddress, txBuffer, txBufferLength,
                              stop ? 0 : I2C_NOSTOP);
    switch (res) {
        case 0: break;
        case ENXIO: res = WIRE_PORT_ERROR_ADDR_NACK;
                    break;
        case EIO: res = WIRE_PORT_ERROR_DATA_NACK;
                  break;
        default: res = WIRE_PORT_ERROR_OTHER;
    }

    i2c_release(ARDUINO_I2C_DEV);

    txBufferIndex = 0;
    txBufferLength = 0;
    txError = 0;
    transmitting = 0;

    return res;
}

uint8_t TwoWire::endTransmission(void)
{
    return endTransmission(true);
}

size_t TwoWire::write(uint8_t data)
{
    DEBUG("[wire] %s: data %02x\n", __func__, data);

    if (!transmitting || txBufferLength >= WIREPORT_BUFFER_LENGTH) {
        txError = WIRE_PORT_ERROR_DATA_TO_LONG;
        return 0;
    }

    txBuffer[txBufferIndex++] = data;
    txBufferLength = txBufferIndex;

    return 1;
}

size_t TwoWire::write(const uint8_t *data, size_t size)
{
    DEBUG("[wire] %s: data %p, size %d\n", __func__, data, size);
    for (size_t i = 0; i < size; i++) {
        if (!write(data[i])) {
            return i;
        }
    }

    return size;
}

int TwoWire::available(void)
{
    DEBUG("[wire] %s: return %d\n", __func__, rxBufferLength - rxBufferIndex);
    return rxBufferLength - rxBufferIndex;
}

int TwoWire::read(void)
{
    DEBUG("[wire] %s\n", __func__);

    int value = -1;
    if (rxBufferIndex < rxBufferLength) {
        value = rxBuffer[rxBufferIndex];
        ++rxBufferIndex;
    }
    return value;
}

int TwoWire::peek(void)
{
    DEBUG("[wire] %s\n", __func__);

    int value = -1;
    if (rxBufferIndex < rxBufferLength) {
        value = rxBuffer[rxBufferIndex];
    }
    return value;
}

void TwoWire::flush(void)
{
    DEBUG("[wire] %s\n", __func__);

    rxBufferIndex = 0;
    rxBufferLength = 0;
    txBufferIndex = 0;
    txBufferLength = 0;
}

/* single instance */

TwoWire Wire;
