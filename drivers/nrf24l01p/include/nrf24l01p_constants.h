#ifndef NRF24L01P_CONSTANTS
#define NRF24L01P_CONSTANTS

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum width of a payload, restricted by
 *          a FIFO size of 32 bytes
 */
#define NRF24L01P_MAX_PAYLOAD_WIDTH         (32)

/**
 * @brief   Supported number of frequency channels
 */
#define NRF24L01P_NUM_CHANNELS              (16)

/**
 * @brief   Maximum number of retransmissions, if
 *          ESB is used as protocol
 */
#define NRF24L01P_MAX_RETRANSMISSIONS       (15)

/**
 * @brief   Base frequency
 */
#define NRF24L01P_BASE_FRQ_MHZ              (2400)

/**
 * @brief   Maximum supported frequency
 */
#define NRF24L01P_MAX_FRQ_MHZ               (2525)

/**
 * @brief   Address that instructs the driver to auto generate
 *          a layer-2 address for pipe 0 and pipe 1
 *          @see module_luid
 */
#define NRF24L01P_L2ADDR_AUTO               { 0x00, 0x00, 0x00, 0x00, 0x00 }

/**
 * @brief   Address that instructs the driver to auto generate
 *          a leyer-2 address for pipe 2, pipe 3, pipe 4 and pipe 5
 *          @see module_luid
 */
#define NRF24L01P_L2ADDR_UNDEF              (0x00)

/**
 * @brief   Reset value of TX_ADDR register
 */
#define NRF24L01P_DEFAULT_TX_ADDR           { 0xE7, 0xE7, 0xE7, 0xE7, 0xE7 }

/**
 * @brief   Reset value of RX_ADDR_P0 register
 */
#define NRF24L01P_DEFAULT_L2ADDR_P0         { 0xE7, 0xE7, 0xE7, 0xE7, 0xE7 }

/**
 * @brief   Reset value of RX_ADDR_P1 register
 */
#define NRF24L01P_DEFAULT_L2ADDR_P1         { 0xC2, 0xC2, 0xC2, 0xC2, 0xC2 }

/**
 * @brief   Reset value of RX_ADDR_P2 register
 */
#define NRF24L01P_DEFAULT_L2ADDR_P2         (0xC3)

/**
 * @brief   Reset value of RX_ADDR_P3 register
 */
#define NRF24L01P_DEFAULT_L2ADDR_P3         (0xC4)

/**
 * @brief   Reset value of RX_ADDR_P4 register
 */
#define NRF24L01P_DEFAULT_L2ADDR_P4         (0xC5)

/**
 * @brief   Reset value of RX_ADDR_P5 register
 */
#define NRF24L01P_DEFAULT_L2ADDR_P5         (0xC6)

/**
 * @brief   Agreed layer-2 address to send broadcast frames to
 *
 * A node that wants to receive broadcast frames must set it´s
 * pipe 1 address to that address.
 */
#define NRF24L01P_BROADCAST_ADDR            { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }

#ifdef MODULE_GNRC_SIXLOWPAN
/**
 * @brief   @ref gnrc_nettype_t of upper layer
 */
#define NRF24L01P_UPPER_LAYER_PROTOCOL      (GNRC_NETTYPE_SIXLOWPAN)
/**
 * @brief   Put source layer-2 address in custom header to
 *          be able to use 6LoWPAN
 */
#define NRF24L01P_CUSTOM_HEADER
/**
 * @brief   Layer-2 MTU
 */
#define NRF24L01P_MTU                       (NRF24L01P_MAX_PAYLOAD_WIDTH - \
                                             (1 + NRF24L01P_MAX_ADDR_WIDTH))
#else
/**
 * @brief   @ref gnrc_nettype_t of upper layer
 */
#define NRF24L01P_UPPER_LAYER_PROTOCOL      (GNRC_NETTYPE_UNDEF)
/**
 * @brief Layer-2 MTU
 */
#define NRF24L01P_MTU                       NRF24L01P_MAX_PAYLOAD_WIDTH
#endif

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_CONSTANTS */