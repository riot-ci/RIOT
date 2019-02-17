/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_mega-xplained
 * @{
 *
 * @file
 * @brief       Common configuration of MCU periphery for Mega Xplained
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

/**
 * @name    Clock configuration
 *
 * Frequency of the internal 8MHz RC oscillator.
 *
 * @{
 */
#define CLOCK_CORECLOCK     (8000000UL)
/** @} */

#include "periph_conf_atmega_common.h"

#endif /* PERIPH_CONF_H */
