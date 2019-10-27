/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup drivers_dcf77
 * @brief
 * @{
 *
 * @file
 * @brief   Bit definitions for DCF77 transmission
 *
 * @author  Michel Gerlach <michel.gerlach@haw-hamburg.de>
 */
#ifndef DCF77_INTERNAL_H
#define DCF77_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Potenzy of the Bit position
 * @{
 */
enum{
  POTENZY1 = 1,
  POTENZY2 = 2,
  POTENZY4 = 4,
  POTENZY8 = 8,
  POTENZY10 = 10,
  POTENZY20 = 20,
  POTENZY40 = 40,
  POTENZY80 = 80
};
/** @} */


/**
 * @name    BIT Position in DCF77 structure
 * @{
 */
 enum {
      DCF77_START             =  0,
      DCF77_WHEATER           =  1,
      DCF77_CALLING           = 15,
      DCF77_MEZ_MESZ_SHIFT    = 16,
      DCF77_MESZ1             = 17,
      DCF77_MESZ2             = 18,
      DCF77_SHIFT_SECOND      = 19,
      DCF77_TIME_INFORMATION  = 20,
      DCF77_MINUTE_SINGLE     = 21,
      DCF77_MINUTE_SECOND     = 22,
      DCF77_MINUTE_QUAD       = 23,
      DCF77_MINUTE_EIGHT      = 24,
      DCF77_MINUTE_TENNER     = 25,
      DCF77_MINUTE_TEWENTIES  = 26,
      DCF77_MINUTE_FOURTIES   = 27,
      DCF77_MINUTE_PR         = 28,
      DCF77_HOUR_SINGLE       = 29,
      DCF77_HOUR_SECOND       = 30,
      DCF77_HOUR_QUAD         = 31,
      DCF77_HOUR_EIGHT        = 32,
      DCF77_HOUR_TENNER       = 33,
      DCF77_HOUR_TEWENTIES    = 34,
      DCF77_HOUR_PR           = 35,
      DCF77_DAY_SINGLE        = 36,
      DCF77_DAY_SECOND        = 37,
      DCF77_DAY_QUAD          = 38,
      DCF77_DAY_EIGHT         = 39,
      DCF77_DAY_TENNER        = 40,
      DCF77_DAY_TEWENTIES     = 41,
      DCF77_WEEKDAY_SINGLE    = 42,
      DCF77_WEEKDAY_SECOND    = 43,
      DCF77_WEEKDAY_QUAD      = 44,
      DCF77_MONTH_SINGLE      = 45,
      DCF77_MONTH_SECOND      = 46,
      DCF77_MONTH_QUAD        = 47,
      DCF77_MONTH_EIGHT       = 48,
      DCF77_MONTH_TENNER      = 49,
      DCF77_YEAR_SINGLE       = 50,
      DCF77_YEAR_SECOND       = 51,
      DCF77_YEAR_QUAD         = 52,
      DCF77_YEAR_EIGHT        = 53,
      DCF77_YEAR_TENNER       = 54,
      DCF77_YEAR_TEWENTIES    = 55,
      DCF77_YEAR_FOURTIES     = 56,
      DCF77_YEAR_EIGHTIES     = 57,
      DCF77_DATE_PR           = 58
 };
/** @} */

/**
 * @brief device internal states
 */
enum {
    DCF77_STATE_IDLE,            /**< Device is in idle state */
    DCF77_STATE_START,           /**< Device is searching the start for a new minute */
    DCF77_STATE_RX,              /**< Device is in RX mode */
};


#ifdef __cplusplus
}
#endif

#endif /* DCF77_INTERNAL_H */
/** @} */
