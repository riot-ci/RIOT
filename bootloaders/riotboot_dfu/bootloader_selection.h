/*
 * Copyright (C) 2020 Christian Amsüss <chrysn@fsfe.org>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/** @ingroup bootloaders
 *
 * @{
 *
 * @file
 * @brief Configuration for the riotboot_dfu bootloader
 *
 * @author Christian Amsüss <chrysn@fsfe.org>
 */

/* Include guards and cplusplus are more of a formality; this header is local
 * to the riotboot_dfu application that isn't written in C++ and not included
 * from anywhere else either, but still here for consistency (and because
 * otherwise the checks complain) */
#ifndef BOOTLOADER_SELECTION_H
#define BOOTLOADER_SELECTION_H

#ifdef __cplusplus
extern "C" {
#endif

/* Not including GPIO headers: we're not actually *doing* anything on GPIO, and
 * if no BTN0_PIN is defined we don't define anything either */
#include <board.h>


/** Pin (typically button) checked by the riotboot_dfu bootloader to decide
 * whether to enter DFU mode even if a valid image is present.
 *
 * Boards that insist on not using any button even though they have some can
 * define BTN_BOOTLOADER_NONE in their `board.h`. */
#if !defined(BTN_BOOTLOADER_PIN) && defined(BTN0_PIN) && !defined(BTN_BOOTLOADER_NONE)
#define BTN_BOOTLOADER_PIN BTN0_PIN
#endif

/** Mode into which the riotboot_dfu bootloader will configure @ref
 * BTN_BOOTLOADER_PIN before reading it */
#ifndef BTN_BOOTLOADER_MODE
#define BTN_BOOTLOADER_MODE BTN0_MODE
#endif

/** Interpretation of @ref BTN_BOOTLOADER_PIN. Set to true for active-low
 * buttons (go to DFU if the pin is low), otherwise to false (go to DFU if the
 * pin is high). */
#ifndef BTN_BOOTLOADER_INVERTED
#define BTN_BOOTLOADER_INVERTED true
#endif

#ifdef __cplusplus
}
#endif

#endif /* BOOTLOADER_SELECTION_H */

/** @} */
