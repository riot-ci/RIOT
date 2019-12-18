/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_ztimer
 * @{
 *
 * @file
 * @brief       ztimer initialization code
 *
 *
 * This file could benefit a lot fro code generation...
 *
 * Anyhow, this configures ztimer as follows:
 *
 * 1. if ztimer_msec in USEMODULE:
 * 1.1. assume ztimer_msec uses periph_timer
 * 1.2a. if no config given
 * 1.2a.1a. use xtimer config if available
 * 1.2a.1b. default to TIMER_DEV(0), 32bit
 * 1.2b. else, use config
 *
 * 2. if ztimer_usec in USEMODULE:
 * 2.1a. if periph_rtt in USEMODULE: use that
 * 2.1b: else: convert from ZTIMER_MSEC
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "board.h"
#include "ztimer.h"
#include "ztimer/convert_frac.h"
#include "ztimer/convert_shift.h"
#include "ztimer/convert_muldiv64.h"
#include "ztimer/periph.h"
#include "ztimer/rtt.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define WIDTH_TO_MAXVAL(width)  (0xffffffff >> (32 - width))

#define CONFIG_ZTIMER_USEC_TYPE_PERIPH (1)

/* for ZTIMER_USEC, use xtimer configuration if available and no ztimer
 * specific configuration is set. */
#if CONFIG_ZTIMER_USEC_TYPE_PERIPH
#  ifndef CONFIG_ZTIMER_USEC_DEV
#    ifdef XTIMER_DEV
#      define CONFIG_ZTIMER_USEC_DEV      XTIMER_DEV
#    endif
#    ifdef XTIMER_HZ
#      define CONFIG_ZTIMER_USEC_FREQ     XTIMER_HZ
#    endif
#    ifdef XTIMER_WIDTH
#      define CONFIG_ZTIMER_USEC_WIDTH    XTIMER_WIDTH
#    endif
#  endif
#endif

#ifndef CONFIG_ZTIMER_USEC_DEV
#define CONFIG_ZTIMER_USEC_DEV (TIMER_DEV(0))
#endif

#ifndef CONFIG_ZTIMER_USEC_FREQ
#define CONFIG_ZTIMER_USEC_FREQ (1000000LU)
#endif

#ifndef CONFIG_ZTIMER_USEC_MIN
#define CONFIG_ZTIMER_USEC_MIN (5)
#endif

#ifndef CONFIG_ZTIMER_USEC_WIDTH
#define CONFIG_ZTIMER_USEC_WIDTH (32)
#endif

#if MODULE_ZTIMER_USEC
#  if CONFIG_ZTIMER_USEC_TYPE_PERIPH
static ztimer_periph_t _ztimer_periph_usec = { .min = CONFIG_ZTIMER_USEC_MIN };
#    if CONFIG_ZTIMER_USEC_FREQ == 1000000LU
ztimer_clock_t *const ZTIMER_USEC = &_ztimer_periph_usec.super;
#    elif CONFIG_ZTIMER_USEC_FREQ == 250000LU
static ztimer_convert_shift_t _ztimer_convert_shift_usec;
ztimer_clock_t *const ZTIMER_USEC = &_ztimer_convert_shift_usec.super.super;
#    elif
static ztimer_convert_frac_t _ztimer_convert_frac_usec;
ztimer_clock_t *const ZTIMER_USEC = &_ztimer_convert_frac_usec.super.super;
#  define ZTIMER_USEC_CONVERT_LOWER (&_ztimer_periph_usec.super)
#    endif
#   else
#     error ztimer_usec selected, but no configuration available!
#  endif
#endif

#if MODULE_ZTIMER_MSEC
#  if MODULE_PERIPH_RTT
static ztimer_rtt_t _ztimer_rtt_msec;
#  define ZTIMER_RTT_INIT (&_ztimer_rtt_msec)
#    if RTT_FREQUENCY!=1000000LU
static ztimer_convert_frac_t _ztimer_convert_frac_msec;
ztimer_clock_t *const ZTIMER_MSEC = &_ztimer_convert_frac_msec.super;
#  define ZTIMER_MSEC_CONVERT_LOWER_FREQ    RTT_FREQUENCY
#  define ZTIMER_MSEC_CONVERT_LOWER         (&_ztimer_rtt_msec)
#    else
ztimer_clock_t *const ZTIMER_MSEC = &_ztimer_rtt_msec.super;
#    endif
#  elif MODULE_ZTIMER_USEC
static ztimer_convert_frac_t _ztimer_convert_frac_msec;
ztimer_clock_t *const ZTIMER_MSEC = &_ztimer_convert_frac_msec.super.super;
#    if CONFIG_ZTIMER_USEC_FREQ < 1000000lu
#      define ZTIMER_MSEC_CONVERT_LOWER         ZTIMER_USEC_CONVERT_LOWER
#      define ZTIMER_MSEC_CONVERT_LOWER_FREQ    CONFIG_ZTIMER_USEC_FREQ
#    else
#      define ZTIMER_MSEC_CONVERT_LOWER (ZTIMER_USEC)
#      define ZTIMER_MSEC_CONVERT_LOWER_FREQ    1000000LU
#    endif
#  else
#  error No suitable ZTIMER_MSEC config. Maybe add USEMODULE += ztimer_usec?
#  endif
#endif

void ztimer_init(void)
{
#if MODULE_ZTIMER_USEC
#  if CONFIG_ZTIMER_USEC_TYPE_PERIPH
    DEBUG(
        "ztimer_init(): ZTIMER_USEC using periph timer %u, freq %lu, width %u\n",
        CONFIG_ZTIMER_USEC_DEV, CONFIG_ZTIMER_USEC_FREQ, CONFIG_ZTIMER_USEC_WIDTH);

    ztimer_periph_init(&_ztimer_periph_usec, CONFIG_ZTIMER_USEC_DEV,
                       CONFIG_ZTIMER_USEC_FREQ,
                       WIDTH_TO_MAXVAL(CONFIG_ZTIMER_USEC_WIDTH));
#  endif
#  if CONFIG_ZTIMER_USEC_FREQ != 1000000LU
#    if CONFIG_ZTIMER_USEC_FREQ == 250000LU
    DEBUG("ztimer_init(): ZTIMER_USEC convert_shift %lu to 1000000\n",
            CONFIG_ZTIMER_USEC_FREQ);
    ztimer_convert_shift_up_init(&_ztimer_convert_shift_usec, &_ztimer_periph_usec.super, 2);
#    else
    DEBUG("ztimer_init(): ZTIMER_USEC convert_frac %lu to 1000000\n",
            CONFIG_ZTIMER_USEC_FREQ);
    ztimer_convert_frac_init(&_ztimer_convert_frac_usec, &_ztimer_periph_usec.super,
            1000000lu, CONFIG_ZTIMER_USEC_FREQ);
#    endif
#  endif
#endif

#ifdef ZTIMER_RTT_INIT
    DEBUG("ztimer_init(): initializing rtt\n");
    ztimer_rtt_init(ZTIMER_RTT_INIT);
#endif

#if MODULE_ZTIMER_MSEC
#  if ZTIMER_MSEC_CONVERT_LOWER_FREQ
    DEBUG("ztimer_init(): ZTIMER_MSEC convert_frac from %lu to 1000\n",
          ZTIMER_MSEC_CONVERT_LOWER_FREQ);
    ztimer_convert_frac_init(&_ztimer_convert_frac_msec,
                             ZTIMER_MSEC_CONVERT_LOWER,
                             1000lu, ZTIMER_MSEC_CONVERT_LOWER_FREQ);
#  endif
#endif
}
