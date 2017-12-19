#ifndef LC709203F_PARAMS_H
#define LC709203F_PARAMS_H

#include "board.h"  /* THIS INCLUDE IS MANDATORY */
#include "lc709203f.h"

/**
 * @brief   Default configuration parameters for LC709203F sensors
 * @{
 */
#ifndef LC709203F_PARAMS_I2C
#define LC709203F_PARAMS_I2C            (I2C_DEV(0))
#endif
#ifndef LC709203F_PARAMS_ADDR
#define LC709203F_PARAMS_ADDR           (0xb)
#endif
#ifndef LC709203F_PARAMS_ALARM_PIN
#define LC709203F_PARAMS_ALARM_PIN      GPIO_PIN(4, 6)
#endif

#ifndef LC709203F_PARAMS
#define LC709203F_PARAMS            { .alarm_pin = LC709203F_PARAMS_ALARM_PIN, \
                                      .bus  = LC709203F_PARAMS_I2C, \
                                      .addr = LC709203F_PARAMS_ADDR }
#endif
/** @} */

/**
 * @brief   Allocation of LC709203f configuration
 */
static const lc709203f_params_t params_default[] = {
    #ifdef LC709203F_PARAMS_BOARD
        LC709203F_PARAMS_BOARD
    #else
        LC709203F_PARAMS
    #endif
};
#endif /*LC709203F_PARAMS_H*/
