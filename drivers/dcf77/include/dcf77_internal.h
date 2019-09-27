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
     START             =  0,
     WHEATER           =  1,
     CALLING           = 15,
     MEZ_MESZ_SHIFT    = 16,
     MEZ               = 17,
     MESZ              = 18,
     SHIFT_SECOND      = 19,
     TIME_INFORMATION  = 20,
     MINUTE_SINGLE     = 21,
     MINUTE_SECOND     = 22,
     MINUTE_QUAD       = 23,
     MINUTE_EIGHT      = 24,
     MINUTE_TENNER     = 25,
     MINUTE_TEWENTIES  = 26,
     MINUTE_FOURTIES   = 27,
     MINUTE_PR         = 28,
     HOUR_SINGLE       = 29,
     HOUR_SECOND       = 30,
     HOUR_QUAD         = 31,
     HOUR_EIGHT        = 32,
     HOUR_TENNER       = 33,
     HOUR_TEWENTIES    = 34,
     HOUR_PR           = 35,
     DAY_SINGLE        = 36,
     DAY_SECOND        = 37,
     DAY_QUAD          = 38,
     DAY_EIGHT         = 39,
     DAY_TENNER        = 40,
     DAY_TEWENTIES     = 41,
     WEEKDAY_SINGLE    = 42,
     WEEKDAY_SECOND    = 43,
     WEEKDAY_QUAD      = 44,
     MONTH_SINGLE      = 45,
     MONTH_SECOND      = 46,
     MONTH_QUAD        = 47,
     MONTH_EIGHT       = 48,
     MONTH_TENNER      = 49,
     YEAR_SINGLE       = 50,
     YEAR_SECOND       = 51,
     YEAR_QUAD         = 52,
     YEAR_EIGHT        = 53,
     YEAR_TENNER       = 54,
     YEAR_TEWENTIES    = 55,
     YEAR_FOURTIES     = 56,
     YEAR_EIGHTIES     = 57,
     YEAR_PR           = 58
 };
/** @} */



#ifdef __cplusplus
}
#endif

#endif /* DCF77_INTERNAL_H */
/** @} */
