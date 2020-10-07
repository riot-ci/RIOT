#ifndef CONTROL_H
#define CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdio.h>
#include "timex.h"

/**
 * @brief Anchor address
 */
#ifndef ANCHOR_ADDRESS
#define ANCHOR_ADDRESS      0x1234
#endif

/**
 * @brief Range request period
 */
#ifndef RANGE_REQUEST_T
#define RANGE_REQUEST_T      (US_PER_SEC / 25)
#endif

/**
 * @brief Starts ranging
 */
void *init_ranging(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* CONTROL_H */
