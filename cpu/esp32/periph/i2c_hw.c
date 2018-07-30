/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cpu_esp32
 * @ingroup drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation for ESP32 SDK
 *
 * @note        This implementation only implements the 7-bit addressing mode.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#ifdef I2C_HW_USED /* hardware implementation used */

/*
   PLEASE NOTE:

   Some part of the implementation were inspired by the Espressif IoT Development
   Framework [ESP-IDF](https://github.com/espressif/esp-idf.git) implementation
   of I2C. These partes are marked with an according copyright notice.
*/

#define ENABLE_DEBUG (0)
#include "debug.h"

#include <stdbool.h>
#include <string.h>

#include "cpu.h"
#include "log.h"
#include "mutex.h"
#include "periph_conf.h"
#include "periph/gpio.h"
#include "periph/i2c.h"
#include "thread_flags.h"

#include "esp_common.h"
#include "gpio_arch.h"
#include "driver/periph_ctrl.h"
#include "irq_arch.h"
#include "rom/ets_sys.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/gpio_struct.h"
#include "soc/i2c_reg.h"
#include "soc/i2c_struct.h"
#include "soc/rtc.h"
#include "soc/soc.h"
#include "syscalls.h"
#include "xtensa/xtensa_api.h"

#if defined(I2C_NUMOF) && I2C_NUMOF > 0

#undef  I2C_CLK_FREQ
#define I2C_CLK_FREQ   rtc_clk_apb_freq_get() /* APB_CLK is used */

/* operation codes used for commands */
#define I2C_CMD_RSTART 0
#define I2C_CMD_WRITE  1
#define I2C_CMD_READ   2
#define I2C_CMD_STOP   3
#define I2C_CMD_END    4

/* maximum number of data that can be written / read in one transfer */
#define I2C_MAX_DATA   30

#define I2C_FIFO_USED  1

struct i2c_hw_t {
    i2c_dev_t* regs;        /* pointer to register data struct of the I2C device */
    uint8_t mod;            /* peripheral hardware module of the I2C interface */
    uint8_t int_src;        /* peripheral interrupt source used by the I2C device */
    uint8_t pin_scl;        /* SCL pin */
    uint8_t pin_sda;        /* SDA pin */
    uint8_t signal_scl_in;  /* SCL signal to the controller */
    uint8_t signal_scl_out; /* SCL signal from the controller */
    uint8_t signal_sda_in;  /* SDA signal to the controller */
    uint8_t signal_sda_out; /* SDA signal from the controller */
};

static const struct i2c_hw_t _i2c_hw[] = {
    #if defined(I2C0_SCL) && defined(I2C0_SDA) && !defined(I2C0_NOT_AVAILABLE)
    {
        .regs = &I2C0,
        .mod = PERIPH_I2C0_MODULE,
        .int_src = ETS_I2C_EXT0_INTR_SOURCE,
        .pin_scl = I2C0_SCL,
        .pin_sda = I2C0_SDA,
        .signal_scl_in = I2CEXT0_SCL_IN_IDX,
        .signal_scl_out = I2CEXT0_SCL_OUT_IDX,
        .signal_sda_in = I2CEXT0_SDA_IN_IDX,
        .signal_sda_out = I2CEXT0_SDA_OUT_IDX
    },
    #endif
    #if defined(I2C1_SCL) && defined(I2C1_SDA) && !defined(I2C1_NOT_AVAILABLE)
    {
        .regs = &I2C1,
        .mod = PERIPH_I2C1_MODULE,
        .int_src = ETS_I2C_EXT1_INTR_SOURCE,
        .pin_scl = I2C1_SCL,
        .pin_sda = I2C1_SDA,
        .signal_scl_in = I2CEXT1_SCL_IN_IDX,
        .signal_scl_out = I2CEXT1_SCL_OUT_IDX,
        .signal_sda_in = I2CEXT1_SDA_IN_IDX,
        .signal_sda_out = I2CEXT1_SDA_OUT_IDX
    }
    #endif
};

struct _i2c_bus_t
{
    i2c_speed_t speed; /* bus speed */
    uint8_t cmd;       /* command index */
    uint8_t data;      /* index in RAM for data */
    mutex_t lock;      /* mutex lock */
    kernel_pid_t pid;  /* PID of thread that triggered a transfer */
    uint32_t results;  /* results of a transfer */
};

static struct _i2c_bus_t _i2c_bus[] =
{
    #if defined(I2C0_SDA) && defined(I2C0_SCL) && !defined(I2C0_NOT_AVAILABLE)
    {
        .speed = I2C_SPEED_NORMAL,
        .cmd = 0,
        .data = 0,
        .lock = MUTEX_INIT
    },
    #endif
    #if defined(I2C1_SDA) && defined(I2C1_SCL) && !defined(I2C1_NOT_AVAILABLE)
    {
        .speed = I2C_SPEED_NORMAL,
        .cmd = 0,
        .data = 0,
        .lock = MUTEX_INIT
    },
    #endif
};

/* forward declaration of internal functions */

static bool _i2c_init_pins(i2c_t dev);
static bool _i2c_start_cmd (i2c_t dev);
static bool _i2c_stop_cmd (i2c_t dev);
static bool _i2c_end_cmd (i2c_t dev);
static bool _i2c_write_cmd(i2c_t dev, uint8_t* data, uint8_t len);
static bool _i2c_read_cmd(i2c_t dev, uint8_t* data, uint8_t len, bool last);
static int  _i2c_transfer (i2c_t dev);
static bool _i2c_reset_hw (i2c_t dev);
static inline void _i2c_delay (uint32_t delay);
static void _i2c_intr_handler (void *arg);

/* implementation of i2c interface */

int i2c_init_master(i2c_t dev, i2c_speed_t speed)
{
    if (I2C_NUMOF != sizeof(_i2c_bus)/sizeof(struct _i2c_bus_t)) {
        LOG_INFO("I2C_NUMOF does not match number of the I2C_SDA_x/I2C_SCL_x definitions\n");
        LOG_INFO("Please check your configuration in file board.h\n");
        assert(I2C_NUMOF < sizeof(_i2c_bus)/sizeof(struct _i2c_bus_t));

        return -1;
    }

    CHECK_PARAM_RET (dev < I2C_NUMOF, -1)

    if (speed == I2C_SPEED_FAST_PLUS || speed == I2C_SPEED_HIGH) {
        LOG_INFO("I2C_SPEED_FAST_PLUS and I2C_SPEED_HIGH are not supported\n");
        return -2;
    }

    _i2c_bus[dev].speed   = speed;
    _i2c_bus[dev].cmd = 0;
    _i2c_bus[dev].data = 0;

    DEBUG ("%s scl=%d sda=%d speed=%d\n", __func__,
           _i2c_hw[dev].pin_scl, _i2c_hw[dev].pin_sda, _i2c_bus[dev].speed);

    /* enable (power on) the according I2C module */
    periph_module_enable(_i2c_hw[dev].mod);

    /* initialize pins */
    if (!_i2c_init_pins(dev)) {
        return -1;
    }

    /* set master mode */
    _i2c_hw[dev].regs->ctr.ms_mode = 1;

    /* set bit order to MSB first */
    _i2c_hw[dev].regs->ctr.tx_lsb_first = 0;
    _i2c_hw[dev].regs->ctr.rx_lsb_first = 0;

    /* determine the half period of clock in APB clock cycles */
    uint32_t half_period = 0;

    switch (speed) {
        case I2C_SPEED_LOW:
            /* 10 kbps (period 100 us) */
            half_period = (I2C_CLK_FREQ >> 1) / 10000;
            break;

        case I2C_SPEED_NORMAL:
            /* 100 kbps (period 10 us) */
            half_period = ((I2C_CLK_FREQ >> 1) / 100000);
            half_period = half_period * 95 / 100; /* correction factor */
            break;

        case I2C_SPEED_FAST:
            /* 400 kbps (period 2.5 us) */
            half_period = ((I2C_CLK_FREQ >> 1) / 400000);
            half_period = half_period * 82 / 100; /* correction factor */
            break;

        case I2C_SPEED_FAST_PLUS:
            /* 1 Mbps (period 1 us) not working */
            half_period = (I2C_CLK_FREQ >> 1) / 1000000;
            break;

        case I2C_SPEED_HIGH:
            /* 3.4 Mbps (period 0.3 us) not working */
            half_period = (I2C_CLK_FREQ >> 1) / 3400000;
            break;

        default:
            LOG_ERROR("Invalid speed value in %s\n", __func__);
            return -2;
    }

    /* set an timeout which is at least 16 times of  half cycle */
    _i2c_hw[dev].regs->timeout.tout = half_period << 4;

    /* timing for SCL (low and high time in APB clock cycles) */
    _i2c_hw[dev].regs->scl_low_period.period = half_period;
    _i2c_hw[dev].regs->scl_high_period.period = half_period;

    /* timing for SDA (sample time after rising edge and hold time after falling edge) */
    _i2c_hw[dev].regs->sda_sample.time = half_period >> 1;
    _i2c_hw[dev].regs->sda_hold.time = half_period >> 1;

    /* timing for START condition (STAR hold and repeated START setup time) */
    _i2c_hw[dev].regs->scl_start_hold.time = half_period >> 1;
    _i2c_hw[dev].regs->scl_rstart_setup.time = half_period >> 1;

    /* timing for STOP condition (STOP hold and STOP setup time) */
    _i2c_hw[dev].regs->scl_stop_hold.time = half_period >> 1;
    _i2c_hw[dev].regs->scl_stop_setup.time = half_period >> 1;

    /* configure open drain outputs */
    _i2c_hw[dev].regs->ctr.scl_force_out = 1;
    _i2c_hw[dev].regs->ctr.sda_force_out = 1;

    /* sample data during high level */
    _i2c_hw[dev].regs->ctr.sample_scl_level = 0;

    /* enable non FIFO access and disable slave FIFO address offset */
    #if I2C_FIFO_USED
    _i2c_hw[dev].regs->fifo_conf.nonfifo_en = 0;
    #else
    _i2c_hw[dev].regs->fifo_conf.nonfifo_en = 1;
    _i2c_hw[dev].regs->fifo_conf.nonfifo_rx_thres = 0;
    _i2c_hw[dev].regs->fifo_conf.nonfifo_tx_thres = 0;
    _i2c_hw[dev].regs->fifo_conf.rx_fifo_full_thrhd = 0;
    _i2c_hw[dev].regs->fifo_conf.tx_fifo_empty_thrhd = 0;

    #endif
    _i2c_hw[dev].regs->fifo_conf.fifo_addr_cfg_en = 0;

    /* route all I2C interrupt sources to same the CPU interrupt */
    intr_matrix_set(PRO_CPU_NUM, _i2c_hw[dev].int_src, CPU_INUM_I2C);

    /* set the interrupt handler and enable the interrupt */
    xt_set_interrupt_handler(CPU_INUM_I2C, _i2c_intr_handler, NULL);
    xt_ints_on(BIT(CPU_INUM_I2C));

    return 0;
}

int i2c_acquire(i2c_t dev)
{
    CHECK_PARAM_RET (dev < I2C_NUMOF, -1)

    mutex_lock(&_i2c_bus[dev].lock);
    return 0;
}

int i2c_release(i2c_t dev)
{
    CHECK_PARAM_RET (dev < I2C_NUMOF, -1)

    mutex_unlock(&_i2c_bus[dev].lock);
    return 0;
}

int i2c_read_byte(i2c_t dev, uint8_t address, void *data)
{
    return i2c_read_bytes(dev, address, data, 1);
}

static int _i2c_read_data (i2c_t dev, uint8_t address, uint8_t *data, int length)
{
    bool result = true;
    int ret = -1;

    /* send START or repeated START condition to switch to read phase */
    result &= _i2c_start_cmd (dev);

    /* send address byte with read flag */
    uint8_t addr = (address << 1) | I2C_FLAG_READ;
    result &= _i2c_write_cmd (dev, &addr, 1);

    uint32_t len = length;
    uint32_t off = 0;

    /* if length > I2C_MAX_DATA read blocks I2C_MAX_DATA bytes at a time */
    while (len > I2C_MAX_DATA)
    {
        /* read data bytes */
        result &= _i2c_read_cmd (dev, data, I2C_MAX_DATA, false);

        /* send end command */
        result &= _i2c_end_cmd (dev);

        /* execute commands */
        if (result) {
            ret = _i2c_transfer (dev);
        }

        /* if transfer was successful, fetch data from I2C ram */
        if (ret == 0) {
            for (unsigned i = 0; i < I2C_MAX_DATA; i++) {
                #if I2C_FIFO_USED
                data[i + off] = _i2c_hw[dev].regs->fifo_data.data;
                #else
                data[i + off] = _i2c_hw[dev].regs->ram_data[i];
                #endif
            }
        }

        len -= I2C_MAX_DATA;
        off += I2C_MAX_DATA;
    }

    /* read remaining data bytes */
    result &= _i2c_read_cmd (dev, data, len, true);

    /* send STOP condition */
    result &= _i2c_stop_cmd (dev);

    /* execute commands */
    if (result) {
        ret = _i2c_transfer (dev);
    }

    /* if transfer was successful, fetch data from I2C ram */
    if (ret == 0) {
        for (unsigned i = 0; i < len; i++) {
            #if I2C_FIFO_USED
            ((uint8_t*)data)[i + off] = _i2c_hw[dev].regs->fifo_data.data;
            #else
            ((uint8_t*)data)[i + off] = _i2c_hw[dev].regs->ram_data[i];
            #endif
        }
    }

    return ret == 0 ? length : -1;
}

int i2c_read_bytes(i2c_t dev, uint8_t address, void *data, int length)
{
    DEBUG ("%s dev=%u addr=%02x data=%p len=%d\n", __func__, dev, address, data, length);

    CHECK_PARAM_RET (dev < I2C_NUMOF, -1);
    CHECK_PARAM_RET (length > 0, 0);
    CHECK_PARAM_RET (data != NULL, 0);

    /* reset hardware module to bring the FSMs in definite state */
    _i2c_reset_hw(dev);

    /* call read phase */
    return _i2c_read_data(dev, address, (uint8_t*)data, length);
}

int i2c_read_reg(i2c_t dev, uint8_t address, uint8_t reg, void *data)
{
    return i2c_read_regs(dev, address, reg, data, 1);
}

int i2c_read_regs(i2c_t dev, uint8_t address, uint8_t reg, void *data, int length)
{
    DEBUG ("%s dev=%u addr=%02x reg=%02x data=%p len=%d\n", __func__,
           dev, address, reg, data, length);

    CHECK_PARAM_RET (dev < I2C_NUMOF, -1)
    CHECK_PARAM_RET (length > 0, 0)
    CHECK_PARAM_RET (data != NULL, 0);

    bool result = true;
    int ret = -1;

    /* reset hardware module to bring the FSMs in definite state */
    _i2c_reset_hw(dev);

    /* send START condition */
    result &= _i2c_start_cmd (dev);

    /* send address byte with write flag */
    uint8_t addr = (address << 1) | I2C_FLAG_WRITE;
    result &= _i2c_write_cmd (dev, &addr, 1);

    /* send register address */
    result &= _i2c_write_cmd (dev, &reg, 1);

    if (result) {
        /* call read phase */
        ret = _i2c_read_data(dev, address, (uint8_t*)data, length);
    }

    return ret;
}

static int _i2c_write_data (i2c_t dev, uint8_t address, uint8_t *data, int length)
{
    bool result = true;
    int ret = -1;

    uint32_t len = length;
    uint32_t off = 0;

    /* if length > I2C_MAX_DATA write blocks I2C_MAX_DATA bytes at a time */
    while (len > I2C_MAX_DATA)
    {
        /* write data bytes */
        result &= _i2c_write_cmd (dev, data + off, I2C_MAX_DATA);

        /* send end command */
        result &= _i2c_end_cmd (dev);

        /* execute commands */
        if (result) {
            ret = _i2c_transfer (dev);
        }

        len -= I2C_MAX_DATA;
        off += I2C_MAX_DATA;
    }

    /* write remaining data bytes */
    result &= _i2c_write_cmd (dev, data + off, len);

    /* send STOP condition */
    result &= _i2c_stop_cmd (dev);

    /* execute commands */
    if (result) {
        ret = _i2c_transfer (dev);
    }

    return ret == 0 ? length : -1;
}

int i2c_write_byte(i2c_t dev, uint8_t address, uint8_t data)
{
    return i2c_write_bytes(dev, address, &data, 1);
}

int i2c_write_bytes(i2c_t dev, uint8_t address, const void *data, int length)
{
    DEBUG ("%s dev=%u addr=%02x data=%p len=%d\n", __func__, dev, address, data, length);

    CHECK_PARAM_RET (dev < I2C_NUMOF, -1);
    CHECK_PARAM_RET (length > 0, 0);
    CHECK_PARAM_RET (data != NULL, 0);

    bool result = true;
    int ret = -1;

    /* reset hardware module to bring the FSMs in definite state */
    _i2c_reset_hw(dev);

    /* send START condition */
    result &= _i2c_start_cmd (dev);

    /* send address byte with write flag */
    uint8_t addr = (address << 1) | I2C_FLAG_WRITE;
    result &= _i2c_write_cmd (dev, &addr, 1);

    /* call write phase */
    if (result) {
        ret = _i2c_write_data(dev, address, (uint8_t*)data, length);
    }

    return ret;
}

int i2c_write_reg(i2c_t dev, uint8_t address, uint8_t reg, uint8_t data)
{
    return i2c_write_regs(dev, address, reg, &data, 1);
}

int i2c_write_regs(i2c_t dev, uint8_t address, uint8_t reg,
                              const void *data, int length)
{
    DEBUG ("%s dev=%u addr=%02x reg=%02x data=%p len=%d\n", __func__,
           dev, address, reg, data, length);

    CHECK_PARAM_RET (dev < I2C_NUMOF, -1)
    CHECK_PARAM_RET (length > 0, 0)
    CHECK_PARAM_RET (data != NULL, 0);

    bool result = true;
    int ret = -1;

    /* reset hardware module to bring the FSMs in definite state */
    _i2c_reset_hw(dev);

    /* send START condition */
    result &= _i2c_start_cmd(dev);

    /* send address byte with write flag */
    uint8_t addr = (address << 1) | I2C_FLAG_WRITE;
    result &= _i2c_write_cmd (dev, &addr, 1);

    /* send register address */
    result &= _i2c_write_cmd (dev, &reg, 1);

    /* call write phase */
    if (result) {
        ret = _i2c_write_data(dev, address, (uint8_t*)data, length);
    }

    return ret;
}

void i2c_poweron(i2c_t dev)
{
    CHECK_PARAM (dev < I2C_NUMOF);

    /* enable (power on) the according I2C module */
    periph_module_enable(_i2c_hw[dev].mod);
}

void i2c_poweroff(i2c_t dev)
{
    CHECK_PARAM (dev < I2C_NUMOF);

    /* enable (power on) the according I2C module */
    periph_module_disable(_i2c_hw[dev].mod);
}

/* internal functions */

static bool _i2c_init_pins(i2c_t dev)
{
    /* reset GPIO usage type if the pins were used already for I2C before to
       make it possible to reinitialize I2C */
    if (gpio_get_pin_usage(_i2c_hw[dev].pin_scl) == _I2C) {
        gpio_set_pin_usage(_i2c_hw[dev].pin_scl, _GPIO);
    }
    if (gpio_get_pin_usage(_i2c_hw[dev].pin_sda) == _I2C) {
        gpio_set_pin_usage(_i2c_hw[dev].pin_sda, _GPIO);
    }

    /* try to configure SDA and SCL pin as GPIO in open-drain mode with enabled pull-ups */
    if (gpio_init (_i2c_hw[dev].pin_scl, GPIO_IN_OD_PU) ||
        gpio_init (_i2c_hw[dev].pin_sda, GPIO_IN_OD_PU)) {
        return false;
    }

    /* bring signals to high */
    gpio_set(_i2c_hw[dev].pin_scl);
    gpio_set(_i2c_hw[dev].pin_sda);

    /* store the usage type in GPIO table */
    gpio_set_pin_usage(_i2c_hw[dev].pin_scl, _I2C);
    gpio_set_pin_usage(_i2c_hw[dev].pin_sda, _I2C);

    /* connect SCL and SDA pins to output signals through the GPIO matrix */
    GPIO.func_out_sel_cfg[_i2c_hw[dev].pin_scl].func_sel = _i2c_hw[dev].signal_scl_out;
    GPIO.func_out_sel_cfg[_i2c_hw[dev].pin_sda].func_sel = _i2c_hw[dev].signal_sda_out;

    /* connect SCL and SDA input signals to pins through the GPIO matrix */
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_scl_in].sig_in_sel = 1;
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_scl_in].sig_in_inv = 0;
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_scl_in].func_sel = _i2c_hw[dev].pin_scl;
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_sda_in].sig_in_sel = 1;
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_sda_in].sig_in_inv = 0;
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_sda_in].func_sel = _i2c_hw[dev].pin_sda;

    return true;
}

static bool _i2c_start_cmd(i2c_t dev)
{
    DEBUG ("%s\n", __func__);

    /* place START condition command in command queue */
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].val = 0;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].op_code = I2C_CMD_RSTART;

    /* increment the command counter */
    _i2c_bus[dev].cmd++;

    return true;
}

static bool _i2c_stop_cmd (i2c_t dev)
{
    DEBUG ("%s\n", __func__);

    /* place STOP condition command in command queue */
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].val = 0;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].op_code = I2C_CMD_STOP;

    /* increment the command counter */
    _i2c_bus[dev].cmd++;

    return true;
}

static bool _i2c_end_cmd (i2c_t dev)
{
    DEBUG ("%s\n", __func__);

    /* place STOP condition command in command queue */
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].val = 0;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].op_code = I2C_CMD_END;

    /* increment the command counter */
    _i2c_bus[dev].cmd++;

    return true;
}

static bool _i2c_write_cmd (i2c_t dev, uint8_t* data, uint8_t len)
{
    DEBUG ("%s dev=%u data=%p len=%d\n", __func__, dev, data, len);

    if (_i2c_bus[dev].data + len > I2C_MAX_DATA) {
        LOG_ERROR("Maximum number of bytes (32 bytes) that can be sent with "
                  "on transfer reached\n");
        return false;
    }

    /* store the byte in RAM of I2C controller and increment the data counter */
    for (int i = 0; i < len; i++) {
        #if I2C_FIFO_USED
        WRITE_PERI_REG(I2C_DATA_APB_REG(dev), data[i]);
        #else
        _i2c_hw[dev].regs->ram_data[_i2c_bus[dev].data++] = (uint32_t)data[i];
        #endif

    }

    /* place WRITE command for multiple bytes in command queue */
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].val = 0;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].byte_num = len;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].ack_en = 1;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].ack_exp = 0;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].ack_val = 0;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].op_code = I2C_CMD_WRITE;

    /* increment the command counter */
    _i2c_bus[dev].cmd++;

    return true;
}

static bool _i2c_read_cmd (i2c_t dev, uint8_t* data, uint8_t len, bool last)
{
    DEBUG ("%s dev=%u data=%p len=%d\n", __func__, dev, data, len);

    if (len < 1 || len > I2C_MAX_DATA) {
        /* at least one byte has to be read */
        LOG_ERROR("At least one byte has to be read\n");
        return false;
    }

    if (len > 1)
    {
        /* place READ command for len-1 bytes with positive ack in command queue*/
        _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].val = 0;
        _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].byte_num = len-1;
        _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].ack_en = 0;
        _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].ack_exp = 0;
        _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].ack_val = 0;
        _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].op_code = I2C_CMD_READ;

        /* increment the command counter */
        _i2c_bus[dev].cmd++;
    }

    /* place READ command for last byte with negative ack in last segment in command queue*/
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].val = 0;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].byte_num = 1;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].ack_en = 0;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].ack_exp = 0;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].ack_val = last ? 1 : 0;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].op_code = I2C_CMD_READ;

    /* increment the command counter */
    _i2c_bus[dev].cmd++;

    return true;
}

#if 0
static bool _i2c_clear_bus(i2c_t dev)
{
    /* reset the usage type in GPIO table */
    gpio_set_pin_usage(_i2c_hw[dev].pin_scl, _GPIO);
    gpio_set_pin_usage(_i2c_hw[dev].pin_sda, _GPIO);

    /* configure SDA and SCL pin as GPIO in open-drain mode temporarily */
    gpio_init (_i2c_hw[dev].pin_scl, GPIO_IN_OD_PU);
    gpio_init (_i2c_hw[dev].pin_sda, GPIO_IN_OD_PU);

    /* master send some clock pulses to make the slave release the bus */
    gpio_set (_i2c_hw[dev].pin_scl);
    gpio_set (_i2c_hw[dev].pin_sda);
    gpio_clear (_i2c_hw[dev].pin_sda);
    for (int i = 0; i < 20; i++) {
        gpio_toggle(_i2c_hw[dev].pin_scl);
    }
    gpio_set(_i2c_hw[dev].pin_sda);

    /* store the usage type in GPIO table */
    gpio_set_pin_usage(_i2c_hw[dev].pin_scl, _I2C);
    gpio_set_pin_usage(_i2c_hw[dev].pin_sda, _I2C);

    /* connect SCL and SDA pins to output signals through the GPIO matrix */
    GPIO.func_out_sel_cfg[_i2c_hw[dev].pin_scl].func_sel = _i2c_hw[dev].signal_scl_out;
    GPIO.func_out_sel_cfg[_i2c_hw[dev].pin_sda].func_sel = _i2c_hw[dev].signal_sda_out;

    /* connect SCL and SDA input signals to pins through the GPIO matrix */
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_scl_in].sig_in_sel = 1;
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_scl_in].sig_in_inv = 0;
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_scl_in].func_sel = _i2c_hw[dev].pin_scl;
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_sda_in].sig_in_sel = 1;
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_sda_in].sig_in_inv = 0;
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_sda_in].func_sel = _i2c_hw[dev].pin_sda;

    return true;
}
#endif

static inline void _i2c_delay (uint32_t cycles)
{
    /* produces a delay of 0,0625 us per cycle for -O2 compile option */
    /* 1 us = ca. 16 cycles (80 MHz) / 1 us = 32 cycles (160 MHz) */

    if (cycles) {
        __asm__ volatile ("1: _addi.n  %0, %0, -1 \n"
                          "   bnez     %0, 1b     \n" : "=r" (cycles) : "0" (cycles));
    }
}

/* transfer related interrupts */
static const uint32_t transfer_int_mask = I2C_TRANS_COMPLETE_INT_ENA
                                        | I2C_ACK_ERR_INT_ENA
                                        | I2C_TIME_OUT_INT_ENA
                                        | I2C_ARBITRATION_LOST_INT_ENA
                                        | I2C_END_DETECT_INT_ENA;

#define I2C_THREAD_FLAG BIT(0)

/* Transfer of commands in I2C controller command pipeline */
static int _i2c_transfer (i2c_t dev)
{
    DEBUG("%s\n", __func__);

    #if FIFO_USED
    /* reset RX FIFO queue */
    _i2c_hw[dev].regs->fifo_conf.rx_fifo_rst = 1;
    _i2c_hw[dev].regs->fifo_conf.rx_fifo_rst = 0;
    #endif

    /* disable and enable all transmission interrupts and clear current status */
    _i2c_hw[dev].regs->int_ena.val &= ~transfer_int_mask;
    _i2c_hw[dev].regs->int_ena.val |= transfer_int_mask;
    _i2c_hw[dev].regs->int_clr.val  = transfer_int_mask;

    /* start execution of commands in command pipeline registers */
    _i2c_bus[dev].results = 0;
    _i2c_hw[dev].regs->ctr.trans_start = 0;
    _i2c_hw[dev].regs->ctr.trans_start = 1;

    /* wait for transfer results */
    _i2c_bus[dev].pid = thread_getpid();
    thread_flags_wait_one(I2C_THREAD_FLAG);

    DEBUG("%s results=%08x\n", __func__, _i2c_bus[dev].results);

    /* tranmission complete and end results are considered as success */
    _i2c_bus[dev].results &= ~(I2C_TRANS_COMPLETE_INT_ENA | I2C_END_DETECT_INT_ENA);

    #if FIFO_USED
    /* reset TX FIFO queue */
    _i2c_hw[dev].regs->fifo_conf.tx_fifo_rst = 1;
    _i2c_hw[dev].regs->fifo_conf.tx_fifo_rst = 0;
    #endif

    /* reset command and data index */
    _i2c_bus[dev].cmd = 0;
    _i2c_bus[dev].data = 0;

    return _i2c_bus[dev].results;
}

static void /* IRAM */ _i2c_intr_handler (void *arg)
{
    /* to satisfy the compiler */
    (void)arg;

    irq_isr_enter ();

    /* all I2C peripheral interrupt sources are routed to the same interrupt,
       so we have to use the status register to distinguish interruptees */
    for (unsigned dev = 0; dev < I2C_NUMOF; dev++) {
        /* test for transmission complete or end interrupt */
        if (_i2c_hw[dev].regs->int_status.trans_complete ||
            _i2c_hw[dev].regs->int_status.end_detect) {
            /* set transfer result */
            _i2c_bus[dev].results |= _i2c_hw[dev].regs->int_status.val;
            /* wake up the thread that is waiting for the results */
            thread_flags_set((thread_t*)thread_get(_i2c_bus[dev].pid), I2C_THREAD_FLAG);
        }
        else if (_i2c_hw[dev].regs->int_status.val) {
            /* set transfer result */
            _i2c_bus[dev].results |= _i2c_hw[dev].regs->int_status.val;
        }
        /* clear all interrupts */
        _i2c_hw[dev].regs->int_clr.val = ~0x0;
    }

    irq_isr_exit ();
}

#endif /* defined(I2C_NUMOF) && I2C_NUMOF > 0 */

void i2c_print_config(void)
{
    #if defined(I2C_NUMOF) && I2C_NUMOF
    for (unsigned bus = 0; bus < I2C_NUMOF; bus++) {
        LOG_INFO("I2C_DEV(%d): scl=%d sda=%d\n", bus,
                 _i2c_hw[bus].pin_scl, _i2c_hw[bus].pin_sda);
    }
    #else
    LOG_INFO("I2C: no devices\n");
    #endif /* defined(I2C_NUMOF) && I2C_NUMOF > 0 */
}

/*
 * PLEASE NOTE: The following function is from the ESP-IDF and is licensed
 * under the Apache License, Version 2.0 (the "License").
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
 */
static bool _i2c_reset_hw (i2c_t dev)
{
    /* save current configuration */
    uint32_t ctr              = _i2c_hw[dev].regs->ctr.val;
    uint32_t fifo_conf        = _i2c_hw[dev].regs->fifo_conf.val;
    uint32_t scl_low_period   = _i2c_hw[dev].regs->scl_low_period.val;
    uint32_t scl_high_period  = _i2c_hw[dev].regs->scl_high_period.val;
    uint32_t scl_start_hold   = _i2c_hw[dev].regs->scl_start_hold.val;
    uint32_t scl_rstart_setup = _i2c_hw[dev].regs->scl_rstart_setup.val;
    uint32_t scl_stop_hold    = _i2c_hw[dev].regs->scl_stop_hold.val;
    uint32_t scl_stop_setup   = _i2c_hw[dev].regs->scl_stop_setup.val;
    uint32_t sda_hold         = _i2c_hw[dev].regs->sda_hold.val;
    uint32_t sda_sample       = _i2c_hw[dev].regs->sda_sample.val;
    uint32_t timeout          = _i2c_hw[dev].regs->timeout.val;
    uint32_t scl_filter_cfg   = _i2c_hw[dev].regs->scl_filter_cfg.val;
    uint32_t sda_filter_cfg   = _i2c_hw[dev].regs->sda_filter_cfg.val;

    /* reset hardware mpdule */
    i2c_poweroff(dev);
    /* TODO Improvement _i2c_clear_bus(dev); */
    i2c_poweron(dev);

    /* restore configuration */
    _i2c_hw[dev].regs->int_ena.val          = 0;
    _i2c_hw[dev].regs->ctr.val              = ctr & (~I2C_TRANS_START_M);
    _i2c_hw[dev].regs->fifo_conf.val        = fifo_conf;
    _i2c_hw[dev].regs->scl_low_period.val   = scl_low_period;
    _i2c_hw[dev].regs->scl_high_period.val  = scl_high_period;
    _i2c_hw[dev].regs->scl_start_hold.val   = scl_start_hold;
    _i2c_hw[dev].regs->scl_rstart_setup.val = scl_rstart_setup;
    _i2c_hw[dev].regs->scl_stop_hold.val    = scl_stop_hold;
    _i2c_hw[dev].regs->scl_stop_setup.val   = scl_stop_setup;
    _i2c_hw[dev].regs->sda_hold.val         = sda_hold;
    _i2c_hw[dev].regs->sda_sample.val       = sda_sample;
    _i2c_hw[dev].regs->timeout.val          = timeout;
    _i2c_hw[dev].regs->scl_filter_cfg.val   = scl_filter_cfg;
    _i2c_hw[dev].regs->sda_filter_cfg.val   = sda_filter_cfg;

    /* disable and clear all interrupt sources */
    _i2c_hw[dev].regs->int_ena.val = 0;
    _i2c_hw[dev].regs->int_clr.val = ~0x0;

    return true;
}

#endif /* I2C_HW_USED */
