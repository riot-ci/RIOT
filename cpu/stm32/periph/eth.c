/*
 * Copyright (C) 2016 TriaGnoSys GmbH
 *               2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Low-level ETH driver implementation
 *
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */
#include <errno.h>
#include <string.h>

#include "bitarithm.h"
#include "mutex.h"
#include "luid.h"

#include "iolist.h"
#include "net/ethernet.h"

#include "periph/gpio.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* Set the value of the divider with the clock configured */
#if !defined(CLOCK_CORECLOCK) || CLOCK_CORECLOCK < (20000000U)
#error This peripheral requires a CORECLOCK of at least 20MHz
#elif CLOCK_CORECLOCK < (35000000U)
#define CLOCK_RANGE ETH_MACMIIAR_CR_Div16
#elif CLOCK_CORECLOCK < (60000000U)
#define CLOCK_RANGE ETH_MACMIIAR_CR_Div26
#elif CLOCK_CORECLOCK < (100000000U)
#define CLOCK_RANGE ETH_MACMIIAR_CR_Div42
#elif CLOCK_CORECLOCK < (150000000U)
#define CLOCK_RANGE ETH_MACMIIAR_CR_Div62
#else /* CLOCK_CORECLOCK < (20000000U) */
#define CLOCK_RANGE ETH_MACMIIAR_CR_Div102
#endif /* CLOCK_CORECLOCK < (20000000U) */

#define MINIMUM(a, b) (((a) <= (b)) ? (a) : (b))

/* Descriptors */
static edma_desc_t rx_desc[ETH_RX_BUFFER_COUNT];
static edma_desc_t tx_desc[ETH_TX_BUFFER_COUNT];
static edma_desc_t *rx_curr;
static edma_desc_t *tx_curr;

/* Buffers */
static char rx_buffer[ETH_RX_BUFFER_COUNT][ETH_RX_BUFFER_SIZE];
static char tx_buffer[ETH_TX_BUFFER_COUNT][ETH_TX_BUFFER_SIZE];

/** Read or write a phy register, to write the register ETH_MACMIIAR_MW is to
 * be passed as the higher nibble of the value */
static unsigned _rw_phy(unsigned addr, unsigned reg, unsigned value)
{
    unsigned tmp;

    while (ETH->MACMIIAR & ETH_MACMIIAR_MB) {}
    DEBUG("stm32_eth: rw_phy %x (%x): %x\n", addr, reg, value);

    tmp = (ETH->MACMIIAR & ETH_MACMIIAR_CR) | ETH_MACMIIAR_MB;
    tmp |= (((addr & 0x1f) << 11) | ((reg & 0x1f) << 6));
    tmp |= (value >> 16);

    ETH->MACMIIDR = (value & 0xffff);
    ETH->MACMIIAR = tmp;
    while (ETH->MACMIIAR & ETH_MACMIIAR_MB) {}

    DEBUG("stm32_eth: %lx\n", ETH->MACMIIDR);
    return (ETH->MACMIIDR & 0x0000ffff);
}

int32_t stm32_eth_phy_read(uint16_t addr, uint8_t reg)
{
    return _rw_phy(addr, reg, 0);
}

int32_t stm32_eth_phy_write(uint16_t addr, uint8_t reg, uint16_t value)
{
    _rw_phy(addr, reg, (value & 0xffff) | (ETH_MACMIIAR_MW << 16));
    return 0;
}

void stm32_eth_get_mac(char *out)
{
    unsigned t;

    t = ETH->MACA0HR;
    out[5] = (t >> 8);
    out[4] = (t & 0xff);

    t = ETH->MACA0LR;
    out[3] = (t >> 24);
    out[2] = (t >> 16);
    out[1] = (t >> 8);
    out[0] = (t & 0xff);
}

/** Set the mac address. The peripheral supports up to 4 MACs but only one is
 * implemented */
void stm32_eth_set_mac(const char *mac)
{
    ETH->MACA0HR &= 0xffff0000;
    ETH->MACA0HR |= ((mac[5] << 8) | mac[4]);
    ETH->MACA0LR = ((mac[3] << 24) | (mac[2] << 16) | (mac[1] << 8) | mac[0]);
}

/** Initialization of the DMA descriptors to be used */
static void _init_buffer(void)
{
    size_t i;
    for (i = 0; i < ETH_RX_BUFFER_COUNT; i++) {
        rx_desc[i].status = RX_DESC_STAT_OWN;
        rx_desc[i].control = RX_DESC_CTRL_RCH | (ETH_RX_BUFFER_SIZE & 0x0fff);
        rx_desc[i].buffer_addr = &rx_buffer[i][0];
        if((i+1) < ETH_RX_BUFFER_COUNT) {
            rx_desc[i].desc_next = &rx_desc[i + 1];
        }
    }
    rx_desc[i - 1].desc_next = &rx_desc[0];

    for (i = 0; i < ETH_TX_BUFFER_COUNT; i++) {
        tx_desc[i].status = TX_DESC_STAT_TCH | TX_DESC_STAT_CIC;
        tx_desc[i].buffer_addr = &tx_buffer[i][0];
        if ((i + 1) < ETH_RX_BUFFER_COUNT) {
            tx_desc[i].desc_next = &tx_desc[i + 1];
        }
    }

    tx_desc[i - 1].desc_next = &tx_desc[0];

    rx_curr = &rx_desc[0];
    tx_curr = &tx_desc[0];

    ETH->DMARDLAR = (uintptr_t)rx_curr;
    ETH->DMATDLAR = (uintptr_t)tx_curr;
}

int stm32_eth_init(void)
{
    /* enable APB2 clock */
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    /* select RMII if necessary */
    if (eth_config.mode == RMII) {
        SYSCFG->PMC |= SYSCFG_PMC_MII_RMII_SEL;
    }

    /* initialize GPIO */
    for (int i = 0; i < (int) eth_config.mode; i++) {
        gpio_init(eth_config.pins[i], GPIO_OUT);
        gpio_init_af(eth_config.pins[i], GPIO_AF11);
    }

    /* enable all clocks */
    RCC->AHB1ENR |= (RCC_AHB1ENR_ETHMACEN | RCC_AHB1ENR_ETHMACTXEN |
                     RCC_AHB1ENR_ETHMACRXEN | RCC_AHB1ENR_ETHMACPTPEN);

    /* reset the peripheral */
    RCC->AHB1RSTR |= RCC_AHB1RSTR_ETHMACRST;
    RCC->AHB1RSTR &= ~RCC_AHB1RSTR_ETHMACRST;

    /* software reset */
    ETH->DMABMR |= ETH_DMABMR_SR;
    while (ETH->DMABMR & ETH_DMABMR_SR) {}

    /* set the clock divider */
    while (ETH->MACMIIAR & ETH_MACMIIAR_MB) {}
    ETH->MACMIIAR = CLOCK_RANGE;

    /* configure the PHY (standard for all PHY's) */
    /* if there's no PHY, this has no effect */
    stm32_eth_phy_write(eth_config.phy_addr, PHY_BMCR, BMCR_RESET);

    /* speed from conf */
    ETH->MACCR |= (ETH_MACCR_ROD | ETH_MACCR_IPCO | ETH_MACCR_APCS |
                   ((eth_config.speed & 0x0100) << 3) |
                   ((eth_config.speed & 0x2000) << 1));

    /* pass all */
    //ETH->MACFFR |= ETH_MACFFR_RA;
    /* pass on perfect filter match and pass all multicast address matches */
    ETH->MACFFR |= ETH_MACFFR_PAM;

    /* store forward */
    ETH->DMAOMR |= (ETH_DMAOMR_RSF | ETH_DMAOMR_TSF | ETH_DMAOMR_OSF);

    /* configure DMA */
    ETH->DMABMR = (ETH_DMABMR_DA | ETH_DMABMR_AAB | ETH_DMABMR_FB |
                   ETH_DMABMR_RDP_32Beat | ETH_DMABMR_PBL_32Beat | ETH_DMABMR_EDE);

    if(eth_config.mac[0] != 0) {
      stm32_eth_set_mac(eth_config.mac);
    }
    else {
      eui48_t hwaddr;
      luid_get_eui48(&hwaddr);
      stm32_eth_set_mac((const char *)hwaddr.uint8);
    }

    _init_buffer();

    NVIC_EnableIRQ(ETH_IRQn);
    ETH->DMAIER |= ETH_DMAIER_NISE | ETH_DMAIER_TIE | ETH_DMAIER_RIE;

    /* enable transmitter and receiver */
    ETH->MACCR |= ETH_MACCR_TE | ETH_MACCR_RE;
    /* flush transmit FIFO */
    ETH->DMAOMR |= ETH_DMAOMR_FTF;
    /* wait for FIFO flushing to complete */
    while (ETH->DMAOMR & ETH_DMAOMR_FTF) { }

    /* enable DMA TX and RX */
    ETH->DMAOMR |= ETH_DMAOMR_ST | ETH_DMAOMR_SR;

    /* configure speed, do it at the end so the PHY had time to
     * reset */
    stm32_eth_phy_write(eth_config.phy_addr, PHY_BMCR, eth_config.speed);

    return 0;
}

int stm32_eth_send(const struct iolist *iolist)
{
    unsigned len = iolist_size(iolist);
    int ret = 0;

    /* safety check */
    if (len > ETH_TX_BUFFER_SIZE) {
        DEBUG("stm32_eth: Error iolist_size > ETH_TX_BUFFER_SIZE\n");
        return -1;
    }

    /* block until there's an available descriptor */
    while (tx_curr->status & TX_DESC_STAT_OWN) {
        DEBUG("stm32_eth: not avail\n");
    }

    /* clear status field */
    tx_curr->status &= 0x0fffffff;

    dma_acquire(eth_config.dma);
    for (; iolist; iolist = iolist->iol_next) {
        ret += dma_transfer(
                eth_config.dma, eth_config.dma_chan,
                iolist->iol_base, tx_curr->buffer_addr + ret, iolist->iol_len,
                DMA_MEM_TO_MEM, DMA_INC_BOTH_ADDR);
    }

    dma_release(eth_config.dma);
    if (ret < 0) {
        DEBUG("stm32_eth: Failure in dma_transfer\n");
        return ret;
    }
    tx_curr->control = (len & 0x1fff);

    /* set flags for first and last frames */
    tx_curr->status |= TX_DESC_STAT_FS;
    tx_curr->status |= TX_DESC_STAT_LS | TX_DESC_STAT_IC;

    /* give the descriptors to the DMA */
    tx_curr->status |= TX_DESC_STAT_OWN;
    tx_curr = tx_curr->desc_next;

    /* start tx */
    ETH->DMATPDR = 0;
    return ret;
}

static ssize_t get_rx_frame_size(void)
{
    edma_desc_t *i = rx_curr;
    uint32_t status;
    while (1) {
        /* Wait until DMA gave up control over descriptor */
        while ((status = i->status) & RX_DESC_STAT_OWN) { }
        DEBUG("stm32_eth: get_rx_frame_size(): FS=%c, LS=%c, DE=%c, FL=%lu\n",
              (status & RX_DESC_STAT_FS) ? '1' : '0',
              (status & RX_DESC_STAT_LS) ? '1' : '0',
              (status & RX_DESC_STAT_DE) ? '1' : '0',
              ((status >> 16) & 0x3fff) - 4);
        if (status & RX_DESC_STAT_LS) {
            break;
        }
        i = i->desc_next;
    }

    if (status & RX_DESC_STAT_DE) {
        return -1;
    }

    /* bits 16-29 contain the frame length including 4 B frame check sequence */
    return ((status >> 16) & 0x3fff) - 4;
}

static void drop_frame_and_update_rx_curr(void)
{
    while (1) {
        uint32_t old_status = rx_curr->status;
        /* hand over old descriptor to DMA */
        rx_curr->status = RX_DESC_STAT_OWN;
        rx_curr = rx_curr->desc_next;
        if (old_status & RX_DESC_STAT_LS) {
            /* reached last DMA descriptor of frame ==> done */
            return;
        }
    }
}

int stm32_eth_receive(void *buf, size_t max_len)
{
    char *data = buf;
    /* Determine the size of received frame. The frame might span multiple
     * DMA buffers */
    ssize_t size = get_rx_frame_size();

    if (size == -1) {
        DEBUG("stm32_eth: Received frame was too large for DMA buffer(s)\n");
        drop_frame_and_update_rx_curr();
        return -EOVERFLOW;
    }

    if (!buf) {
        if (max_len) {
            DEBUG("stm32_eth: Dropping frame as requested by upper layer\n");
            drop_frame_and_update_rx_curr();
        }
        return size;
    }

    if (max_len < (size_t)size) {
        DEBUG("stm32_eth: Buffer provided by upper layer is too small\n");
        drop_frame_and_update_rx_curr();
        return -ENOBUFS;
    }

    size_t remain = size;
    while (remain) {
        size_t chunk = MINIMUM(remain, ETH_RX_BUFFER_SIZE);
        memcpy(data, rx_curr->buffer_addr, chunk);
        data += chunk;
        remain -= chunk;
        /* Hand over descriptor to DMA */
        rx_curr->status = RX_DESC_STAT_OWN;
        rx_curr = rx_curr->desc_next;
    }

    return size;
}

int stm32_eth_get_rx_status_owned(void)
{
    return (!(rx_curr->status & RX_DESC_STAT_OWN));
}

void stm32_eth_isr_eth_wkup(void)
{
    cortexm_isr_end();
}
