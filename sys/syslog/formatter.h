/*
 * Copyright (C) 2021 Continental
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_syslog
 *
 * @brief       Syslog formatter definitions
 *
 * @{
 * @file
 *
 * @author      Vincent Dupont <vincent.dupont@continental-its.com>
 */

#ifndef FORMATTER_H
#define FORMATTER_H

#include <stdarg.h>
#include <sys/types.h>

#include "syslog_backend.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   rfc5424-compatible time string format
 *
 * This should be used with @p TIME_PARAMS()
 */
#define TIME_FMT        "%d-%02d-%02dT%02d:%02d:%02dZ"

/**
 * @brief   @p TIME_FMT params
 *
 * @param   t   a @p tm_t structure
 */
#define TIME_PARAMS(t)  ((t).tm_year + 1900), ((t).tm_mon + 1), ((t).tm_mday), ((t).tm_hour), \
                        ((t).tm_min), ((t).tm_sec)

/**
 * @brief   Build syslog string message header
 *
 * This function formats a log message header. The default implementation uses a rfc5424-compatible
 * header.
 *
 * ```
 * RFC5424 specifies a syslog message as:
 * SYSLOG-MSG      = HEADER SP STRUCTURED-DATA [SP MSG]
 *
 * HEADER          = PRI VERSION SP TIMESTAMP SP HOSTNAME
 *                   SP APP-NAME SP PROCID SP MSGID
 * PRI             = "<" PRIVAL ">"
 * PRIVAL          = 1*3DIGIT ; range 0 .. 191
 * VERSION         = NONZERO-DIGIT 0*2DIGIT
 * HOSTNAME        = NILVALUE / 1*255PRINTUSASCII
 *
 * APP-NAME        = NILVALUE / 1*48PRINTUSASCII
 * PROCID          = NILVALUE / 1*128PRINTUSASCII
 * MSGID           = NILVALUE / 1*32PRINTUSASCII
 *
 * TIMESTAMP       = NILVALUE / FULL-DATE "T" FULL-TIME
 * FULL-DATE       = DATE-FULLYEAR "-" DATE-MONTH "-" DATE-MDAY
 * DATE-FULLYEAR   = 4DIGIT
 * DATE-MONTH      = 2DIGIT  ; 01-12
 * DATE-MDAY       = 2DIGIT  ; 01-28, 01-29, 01-30, 01-31 based on
 *                           ; month/year
 * FULL-TIME       = PARTIAL-TIME TIME-OFFSET
 * PARTIAL-TIME    = TIME-HOUR ":" TIME-MINUTE ":" TIME-SECOND
 *                   [TIME-SECFRAC]
 * TIME-HOUR       = 2DIGIT  ; 00-23
 * TIME-MINUTE     = 2DIGIT  ; 00-59
 * TIME-SECOND     = 2DIGIT  ; 00-59
 * TIME-SECFRAC    = "." 1*6DIGIT
 * TIME-OFFSET     = "Z" / TIME-NUMOFFSET
 * TIME-NUMOFFSET  = ("+" / "-") TIME-HOUR ":" TIME-MINUTE
 *
 * STRUCTURED-DATA = NILVALUE / 1*SD-ELEMENT
 * SD-ELEMENT      = "[" SD-ID *(SP SD-PARAM) "]"
 * SD-PARAM        = PARAM-NAME "=" %d34 PARAM-VALUE %d34
 * SD-ID           = SD-NAME
 * PARAM-NAME      = SD-NAME
 * PARAM-VALUE     = UTF-8-STRING ; characters '"', '\' and
 *                                ; ']' MUST be escaped.
 * SD-NAME         = 1*32PRINTUSASCII
 *                   ; except '=', SP, ']', %d34 (")
 *
 * MSG             = MSG-ANY / MSG-UTF8
 * MSG-ANY         = *OCTET ; not starting with BOM
 * MSG-UTF8        = BOM UTF-8-STRING
 * BOM             = %xEF.BB.BF
 *
 * UTF-8-STRING    = *OCTET ; UTF-8 string as specified
 *                   ; in RFC 3629
 *
 * OCTET           = %d00-255
 * SP              = %d32
 * PRINTUSASCII    = %d33-126
 * NONZERO-DIGIT   = %d49-57
 * DIGIT           = %d48 / NONZERO-DIGIT
 * NILVALUE        = "-"
 * ```
 *
 * In the default implementation:
 *  - MSGID is not used (set to NILVALUE)
 *  - STRUCTURED-DATA is not used (set to NILVALUE)
 *  - Time is assumed UTC
 *
 * @param[in,out]   msg         syslog message to format
 * @param[in]       hostname    hostname
 *
 * @return number of characters written in @p msg->msg (excluding null-terminating character) on success
 * @return < 0 on error
 */
ssize_t syslog_build_head(struct syslog_msg *msg, const char *hostname);

/**
 * @brief   Build syslog string message
 *
 * @param[in,out]   msg         syslog message to format
 * @param[in]       fmt         format string (printf-like)
 * @param[in]       ap          parameter list
 *
 * @return number of bytes added to @p msg->msg on success
 * @return < 0 on error
 */
ssize_t syslog_build_msg(struct syslog_msg *msg, const char *fmt, va_list ap);

/**
 * @brief   Build syslog string message footer
 *
 * @param[in,out]   msg         syslog message to format
 *
 * @return number of bytes added to @p msg->msg on success
 * @return < 0 on error
 */
ssize_t syslog_build_foot(struct syslog_msg *msg);

#if IS_USED(MODULE_SYSLOG_FORMATTER_DEFAULT) || IS_USED(MODULE_SYSLOG_FORMATTER_FMT) || DOXYGEN
/**
 * @brief   Enable usage of default message formatter (vsprintf-based)
 */
#define SYSLOG_FORMATTER_NEEDS_DEFAULT_MSG  1

/**
 * @brief   Enable usage of default footer formatter (empty footer)
 */
#define SYSLOG_FORMATTER_NEEDS_DEFAULT_FOOT 1
#endif

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* FORMATTER_H */
