/*
 * Copyright (C) 2014 Simon Brummer
 *               2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_dac DAC
 * @ingroup     drivers_periph
 * @brief       DAC peripheral driver interface
 *
 * Similar to the ADC driver interface (@ref drivers_periph_adc), the DAC
 * interface uses the concept of lines, corresponds to a tuple of a DAC device
 * and a DAC output channel.
 *
 * The DAC interface expects data to be served as a 16-bit unsigned integer,
 * independent of the actual resolution of the hardware device. It is up to the
 * DAC driver, to scale the given value to the maximal width that can be
 * handled. The device driver should, on the other hand, implement the DAC in a
 * way, that it will use the bit width, that comes closest to 16-bit.
 *
 * This kind of 'auto-scaling' is quite sufficient for this interface, as
 * standard DAC peripherals use a fixed conversion resolution internally anyway,
 * so that any particular bit-width configuration on this driver level would not
 * have much effect...
 *
 * # (Low-) Power Implications
 *
 * The configured DAC peripherals are active (and consume power) from the point
 * of initialization. When calling dac_poweroff(), the implementation **should**
 * disable the given DAC line and put the DAC peripheral to sleep (e.g. through
 * peripheral clock gating). When woken up again through dac_poweron(), the
 * given DAC line **should** transparently continue it's previous operation.
 *
 * The DAC driver implementation may need to block (and free) certain power
 * modes in the driver's dac_init(), dac_poweron(), and the dac_poweroff()
 * functions.
 *
 * @{
 * @file
 * @brief       DAC peripheral driver interface definition
 *
 * @author      Simon Brummer <simon.brummer@haw-hamburg.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_DAC_H
#define PERIPH_DAC_H

#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#include "periph_cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define default DAC type identifier
 */
#ifndef HAVE_DAC_T
typedef unsigned int dac_t;
#endif

/**
 * @brief   Return codes used by the DAC driver interface
 */
enum {
    DAC_OK     = 0,
    DAC_NOLINE = -1
};

/**
 * @brief   Default DAC undefined value
 */
#ifndef DAC_UNDEF
#define DAC_UNDEF           (UINT_MAX)
#endif

/**
 * @brief   Default DAC access macro
 */
#ifndef DAC_LINE
#define DAC_LINE(x)         (x)
#endif

/**
 * @brief   Initialize the given DAC line
 *
 * After initialization, the corresponding DAC line is active and its output is
 * set to 0.
 *
 * @param[in] line         DAC line to initialize
 *
 * @return  DAC_OK on success
 * @return  DAC_NOLINE on invalid DAC line
 */
int8_t dac_init(dac_t line);

/**
 * @brief   The callback that will be called when the end of the current sample buffer
 *          has been reached.
 *          Should be used to start filling the next sample buffer with @ref dac_play.
 *
 * @note    Will be called in interrupt context. Only use the callback to signal a
 *          thread. Don't directly fill the sample buffer in the callback.
 */
typedef void (*dac_cb_t)(void *arg);

/**
 * @brief   A sample has a resolution of 8 bit
 */
#ifndef DAC_FLAG_8BIT
#define DAC_FLAG_8BIT   (0x0)
#endif

/**
 * @brief   A sample has a resolution of 16 bit
 */
#ifndef DAC_FLAG_16BIT
#define DAC_FLAG_16BIT  (0x1)
#endif

/**
 * @brief   Write a value onto DAC Device on a given Channel
 *
 * The value is always given as 16-bit value and is internally scaled to the
 * actual resolution that the DAC unit provides (e.g. 12-bit). So to get the
 * maximum output voltage, this function has to be called with @p value set to
 * 65535 (UINT16_MAX).
 *
 * @param[in] line         DAC line to set
 * @param[in] value        value to set @p line to
 */
void dac_set(dac_t line, uint16_t value);

/**
 * @brief   Enable the given DAC line
 *
 * @param[in] line          DAC line to power on
 */
void dac_poweron(dac_t line);

/**
 * @brief   Disable the given DAC line
 *
 * @param[in] line          DAC line to power off
 */
void dac_poweroff(dac_t line);

/**
 * @brief   Initialize a DAC for playing audio samples
 *          A user defined callback can be provided that will be called when
 *          the next buffer can be queued.
 * @experimental
 *
 * @param[in] dac           The DAC to initialize
 * @param[in] sample_rate   The sample rate in Hz
 * @param[in] flags         Optional flags (@ref DAC_FLAG_16BIT)
 * @param[in] cb            Will be called when the next buffer can be queued
 * @param[in] cb_arg        Callback argument
 */
void dac_play_init(dac_t dac, uint16_t sample_rate, uint8_t flags,
                   dac_cb_t cb, void *cb_arg);

/**
 * @brief   Change the 'buffer done' callback.
 *          A user defined callback can be provided that will be called when
 *          the next buffer can be queued.
 *          This function can be used to change the callback on the fly.
 * @experimental
 *
 * @param[in] dac           The DAC to configure
 * @param[in] cb            Called when the played buffer is done
 * @param[in] cb_arg        Callback argument
 */
void dac_play_set_cb(dac_t dac, dac_cb_t cb, void *cb_arg);

/**
 * @brief   Play a buffer of (audio) samples on a DAC
 *
 *          If this function is called while another buffer is already
 *          being played, the new `buf` will be played when the current
 *          buffer has finished playing.
 *
 *          The DAC implementations allows one buffer to be queued
 *          (double buffering).
 *
 *          Whenever a new buffer can be queued, the @ref dac_cb_t
 *          callback function will be executed.
 *
 * @experimental
 *
 * @param[in] dac           The DAC to play the sample on
 * @param[in] buf           A buffer with (audio) samples
 * @param[in] len           Number of bytes to be played
 */
void dac_play(dac_t dac, const void *buf, size_t len);

/**
 * @brief   Stop playback of the current sample buffer
 *
 * @param[in] dac           The DAC to stop
 */
void dac_play_stop(dac_t dac);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_DAC_H */
/** @} */
