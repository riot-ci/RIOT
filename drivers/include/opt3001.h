#ifndef OPT3001_H
#define OPT3001_H

#include <stdint.h>
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEV_I2C     (dev->params.i2c_dev) /**< BUS */
#define DEV_ADDR    (dev->params.i2c_addr) /**< ADDR */

#ifndef OPT3001_I2C_ADDRESS
#define OPT3001_I2C_ADDRESS   (0x45)
#endif

/* 0x0000 for 100ms and 0x0800 for 800ms */
#ifndef OPT3001_CONVERSION_TIME
#define OPT3001_CONVERSION_TIME   OPT3001_CONVERSION_TIME_800_MS
#endif

/**
 * @brief   Default raw value mode
 *
 * If set to 0, measurements will be converted to lux.
 * If set to 1, raw readings will be returned.
 */
#ifndef OPT3001_USE_RAW_VALUES
#define OPT3001_USE_RAW_VALUES (0)
#endif

/**
 * @brief   Parameters needed for device initialization
 */
typedef struct {
    i2c_t i2c_dev;
    uint8_t i2c_addr;
    uint16_t conversion_time;
} opt3001_params_t;

typedef struct {
    opt3001_params_t params;
} opt3001_t;

enum {
    OPT3001_OK,
    OPT3001_ERROR_BUS,
    OPT3001_ERROR_DEV,
    OPT3001_ERROR_CONF,
    OPT3001_ERROR
};

int opt3001_init(opt3001_t *dev, const opt3001_params_t *params);

/**
 * @brief   Reset the OPT3001 sensor, afterwards it should be reinitialized.
 *
 * @param[out] dev          device descriptor of sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int opt3001_reset(const opt3001_t *dev);

/**
 * @brief   Set active mode, this enables periodic measurements.
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
<<<<<<< HEAD
int opt3001_set_active(const opt3001_t *dev);
=======
int opt3001_read(const opt3001_t *dev, uint16_t *crf, uint16_t *rawl);
>>>>>>> e8bc7e708... driver/opt3001: Saul implementation

/**
 * @brief   Read sensor's data.
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] rawl         raw lux value
 * @param[out] crf          conversion ready, 0 if a conversion is in progress
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int opt3001_read(const opt3001_t *dev, uint16_t *crf, uint16_t *rawl);

void opt3001_convert(int16_t rawl, float *convl);

#ifdef __cplusplus
}
#endif

#endif /* OPT3001_H */
/** @} */
