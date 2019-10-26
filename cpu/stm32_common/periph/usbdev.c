/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cpu_stm32_common_usb
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
#define _TOTAL_NUM_ENDPOINTS  (STM32_USB_FS_NUM_EP + STM32_USB_HS_NUM_EP)
#elif defined(STM32_USB_OTG_FS_ENABLED)
#define _TOTAL_NUM_ENDPOINTS  (STM32_USB_FS_NUM_EP)
#elif defined(STM32_USB_OTG_HS_ENABLED)
#define _TOTAL_NUM_ENDPOINTS  (STM32_USB_HS_NUM_EP)
#endif

/* Mask for the set of interrupts used */
#define STM32_FSHS_USB_GINT_MASK    \
            (USB_OTG_GINTMSK_USBSUSPM |\
             USB_OTG_GINTMSK_WUIM     |\
             USB_OTG_GINTMSK_ENUMDNEM |\
             USB_OTG_GINTMSK_USBRST   |\
             USB_OTG_GINTMSK_OTGINT   |\
             USB_OTG_GINTMSK_IEPINT   |\
             USB_OTG_GINTMSK_OEPINT   |\
             USB_OTG_GINTMSK_RXFLVLM)

#define STM32_PKTSTS_GONAK          0x01 /**< Rx fifo global out nak */
#define STM32_PKTSTS_DATA_UPDT      0x02 /**< Rx fifo data update    */
#define STM32_PKTSTS_XFER_COMP      0x03 /**< Rx fifo data complete  */
#define STM32_PKTSTS_SETUP_COMP     0x04 /**< Rx fifo setup complete */
#define STM32_PKTSTS_SETUP_UPDT     0x06 /**< Rx fifo setup update   */

/* List of instantiated USB peripherals */
static stm32_fshs_usb_t _usbdevs[USBDEV_NUMOF] = { 0 };

static stm32_fshs_usb_ep_t _out[_TOTAL_NUM_ENDPOINTS];
static usbdev_ep_t _in[_TOTAL_NUM_ENDPOINTS];

/* Forward declaration for the usb device driver */
const usbdev_driver_t driver;

/*************************************************************************
 * Conversion function from the base address to specific register blocks *
 *************************************************************************/
static USB_OTG_GlobalTypeDef *_global_regs(const stm32_fshs_usb_config_t *conf)
{
    return (USB_OTG_GlobalTypeDef*)(conf->periph + USB_OTG_GLOBAL_BASE);
}

static USB_OTG_DeviceTypeDef *_device_regs(const stm32_fshs_usb_config_t *conf)
{
    return (USB_OTG_DeviceTypeDef*)(conf->periph + USB_OTG_DEVICE_BASE);
}

static USB_OTG_INEndpointTypeDef *_in_regs(const stm32_fshs_usb_config_t *conf,
                                           size_t endpoint)
{
    return (USB_OTG_INEndpointTypeDef*)(conf->periph +
                                        USB_OTG_IN_ENDPOINT_BASE +
                                        USB_OTG_EP_REG_SIZE * endpoint);
}

static USB_OTG_OUTEndpointTypeDef *_out_regs(const stm32_fshs_usb_config_t *conf,
                                             size_t endpoint)
{
    return (USB_OTG_OUTEndpointTypeDef*)(conf->periph +
                                         USB_OTG_OUT_ENDPOINT_BASE +
                                         USB_OTG_EP_REG_SIZE * endpoint);
}

static __I uint32_t *_rx_fifo(const stm32_fshs_usb_config_t *conf)
{
    return (__I uint32_t *)(conf->periph + USB_OTG_FIFO_BASE);
}

static __O uint32_t *_tx_fifo(const stm32_fshs_usb_config_t *conf, size_t num)
{
    return (__O uint32_t *)(conf->periph +
                            USB_OTG_FIFO_BASE +
                            USB_OTG_FIFO_SIZE * num);
}

static __IO uint32_t *_pcgcctl_reg(const stm32_fshs_usb_config_t *conf)
{
    return (__IO uint32_t*)(conf->periph + USB_OTG_PCGCCTL_BASE);
}

/* end of conversion functions */

static size_t _max_endpoints(const stm32_fshs_usb_config_t *config)
{
    return (config->type == STM32_USB_OTG_FS) ?
            STM32_USB_FS_NUM_EP :
            STM32_USB_HS_NUM_EP;
}

static size_t _setup(stm32_fshs_usb_t *usbdev,
                      const stm32_fshs_usb_config_t *config, size_t idx)
{
    usbdev->usbdev.driver = &driver;
    usbdev->config = config;
    usbdev->out = &_out[idx];
    usbdev->in = &_in[idx];
    return _max_endpoints(config);
}

void usbdev_init_lowlevel(void)
{
    size_t ep_idx = 0;
    for (size_t i = 0; i < USBDEV_NUMOF; i++) {
        ep_idx += _setup(&_usbdevs[i], &stm32_fshs_usb_config[i], ep_idx);
    }
    assert(ep_idx == _TOTAL_NUM_ENDPOINTS);
}

usbdev_t *usbdev_get_ctx(unsigned num)
{
    assert(num < USBDEV_NUMOF);
    return &_usbdevs[num].usbdev;
}

uint32_t _type_to_reg(usb_ep_type_t type)
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
            return 0;
    }
}

uint32_t _ep0_size(size_t size)
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
            return 0x00;
    }
}

static void _ep_in_disable(usbdev_ep_t *ep)
{
    stm32_fshs_usb_t *usbdev = (stm32_fshs_usb_t*)ep->dev;
    const stm32_fshs_usb_config_t *conf = usbdev->config;
    if (_in_regs(conf, ep->num)->DIEPCTL & USB_OTG_DIEPCTL_EPENA) {
        _in_regs(conf, ep->num)->DIEPCTL |= USB_OTG_DIEPCTL_EPDIS;
        while(_in_regs(conf, ep->num)->DIEPCTL & USB_OTG_DIEPCTL_EPDIS) {}
    }
}

static void _ep_out_disable(usbdev_ep_t *ep)
{
    stm32_fshs_usb_t *usbdev = (stm32_fshs_usb_t*)ep->dev;
    const stm32_fshs_usb_config_t *conf = usbdev->config;
    if (_out_regs(conf, ep->num)->DOEPCTL & USB_OTG_DOEPCTL_EPENA) {
        _out_regs(conf, ep->num)->DOEPCTL |= USB_OTG_DOEPCTL_EPDIS;
        while(_out_regs(conf, ep->num)->DOEPCTL & USB_OTG_DOEPCTL_EPDIS) {}
    }
}

static void _ep_enable(usbdev_ep_t *ep)
{
    stm32_fshs_usb_t *usbdev = (stm32_fshs_usb_t*)ep->dev;
    const stm32_fshs_usb_config_t *conf = usbdev->config;
    if (ep->dir == USB_EP_DIR_IN) {
        _device_regs(conf)->DAINTMSK |= 1 << ep->num;
        _in_regs(conf, ep->num)->DIEPCTL |= USB_OTG_DIEPCTL_SNAK |
                                     USB_OTG_DIEPCTL_USBAEP |
                                     _type_to_reg(ep->type) |
                                     ep->num << USB_OTG_DIEPCTL_TXFNUM_Pos;
        _ep_in_disable(ep);
        if (ep->num != 0) {
            _in_regs(conf, ep->num)->DIEPCTL |= ep->len;
            _in_regs(conf, ep->num)->DIEPCTL |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM;
        }
        else {
            _in_regs(conf, ep->num)->DIEPCTL |= _ep0_size(ep->len);
        }
    }
    else {
        _device_regs(conf)->DAINTMSK |= 1 << (ep->num + 16);
        _out_regs(conf, ep->num)->DOEPCTL |= USB_OTG_DOEPCTL_SNAK |
                                      USB_OTG_DOEPCTL_USBAEP;
                                      _type_to_reg(ep->type);
        _ep_out_disable(ep);
        if (ep->num != 0) {
            _out_regs(conf, ep->num)->DOEPCTL |= ep->len;
            _out_regs(conf, ep->num)->DOEPCTL |= USB_OTG_DOEPCTL_SD0PID_SEVNFRM;
        }
        else {
            _out_regs(conf, ep->num)->DOEPCTL |= _ep0_size(ep->len);
        }
    }
}

static inline void _usb_attach(stm32_fshs_usb_t *usbdev)
{
    DEBUG("Attaching to host\n");
    _device_regs(usbdev->config)->DCTL &= ~USB_OTG_DCTL_SDIS;
}

static inline void _usb_detach(stm32_fshs_usb_t *usbdev)
{
    _device_regs(usbdev->config)->DCTL |= USB_OTG_DCTL_SDIS;
}

static void _set_address(stm32_fshs_usb_t *usbdev, uint8_t address)
{
    _device_regs(usbdev->config)->DCFG =
        (_device_regs(usbdev->config)->DCFG & ~(USB_OTG_DCFG_DAD_Msk)) |
        (address << USB_OTG_DCFG_DAD_Pos);
}

static usbdev_ep_t *_get_ep(stm32_fshs_usb_t *usbdev, unsigned num, usb_ep_dir_t dir)
{
    if (num >= STM32_USB_FS_NUM_EP) {
        return NULL;
    }
    return dir == USB_EP_DIR_IN ? &usbdev->in[num] : &usbdev->out[num].ep;
}

static usbdev_ep_t *_usbdev_new_ep(usbdev_t *dev, usb_ep_type_t type,
                                   usb_ep_dir_t dir, size_t buf_len)
{
    stm32_fshs_usb_t *usbdev = (stm32_fshs_usb_t*)dev;
    usbdev_ep_t *ep = NULL;

    if (type == USB_EP_TYPE_CONTROL) {
        if (dir == USB_EP_DIR_IN) {
            ep = &usbdev->in[0];
        }
        else {
            ep = &usbdev->out[0].ep;
        }
        ep->num = 0;
    }
    else {
        /* Find the first unassigned ep with proper dir */
        for (unsigned idx = 1; idx < STM32_USB_FS_NUM_EP && !ep; idx++) {
            usbdev_ep_t *candidate_ep = _get_ep(usbdev, idx, dir);
            if (candidate_ep->type == USB_EP_TYPE_NONE) {
                ep = candidate_ep;
                ep->num = idx;
            }
        }
    }
    if (ep) {
        if (usbdev->occupied + buf_len < STM32_USB_FS_BUF_SPACE) {
            ep->buf = usbdev->buffer + usbdev->occupied;
            ep->len = buf_len;
            usbdev->occupied += buf_len;
            ep->dir = dir;
            ep->type = type;
            ep->dev = dev;
        }
    }

    return ep;
}

static void _reset_fifo(stm32_fshs_usb_t *usbdev, uint8_t fifo_num)
{
    const stm32_fshs_usb_config_t *conf = usbdev->config;
    uint32_t reg = _global_regs(conf)->GRSTCTL & ~(USB_OTG_GRSTCTL_TXFNUM);

    reg |= fifo_num << USB_OTG_GRSTCTL_TXFNUM_Pos | USB_OTG_GRSTCTL_TXFFLSH;
    _global_regs(conf)->GRSTCTL = reg;
    /* Wait for flush to finish */
    while (_global_regs(conf)->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH) {}
}

static void _reset_rx_fifo(stm32_fshs_usb_t *usbdev)
{
    const stm32_fshs_usb_config_t *conf = usbdev->config;
    _global_regs(conf)->GRSTCTL |= USB_OTG_GRSTCTL_RXFFLSH;
    while (_global_regs(conf)->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH) {}
}

static void _reset_periph(stm32_fshs_usb_t *usbdev)
{
    const stm32_fshs_usb_config_t *conf = usbdev->config;
    /* Wait for AHB idle */
    while (!(_global_regs(conf)->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL)) {}
    _global_regs(conf)->GRSTCTL |= USB_OTG_GRSTCTL_CSRST;
    /* Wait for reset done */
    while (_global_regs(conf)->GRSTCTL & USB_OTG_GRSTCTL_CSRST) {}
}

static void _enable_gpio(const stm32_fshs_usb_config_t *conf)
{
    /* Enables clock on the GPIO bus */
    gpio_init(conf->dp, GPIO_IN);
    gpio_init(conf->dm, GPIO_IN);
    /* Configure AF for the pins */
    gpio_init_af(conf->dp, conf->af);
    gpio_init_af(conf->dm, conf->af);
}

static void _configure_fifo(stm32_fshs_usb_t *usbdev)
{
    /* TODO: cleanup */
    const stm32_fshs_usb_config_t *conf = usbdev->config;
    _global_regs(conf)->GRXFSIZ = (_global_regs(conf)->GRXFSIZ & ~USB_OTG_GRXFSIZ_RXFD) | STM32_FS_OUT_FIFO_SIZE;
    _global_regs(conf)->DIEPTXF0_HNPTXFSIZ = (64 << USB_OTG_TX0FD_Pos) | STM32_FS_OUT_FIFO_SIZE;
    _global_regs(conf)->DIEPTXF[0] = (64 << USB_OTG_TX0FD_Pos) | (STM32_FS_OUT_FIFO_SIZE + 64);
    _global_regs(conf)->DIEPTXF[1] = (64 << USB_OTG_TX0FD_Pos) | (STM32_FS_OUT_FIFO_SIZE + 128);
    _global_regs(conf)->DIEPTXF[2] = (64 << USB_OTG_TX0FD_Pos) | (STM32_FS_OUT_FIFO_SIZE + 192);
}

static void _set_mode_device(stm32_fshs_usb_t *usbdev)
{
    const stm32_fshs_usb_config_t *conf = usbdev->config;
    /* Force device mode */
    _global_regs(conf)->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD;
    /* Spinlock to prevent a context switch here */
    xtimer_spin(xtimer_ticks_from_usec(50 * US_PER_MS));
}

static void _usbdev_init(usbdev_t *dev)
{
    pm_block(STM32_PM_STOP);

    stm32_fshs_usb_t *usbdev = (stm32_fshs_usb_t*)dev;
    const stm32_fshs_usb_config_t *conf = usbdev->config;

    /* Enable the clock to the peripheral */
    periph_clk_en(conf->ahb, conf->rcc_mask);

    _enable_gpio(conf);

    /* TODO: implement ULPI mode */
#ifdef STM32_USB_OTG_HS_ENABLED
    if (conf->type == STM32_USB_OTG_HS) {
        periph_lpclk_dis(conf->ahb, RCC_AHB1LPENR_OTGHSULPILPEN);
        assert(conf->phy == STM32_USB_OTG_PHY_BUILTIN);
        _global_regs(conf)->GUSBCFG |= USB_OTG_GUSBCFG_PHYSEL;
    }
#endif

    _reset_periph(usbdev);

    *_pcgcctl_reg(conf) = 0;

    _set_mode_device(usbdev);

    /* Disable V_bus detection and force the pull-up on */
#ifdef STM32_USB_OTG_CID_12
    /* Enable no Vbus sensing and enable 'Power Down Disable */
    _global_regs(usbdev->config)->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS | USB_OTG_GCCFG_PWRDWN;
#else
    /* Enable no Vbus Detect enable  and enable 'Power Down Disable */
    _global_regs(usbdev->config)->GCCFG |= USB_OTG_GCCFG_VBDEN | USB_OTG_GCCFG_PWRDWN;
    /* Force Vbus Detect values and ID detect values to device mode */
    _global_regs(usbdev->config)->GOTGCTL |= USB_OTG_GOTGCTL_VBVALOVAL |
                                             USB_OTG_GOTGCTL_VBVALOEN |
                                             USB_OTG_GOTGCTL_BVALOEN |
                                             USB_OTG_GOTGCTL_BVALOVAL;
#endif
    /* disable fancy USB features */
    _global_regs(conf)->GUSBCFG &= ~(USB_OTG_GUSBCFG_HNPCAP | USB_OTG_GUSBCFG_SRPCAP);

    /* Device mode init */
    _device_regs(conf)->DCFG |= USB_OTG_DCFG_DSPD_Msk ; /* Full speed! */

    _configure_fifo(usbdev);

    /* Reset the receive FIFO */
    _reset_rx_fifo(usbdev);
    /* Reset all TX FIFOs */
    _reset_fifo(usbdev, 0x10);

    /* Values from the reference manual tables on TRDT */
    uint8_t trdt = conf->type == STM32_USB_OTG_FS ? 0x06 : 0x09;
    _global_regs(conf)->GUSBCFG = (_global_regs(conf)->GUSBCFG & ~USB_OTG_GUSBCFG_TRDT) |
                            (trdt << USB_OTG_GUSBCFG_TRDT_Pos);

#ifdef STM32_USB_OTG_HS_ENABLED
    if (conf->dma) {
        _global_regs(usbdev->config)->GAHBCFG |=
            /* Configure DMA */
            USB_OTG_GAHBCFG_DMAEN |
            /* DMA configured as 8 x 32bit accesses */
            (0x05 << USB_OTG_GAHBCFG_HBSTLEN_Pos);
    }
#endif /* STM32_USB_OTG_HS_ENABLED */

    _out_regs(conf, 0)->DOEPTSIZ = 0;
    _in_regs(conf, 0)->DIEPTSIZ = 0;

    /* Set the NAK for all endpoints */
    for (size_t i = 0; i < _max_endpoints(conf); i++) {
        _out_regs(conf, i)->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
        _in_regs(conf, i)->DIEPCTL |= USB_OTG_DIEPCTL_SNAK;
    }

    /* Disable the global NAK for both directions */
    _device_regs(conf)->DCTL |= USB_OTG_DCTL_CGONAK | USB_OTG_DCTL_CGINAK;

    /* Unmask the transfer complete interrupts */
    _device_regs(conf)->DOEPMSK |= USB_OTG_DOEPMSK_XFRCM;
    _device_regs(conf)->DIEPMSK |= USB_OTG_DIEPMSK_XFRCM;

    /* Clear the interrupt flags and unmask those interrupts */
    _global_regs(conf)->GINTSTS |= STM32_FSHS_USB_GINT_MASK;
    _global_regs(conf)->GINTMSK |= STM32_FSHS_USB_GINT_MASK;


    DEBUG("USB peripheral currently in %s mode\n",
         (_global_regs(conf)->GINTSTS & USB_OTG_GINTSTS_CMOD) ? "host" : "device");

    /* Enable interrupts and configure the TX level to interrupt on empty */
    _global_regs(conf)->GAHBCFG |= USB_OTG_GAHBCFG_GINT | USB_OTG_GAHBCFG_TXFELVL;

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

    stm32_fshs_usb_t *usbdev = (stm32_fshs_usb_t*)dev;
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

    stm32_fshs_usb_t *usbdev = (stm32_fshs_usb_t*)dev;
    const stm32_fshs_usb_config_t *conf = usbdev->config;
    if (_global_regs(conf)->GINTSTS & USB_OTG_GINTSTS_ENUMDNE) {
        /* Reset done */
        _global_regs(conf)->GINTSTS |= USB_OTG_GINTSTS_ENUMDNE;
        DEBUG("Reset done\n");
        usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_RESET);
    }
    if (_global_regs(conf)->GINTSTS & USB_OTG_GINTSTS_USBRST) {
        /* Reset done */
        _global_regs(conf)->GINTSTS |= USB_OTG_GINTSTS_USBRST;

        _set_address(usbdev, 0);
        _reset_rx_fifo(usbdev);

        for (size_t i = 0; i < _max_endpoints(conf); i++) {
            _out_regs(conf, i)->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
            _in_regs(conf, i)->DIEPCTL |= USB_OTG_DIEPCTL_SNAK;
            _in_regs(conf, i)->DIEPCTL |= (i) << USB_OTG_DIEPCTL_TXFNUM_Pos;
        }
        _reset_fifo(usbdev, 0x10);
        DEBUG("Reset start\n");
    }
    if (_global_regs(conf)->GINTSTS & USB_OTG_GINTSTS_SRQINT) {
        /* Reset done */
        _global_regs(conf)->GINTSTS |= USB_OTG_GINTSTS_SRQINT;
        DEBUG("Session request\n");
    }
    if (_global_regs(conf)->GINTSTS & USB_OTG_GINTSTS_USBSUSP) {
        usbdev->suspended = true;
        _global_regs(conf)->GINTSTS |= USB_OTG_GINTSTS_USBSUSP;
        usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_SUSPEND);
    }
    if (_global_regs(conf)->GINTSTS & USB_OTG_GINTSTS_WKUINT) {
        usbdev->suspended = false;
        _global_regs(conf)->GINTSTS |= USB_OTG_GINTSTS_WKUINT;
        usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_RESUME);
    }

    _global_regs(conf)->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
}

static void _usbdev_ep_init(usbdev_ep_t *ep)
{
    stm32_fshs_usb_t *usbdev = (stm32_fshs_usb_t*)ep->dev;
    const stm32_fshs_usb_config_t *conf = usbdev->config;

    DEBUG("STM32 Initializing EP %u, %s\n", ep->num, ep->dir == USB_EP_DIR_IN ? "IN" : "OUT");

    if (ep->dir == USB_EP_DIR_IN) {
        if (ep->num == 0) {
            _global_regs(conf)->DIEPTXF0_HNPTXFSIZ = (64 << USB_OTG_TX0FD_Pos) | STM32_FS_OUT_FIFO_SIZE;
            _in_regs(conf, 0)->DIEPCTL |= USB_OTG_DIEPCTL_USBAEP | USB_OTG_DIEPCTL_SNAK;
        }
    }
}

static int _usbdev_ep_get(usbdev_ep_t *ep, usbopt_ep_t opt,
                          void *value, size_t max_len)
{
    (void)max_len;
    int res = -ENOTSUP;
    switch (opt) {
        case USBOPT_EP_AVAILABLE:
            assert(max_len == sizeof(size_t));
            stm32_fshs_usb_ep_t *stm_ep = (stm32_fshs_usb_ep_t*)ep;
            *(size_t *)value = stm_ep->len;
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
    stm32_fshs_usb_t *usbdev = (stm32_fshs_usb_t*)ep->dev;
    const stm32_fshs_usb_config_t *conf = usbdev->config;

    (void)enable;

    if (ep->dir == USB_EP_DIR_IN) {
        if (_in_regs(conf, ep->num)->DIEPCTL & USB_OTG_DIEPCTL_EPENA) {
            _in_regs(conf, ep->num)->DIEPCTL |= USB_OTG_DIEPCTL_EPDIS;
        }
        _in_regs(conf, ep->num)->DIEPCTL |= USB_OTG_DIEPCTL_STALL;
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
                _ep_enable(ep);
            }
            else {
                //_ep_disable(ep);
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
    stm32_fshs_usb_t *usbdev = (stm32_fshs_usb_t*)ep->dev;
    const stm32_fshs_usb_config_t *conf = usbdev->config;

    if (ep->dir == USB_EP_DIR_IN) {
        /* Abort when the endpoint is not active, prevents hangs,
         * could be an assert in the future maybe */
        if (!(_in_regs(conf, ep->num)->DIEPCTL & USB_OTG_DIEPCTL_USBAEP)) {
            return -1;
        }

        size_t words = (len + 3)/4;
        uint32_t *ep_buf = (uint32_t*)ep->buf;
        __O uint32_t *fifo = _tx_fifo(conf, ep->num);

#ifdef STM32_USB_OTG_HS_ENABLED
        if (conf->dma) {
            _in_regs(conf, ep->num)->DIEPDMA = (uint32_t)ep->buf;
        }
#endif

        _in_regs(conf, ep->num)->DIEPTSIZ = (1 << USB_OTG_DIEPTSIZ_PKTCNT_Pos) | (len & USB_OTG_DIEPTSIZ_XFRSIZ_Msk);
        _in_regs(conf, ep->num)->DIEPCTL |= USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA;
        if (len > 0
#ifdef STM32_USB_OTG_HS_ENABLED
                && (conf->dma == 0)
#endif
                ) {
            for (size_t i = 0; i < words; i++) {
                fifo[i] = ep_buf[i];
            }
        }
        _device_regs(conf)->DAINTMSK |= 1 << ep->num;
        if (ep->num == 0 || conf->dma) {
            _device_regs(conf)->DIEPEMPMSK |= 1 << ep->num;
        }
    }
    else {
        if (!(_out_regs(conf, ep->num)->DOEPCTL & USB_OTG_DOEPCTL_USBAEP)) {
            return -1;
        }
#ifdef STM32_USB_OTG_HS_ENABLED
        if (conf->dma) {
            _out_regs(conf, ep->num)->DOEPDMA = (uint32_t)ep->buf;
        }
#endif
        _out_regs(conf, ep->num)->DOEPTSIZ =  1 << USB_OTG_DOEPTSIZ_PKTCNT_Pos | ep->len;
        if (ep->num == 0) {
            _out_regs(conf, ep->num)->DOEPTSIZ |= 1 << USB_OTG_DOEPTSIZ_STUPCNT_Pos;
        }
        _out_regs(conf, ep->num)->DOEPCTL |= USB_OTG_DOEPCTL_CNAK |
                                      USB_OTG_DOEPCTL_EPENA |
                                      _type_to_reg(ep->type);
    }

    return 0;
}

static void _copy_rxfifo(stm32_fshs_usb_t* usbdev, uint8_t *buf, size_t len)
{
    uint32_t *buf32 = (uint32_t *)buf;
    __I uint32_t *fifo32 = _rx_fifo(usbdev->config);
    size_t count = (len + 3)/4;
    for (size_t i = 0; i < count; i++) {
        buf32[i] = fifo32[i];
    }
}

static void _read_packet(stm32_fshs_usb_ep_t* ep)
{
    stm32_fshs_usb_t *usbdev = (stm32_fshs_usb_t*)ep->ep.dev;
    const stm32_fshs_usb_config_t *conf = usbdev->config;
    uint32_t status = _global_regs(conf)->GRXSTSP;

    /* Pop status from the receive fifo status register */
    unsigned pkt_status = (status & USB_OTG_GRXSTSP_PKTSTS_Msk) >> USB_OTG_GRXSTSP_PKTSTS_Pos;
    size_t len = (status & USB_OTG_GRXSTSP_BCNT_Msk) >> USB_OTG_GRXSTSP_BCNT_Pos;
    unsigned epnum = status & USB_OTG_GRXSTSP_EPNUM_Msk;

    assert(epnum == ep->ep.num);

    if (pkt_status == STM32_PKTSTS_DATA_UPDT || pkt_status == STM32_PKTSTS_SETUP_UPDT) {
#ifdef STM32_USB_OTG_HS_ENABLED
        if (!conf->dma) {
            _copy_rxfifo(usbdev, ep->ep.buf, len);
        }
#else
        _copy_rxfifo(usbdev, ep->ep.buf, len);
#endif
        ep->len = len;
        if (epnum == 0) {
            _reset_fifo(usbdev, ep->ep.num);
        }
#ifdef STM32_USB_OTG_CID_20
        if (ep->ep.num == 0 && len) {
            usbdev->usbdev.epcb(&usbdev->out[epnum].ep, USBDEV_EVENT_TR_COMPLETE);
        }
#endif
    }
    else if (pkt_status == STM32_PKTSTS_XFER_COMP || pkt_status == STM32_PKTSTS_SETUP_COMP) {
        usbdev->usbdev.epcb(&usbdev->out[epnum].ep, USBDEV_EVENT_TR_COMPLETE);
    }
}

static void _usbdev_ep_esr(usbdev_ep_t *ep)
{
    stm32_fshs_usb_t *usbdev = (stm32_fshs_usb_t*)ep->dev;
    const stm32_fshs_usb_config_t *conf = usbdev->config;

    if (ep->dir == USB_EP_DIR_IN) {
        uint32_t status = _in_regs(conf, ep->num)->DIEPINT;

        if (status & USB_OTG_DIEPINT_XFRC) {
            _in_regs(conf, ep->num)->DIEPINT = USB_OTG_DIEPINT_XFRC;
            if (ep->num != 0) {
                usbdev->usbdev.epcb(ep, USBDEV_EVENT_TR_COMPLETE);
            }
        }
        else if (status & USB_OTG_DIEPINT_TXFE) {
            _in_regs(conf, ep->num)->DIEPINT = USB_OTG_DIEPINT_TXFE;
            _device_regs(conf)->DIEPEMPMSK &= ~(1 << ep->num);
            usbdev->usbdev.epcb(ep, USBDEV_EVENT_TR_COMPLETE);
        }
    }
    else {
        if ((_global_regs(conf)->GINTSTS & USB_OTG_GINTSTS_RXFLVL) &&
            (_global_regs(conf)->GRXSTSR & USB_OTG_GRXSTSP_EPNUM_Msk) == ep->num) {
            /* ep number found */
            _read_packet((stm32_fshs_usb_ep_t *)ep);
        }
        else if (_out_regs(conf, ep->num)->DOEPINT & USB_OTG_DOEPINT_XFRC) {
            _out_regs(conf, ep->num)->DOEPINT = USB_OTG_DOEPINT_XFRC;
            if (conf->dma) {
                stm32_fshs_usb_ep_t *stmep = (stm32_fshs_usb_ep_t *)ep;
                stmep->len = ep->len - (_out_regs(conf, ep->num)->DOEPTSIZ & USB_OTG_DOEPTSIZ_XFRSIZ_Msk);
                usbdev->usbdev.epcb(ep, USBDEV_EVENT_TR_COMPLETE);
            }
        }
    }
    _global_regs(conf)->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
}

void isr_ep(stm32_fshs_usb_t *usbdev)
{
    const stm32_fshs_usb_config_t *conf = usbdev->config;
    uint32_t active_ep = _device_regs(conf)->DAINT;
    if (active_ep) {
        unsigned epnum = bitarithm_lsb(active_ep);
        if (epnum > 15) {
            usbdev->usbdev.epcb(&usbdev->out[epnum-16].ep, USBDEV_EVENT_ESR);
        }
        else {
            usbdev->usbdev.epcb(&usbdev->in[epnum], USBDEV_EVENT_ESR);
        }
    }
}

void _isr_common(stm32_fshs_usb_t *usbdev)
{
    const stm32_fshs_usb_config_t *conf = usbdev->config;

    uint32_t status = _global_regs(conf)->GINTSTS;
    if (status) {
        if (status & USB_OTG_GINTSTS_RXFLVL) {
            unsigned epnum = _global_regs(conf)->GRXSTSR & USB_OTG_GRXSTSP_EPNUM_Msk;
            usbdev->usbdev.epcb(&usbdev->out[epnum].ep, USBDEV_EVENT_ESR);
        }
        else if (_global_regs(conf)->GINTSTS & (USB_OTG_GINTSTS_OEPINT | USB_OTG_GINTSTS_IEPINT) ) {
            isr_ep(usbdev);
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
    stm32_fshs_usb_t *usbdev = &_usbdevs[0];
    _isr_common(usbdev);
}
#endif

#ifdef STM32_USB_OTG_HS_ENABLED
void isr_otg_hs(void)
{
    /* Take the last usbdev device from the list */
    stm32_fshs_usb_t *usbdev = &_usbdevs[USBDEV_NUMOF - 1];
    _isr_common(usbdev);
}
#endif

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
