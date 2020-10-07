/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
#ifndef CONFIG_CONFIG_H
#define CONFIG_CONFIG_H

#include <errno.h>
#include <stdint.h>

#include "dpl/queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Type of configuration value.
 */
typedef enum conf_type {
    CONF_NONE = 0,
    CONF_DIR,
    /** 8-bit signed integer */
    CONF_INT8,
    /** 16-bit signed integer */
    CONF_INT16,
    /** 32-bit signed integer */
    CONF_INT32,
    /** 64-bit signed integer */
    CONF_INT64,
    /** String */
    CONF_STRING,
    /** Bytes */
    CONF_BYTES,
    /** Floating point */
    CONF_FLOAT,
    /** Double precision */
    CONF_DOUBLE,
    /** Boolean */
    CONF_BOOL,
} __attribute__((__packed__)) conf_type_t;

/**
 * Convenience routine for converting value passed as a string to native
 * data type.
 *
 * @param val_str Value of the configuration item as string.
 * @param type   Type of the value to convert to.
 * @param vp     Pointer to variable to fill with the decoded value.
 * @param maxlen Size of that variable.
 *
 * @return 0 on success, non-zero on failure.
 */
int conf_value_from_str(char *val_str, enum conf_type type, void *vp,
                        int maxlen);
/**
 * Convert a string into a value of type
 */
#define CONF_VALUE_SET(str, type, val) \
    conf_value_from_str((str), (type), &(val), sizeof(val))

#ifdef __cplusplus
}
#endif

/**
 * @} SysConfig
 */

#endif /* CONFIG_CONFIG_H */
