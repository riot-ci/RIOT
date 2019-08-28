#ifndef OPT3001_PARAMS_H
#define OPT3001_PARAMS_H

#include "board.h"
#include "opt3001.h"
#include "opt3001_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters fot the opt3001
 * @{
 */
 #ifndef OPT3001_PARAM_I2C_DEV
 #define OPT3001_PARAM_I2C_DEV     I2C_DEV(0)
 #endif
 #ifndef OPT3001_PARAM_I2C_ADDR
 #define OPT3001_PARAM_I2C_ADDR    (OPT3001_I2C_ADDRESS)
 #endif

 #ifndef OPT3001_PARAMS
 #define OPT3001_PARAMS            { .i2c_dev           = OPT3001_PARAM_I2C_DEV,  \
                                     .i2c_addr          = OPT3001_PARAM_I2C_ADDR}

 #endif
 #ifndef OPT3001_SAUL_INFO
 #define OPT3001_SAUL_INFO         { .name = "opt3001" }
 #endif
 /**@}*/

 /**
  * @brief   OPT3001 configuration
  */
 static const opt3001_params_t opt3001_params[] =
 {
     OPT3001_PARAMS
 };

#ifdef __cplusplus
}
#endif

#endif /* OPT3001_PARAMS_H */
/** @} */
