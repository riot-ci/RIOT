/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @brief       Compute clock constants for STM32F[2|4|7] CPUs
 *
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#ifndef CLK_CONF_H
#define CLK_CONF_H

#include <stdbool.h>

/**
 * @name STM32 families
 * @ {
 */
enum fam {
    STM32F0,
    STM32F1,
    STM32F2,
    STM32F3,
    STM32F4,
    STM32F7,
    FAM_MAX,
};
/** @} */

/**
 * @name Supported models
 * @{
 */
enum {
    STM32F030,
    STM32F070,
    STM32F031,
    STM32F051,
    STM32F071,
    STM32F091,
    STM32F042,
    STM32F072,
    STM32F038,
    STM32F048,
    STM32F058,
    STM32F078,
    STM32F098,

    STM32F100,
    STM32F101,
    STM32F102,
    STM32F103,

    STM32F205,
    STM32F207,
    STM32F215,
    STM32F217,

    STM32F301,
    STM32F302,
    STM32F303,
    STM32F334,
    STM32F373,
    STM32F318,
    STM32F328,
    STM32F358,
    STM32F378,
    STM32F398,

    STM32F401,
    STM32F405,
    STM32F407,
    STM32F410,
    STM32F411,
    STM32F412,
    STM32F413,
    STM32F415,
    STM32F417,
    STM32F423,
    STM32F427,
    STM32F429,
    STM32F437,
    STM32F439,
    STM32F446,
    STM32F469,
    STM32F479,

    STM32F722,
    STM32F732,
    STM32F746,
    STM32F756,
    STM32F767,
    STM32F769,
    STM32F777,
    STM32F779,

    MODEL_MAX,
};
/** @} */

/**
 * @brief PLL configuration parameters
 *
 * PLL configuration follows the model:
 * ```
 *
 * pll_in  +----+  vco_in  +------------------------------+
 * --------| /M |----------|\  +----+ vco_out     +----+  |
 *         +----+          | --| xN |-------------| /P |--|-- pll_p_out
 *                         |   +----+         \   +----+  |
 *                         |                   |  +----+  |
 *                         |                   ---| /Q |--|-- pll_q_out
 *                         |                   |  +----+  |
 *                         |                   |  +----+  |
 *                         |                   ---| /R |--|-- pll_r_out
 *                         |                      +----+  |
 *                         +------------------------------+
 * ```
 *
 * vco_in = pll_in / M;
 * vco_out = vco_in * N;
 * pll_p_out = vco_out / P;
 * pll_q_out = vco_out / Q;
 * pll_r_out = vco_out / R;
 */
typedef struct {
    unsigned min_vco_input;  /**< Min VCO input */
    unsigned max_vco_input;  /**< Max VCO input */
    unsigned min_vco_output; /**< Min VCO output */
    unsigned max_vco_output; /**< Max VCO output */

    unsigned min_n; /**< Min N */
    unsigned max_n; /**< Max N */
    unsigned inc_n; /**< Increment between two values of N */

    unsigned min_m; /**< Min M */
    unsigned max_m; /**< Max M */
    unsigned inc_m; /**< Increment between two values of M */

    unsigned min_p; /**< Min P */
    unsigned max_p; /**< Max P */
    unsigned inc_p; /**< Increment between two values of P */

    unsigned min_q; /**< Min Q */
    unsigned max_q; /**< Max Q */
    unsigned inc_q; /**< Increment between two values of Q */
} pll_cfg_t;

/**
 * @brief Clock configuration
 */
typedef struct {
    enum fam family; /**< Family */

    unsigned max_coreclock; /**< Max coreclock */
    unsigned max_apb1;      /**< Max APB1 clock */
    unsigned max_apb2;      /**< Max APB2 clock */

    unsigned hsi; /**< HSI frequency */

    pll_cfg_t pll; /**< PLL configuration */

    bool has_pll_i2s;   /**< PLL I2S available */
    bool has_pll_sai;   /**< PLL SAI available */
    bool has_pll_i2s_m; /**< PLL I2S has a M factor */
    bool has_pll_sai_m; /**< PLL SAI has a M factor */
    bool has_pll_i2s_alt_input; /**< PLL I2S has an external input available */

    unsigned hsi_prediv;    /**< Value if HSI has a fixed prediv, 0 otherwise */

    int has_alt_48MHz; /**< 48MHz can be generated by an alternate source */
    bool need_48MHz;   /**< 48MHz is needed */
} clk_cfg_t;

/**
 * @name Alternative 48MHz sources
 * @{
 */
#define ALT_48MHZ_NO  0
#define ALT_48MHZ_I2S 1
#define ALT_48MHZ_SAI 2

#define ALT_48MHZ_Q   0
#define ALT_48MHZ_P   4
/** @} */

#define STM32F(x) [STM32F##x] = x
#define STM32F0(x) [STM32F0##x] = x

/** List of supported models */
static const unsigned stm32_model[] = {
    STM32F0(30),
    STM32F0(70),
    STM32F0(31),
    STM32F0(51),
    STM32F0(71),
    STM32F0(91),
    STM32F0(42),
    STM32F0(72),
    STM32F0(38),
    STM32F0(48),
    STM32F0(58),
    STM32F0(78),
    STM32F0(98),

    STM32F(100),
    STM32F(101),
    STM32F(102),
    STM32F(103),

    STM32F(205),
    STM32F(207),
    STM32F(215),
    STM32F(217),

    STM32F(301),
    STM32F(302),
    STM32F(303),
    STM32F(334),
    STM32F(373),
    STM32F(318),
    STM32F(328),
    STM32F(358),
    STM32F(378),
    STM32F(398),

    STM32F(401),
    STM32F(405),
    STM32F(407),
    STM32F(410),
    STM32F(411),
    STM32F(412),
    STM32F(413),
    STM32F(415),
    STM32F(417),
    STM32F(423),
    STM32F(427),
    STM32F(429),
    STM32F(437),
    STM32F(439),
    STM32F(446),
    STM32F(469),
    STM32F(479),

    STM32F(722),
    STM32F(732),
    STM32F(746),
    STM32F(756),
    STM32F(767),
    STM32F(769),
    STM32F(777),
    STM32F(779),
};

/** STM32F2xx / STM32F401 PLL config */
#define stm32f2_4_192_pll_cfg  { \
    .min_vco_input = 1000000U, \
    .max_vco_input = 2000000U, \
    .min_vco_output = 192000000U, \
    .max_vco_output = 432000000U, \
    .min_n = 50, \
    .max_n = 432, \
    .inc_n = 1, \
    .min_m = 2, \
    .max_m = 63, \
    .inc_m = 1, \
    .min_p = 2, \
    .max_p = 8, \
    .inc_p = 2, \
    .min_q = 2, \
    .max_q = 15, \
    .inc_q = 1, \
}

/** STM32F4 (except 401) / STM32F7 PLL config */
#define stm32f4_7_pll_cfg  { \
    .min_vco_input = 1000000U, \
    .max_vco_input = 2000000U, \
    .min_vco_output = 192000000U, \
    .max_vco_output = 432000000U, \
    .min_n = 50, \
    .max_n = 432, \
    .inc_n = 1, \
    .min_m = 2, \
    .max_m = 63, \
    .inc_m = 1, \
    .min_p = 2, \
    .max_p = 8, \
    .inc_p = 2, \
    .min_q = 2, \
    .max_q = 15, \
    .inc_q = 1, \
}

/**
 * @brief Clock config for supported cpu
 */
static const clk_cfg_t stm32_clk_cfg[] = {
    [STM32F030 ... STM32F098] = {
        .family = STM32F0,
        .max_coreclock = 48000000U,
        .max_apb1 = 48000000U,
        .max_apb2 = 0,
        .hsi = 8000000U,
        .pll = {
            .min_vco_input = 1000000U,
            .max_vco_input = 24000000U,
            .min_vco_output = 16000000U,
            .max_vco_output = 48000000U,
            .min_m = 1,
            .max_m = 16,
            .inc_m = 1,
            .min_n = 2,
            .max_n = 16,
            .inc_n = 1,
            .min_p = 1,
            .max_p = 1,
            .inc_p = 1,
        },
        .has_pll_i2s = false,
        .has_pll_sai = false,
        .has_pll_i2s_alt_input = false,
        .has_alt_48MHz = 0,
        .hsi_prediv = 2,
        .need_48MHz = false,
    },
    [STM32F100] = {
        .family = STM32F1,
        .max_coreclock = 24000000U,
        .max_apb1 = 24000000U,
        .max_apb2 = 24000000U,
        .hsi = 8000000U,
        .pll = {
            .min_vco_input = 1000000U,
            .max_vco_input = 24000000U,
            .min_vco_output = 16000000U,
            .max_vco_output = 24000000U,
            .min_m = 1,
            .max_m = 16,
            .inc_m = 1,
            .min_n = 2,
            .max_n = 16,
            .inc_n = 1,
            .min_p = 1,
            .max_p = 1,
            .inc_p = 1,
        },
        .has_pll_i2s = false,
        .has_pll_sai = false,
        .has_pll_i2s_alt_input = false,
        .has_alt_48MHz = 0,
        .hsi_prediv = 2,
        .need_48MHz = false,
    },
    [STM32F101 ... STM32F103] = {
        .family = STM32F1,
        .max_coreclock = 72000000U,
        .max_apb1 = 36000000U,
        .max_apb2 = 72000000U,
        .hsi = 8000000U,
        .pll = {
            .min_vco_input = 1000000U,
            .max_vco_input = 25000000U,
            .min_vco_output = 1000000U,
            .max_vco_output = 72000000U,
            .min_m = 1,
            .max_m = 16,
            .inc_m = 1,
            .min_n = 2,
            .max_n = 16,
            .inc_n = 1,
            .min_p = 1,
            .max_p = 1,
            .inc_p = 1,
        },
        .has_pll_i2s = false,
        .has_pll_sai = false,
        .has_pll_i2s_alt_input = false,
        .has_alt_48MHz = 0,
        .hsi_prediv = 2,
        .need_48MHz = false,
    },
    [STM32F205 ... STM32F217] = {
        .family = STM32F2,
        .max_coreclock = 120000000U,
        .max_apb1 = 30000000U,
        .max_apb2 = 60000000U,
        .hsi = 16000000U,
        .pll = stm32f2_4_192_pll_cfg,
        .has_pll_i2s = true,
        .has_pll_sai = false,
        .has_pll_i2s_alt_input = false,
        .has_alt_48MHz = 0,
        .need_48MHz = true,
    },
    [STM32F301 ... STM32F398] = {
        .family = STM32F3,
        .max_coreclock = 72000000U,
        .max_apb1 = 36000000U,
        .max_apb2 = 72000000U,
        .hsi = 8000000U,
        .pll = {
            .min_vco_input = 1000000U,
            .max_vco_input = 25000000U,
            .min_vco_output = 1000000U,
            .max_vco_output = 72000000U,
            .min_m = 1,
            .max_m = 16,
            .inc_m = 1,
            .min_n = 2,
            .max_n = 16,
            .inc_n = 1,
            .min_p = 1,
            .max_p = 1,
            .inc_p = 1,
        },
        .has_pll_i2s = false,
        .has_pll_sai = false,
        .has_pll_i2s_alt_input = false,
        .has_alt_48MHz = 0,
        .hsi_prediv = 2,
        .need_48MHz = false,
    },
    [STM32F401] = {
        .family = STM32F4,
        .max_coreclock = 84000000U,
        .max_apb1 = 42000000U,
        .max_apb2 = 84000000U,
        .hsi = 16000000U,
        .pll = stm32f2_4_192_pll_cfg,
        .has_pll_i2s = true,
        .has_pll_sai = false,
        .has_pll_i2s_m = false,
        .has_pll_i2s_alt_input = false,
        .has_alt_48MHz = 0,
        .need_48MHz = true,
    },
    [STM32F405 ... STM32F407] = {
        .family = STM32F4,
        .max_coreclock = 168000000U,
        .max_apb1 = 42000000U,
        .max_apb2 = 84000000U,
        .hsi = 16000000U,
        .pll = stm32f4_7_pll_cfg,
        .has_pll_i2s = true,
        .has_pll_sai = false,
        .has_pll_i2s_m = false,
        .has_pll_i2s_alt_input = false,
        .has_alt_48MHz = 0,
        .need_48MHz = true,
    },
    [STM32F410] = {
        .family = STM32F4,
        .max_coreclock = 100000000U,
        .max_apb1 = 50000000U,
        .max_apb2 = 100000000U,
        .hsi = 16000000U,
        .pll = stm32f4_7_pll_cfg,
        .has_pll_i2s = false,
        .has_pll_sai = false,
        .has_pll_i2s_m = false,
        .has_pll_i2s_alt_input = false,
        .has_alt_48MHz = 0,
        .need_48MHz = true,
    },
    [STM32F411] = {
        .family = STM32F4,
        .max_coreclock = 100000000U,
        .max_apb1 = 50000000U,
        .max_apb2 = 100000000U,
        .hsi = 16000000U,
        .pll = stm32f4_7_pll_cfg,
        .has_pll_i2s = true,
        .has_pll_sai = false,
        .has_pll_i2s_m = true,
        .has_pll_i2s_alt_input = false,
        .has_alt_48MHz = 0,
        .need_48MHz = true,
    },
    [STM32F412 ... STM32F413] = {
        .family = STM32F4,
        .max_coreclock = 100000000U,
        .max_apb1 = 50000000U,
        .max_apb2 = 100000000U,
        .hsi = 16000000U,
        .pll = stm32f4_7_pll_cfg,
        .has_pll_i2s = true,
        .has_pll_sai = true,
        .has_pll_i2s_m = true,
        .has_pll_sai_m = false,
        .has_pll_i2s_alt_input = true,
        .has_alt_48MHz = ALT_48MHZ_I2S,
        .need_48MHz = true,
    },
    [STM32F415 ... STM32F417] = {
        .family = STM32F4,
        .max_coreclock = 168000000U,
        .max_apb1 = 42000000U,
        .max_apb2 = 84000000U,
        .hsi = 16000000U,
        .pll = stm32f4_7_pll_cfg,
        .has_pll_i2s = true,
        .has_pll_sai = false,
        .has_pll_i2s_m = false,
        .has_pll_i2s_alt_input = false,
        .has_alt_48MHz = 0,
        .need_48MHz = true,
    },
    [STM32F423] = {
        .family = STM32F4,
        .max_coreclock = 100000000U,
        .max_apb1 = 50000000U,
        .max_apb2 = 100000000U,
        .hsi = 16000000U,
        .pll = stm32f4_7_pll_cfg,
        .has_pll_i2s = true,
        .has_pll_sai = true,
        .has_pll_i2s_m = true,
        .has_pll_sai_m = false,
        .has_pll_i2s_alt_input = true,
        .has_alt_48MHz = ALT_48MHZ_I2S,
        .need_48MHz = true,
    },
    [STM32F427 ... STM32F439] = {
        .family = STM32F4,
        .max_coreclock = 180000000U,
        .max_apb1 = 45000000U,
        .max_apb2 = 90000000U,
        .hsi = 16000000U,
        .pll = stm32f4_7_pll_cfg,
        .has_pll_i2s = true,
        .has_pll_sai = true,
        .has_pll_i2s_m = false,
        .has_pll_sai_m = false,
        .has_pll_i2s_alt_input = false,
        .has_alt_48MHz = 0,
        .need_48MHz = true,
    },
    [STM32F446] = {
        .family = STM32F4,
        .max_coreclock = 180000000U,
        .max_apb1 = 45000000U,
        .max_apb2 = 90000000U,
        .hsi = 16000000U,
        .pll = stm32f4_7_pll_cfg,
        .has_pll_i2s = true,
        .has_pll_sai = true,
        .has_pll_i2s_m = true,
        .has_pll_sai_m = true,
        .has_pll_i2s_alt_input = false,
        .has_alt_48MHz = ALT_48MHZ_SAI | ALT_48MHZ_P,
        .need_48MHz = true,
    },
    [STM32F469 ... STM32F479] = {
        .family = STM32F4,
        .max_coreclock = 180000000U,
        .max_apb1 = 45000000U,
        .max_apb2 = 90000000U,
        .hsi = 16000000U,
        .pll = stm32f4_7_pll_cfg,
        .has_pll_i2s = true,
        .has_pll_sai = true,
        .has_pll_i2s_m = false,
        .has_pll_sai_m = false,
        .has_pll_i2s_alt_input = false,
        .has_alt_48MHz = ALT_48MHZ_SAI | ALT_48MHZ_P,
        .need_48MHz = true,
    },
    [STM32F722 ... STM32F779] = {
        .family = STM32F7,
        .max_coreclock = 216000000U,
        .max_apb1 = 54000000U,
        .max_apb2 = 108000000U,
        .hsi = 16000000U,
        .pll = stm32f4_7_pll_cfg,
        .has_pll_i2s = true,
        .has_pll_sai = true,
        .has_pll_i2s_m = false,
        .has_pll_sai_m = false,
        .has_pll_i2s_alt_input = false,
        .has_alt_48MHz = ALT_48MHZ_SAI | ALT_48MHZ_P,
        .need_48MHz = true,
    },
};

#endif /* CLK_CONF_H */
