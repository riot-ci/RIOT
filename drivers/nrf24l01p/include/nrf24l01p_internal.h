/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup drivers_nrf24l01p
 * @{
 *
 * @file
 * @brief   Internal helper functions for NRF24L01P device driver
 *
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef NRF24L01P_INTERNAL_H
#define NRF24L01P_INTERNAL_H

#include "nrf24l01p_constants.h"
#include "nrf24l01p.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NRF24L01P_CUSTOM_HEADER
/**
 * @brief Indicate header byte
 */
#define NRF24L01P_PREEMBLE      (0x80)
/**
 * @brief Padding byte for ShockBurst
 */
#define NRF24L01P_PADDING       (0x00)
#endif

/**
 * @brief   Header of a ShockBurst frame
 */
typedef struct {
    /**
     * @brief    7 6 5 4 3 2 1 0
     *          +-+-+-+-+-+-+-+-+
     *          |1|r|d|d|d|s|s|s|
     *          +-+-+-+-+-+-+-+-+
     *          r = reserved
     *          d = destination address width
     *          s = source address width
     * If the protocol is ShockBurst, received frames may
     * be padded due to static payload length. The 7th bin
     * in the hader must be 1. Leading bytes starting with
     * 0 are padding bytes and must be removed.
     * Any frame having a value of ddd or sss not being in
     * [NRF24L01P_MIN_ADDR_WIDTH; NRF24L01P_MAX_ADDR_WIDTH]
     * shall be dropped.
     */
    uint8_t addr_width;
    /**
     * @brief   Recipient address
     */
    uint8_t dst_addr[NRF24L01P_MAX_ADDR_WIDTH];
#ifdef NRF24L01P_CUSTOM_HEADER
    /**
     * @brief   Sender address
     */
    uint8_t src_addr[NRF24L01P_MAX_ADDR_WIDTH];
#endif
} shockburst_hdr_t;

/**
 * @brief   Set custom header preemble
 */
static inline void sb_hdr_init(shockburst_hdr_t *hdr)
{
#ifdef NRF24L01P_CUSTOM_HEADER
    hdr->addr_width = NRF24L01P_PREEMBLE;
#else
    hdr->addr_width = 0;
#endif
}

/**
 * @brief   Set destination address field in a ShockBurst header
 *
 * @param[out] hdr      SB header
 * @param[in] width     Destination address width
 */
static inline void sb_hdr_set_dst_addr_width(shockburst_hdr_t *hdr,
                                             uint8_t width)
{
    hdr->addr_width &= ~(0b00111000);
    hdr->addr_width |= ((width & 7) << 3);
}

/**
 * @brief   Get destination address field in a ShockBurst header
 *
 * @param[out] hdr      SB header
 *
 * @return              Destination address width
 */
static inline uint8_t sb_hdr_get_dst_addr_width(shockburst_hdr_t *hdr)
{
    return (hdr->addr_width >> 3) & 7;
}

#ifdef NRF24L01P_CUSTOM_HEADER

/**
 * @brief   Set source address field in a ShockBurst header
 *
 * @param[out] hdr      SB header
 * @param[in] width     Source address width
 */
static inline void sb_hdr_set_src_addr_width(shockburst_hdr_t *hdr,
                                             uint8_t width)
{
    hdr->addr_width &= ~(0b00000111);
    hdr->addr_width |= (width & 7);
}

/**
 * @brief   Get source address field in a ShockBurst header
 *
 * @param[out] hdr      SB header
 *
 * @return              Source address width
 */
static inline uint8_t sb_hdr_get_src_addr_width(shockburst_hdr_t *hdr)
{
    return hdr->addr_width & 7;
}
#endif

/**
 * @brief   Check if device is awake
 *
 * @param[in] dev           NRF24L01P device handle
 *
 * @retval !=0              Awake
 * @retval 0                Asleep
 */
int nrf24l01p_assert_awake(nrf24l01p_t *dev);

/**
 * @brief   Restore rx address from params after a transmission
 *
 * @param[in] dev           NRF24L01P device handle
 */
void nrf24l01p_restore_address(nrf24l01p_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_INTERNAL_H */
/** @} */
