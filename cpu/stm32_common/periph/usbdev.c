/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cpu_stm32_common_usbdev
 * @{
 * @file
 * @brief   Low level USB interface functions for the stm32 FS/HS devices
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 * @}
 */

#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "bitarithm.h"
#include "xtimer.h"
#include "cpu.h"
#include "cpu_conf.h"
#include "periph/pm.h"
#include "periph/gpio.h"
#include "periph/usbdev.h"
#include "usbdev_stm32.h"

/**
 * Be careful with enabling debug here. As with all timing critical systems it
 * is able to interfere with USB functionality and you might see different
 * errors than debug disabled
 */
#define ENABLE_DEBUG (0)
#include "debug.h"

#if defined(STM32_USB_OTG_FS_ENABLED) && defined(STM32_USB_OTG_HS_ENABLED)
#define _TOTAL_NUM_ENDPOINTS  (STM32_USB_OTG_FS_NUM_EP + \
                               STM32_USB_OTG_HS_NUM_EP)
#elif defined(STM32_USB_OTG_FS_ENABLED)
#define _TOTAL_NUM_ENDPOINTS  (STM32_USB_OTG_FS_NUM_EP)
#elif defined(STM32_USB_OTG_HS_ENABLED)
#define _TOTAL_NUM_ENDPOINTS  (STM32_USB_OTG_HS_NUM_EP)
#endif

/* Mask for the set of interrupts used */
#define STM32_FSHS_USB_GINT_MASK    \
    (USB_OTG_GINTMSK_USBSUSPM | \
     USB_OTG_GINTMSK_WUIM     | \
     USB_OTG_GINTMSK_ENUMDNEM | \
     USB_OTG_GINTMSK_USBRST   | \
     USB_OTG_GINTMSK_OTGINT   | \
     USB_OTG_GINTMSK_IEPINT   | \
     USB_OTG_GINTMSK_OEPINT   | \
     USB_OTG_GINTMSK_RXFLVLM)

#define STM32_PKTSTS_GONAK          0x01    /**< Rx fifo global out nak */
#define STM32_PKTSTS_DATA_UPDT      0x02    /**< Rx fifo data update    */
#define STM32_PKTSTS_XFER_COMP      0x03    /**< Rx fifo data complete  */
#define STM32_PKTSTS_SETUP_COMP     0x04    /**< Rx fifo setup complete */
#define STM32_PKTSTS_SETUP_UPDT     0x06    /**< Rx fifo setup update   */

/* List of instantiated USB peripherals */
static stm32_usb_otg_fshs_t _usbdevs[USBDEV_NUMOF] = { 0 };

static usbdev_ep_t _out[_TOTAL_NUM_ENDPOINTS];
static usbdev_ep_t _in[_TOTAL_NUM_ENDPOINTS];

/* Forward declaration for the usb device driver */
const usbdev_driver_t driver;

/*************************************************************************
* Conversion function from the base address to specific register blocks *
*************************************************************************/
static USB_OTG_GlobalTypeDef *_global_regs(
    const stm32_usb_otg_fshs_config_t *conf)
{
    return (USB_OTG_GlobalTypeDef *)(conf->periph + USB_OTG_GLOBAL_BASE);
}

static USB_OTG_DeviceTypeDef *_device_regs(
    const stm32_usb_otg_fshs_config_t *conf)
{
    return (USB_OTG_DeviceTypeDef *)(conf->periph + USB_OTG_DEVICE_BASE);
}

static USB_OTG_INEndpointTypeDef *_in_regs(
    const stm32_usb_otg_fshs_config_t *conf,
    size_t endpoint)
{
    return (USB_OTG_INEndpointTypeDef *)(conf->periph +
                                         USB_OTG_IN_ENDPOINT_BASE +
                                         USB_OTG_EP_REG_SIZE * endpoint);
}

static USB_OTG_OUTEndpointTypeDef *_out_regs(
    const stm32_usb_otg_fshs_config_t *conf,
    size_t endpoint)
{
    return (USB_OTG_OUTEndpointTypeDef *)(conf->periph +
                                          USB_OTG_OUT_ENDPOINT_BASE +
                                          USB_OTG_EP_REG_SIZE * endpoint);
}

static __I uint32_t *_rx_fifo(const stm32_usb_otg_fshs_config_t *conf)
{
    return (__I uint32_t *)(conf->periph + USB_OTG_FIFO_BASE);
}

static __O uint32_t *_tx_fifo(const stm32_usb_otg_fshs_config_t *conf,
                              size_t num)
{
    return (__O uint32_t *)(conf->periph +
                            USB_OTG_FIFO_BASE +
                            USB_OTG_FIFO_SIZE * num);
}

static __IO uint32_t *_pcgcctl_reg(const stm32_usb_otg_fshs_config_t *conf)
{
    return (__IO uint32_t *)(conf->periph + USB_OTG_PCGCCTL_BASE);
}
/* end of conversion functions */

/**
 * @brief Determine the number of available endpoints for the peripheral based
 *        on the type and the CID version
 *
 * @param config    configuration struct
 */
static size_t _max_endpoints(const stm32_usb_otg_fshs_config_t *config)
{
    return (config->type == STM32_USB_OTG_FS) ?
           STM32_USB_OTG_FS_NUM_EP :
           STM32_USB_OTG_HS_NUM_EP;
}

static size_t _setup(stm32_usb_otg_fshs_t *usbdev,
                     const stm32_usb_otg_fshs_config_t *config, size_t idx)
{
    usbdev->usbdev.driver = &driver;
    usbdev->config = config;
    usbdev->out = &_out[idx];
    usbdev->in = &_in[idx];
    return _max_endpoints(config);
}

/**
 * @brief Low level usbdev struct setup
 *
 * Distributes the available endpoints among the enabled peripherals
 */
void usbdev_init_lowlevel(void)
{
    size_t ep_idx = 0;

    for (size_t i = 0; i < USBDEV_NUMOF; i++) {
        ep_idx += _setup(&_usbdevs[i], &stm32_usb_otg_fshs_config[i], ep_idx);
    }
    assert(ep_idx == _TOTAL_NUM_ENDPOINTS);
}

usbdev_t *usbdev_get_ctx(unsigned num)
{
    assert(num < USBDEV_NUMOF);
    return &_usbdevs[num].usbdev;
}

static uint32_t _type_to_reg(usb_ep_type_t type)
{
    switch (type) {
        case USB_EP_TYPE_CONTROL:
            return 0x00 << USB_OTG_DOEPCTL_EPTYP_Pos;
        case USB_EP_TYPE_ISOCHRONOUS:
            return 0x01 << USB_OTG_DOEPCTL_EPTYP_Pos;
        case USB_EP_TYPE_BULK:
            return 0x02 << USB_OTG_DOEPCTL_EPTYP_Pos;
        case USB_EP_TYPE_INTERRUPT:
            return 0x04 << USB_OTG_DOEPCTL_EPTYP_Pos;
        default:
            assert(false);
            return 0;
    }
}

static uint32_t _ep0_size(size_t size)
{
    switch (size) {
        case 64:
            return 0x00;
        case 32:
            return 0x01;
        case 16:
            return 0x02;
        case 8:
            return 0x03;
        default:
            assert(false);
            return 0x00;
    }
}

/**
 * @brief Disables an IN type endpoint
 *
 * Endpoint is only deactivated if it was activated
 */
static void _ep_in_disable(usbdev_ep_t *ep)
{
    stm32_usb_otg_fshs_t *usbdev = (stm32_usb_otg_fshs_t *)ep->dev;
    const stm32_usb_otg_fshs_config_t *conf = usbdev->config;

    if (_in_regs(conf, ep->num)->DIEPCTL & USB_OTG_DIEPCTL_EPENA) {
        _in_regs(conf, ep->num)->DIEPCTL |= USB_OTG_DIEPCTL_EPDIS;
        while (_in_regs(conf, ep->num)->DIEPCTL & USB_OTG_DIEPCTL_EPDIS) {}
    }
}

/**
 * @brief Disables an OUT type endpoint
 *
 * Endpoint is only deactivated if it was activated
 */
static void _ep_out_disable(usbdev_ep_t *ep)
{
    stm32_usb_otg_fshs_t *usbdev = (stm32_usb_otg_fshs_t *)ep->dev;
    const stm32_usb_otg_fshs_config_t *conf = usbdev->config;

    if (_out_regs(conf, ep->num)->DOEPCTL & USB_OTG_DOEPCTL_EPENA) {
        _out_regs(conf, ep->num)->DOEPCTL |= USB_OTG_DOEPCTL_EPDIS;
        while (_out_regs(conf, ep->num)->DOEPCTL & USB_OTG_DOEPCTL_EPDIS) {}
    }
}

static void _ep_deactivate(usbdev_ep_t *ep)
{
    stm32_usb_otg_fshs_t *usbdev = (stm32_usb_otg_fshs_t *)ep->dev;
    const stm32_usb_otg_fshs_config_t *conf = usbdev->config;

    if (ep->dir == USB_EP_DIR_IN) {
        _ep_in_disable(ep);
        _in_regs(conf, ep->num)->DIEPCTL &= USB_OTG_DIEPCTL_USBAEP;
    }
    else {
        _ep_out_disable(ep);
        _out_regs(conf, ep->num)->DOEPCTL &= USB_OTG_DOEPCTL_USBAEP;
    }
}

static void _ep_activate(usbdev_ep_t *ep)
{
    stm32_usb_otg_fshs_t *usbdev = (stm32_usb_otg_fshs_t *)ep->dev;
    const stm32_usb_otg_fshs_config_t *conf = usbdev->config;

    if (ep->dir == USB_EP_DIR_IN) {
        _ep_in_disable(ep);
        _device_regs(conf)->DAINTMSK |= 1 << ep->num;
        uint32_t diepctl = USB_OTG_DIEPCTL_SNAK |
                           USB_OTG_DIEPCTL_USBAEP |
                           _type_to_reg(ep->type) |
                           ep->num << USB_OTG_DIEPCTL_TXFNUM_Pos;
        if (ep->num == 0) {
            diepctl |= _ep0_size(ep->len);
        }
        else {
            diepctl |= ep->len;
            diepctl |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM;
        }
        _in_regs(conf, ep->num)->DIEPCTL |= diepctl;
    }
    else {
        _ep_out_disable(ep);
        _device_regs(conf)->DAINTMSK |= 1 << (ep->num + 16);
        _out_regs(conf, ep->num)->DOEPCTL |= USB_OTG_DOEPCTL_SNAK |
                                             USB_OTG_DOEPCTL_USBAEP;
        _type_to_reg(ep->type);
        if (ep->num == 0) {
            _out_regs(conf, ep->num)->DOEPCTL |= _ep0_size(ep->len);
        }
        else {
            _out_regs(conf, ep->num)->DOEPCTL |= ep->len;
            _out_regs(conf, ep->num)->DOEPCTL |= USB_OTG_DOEPCTL_SD0PID_SEVNFRM;
        }
    }
}

static inline void _usb_attach(stm32_usb_otg_fshs_t *usbdev)
{
    DEBUG("Attaching to host\n");
    /* Disable the soft disconnect feature */
    _device_regs(usbdev->config)->DCTL &= ~USB_OTG_DCTL_SDIS;
}

static inline void _usb_detach(stm32_usb_otg_fshs_t *usbdev)
{
    DEBUG("Detaching from host\n");
    /* Enable the soft disconnect feature */
    _device_regs(usbdev->config)->DCTL |= USB_OTG_DCTL_SDIS;
}

static void _set_address(stm32_usb_otg_fshs_t *usbdev, uint8_t address)
{
    _device_regs(usbdev->config)->DCFG =
        (_device_regs(usbdev->config)->DCFG & ~(USB_OTG_DCFG_DAD_Msk)) |
        (address << USB_OTG_DCFG_DAD_Pos);
}

static usbdev_ep_t *_get_ep(stm32_usb_otg_fshs_t *usbdev, unsigned num,
                            usb_ep_dir_t dir)
{
    if (num >= STM32_USB_OTG_FS_NUM_EP) {
        return NULL;
    }
    return dir == USB_EP_DIR_IN ? &usbdev->in[num] : &usbdev->out[num];
}

#ifdef DEVELHELP
static size_t _total_fifo_size(const stm32_usb_otg_fshs_config_t *conf)
{
    if (conf->type == STM32_USB_OTG_FS) {
#ifdef STM32_USB_OTG_FS_ENABLED
        return USB_OTG_FS_TOTAL_FIFO_SIZE;
#else
        return 0;
#endif  /* STM32_USB_OTG_FS_ENABLED */
    }
    else {
#ifdef STM32_USB_OTG_HS_ENABLED
        return USB_OTG_HS_TOTAL_FIFO_SIZE;
#else
        return 0;
#endif  /* STM32_USB_OTG_HS_ENABLED */
    }

}
#endif /* DEVELHELP */

static void _configure_tx_fifo(stm32_usb_otg_fshs_t *usbdev, size_t num,
                               size_t len)
{
    /* TX Fifo size must be at least 16 words long and must be word aligned */
    size_t wordlen = len < (16 * sizeof(uint32_t))
                     ? 16
                     : (len + (sizeof(uint32_t) - 1)) / sizeof(uint32_t);

    /* Check max size */
    assert(usbdev->fifo_pos + wordlen <=
           _total_fifo_size(usbdev->config) / sizeof(uint32_t));

    _global_regs(usbdev->config)->DIEPTXF[num - 1] =
        (wordlen << USB_OTG_TX0FD_Pos) |
        (usbdev->fifo_pos);
    usbdev->fifo_pos += wordlen;
}

static void _configure_fifo(stm32_usb_otg_fshs_t *usbdev)
{
    /* TODO: cleanup, more dynamic, etc */
    const stm32_usb_otg_fshs_config_t *conf = usbdev->config;
    size_t rx_size = conf->type == STM32_USB_OTG_FS
                     ? STM32_USB_OTG_FS_RX_FIFO_SIZE
                     : STM32_USB_OTG_HS_RX_FIFO_SIZE;

    _global_regs(conf)->GRXFSIZ =
        (_global_regs(conf)->GRXFSIZ & ~USB_OTG_GRXFSIZ_RXFD) |
        rx_size;
    _global_regs(conf)->DIEPTXF0_HNPTXFSIZ = (16 << USB_OTG_TX0FD_Pos) |
                                             rx_size;
    usbdev->fifo_pos = (rx_size + 16);
}

static usbdev_ep_t *_usbdev_new_ep(usbdev_t *dev, usb_ep_type_t type,
                                   usb_ep_dir_t dir, size_t buf_len)
{
    stm32_usb_otg_fshs_t *usbdev = (stm32_usb_otg_fshs_t *)dev;
    usbdev_ep_t *ep = NULL;

    if (type == USB_EP_TYPE_CONTROL) {
        if (dir == USB_EP_DIR_IN) {
            ep = &usbdev->in[0];
        }
        else {
            ep = &usbdev->out[0];
        }
        ep->num = 0;
    }
    else {
        /* Find the first unassigned ep with matching direction */
        for (unsigned idx = 1; idx < STM32_USB_OTG_FS_NUM_EP && !ep; idx++) {
            usbdev_ep_t *candidate_ep = _get_ep(usbdev, idx, dir);
            if (candidate_ep->type == USB_EP_TYPE_NONE) {
                ep = candidate_ep;
                ep->num = idx;
            }
        }
    }

    if (ep) {
        if (usbdev->occupied + buf_len < STM32_USB_OTG_BUF_SPACE) {
            ep->buf = usbdev->buffer + usbdev->occupied;
            ep->dir = dir;
            ep->type = type;
            ep->dev = dev;
            ep->len = buf_len;
            usbdev->occupied += buf_len;
            if (ep->dir == USB_EP_DIR_IN && ep->num != 0) {
                _configure_tx_fifo(usbdev, ep->num, ep->len);
            }
        }
    }
    return ep;
}

/**
 * @brief reset a TX fifo.
 *
 * @param   usbdev      usbdev context
 * @param   fifo_num    fifo number to reset, 0x10 for all fifos
 */
static void _flush_fifo(stm32_usb_otg_fshs_t *usbdev, uint8_t fifo_num)
{
    const stm32_usb_otg_fshs_config_t *conf = usbdev->config;
    uint32_t reg = _global_regs(conf)->GRSTCTL & ~(USB_OTG_GRSTCTL_TXFNUM);

    reg |= fifo_num << USB_OTG_GRSTCTL_TXFNUM_Pos | USB_OTG_GRSTCTL_TXFFLSH;
    _global_regs(conf)->GRSTCTL = reg;
    /* Wait for flush to finish */
    while (_global_regs(conf)->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH) {}
}

static void _flush_rx_fifo(stm32_usb_otg_fshs_t *usbdev)
{
    const stm32_usb_otg_fshs_config_t *conf = usbdev->config;

    _global_regs(conf)->GRSTCTL |= USB_OTG_GRSTCTL_RXFFLSH;
    while (_global_regs(conf)->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH) {}
}

static void _reset_eps(stm32_usb_otg_fshs_t *usbdev)
{
    const stm32_usb_otg_fshs_config_t *conf = usbdev->config;

    /* Set the NAK for all endpoints */
    for (size_t i = 0; i < _max_endpoints(conf); i++) {
        _out_regs(conf, i)->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
        _in_regs(conf, i)->DIEPCTL |= USB_OTG_DIEPCTL_SNAK;
        _in_regs(conf, i)->DIEPCTL |= (i) << USB_OTG_DIEPCTL_TXFNUM_Pos;
    }
}

static void _reset_periph(stm32_usb_otg_fshs_t *usbdev)
{
    const stm32_usb_otg_fshs_config_t *conf = usbdev->config;

    /* Wait for AHB idle */
    while (!(_global_regs(conf)->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL)) {}
    _global_regs(conf)->GRSTCTL |= USB_OTG_GRSTCTL_CSRST;
    /* Wait for reset done */
    while (_global_regs(conf)->GRSTCTL & USB_OTG_GRSTCTL_CSRST) {}
}

static void _enable_gpio(const stm32_usb_otg_fshs_config_t *conf)
{
    /* Enables clock on the GPIO bus */
    gpio_init(conf->dp, GPIO_IN);
    gpio_init(conf->dm, GPIO_IN);
    /* Configure AF for the pins */
    gpio_init_af(conf->dp, conf->af);
    gpio_init_af(conf->dm, conf->af);
}

static void _set_mode_device(stm32_usb_otg_fshs_t *usbdev)
{
    const stm32_usb_otg_fshs_config_t *conf = usbdev->config;

    /* Force device mode */
    _global_regs(conf)->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD;
    /* Spinlock to prevent a context switch here, needs a delay of 25 ms when
     * force switching mode */
    xtimer_spin(xtimer_ticks_from_usec(25 * US_PER_MS));
}

static void _usbdev_init(usbdev_t *dev)
{
    /* Block both STOP and STANDBY, STOP is unblocked during USB suspend
     * status */
    pm_block(STM32_PM_STOP);
    pm_block(STM32_PM_STANDBY);

    stm32_usb_otg_fshs_t *usbdev = (stm32_usb_otg_fshs_t *)dev;
    const stm32_usb_otg_fshs_config_t *conf = usbdev->config;

    /* Enable the clock to the peripheral */
    periph_clk_en(conf->ahb, conf->rcc_mask);

    _enable_gpio(conf);

    /* TODO: implement ULPI mode when a board is available */
#ifdef STM32_USB_OTG_HS_ENABLED
    if (conf->type == STM32_USB_OTG_HS) {
        /* Disable the ULPI clock in low power mode, this is essential for the
         * peripheral when using the built-in phy */
        periph_lpclk_dis(conf->ahb, RCC_AHB1LPENR_OTGHSULPILPEN);
        /* Only the built-in phy supported for now */
        assert(conf->phy == STM32_USB_OTG_PHY_BUILTIN);
        _global_regs(conf)->GUSBCFG |= USB_OTG_GUSBCFG_PHYSEL;
    }
#endif

    /* Reset the peripheral after phy selection */
    _reset_periph(usbdev);

    /* Reset clock */
    *_pcgcctl_reg(conf) = 0;

    /* Force the peripheral to device mode */
    _set_mode_device(usbdev);

    /* Disable Vbus detection and force the pull-up on */
#ifdef STM32_USB_OTG_CID_1x
    /* Enable no Vbus sensing and enable 'Power Down Disable */
    _global_regs(usbdev->config)->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS |
                                           USB_OTG_GCCFG_PWRDWN;
#else
    /* Enable no Vbus Detect enable  and enable 'Power Down Disable */
    _global_regs(usbdev->config)->GCCFG |= USB_OTG_GCCFG_VBDEN |
                                           USB_OTG_GCCFG_PWRDWN;
    /* Force Vbus Detect values and ID detect values to device mode */
    _global_regs(usbdev->config)->GOTGCTL |= USB_OTG_GOTGCTL_VBVALOVAL |
                                             USB_OTG_GOTGCTL_VBVALOEN |
                                             USB_OTG_GOTGCTL_BVALOEN |
                                             USB_OTG_GOTGCTL_BVALOVAL;
#endif
    /* disable fancy USB features */
    _global_regs(conf)->GUSBCFG &=
        ~(USB_OTG_GUSBCFG_HNPCAP | USB_OTG_GUSBCFG_SRPCAP);

    /* Device mode init */
    _device_regs(conf)->DCFG |= USB_OTG_DCFG_DSPD_Msk;  /* Full speed! */

    _configure_fifo(usbdev);

    /* Reset the receive FIFO */
    _flush_rx_fifo(usbdev);

    /* Reset all TX FIFOs */
    _flush_fifo(usbdev, 0x10);

    /* Values from the reference manual tables on TRDT */
    uint8_t trdt = conf->type == STM32_USB_OTG_FS ? 0x06 : 0x09;
    _global_regs(conf)->GUSBCFG =
        (_global_regs(conf)->GUSBCFG & ~USB_OTG_GUSBCFG_TRDT) |
        (trdt << USB_OTG_GUSBCFG_TRDT_Pos);

    _reset_eps(usbdev);

    /* Disable the global NAK for both directions */
    _device_regs(conf)->DCTL |= USB_OTG_DCTL_CGONAK | USB_OTG_DCTL_CGINAK;

#ifdef STM32_USB_OTG_HS_ENABLED
    if (conf->dma) {
        _global_regs(usbdev->config)->GAHBCFG |=
            /* Configure DMA */
            USB_OTG_GAHBCFG_DMAEN |
            /* DMA configured as 8 x 32bit accesses */
            (0x05 << USB_OTG_GAHBCFG_HBSTLEN_Pos);

        /* Unmask the transfer complete interrupts
         * Only needed when using DMA, otherwise the RX FIFO not empty
         * interrupt is used */
        _device_regs(conf)->DOEPMSK |= USB_OTG_DOEPMSK_XFRCM;
        _device_regs(conf)->DIEPMSK |= USB_OTG_DIEPMSK_XFRCM;
    }
#endif /* STM32_USB_OTG_HS_ENABLED */

    /* Clear the interrupt flags and unmask those interrupts */
    _global_regs(conf)->GINTSTS |= STM32_FSHS_USB_GINT_MASK;
    _global_regs(conf)->GINTMSK |= STM32_FSHS_USB_GINT_MASK;

    DEBUG("USB peripheral currently in %s mode\n",
          (_global_regs(
               conf)->GINTSTS & USB_OTG_GINTSTS_CMOD) ? "host" : "device");

    /* Enable interrupts and configure the TX level to interrupt on empty */
    _global_regs(conf)->GAHBCFG |= USB_OTG_GAHBCFG_GINT |
                                   USB_OTG_GAHBCFG_TXFELVL;

    /* Unmask the interrupt in the NVIC */
    NVIC_EnableIRQ(conf->irqn);
}

static int _usbdev_get(usbdev_t *dev, usbopt_t opt,
                       void *value, size_t max_len)
{
    (void)dev;
    (void)max_len;
    int res = -ENOTSUP;
    switch (opt) {
        case USBOPT_MAX_VERSION:
            assert(max_len == sizeof(usb_version_t));
            *(usb_version_t *)value = USB_VERSION_20;
            res = sizeof(usb_version_t);
            break;
        case USBOPT_MAX_SPEED:
            assert(max_len == sizeof(usb_speed_t));
            *(usb_speed_t *)value = USB_SPEED_FULL;
            res = sizeof(usb_speed_t);
            break;
        default:
            DEBUG("Unhandled get call: 0x%x\n", opt);
            break;
    }
    return res;
}

static int _usbdev_set(usbdev_t *dev, usbopt_t opt,
                       const void *value, size_t value_len)
{
    (void)value_len;

    stm32_usb_otg_fshs_t *usbdev = (stm32_usb_otg_fshs_t *)dev;
    int res = -ENOTSUP;

    switch (opt) {
        case USBOPT_ADDRESS:
            assert(value_len == sizeof(uint8_t));
            uint8_t addr = (*((uint8_t *)value));
            _set_address(usbdev, addr);
            break;
        case USBOPT_ATTACH:
            assert(value_len == sizeof(usbopt_enable_t));
            if (*((usbopt_enable_t *)value)) {
                _usb_attach(usbdev);
            }
            else {
                _usb_detach(usbdev);
            }
            res = sizeof(usbopt_enable_t);
            break;
        default:
            DEBUG("Unhandled set call: 0x%x\n", opt);
            break;
    }
    return res;
}

static void _usbdev_esr(usbdev_t *dev)
{
    stm32_usb_otg_fshs_t *usbdev = (stm32_usb_otg_fshs_t *)dev;
    const stm32_usb_otg_fshs_config_t *conf = usbdev->config;

    uint32_t int_status = _global_regs(conf)->GINTSTS;
    uint32_t event = 0;

    if (int_status & USB_OTG_GINTSTS_ENUMDNE) {
        event = USB_OTG_GINTSTS_ENUMDNE;
        /* Reset condition done */
        DEBUG("Reset done\n");
        usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_RESET);
    }
    else if (int_status & USB_OTG_GINTSTS_USBRST) {
        /* Start of reset condition */
        event = USB_OTG_GINTSTS_USBRST;

        /* Reset all the things! */
        _set_address(usbdev, 0);
        _flush_rx_fifo(usbdev);
        _reset_eps(usbdev);
        _flush_fifo(usbdev, 0x10);

        DEBUG("Reset start\n");
    }
    else if (int_status & USB_OTG_GINTSTS_SRQINT) {
        /* Reset done */
        event = USB_OTG_GINTSTS_SRQINT;
        DEBUG("Session request\n");
    }
    else if (int_status & USB_OTG_GINTSTS_USBSUSP) {
        DEBUG("SUSPEND\n");
        event = USB_OTG_GINTSTS_USBSUSP;
        usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_SUSPEND);
        /* Disable USB clock */
        *_pcgcctl_reg(conf) |= USB_OTG_PCGCCTL_STOPCLK;
        pm_unblock(STM32_PM_STOP);
    }
    else if (int_status & USB_OTG_GINTSTS_WKUINT) {
        pm_block(STM32_PM_STOP);
        DEBUG("WAKE\n");
        /* re-enable USB clock */
        *_pcgcctl_reg(conf) &= ~USB_OTG_PCGCCTL_STOPCLK;
        _global_regs(conf)->GINTSTS |= USB_OTG_GINTSTS_WKUINT;
        _flush_rx_fifo(usbdev);
        usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_RESUME);
    }

    _global_regs(conf)->GINTSTS |= event;
    _global_regs(conf)->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
}

static void _usbdev_ep_init(usbdev_ep_t *ep)
{
    DEBUG("STM32 Initializing EP %u, %s\n", ep->num,
          ep->dir == USB_EP_DIR_IN ? "IN" : "OUT");
}

static size_t _get_available(usbdev_ep_t *ep)
{
    stm32_usb_otg_fshs_t *usbdev = (stm32_usb_otg_fshs_t *)ep->dev;
    const stm32_usb_otg_fshs_config_t *conf = usbdev->config;

    return ep->len -
           (_out_regs(conf, ep->num)->DOEPTSIZ & USB_OTG_DOEPTSIZ_XFRSIZ_Msk);
}

static int _usbdev_ep_get(usbdev_ep_t *ep, usbopt_ep_t opt,
                          void *value, size_t max_len)
{
    (void)max_len;
    int res = -ENOTSUP;
    switch (opt) {
        case USBOPT_EP_AVAILABLE:
            assert(max_len == sizeof(size_t));
            *(size_t *)value = _get_available(ep);
            res = sizeof(size_t);
            break;
        default:
            DEBUG("otg_fs: Unhandled get call: 0x%x\n", opt);
            break;
    }
    return res;
}

static void _ep_set_stall(usbdev_ep_t *ep, bool enable)
{
    stm32_usb_otg_fshs_t *usbdev = (stm32_usb_otg_fshs_t *)ep->dev;
    const stm32_usb_otg_fshs_config_t *conf = usbdev->config;

    (void)enable;

    if (ep->dir == USB_EP_DIR_IN) {
        /* Disable first */
        _ep_in_disable(ep);
        _in_regs(conf, ep->num)->DIEPCTL |= USB_OTG_DIEPCTL_STALL;
    }
    else {
        /* Disable first */
        _ep_out_disable(ep);
        _out_regs(conf, ep->num)->DOEPCTL |= USB_OTG_DOEPCTL_STALL;
    }
}

static int _usbdev_ep_set(usbdev_ep_t *ep, usbopt_ep_t opt,
                          const void *value, size_t value_len)
{
    (void)value_len;
    int res = -ENOTSUP;
    switch (opt) {
        case USBOPT_EP_ENABLE:
            assert(value_len == sizeof(usbopt_enable_t));
            if (*((usbopt_enable_t *)value)) {
                _ep_activate(ep);
            }
            else {
                _ep_deactivate(ep);
            }
            res = sizeof(usbopt_enable_t);
            break;
        case USBOPT_EP_STALL:
            assert(value_len == sizeof(usbopt_enable_t));
            _ep_set_stall(ep, *(usbopt_enable_t *)value);
            res = sizeof(usbopt_enable_t);
            break;
        default:
            break;
    }
    return res;
}

static int _usbdev_ep_ready(usbdev_ep_t *ep, size_t len)
{
    stm32_usb_otg_fshs_t *usbdev = (stm32_usb_otg_fshs_t *)ep->dev;
    const stm32_usb_otg_fshs_config_t *conf = usbdev->config;

    if (ep->dir == USB_EP_DIR_IN) {
        /* Abort when the endpoint is not active, prevents hangs,
         * could be an assert in the future maybe */
        if (!(_in_regs(conf, ep->num)->DIEPCTL & USB_OTG_DIEPCTL_USBAEP)) {
            return -1;
        }


#ifdef STM32_USB_OTG_HS_ENABLED
        if (conf->dma) {
            _in_regs(conf, ep->num)->DIEPDMA = (uint32_t)ep->buf;
        }
#endif  /* STM32_USB_OTG_HS_ENABLED */

        /* The order here is crucial (AFAIK), it is required to first set the
         * size and the packet count, then clear the NAK and enable the
         * endpoint, and finally fill the transmit FIFO with the packet data.
         * When DMA is enabled, filling the transmit FIFO is handled by the DMA
         * controller in the peripheral
         */

        /* Packet count seems not to decrement below 1 and thus is broken in
         * combination with the TXFE irq, it does however work with control
         * transfers and when using DMA */
        uint32_t dieptsiz = (len & USB_OTG_DIEPTSIZ_XFRSIZ_Msk);
        if (ep->num == 0 || conf->dma) {
            dieptsiz |= (1 << USB_OTG_DIEPTSIZ_PKTCNT_Pos);
        }
        _in_regs(conf, ep->num)->DIEPTSIZ = dieptsiz;

        /* Intentionally enabling this before the FIFO is filled, unmasking the
        * interrupts after the FIFO is filled doesn't always trigger the ISR */
        /* TX FIFO empty interrupt is only used in non-dma mode */
        _device_regs(conf)->DAINTMSK |= 1 << ep->num;
        _device_regs(conf)->DIEPEMPMSK |= 1 << ep->num;

        _in_regs(conf, ep->num)->DIEPCTL |= USB_OTG_DIEPCTL_CNAK |
                                            USB_OTG_DIEPCTL_EPENA;

        if (len > 0
#ifdef STM32_USB_OTG_HS_ENABLED
            && (conf->dma == 0)
#endif  /* STM32_USB_OTG_HS_ENABLED */
            ) {
            /* The FIFO requires 32 bit word reads/writes */
            size_t words = (len + 3) / 4;
            uint32_t *ep_buf = (uint32_t *)ep->buf;
            __O uint32_t *fifo = _tx_fifo(conf, ep->num);
            for (size_t i = 0; i < words; i++) {
                fifo[i] = ep_buf[i];
            }
        }
    }
    else {
        /* Abort when the endpoint is not active, prevents hangs,
         * could be an assert in the future maybe */
        if (!(_out_regs(conf, ep->num)->DOEPCTL & USB_OTG_DOEPCTL_USBAEP)) {
            return -1;
        }
#ifdef STM32_USB_OTG_HS_ENABLED
        if (conf->dma) {
            _out_regs(conf, ep->num)->DOEPDMA = (uint32_t)ep->buf;
        }
#endif  /* STM32_USB_OTG_HS_ENABLED */

        /* Configure to receive one packet with ep->len as max length */
        uint32_t doeptsiz = (1 << USB_OTG_DOEPTSIZ_PKTCNT_Pos) |
                            (ep->len & USB_OTG_DOEPTSIZ_XFRSIZ_Msk);
        doeptsiz |= (ep->num == 0) ? 1 << USB_OTG_DOEPTSIZ_STUPCNT_Pos : 0;
        _out_regs(conf, ep->num)->DOEPTSIZ = doeptsiz;
        _out_regs(conf, ep->num)->DOEPCTL |= USB_OTG_DOEPCTL_CNAK |
                                             USB_OTG_DOEPCTL_EPENA |
                                             _type_to_reg(ep->type);
    }

    return 0;
}

static void _copy_rxfifo(stm32_usb_otg_fshs_t *usbdev, uint8_t *buf, size_t len)
{
    /* The FIFO requires 32 bit word reads/writes */
    uint32_t *buf32 = (uint32_t *)buf;
    __I uint32_t *fifo32 = _rx_fifo(usbdev->config);
    size_t count = (len + 3) / 4;

    for (size_t i = 0; i < count; i++) {
        buf32[i] = fifo32[i];
    }
}

static void _read_packet(usbdev_ep_t *ep)
{
    stm32_usb_otg_fshs_t *usbdev = (stm32_usb_otg_fshs_t *)ep->dev;
    const stm32_usb_otg_fshs_config_t *conf = usbdev->config;
    /* Pop status from the receive fifo status register */
    uint32_t status = _global_regs(conf)->GRXSTSP;

    /* Packet status code */
    unsigned pkt_status = (status & USB_OTG_GRXSTSP_PKTSTS_Msk) >>
                          USB_OTG_GRXSTSP_PKTSTS_Pos;
    size_t len = (status & USB_OTG_GRXSTSP_BCNT_Msk) >>
                 USB_OTG_GRXSTSP_BCNT_Pos;

    /* Packet is copied on the update status and copied on the transfer
     * complete status*/
    if (pkt_status == STM32_PKTSTS_DATA_UPDT ||
        pkt_status == STM32_PKTSTS_SETUP_UPDT) {
        _copy_rxfifo(usbdev, ep->buf, len);
#ifdef STM32_USB_OTG_CID_2x
        /* CID 2x doesn't signal SETUP_COMP on non-zero length packets, signal
         * the TR_COMPLETE event immediately */
        if (ep->num == 0 && len) {
            usbdev->usbdev.epcb(&usbdev->out[ep->num],
                                USBDEV_EVENT_TR_COMPLETE);
        }
#endif  /* STM32_USB_OTG_CID_2x */
    }
    /* On zero length frames, only the COMP status is signalled and the UPDT
     * status is skipped */
    else if (pkt_status == STM32_PKTSTS_XFER_COMP ||
             pkt_status == STM32_PKTSTS_SETUP_COMP) {
        usbdev->usbdev.epcb(&usbdev->out[ep->num], USBDEV_EVENT_TR_COMPLETE);
    }
}

/* This signals to the upper stack a completed transfer. Control transfers
 * behave slightly different with the interrupts, so a number of conditionals
 * filter interrupts to events
 */
static void _usbdev_ep_esr(usbdev_ep_t *ep)
{
    stm32_usb_otg_fshs_t *usbdev = (stm32_usb_otg_fshs_t *)ep->dev;
    const stm32_usb_otg_fshs_config_t *conf = usbdev->config;

    if (ep->dir == USB_EP_DIR_IN) {
        uint32_t status = _in_regs(conf, ep->num)->DIEPINT;

        /* XFRC interrupt is used for all endpoints when DMA is enabled */
#ifdef STM32_USB_OTG_HS_ENABLED
        if (status & USB_OTG_DIEPINT_XFRC && conf->dma) {
            _in_regs(conf, ep->num)->DIEPINT = USB_OTG_DIEPINT_XFRC;
            if (ep->num != 0) {
                usbdev->usbdev.epcb(ep, USBDEV_EVENT_TR_COMPLETE);
            }
        }
        else
#endif  /* STM32_USB_OTG_HS_ENABLED */
        /* TXFE empty interrupt is only used with DMA disabled */
        if (status & USB_OTG_DIEPINT_TXFE) {
            _device_regs(conf)->DIEPEMPMSK &= ~(1 << ep->num);
            usbdev->usbdev.epcb(ep, USBDEV_EVENT_TR_COMPLETE);
        }
    }
    else {
        /* RX FIFO not empty and the endpoint matches the function argument */
        if ((_global_regs(conf)->GINTSTS & USB_OTG_GINTSTS_RXFLVL) &&
            (_global_regs(conf)->GRXSTSR & USB_OTG_GRXSTSP_EPNUM_Msk) ==
            ep->num
#ifdef STM32_USB_OTG_HS_ENABLED
            && !conf->dma
#endif  /* STM32_USB_OTG_HS_ENABLED */
            ) {
            _read_packet(ep);
        }
        /* Transfer complete seems only reliable when used with DMA */
        else if (_out_regs(conf, ep->num)->DOEPINT & USB_OTG_DOEPINT_XFRC) {
            _out_regs(conf, ep->num)->DOEPINT = USB_OTG_DOEPINT_XFRC;
#ifdef STM32_USB_OTG_HS_ENABLED
            if (conf->dma) {
                usbdev->usbdev.epcb(ep, USBDEV_EVENT_TR_COMPLETE);
            }
#endif      /* STM32_USB_OTG_HS_ENABLED */
        }
    }
    /* Enable the peripheral interrupts again */
    _global_regs(conf)->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
}

static void _isr_ep(stm32_usb_otg_fshs_t *usbdev)
{
    const stm32_usb_otg_fshs_config_t *conf = usbdev->config;
    /* Top 16 bits of the register is OUT endpoints, bottom 16 is IN
     * endpoints */
    uint32_t active_ep = _device_regs(conf)->DAINT;

    if (active_ep) {
        unsigned epnum = bitarithm_lsb(active_ep);
        if (epnum > 15) {
            if (epnum != 16) {
                DEBUG("IRQ %u\n", epnum - 16);
            }
            usbdev->usbdev.epcb(&usbdev->out[epnum - 16], USBDEV_EVENT_ESR);
        }
        else {
            if (epnum != 0) {
                DEBUG("IRQ %u\n", epnum);
            }
            usbdev->usbdev.epcb(&usbdev->in[epnum], USBDEV_EVENT_ESR);
        }
    }
}

void _isr_common(stm32_usb_otg_fshs_t *usbdev)
{
    const stm32_usb_otg_fshs_config_t *conf = usbdev->config;

    uint32_t status = _global_regs(conf)->GINTSTS;

    if (status) {
        if (status & USB_OTG_GINTSTS_RXFLVL) {
            unsigned epnum = _global_regs(conf)->GRXSTSR &
                             USB_OTG_GRXSTSP_EPNUM_Msk;
            usbdev->usbdev.epcb(&usbdev->out[epnum], USBDEV_EVENT_ESR);
        }
        else if (_global_regs(conf)->GINTSTS &
                 (USB_OTG_GINTSTS_OEPINT | USB_OTG_GINTSTS_IEPINT)) {
            _isr_ep(usbdev);
        }
        else {
            /* Global interrupt */
            usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_ESR);
        }
        _global_regs(conf)->GAHBCFG &= ~USB_OTG_GAHBCFG_GINT;
    }
    cortexm_isr_end();
}

#ifdef STM32_USB_OTG_FS_ENABLED
void isr_otg_fs(void)
{
    /* Take the first device from the list */
    stm32_usb_otg_fshs_t *usbdev = &_usbdevs[0];

    _isr_common(usbdev);
}
#endif /* STM32_USB_OTG_FS_ENABLED */

#ifdef STM32_USB_OTG_HS_ENABLED
void isr_otg_hs(void)
{
    /* Take the last usbdev device from the list */
    stm32_usb_otg_fshs_t *usbdev = &_usbdevs[USBDEV_NUMOF - 1];

    _isr_common(usbdev);
}
#endif /* STM32_USB_OTG_HS_ENABLED */

const usbdev_driver_t driver = {
    .init = _usbdev_init,
    .new_ep = _usbdev_new_ep,
    .get = _usbdev_get,
    .set = _usbdev_set,
    .esr = _usbdev_esr,
    .ep_init = _usbdev_ep_init,
    .ep_get = _usbdev_ep_get,
    .ep_set = _usbdev_ep_set,
    .ep_esr = _usbdev_ep_esr,
    .ready = _usbdev_ep_ready,
};
