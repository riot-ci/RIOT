/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_ata8520
 * @{
 *
 * @file
 *
 * @brief       Device interface for ATA8520 SigFox transceiver
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include "periph/spi.h"
#include "periph/gpio.h"
#include "xtimer.h"

#include "ata8520_internals.h"
#include "ata8520.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define SPIDEV               (dev->params.spi)
#define CSPIN                (dev->params.cs_pin)
#define INTPIN               (dev->params.int_pin)
#define RESETPIN             (dev->params.reset_pin)
#define SLEEPPIN             (dev->params.sleep_pin)
#define DELAY_10_MS          (10 * US_PER_MS)   /* 10 ms */
#define SEND_DELAY_10_S      (10 * MS_PER_SEC)  /* 10 s */

#if ENABLE_DEBUG
static void _print_atmel_status(uint8_t status)
{
    DEBUG("[DEBUG] Atmel status: %d\n", status);
    if (status & ATA8520_ATMEL_PA_MASK) {
        DEBUG("[Atmel] PA ON\n");
    }
    else {
        DEBUG("[Atmel] PA OFF\n");
    }
    if ((status >> 1) & ATA8520_ATMEL_SYSTEM_READY_MASK) {
        DEBUG("[Atmel] System ready to operate\n");
        return;
    }
    if ((status >> 1) & ATA8520_ATMEL_FRAME_SENT_MASK) {
        DEBUG("[Atmel] Frame sent\n");
        return;
    }
    switch ((status >> 1) & 0x0F) {
    case ATA8520_ATMEL_OK:
        DEBUG("[Atmel] System is ready\n");
        break;
    case ATA8520_ATMEL_COMMAND_ERROR:
        DEBUG("[Atmel] Command error / not supported\n");
        break;
    case ATA8520_ATMEL_GENERIC_ERROR:
        DEBUG("[Atmel] Generic error\n");
        break;
    case ATA8520_ATMEL_FREQUENCY_ERROR:
        DEBUG("[Atmel] Frequency error\n");
        break;
    case ATA8520_ATMEL_USAGE_ERROR:
        DEBUG("[Atmel] Usage error\n");
        break;
    case ATA8520_ATMEL_OPENING_ERROR:
        DEBUG("[Atmel] Opening error\n");
        break;
    case ATA8520_ATMEL_CLOSING_ERROR:
        DEBUG("[Atmel] Closing error\n");
        break;
    case ATA8520_ATMEL_SEND_ERROR:
        DEBUG("[Atmel] Send error\n");
        break;
    default:
        DEBUG("[Atmel] Invalid status code\n");
        break;
    }
}

static void _print_sigfox_status(uint8_t status)
{
    DEBUG("[DEBUG] SigFox status: %d\n", status);
    switch (status) {
    case ATA8520_SIGFOX_OK:
        DEBUG("[SigFox] OK\n");
        break;
    case ATA8520_SIGFOX_MANUFACTURER_ERROR:
        DEBUG("[SigFox] Manufacturer error\n");
        break;
    case ATA8520_SIGFOX_ID_OR_KEY_ERROR:
        DEBUG("[SigFox] ID or Key error\n");
        break;
    case ATA8520_SIGFOX_STATE_MACHINE_ERROR:
        DEBUG("[SigFox] State machine error\n");
        break;
    case ATA8520_SIGFOX_FRAME_SIZE_ERROR:
        DEBUG("[SigFox] Frame size error\n");
        break;
    case ATA8520_SIGFOX_MANUFACTURER_SEND_ERROR:
        DEBUG("[SigFox] Manufacturer send error\n");
        break;
    case ATA8520_SIGFOX_GET_VOLTAGE_TEMP_ERROR:
        DEBUG("[SigFox] Get voltage/temperature error\n");
        break;
    case ATA8520_SIGFOX_CLOSE_ERROR:
        DEBUG("[SigFox] Close issues encountered\n");
        break;
    case ATA8520_SIGFOX_API_ERROR:
        DEBUG("[SigFox] API error indication\n");
        break;
    case ATA8520_SIGFOX_GET_PN9_ERROR:
        DEBUG("[SigFox] Error getting PN9\n");
        break;
    case ATA8520_SIGFOX_GET_FREQUENCY_ERROR:
        DEBUG("[SigFox] Error getting frequency\n");
        break;
    case ATA8520_SIGFOX_BUILDING_FRAME_ERROR:
        DEBUG("[SigFox] Error building frame\n");
        break;
    case ATA8520_SIGFOX_DELAY_ROUTINE_ERROR:
        DEBUG("[SigFox] Error in delay routine\n");
        break;
    case ATA8520_SIGFOX_CALLBACK_ERROR:
        DEBUG("[SigFox] Callback causes error\n");
        break;
    case ATA8520_SIGFOX_TIMING_ERROR:
        DEBUG("[SigFox] Timing error\n");
        break;
    case ATA8520_SIGFOX_FREQUENCY_ERROR:
        DEBUG("[SigFox] Frequency error\n");
        break;
    default:
        DEBUG("[SigFox] Invalid status code [%d]\n", status);
        break;
    }
}
#endif

static void _irq_handler(void *arg)
{
    ata8520_t *dev = (ata8520_t *) arg;
    (void) dev;
    DEBUG("Event received !\n");
}

static void _getbus(const ata8520_t *dev)
{
    if (spi_acquire(SPIDEV, CSPIN, SPI_MODE_0, dev->params.spi_clk) < 0) {
        DEBUG("[ERROR] Cannot acquire SPI bus!\n");
    }
}

static void _calibrate_crystal(const ata8520_t *dev)
{
    DEBUG("[DEBUG] Calibrating crystal\n");
    xtimer_usleep(5 * US_PER_MS);
    _getbus(dev);
    spi_transfer_byte(SPIDEV, CSPIN, false, ATA8520_START_MEASUREMENT);
    spi_release(SPIDEV);
    xtimer_usleep(US_PER_MS);
    for (int i = 0; i < 6000; i++) {
        if (gpio_read(INTPIN) == 0) {
            DEBUG("[DEBUG] Crystal calibrated, exiting\n");
            ata8520_status(dev);
            break;
        }
        DEBUG("[DEBUG] Calibrating crystal, looping\n");
        xtimer_usleep(100 * US_PER_MS);
    }
    xtimer_usleep(5 * US_PER_MS);
}

static void _reset(const ata8520_t *dev)
{
    gpio_set(RESETPIN);
    xtimer_usleep(DELAY_10_MS);
    gpio_clear(RESETPIN);
    xtimer_usleep(DELAY_10_MS);
    gpio_set(RESETPIN);
}

static void _poweron(const ata8520_t *dev)
{
    gpio_set(SLEEPPIN);
    _reset(dev);
}

static void _poweroff(const ata8520_t *dev)
{
    gpio_clear(SLEEPPIN);
    _getbus(dev);
    spi_transfer_byte(SPIDEV, CSPIN, false, ATA8520_OFF_MODE);
    spi_release(SPIDEV);
    xtimer_usleep(US_PER_MS);
}

int ata8520_init(ata8520_t *dev, const ata8520_params_t *params)
{
    /* write config params to device descriptor */
    memcpy(&dev->params, params, sizeof(ata8520_params_t));

    /* Initialize pins*/
    if (gpio_init_int(INTPIN, GPIO_IN_PU,
                      GPIO_FALLING, _irq_handler, dev) < 0 ) {
        DEBUG("[ERROR] Interrupt pin not initialized\n");
        return -ATA8520_ERR_GPIO_INT;
    }
    if (gpio_init(SLEEPPIN, GPIO_OUT) < 0) {
        DEBUG("[ERROR] Sleep pin not initialized\n");
        return -ATA8520_ERR_GPIO_SLEEP;
    }
    if (gpio_init(RESETPIN, GPIO_OUT) < 0) {
        DEBUG("[ERROR] Reset pin not initialized\n");
        return -ATA8520_ERR_GPIO_RESET;
    }
    _poweron(dev);
    /* Initialize SPI bus*/
    if (spi_init_cs(dev->params.spi, CSPIN) < 0) {
        DEBUG("[ERROR] SPI not initialized\n");
        return -ATA8520_ERR_SPI;
    }

    xtimer_usleep(100 * US_PER_MS); /* 100 ms */

    uint8_t atmel_version[2];
    ata8520_atmel_version(dev, atmel_version);

    uint8_t sigfox_version[2];
    ata8520_sigfox_version(dev, sigfox_version);

    uint8_t sigfox_id[SIGFOX_ID_LENGTH + 1];
    ata8520_id(dev, sigfox_id);

    uint8_t sigfox_pac[SIGFOX_PAC_LENGTH + 1];
    ata8520_pac(dev, sigfox_pac);

    DEBUG("[DEBUG] Atmel version : %d:%d\n", atmel_version[0], atmel_version[1]);
    DEBUG("[DEBUG] SigFox version: %d:%d\n", sigfox_version[0], sigfox_version[1]);
    DEBUG("[DEBUG] SigFox ID: %s\n", (char*)sigfox_id);
    DEBUG("[DEBUG] SigFox PAC: %s\n", (char*)sigfox_pac);

    ata8520_status(dev);

    return ATA8520_OK;
}

void ata8520_system_reset(const ata8520_t *dev)
{
    _getbus(dev);
    spi_transfer_byte(SPIDEV, CSPIN, false, ATA8520_SYSTEM_RESET);
    spi_release(SPIDEV);
    xtimer_usleep(US_PER_MS);
}

void ata8520_atmel_version(const ata8520_t *dev, uint8_t *version)
{
    _getbus(dev);
    spi_transfer_byte(SPIDEV, CSPIN, true, ATA8520_ATMEL_VERSION);
    spi_transfer_byte(SPIDEV, CSPIN, true, 0);
    spi_transfer_bytes(SPIDEV, CSPIN, false, 0, version, sizeof(version));
    spi_release(SPIDEV);
    xtimer_usleep(US_PER_MS);
}

void ata8520_sigfox_version(const ata8520_t *dev, uint8_t *version)
{
    _getbus(dev);
    spi_transfer_byte(SPIDEV, CSPIN, true, ATA8520_SIGFOX_VERSION);
    spi_transfer_byte(SPIDEV, CSPIN, true, 0);
    spi_transfer_bytes(SPIDEV, CSPIN, false, 0, version, sizeof(version));
    spi_release(SPIDEV);
    xtimer_usleep(US_PER_MS);
}

void ata8520_status(const ata8520_t *dev)
{
    _getbus(dev);
    spi_transfer_byte(SPIDEV, CSPIN, true, ATA8520_OFF_MODE);
    spi_transfer_byte(SPIDEV, CSPIN, true, 0);
    spi_transfer_byte(SPIDEV, CSPIN, true, 0); /* SSM unused */
    uint8_t atmel = spi_transfer_byte(SPIDEV, CSPIN, true, 0);
    uint8_t sigfox = spi_transfer_byte(SPIDEV, CSPIN, true, 0);
    spi_transfer_byte(SPIDEV, CSPIN, false, 0);
    spi_release(SPIDEV);
    xtimer_usleep(US_PER_MS);

#if ENABLE_DEBUG
    _print_atmel_status(atmel);
    _print_sigfox_status(sigfox);
#else
    (void) atmel;
    (void) sigfox;
#endif
}

void ata8520_send_msg(const ata8520_t *dev, uint8_t *msg)
{
    DEBUG("[INFO] Sending message '%s'\n", (char*)msg);
    _poweron(dev);
    ata8520_status(dev);

    /* Verify message length */
    size_t msg_len = sizeof(msg);
    if (msg_len > 12) {
        DEBUG("[WARNING] Message exceeds the maximum 12 characters length "
              "allowed. It will be truncated.\n");
        msg_len = 12;
    }

    /* Write message in send buffer */
    _getbus(dev);
    spi_transfer_byte(SPIDEV, CSPIN, true, ATA8520_WRITE_TX_BUFFER);
    spi_transfer_bytes(SPIDEV, CSPIN, false, msg, NULL, msg_len);
    spi_release(SPIDEV);
    xtimer_usleep(US_PER_MS);

    /* Start a measurement to calibrate crystal */
    _calibrate_crystal(dev);

    /* Send the message */
    _getbus(dev);
    spi_transfer_byte(SPIDEV, CSPIN, false, ATA8520_SEND_FRAME);
    spi_release(SPIDEV);
    xtimer_usleep(US_PER_MS);

    /* Wait for the message to be sent and verify status */
    xtimer_usleep(SEND_DELAY_10_S);
    ata8520_status(dev);
    _poweroff(dev);
}

void ata8520_pac(const ata8520_t *dev, uint8_t *pac)
{
    memset(pac, 0, sizeof (pac));
    uint8_t pac_bytes[SIGFOX_PAC_LENGTH];
    _getbus(dev);
    spi_transfer_byte(SPIDEV, CSPIN, true, ATA8520_GET_PAC);
    spi_transfer_byte(SPIDEV, CSPIN, true, 0);
    spi_transfer_bytes(SPIDEV, CSPIN, false, NULL, pac_bytes, SIGFOX_PAC_LENGTH / 2);
    spi_release(SPIDEV);
    xtimer_usleep(US_PER_MS);
    for (size_t i = 0; i < SIGFOX_PAC_LENGTH / 2; ++i) {
        pac += sprintf((char*)pac, "%02X", pac_bytes[i]);
    }
}

void ata8520_id(const ata8520_t *dev, uint8_t *id)
{
    memset(id, 0, sizeof (id));
    uint8_t id_bytes[SIGFOX_ID_LENGTH] = { 0 };
    _getbus(dev);
    spi_transfer_byte(SPIDEV, CSPIN, true, ATA8520_GET_ID);
    spi_transfer_byte(SPIDEV, CSPIN, true, 0);
    spi_transfer_bytes(SPIDEV, CSPIN, false, NULL, id_bytes, SIGFOX_ID_LENGTH);
    spi_release(SPIDEV);
    xtimer_usleep(US_PER_MS);
    /* create id string (4 hexadecimal values) */
    for (size_t i = 0; i < SIGFOX_ID_LENGTH; ++i) {
        id += sprintf((char*)id, "%02X", id_bytes[SIGFOX_ID_LENGTH - i - 1]);
    }
}

void ata8520_internal_measurements(const ata8520_t *dev,
                                   uint16_t *temperature,
                                   uint16_t *idle_voltage,
                                   uint16_t *active_voltage)
{
    _getbus(dev);
    spi_transfer_byte(SPIDEV, CSPIN, false, ATA8520_START_MEASUREMENT);
    spi_release(SPIDEV);

    /* Wait a bit */
    xtimer_usleep(US_PER_MS);
    for (int i = 0; i < 10; i++) {
        if (!gpio_read(INTPIN)) {
            ata8520_status(dev);
            break;
        }
        else {
            xtimer_usleep(DELAY_10_MS);
        }
    }

    uint8_t buffer[2] = { 0 };
    _getbus(dev);
    spi_transfer_byte(SPIDEV, CSPIN, true, ATA8520_READ_SUP_TEMP);
    spi_transfer_byte(SPIDEV, CSPIN, true, 0);
    spi_transfer_bytes(SPIDEV, CSPIN, true, NULL, buffer, 2);
    *idle_voltage = buffer[0] << 8 | buffer[1];
    memset(buffer, 0, 2);
    spi_transfer_bytes(SPIDEV, CSPIN, true, NULL, buffer, 2);
    *active_voltage = buffer[0] << 8 | buffer[1];
    memset(buffer, 0, 2);
    spi_transfer_bytes(SPIDEV, CSPIN, false, NULL, buffer, 2);
    *temperature = buffer[0] << 8 | buffer[1];
    spi_release(SPIDEV);
    xtimer_usleep(US_PER_MS);
}

/** @} */
