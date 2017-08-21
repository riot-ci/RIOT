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
 * @author      Vincent Dupont
 *
 * @}
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define ENABLE_DEBUG (1)
#if ENABLE_DEBUG
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

/** Ma coreclock frequency */
static unsigned max_coreclock = 0;

static unsigned max_apb1 = 0;
static unsigned max_apb2 = 0;

/** Min VCO input (default: 1MHz) */
static unsigned min_vco_input  = 1000000U;
/** Max VCO input freq (default: 2MHz) */
static unsigned max_vco_input  = 2000000U;
/** Min VCO output freq (default: 100MHz) */
static unsigned min_vco_output = 100000000U;
/** Max VCO output freq (default: 432MHz) */
static unsigned max_vco_outut  = 432000000U;

/** CPU has a PLL I2S */
static bool has_pll_i2s = false;
/** CPU has a PLL SAI */
static bool has_pll_sai = false;

/** PLL I2S has a M factor */
static bool has_pll_i2s_m = false;
/** PLL SAI has a M factor */
static bool has_pll_sai_m = false;

/**
 * @name Alternative 48MHz sources
 * @{
 */
#define ALT_48MHZ_NO  0
#define ALT_48MHZ_I2S 1
#define ALT_48MHZ_SAI 2
/** @} */

/** CPU supports alternative 48MHz source */
static int has_alt_48MHz = ALT_48MHZ_NO;

/**
 * @brief Check if N/P pair is valid
 *
 * Check if N/P (alternatively N/Q or N/R) pair is valid with given @p vco_in and
 * @p pll_out
 *
 * @param[in] n
 * @param[in] p
 * @param[in] vco_in
 * @param[in] pll_out
 *
 * @return 1 if pair is valid, 0 otherwise
 */
static int is_n_ok(unsigned n, unsigned p, unsigned vco_in, unsigned pll_out)
{
    if (n >= 50 && n <= 432 &&
            vco_in * n >= min_vco_output && vco_in * n <= max_vco_outut &&
            vco_in * n / p == pll_out) {
        return 1;
    }
    else {
        return 0;
    }
}

/**
 * @brief Compute PLL factors
 *
 *
 * Compute PLL factors with the following model:
 *
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
 *
 *
 * vco_in = pll_in / M;
 * vco_out = vco_in * N;
 * pll_p_out = vco_out / P;
 * pll_q_out = vco_out / Q;
 * pll_r_out = vco_out / R;
 *
 * @param[in] pll_in      PLL input frequency
 * @param[in] pll_p_out   PLL P output frequency (0 if P is not needed)
 * @param[in] pll_q_out   PLL Q output frequency (0 if Q is not needed)
 * @param[in] pll_r_out   PLL R output frequency (0 if R is not needed)
 * @param[in,out] m       M factor, can be preset (0, if it has to be calculated)
 * @param[out] n          N factor
 * @param[out] p          P factor
 * @param[out] q          Q factor
 * @param[out] r          R factor
 *
 * @return -1 if no P,N pair can be computed with given @p pll_in and @p pll_p_out
 * @return 1 if no Q can be computed, M, N and P are valid
 * @return 2 if no R can be computed, M, M and P are valid
 * @return 3 if no Q nor R can be computed, M, M and P are valid
 * @return 0 if M, N, P, Q, R are valid
 */
static int compute_pll(unsigned pll_in, unsigned pll_p_out, unsigned pll_q_out,
                       unsigned pll_r_out, unsigned *m, unsigned *n, unsigned *p,
                       unsigned *q, unsigned *r)
{
    int res = 0;
    unsigned vco_in;

    if (*m == 0) {
        if ((pll_in / 2000000U) * 2000000U == pll_in) {
            vco_in = 2000000U;
        }
        else {
            vco_in = 1000000U;
        }
        *m = pll_in / vco_in;
    }
    else {
        vco_in = pll_in / *m;
    }

    if (vco_in < min_vco_input || vco_in > max_vco_input) {
        DEBUG("Invalid M=%u\n", *m);
        return -1;
    }

    if (pll_p_out) {
        DEBUG("Computing P for freq=%u\n", pll_p_out);
        for (*p = 8; *p >= 2; *p -= 2) {
            *n = *p * pll_p_out / vco_in;
            DEBUG("Trying P=%u: N=%u\n", *p, *n);
            if (is_n_ok(*n, *p, vco_in, pll_p_out)) {
                DEBUG("Found M=%u, N=%u, P=%u\n", *m, *n, *p);
                break;
            }
        }
        if (!is_n_ok(*n, *p, vco_in, pll_p_out)) {
            return -1;
        }
    }

    if (pll_q_out) {
        DEBUG("Computing Q for freq=%u\n", pll_q_out);
        for (*q = 15; *q >= 3; (*q)--) {
            if (!pll_p_out) {
                *n = *q * pll_q_out / vco_in;
            }
            DEBUG("Trying Q=%u: N=%u\n", *q, *n);
            if (is_n_ok(*n, *q, vco_in, pll_q_out)) {
                DEBUG("Found M=%u, N=%u, Q=%u\n", *m, *n, *q);
                break;
            }
        }
        if (!is_n_ok(*n, *q, vco_in, pll_q_out)) {
            *q = 0;
            res |= 1;
        }
    }

    /* todo, compute r */

    return res;
}

#define HSI 0
#define HSE 1

int main(int argc, char **argv)
{
    if (argc < 4) {
        printf("usage: %s <cpu_model> <coreclock> <hse_freq> [pll_i2s_q_out] [pll_sai_q_out]\n", argv[0]);
        return 1;
    }

    unsigned coreclock = atoi(argv[2]);
    unsigned pll_in = atoi(argv[3]);
    int pll_src;
    if (pll_in == 0) {
        pll_in = 16000000U;
        pll_src = HSI;
    }
    else {
        pll_src = HSE;
    }

    unsigned pll_i2s_q_out = 0;
    if (argc > 4) {
        pll_i2s_q_out = atoi(argv[4]);
    }

    unsigned pll_sai_q_out = 0;
    if (argc > 5) {
        pll_sai_q_out = atoi(argv[5]);
    }

    if (strlen(argv[1]) < 9 || !isdigit(argv[1][6])
            || !isdigit(argv[1][7])
            || !isdigit(argv[1][8])) {
        printf("Invalid model : %s\n", argv[1]);
        return 1;
    }

    int model = atoi(argv[1] + 6);
    if (strncmp(argv[1], "stm32f2", 7) == 0) {
        /* set frequencies boundaries */
        max_coreclock = 120000000U;
        max_apb1 = max_coreclock / 4;
        max_apb2 = max_coreclock / 2;
        min_vco_output = 192000000U;
        /* set PLL I2S */
        switch (model) {
        case 205:
        case 207:
        case 215:
        case 217:
            has_pll_i2s = true;
            break;

        }
        /* No PLL SAI for f2 family */
    }
    else if (strncmp(argv[1], "stm32f4", 7) == 0) {
        /* set frequencies boundaries */
        switch (model) {
        case 401:
            max_coreclock = 84000000U;
            max_apb1 = max_coreclock / 2;
            max_apb2 = max_coreclock;
            min_vco_output = 192000000U;
            break;
        case 407:
        case 415:
            max_coreclock = 168000000U;
            max_apb1 = max_coreclock / 4;
            max_apb2 = max_coreclock / 2;
            break;
        case 410:
        case 411:
        case 412:
        case 413:
            max_coreclock = 100000000U;
            max_apb1 = max_coreclock / 2;
            max_apb2 = max_coreclock;
            break;
        case 429:
        case 449:
            max_coreclock = 180000000U;
            max_apb1 = max_coreclock / 4;
            max_apb2 = max_coreclock / 2;
            break;
        }
        /* set PLL I2S */
        switch (model) {
        case 401:
        case 407:
        case 411:
        case 412:
        case 413:
        case 415:
        case 429:
            has_alt_48MHz = ALT_48MHZ_I2S;
        case 446:
            has_pll_i2s = true;
            break;

        }
        /* set PLL SAI */
        switch (model) {
        case 446:
            has_alt_48MHz = ALT_48MHZ_SAI;
        case 429:
            has_pll_sai = true;
            break;
        }
    }
    else if (strncmp(argv[1], "stm32f7", 7) == 0) {
        /* set PLL I2S */
        switch (model) {
        case 746:
        case 767:
        case 769:
            has_pll_i2s = true;
            break;

        }
        /* set PLL SAI */
        switch (model) {
        case 746:
        case 767:
        case 769:
            has_alt_48MHz = ALT_48MHZ_SAI;
            has_pll_sai = true;
            break;
        }

    }
    else {
        printf("Unsupported CPU model %s\n", argv[1]);
        return 1;
    }

    if (max_coreclock && coreclock > max_coreclock) {
        printf("Invalid coreclock (max=%u)\n", max_coreclock);
        return 1;
    }

    printf("Computing factors for stm32f%d CPU\n", model);


    unsigned m = 0;
    unsigned n = 0;
    unsigned p = 0;
    unsigned q = 0;
    unsigned r = 0;

    unsigned m_i2s = 0;
    unsigned n_i2s = 0;
    unsigned p_i2s = 0;
    unsigned q_i2s = 0;
    unsigned r_i2s = 0;

    unsigned m_sai = 0;
    unsigned n_sai = 0;
    unsigned p_sai = 0;
    unsigned q_sai = 0;
    unsigned r_sai = 0;

    bool use_alt_48MHz = false;

    /* main PLL */
    /* try to match coreclock with P output and 48MHz for Q output (USB) */
    switch (compute_pll(pll_in, coreclock, 48000000U, 0, &m, &n, &p, &q, &r)) {
    case -1:
        /* no config available */
        puts("Unable to compute main PLL factors");
        return 1;
    case 1:
        /* Q not OK */
        printf("Need to you an alternate 48MHz src...");
        if (has_pll_i2s && (has_alt_48MHz & ALT_48MHZ_I2S) == ALT_48MHZ_I2S) {
            puts("PLL I2S");
            use_alt_48MHz = true;
            if (pll_i2s_q_out != 0 && pll_i2s_q_out != 48000000U) {
                printf("Invalid PLL I2S Q output freq: %u\n", pll_i2s_q_out);
                return 1;
            }
            pll_i2s_q_out = 48000000U;
        }
        else if (has_pll_sai && (has_alt_48MHz & ALT_48MHZ_SAI)) {
            puts("PLL SAI");
            use_alt_48MHz = true;
            if (pll_sai_q_out != 0 && pll_sai_q_out != 48000000U) {
                printf("Invalid PLL SAI Q output freq: %u\n", pll_sai_q_out);
                return 1;
            }
            pll_sai_q_out = 48000000U;
        }
        else {
            puts("No other source available");
            return 1;
        }
        break;
    default:
        break;
    }

    if (pll_i2s_q_out) {
        unsigned *_m;
        if (has_pll_i2s_m) {
            _m = &m_i2s;
        }
        else {
            _m = &m;
        }
        if (compute_pll(pll_in, 0, pll_i2s_q_out, 0, _m, &n_i2s, &p_i2s, &q_i2s, &r_i2s) != 0) {
            puts("Unable to compute 48MHz output using PLL I2S");
            return 1;
        }
    }

    if (pll_sai_q_out) {
        unsigned *_m;
        if (has_pll_sai_m) {
            _m = &m_sai;
        }
        else {
            _m = &m;
        }
        if (compute_pll(pll_in, 0, pll_sai_q_out, 0, _m, &n_sai, &p_sai, &q_sai, &r_sai) != 0) {
            puts("Unable to compute 48MHz output using PLL I2S");
            return 1;
        }
    }

    unsigned apb1_pre;
    unsigned apb2_pre;

    for (apb1_pre = 1; apb1_pre <= 16; apb1_pre <<= 1) {
        if (coreclock / apb1_pre <= max_apb1) {
            break;
        }
    }
    for (apb2_pre = 1; apb2_pre <= 16; apb2_pre <<= 1) {
        if (coreclock / apb2_pre <= max_apb2) {
            break;
        }
    }


    printf("/**\n"
           " * @name Clock settings\n"
           " * @{\n"
           " */\n");
    printf("/* give the target core clock (HCLK) frequency [in Hz],\n"
           " * maximum: %dMHz */\n", max_coreclock / 1000000U);
    printf("#define CLOCK_CORECLOCK      (%u)\n", coreclock);
    printf("/* 0: no external high speed crystal available\n"
           " * 1: use external high speed crystal */\n");
    printf("#define CLOCK_HSE            (%u)\n", pll_src);
    printf("/* periphral clock setup */\n");
    printf("#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1      /* min 25MHz */\n"
           "#define CLOCK_AHB           (CLOCK_CORECLOCK / 1)\n");
    printf("#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV%u     /* max %uMHz */\n"
           "#define CLOCK_APB1          (CLOCK_CORECLOCK / %u)\n", apb1_pre, max_apb1 / 1000000U, apb1_pre);
    printf("#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV%u     /* max %uMHz */\n"
           "#define CLOCK_APB2          (CLOCK_CORECLOCK / %u\n)", apb2_pre, max_apb2 / 1000000U, apb2_pre);
    printf("/** @} */\n\n");
    printf("/**\n"
           " * @name Main PLL factors\n"
           " * @{\n"
           " */\n");
    printf("#define CLOCK_PLL_M          (%u)\n", m);
    printf("#define CLOCK_PLL_N          (%u)\n", n);
    printf("#define CLOCK_PLL_P          (%u)\n", p);
    printf("#define CLOCK_PLL_Q          (%u)\n", q);
    printf("/** @} */\n\n");
    if (pll_i2s_q_out) {
        printf("/**\n"
               " * @name PLL I2S configuration\n"
               " * @{\n"
               " */\n");
        printf("#define CLOCK_ENABLE_PLL_I2S (1)\n");
        if (has_pll_i2s_m) {
            printf("#define CLOCK_PLL_I2S_M      (%u)\n", m_i2s);
        }
        printf("#define CLOCK_PLL_I2S_N      (%u)\n", n_i2s);
        printf("#define CLOCK_PLL_I2S_P      (%u)\n", p_i2s);
        printf("#define CLOCK_PLL_I2S_Q      (%u)\n", q_i2s);
        printf("/** @} */\n\n");
    }
    if (pll_sai_q_out) {
        printf("/**\n"
               " * @name PLL SAI configuration\n"
               " * @{\n"
               " */\n");
        printf("#define CLOCK_ENABLE_PLL_SAI (1)\n");
        if (has_pll_sai_m) {
            printf("#define CLOCK_PLL_SAI_M      (%u)\n", m_sai);
        }
        printf("#define CLOCK_PLL_SAI_N      (%u)\n", n_i2s);
        printf("#define CLOCK_PLL_SAI_P      (%u)\n", p_i2s);
        printf("#define CLOCK_PLL_SAI_Q      (%u)\n", q_i2s);
        printf("/** @} */\n\n");
    }
    if (use_alt_48MHz) {
        printf("/** @brief Use alternative source for 48MHz clock */\n");
        printf("#define CLOCK_USE_ALT_48MHZ  (1)\n");
    }

    return 0;
}
