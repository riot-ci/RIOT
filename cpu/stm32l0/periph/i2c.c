/*
 * Copyright (C) 2017 we-sens.com
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  driver_periph
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @note This implementation only implements the 7-bit addressing polling mode.
 *
 * @author      Aurélien Fillau <aurelien.fillau@we-sens.com>
 *
 * @}
 */

#include <stdint.h>


#include "cpu.h"
#include "mutex.h"
#include "periph/i2c.h"
#include "periph/gpio.h"
#include "periph_conf.h"


#define ENABLE_DEBUG    (1)
#include "debug.h"


/* guard file in case no I2C device is defined */
#if I2C_NUMOF

/**
 * @brief Array holding one pre-initialized mutex for each I2C device
 */
static mutex_t locks[] =  {
#if I2C_0_EN
    [I2C_0] = MUTEX_INIT,
#endif
#if I2C_1_EN
    [I2C_1] = MUTEX_INIT,
#endif
#if I2C_2_EN
    [I2C_2] = MUTEX_INIT,
#endif
#if I2C_3_EN
    [I2C_3] = MUTEX_INIT
#endif
};

//#define I2C_TIMING    0x10A13E56 /* 100 kHz with analog Filter ON, Rise Time 400ns, Fall Time 100ns */
#define I2C_TIMING      0x00B1112E /* 400 kHz with analog Filter ON, Rise Time 250ns, Fall Time 100ns */
#define TIMING_CLEAR_MASK   (0xF0FFFFFFU)  /*!< I2C TIMING clear register Mask */

/* static function definitions */
static void _i2c_init(I2C_TypeDef *i2c, i2c_speed_t speed);
static void _start(I2C_TypeDef *i2c, uint8_t address, uint8_t nbyte, uint8_t mode, uint8_t rw_flag);
static inline void _write(I2C_TypeDef *i2c, const uint8_t *data, int length);
static inline void _stop(I2C_TypeDef *i2c);
static inline uint32_t _setupTiming(i2c_speed_t spd, uint32_t clockFreq);

int i2c_init_master(i2c_t dev, i2c_speed_t speed)
{
    if ((unsigned int)dev >= I2C_NUMOF) {
        return -1;
    }

    /* read speed configuration */
//    switch (speed) {
//        case I2C_SPEED_NORMAL:
//            ccr = I2C_APBCLK / 200000;
//            break;
//
//        case I2C_SPEED_FAST:
//            ccr = I2C_APBCLK / 800000;
//            break;
//
//        default:
//            return -2;
//    }
    I2C_TypeDef *i2c = i2c_config[dev].dev;

    /* enable I2C clock */
    i2c_poweron(dev);

    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;

    /*Enable Clock for I2C*/
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    /* Enable GPIO Clock */
    RCC->IOPENR |=  (1UL << 1);

    /* configure pins */
    gpio_init(i2c_config[dev].scl, i2c_config[dev].pin_mode);
    gpio_init_af(i2c_config[dev].scl, i2c_config[dev].af);
    gpio_init(i2c_config[dev].sda, i2c_config[dev].pin_mode);
    gpio_init_af(i2c_config[dev].sda, i2c_config[dev].af);

    i2c->CR1 &= ~I2C_CR1_PE;

    /* Disable device */
    i2c->CR1 = 0;
    i2c->CR2 = 0;
    /* Configure I2Cx: Frequency range */
    i2c->TIMINGR = _setupTiming(I2C_SPEED_LOW,32000000);
    i2c->TIMINGR = 0x10A13E56;
    /* Disable Own Address1 before set the Own Address1 configuration */
    i2c->OAR1 &= ~I2C_OAR1_OA1EN;
    /* Configure I2Cx: Own Address1 and ack own address1 mode */
    i2c->OAR1 = (I2C_OAR1_OA1EN | 0);

//
//    /* Enable the AUTOEND by default, and enable NACK (should be disable only during Slave process */
//    i2c->CR2 |= (I2C_CR2_AUTOEND | I2C_CR2_NACK);
    /*---------------------------- I2Cx OAR2 Configuration ---------------------*/
//    /* Disable Own Address2 before set the Own Address2 configuration */
//    i2c->OAR2 &= ~I2C_DUALADDRESS_ENABLE;

    /* Configure device */
    i2c->OAR1 = 0;              /* makes sure we are in 7-bit address mode */
    /* Enable device */
    i2c->CR1 |= I2C_CR1_PE;

    return 0;
}

static inline uint32_t _setupTiming(i2c_speed_t spd, uint32_t clockFreq) {
    (void) spd;
    (void) clockFreq;
    uint32_t presc = 0;
    uint32_t sdadel = 2;
    uint32_t scldel = 2;
    uint32_t scll = 6;
    uint32_t sclh = 7;

    return  presc << 28 |
            scldel << 20 |
            sdadel << 16 |
            sclh << 8 |
            scll;
}

int i2c_acquire(i2c_t dev)
{
    if (dev >= I2C_NUMOF) {
        return -1;
    }
    mutex_lock(&locks[dev]);
    return 0;
}

int i2c_release(i2c_t dev)
{
    if (dev >= I2C_NUMOF) {
        return -1;
    }
    mutex_unlock(&locks[dev]);
    return 0;
}

int i2c_read_byte(i2c_t dev, uint8_t address, void *data)
{
    return i2c_read_bytes(dev, address, data, 1);
}

int i2c_read_bytes(i2c_t dev, uint8_t address, void *data, int length)
{
	if ((unsigned int)dev >= I2C_NUMOF) {
		return -1;
	}
    I2C_TypeDef *i2c = i2c_config[dev].dev;

	_start(i2c, address<<1, length, 1, I2C_FLAG_READ);

	while (length) {
		while (!(i2c->ISR & I2C_ISR_RXNE)) {} //AFU timeout ?

		*(uint8_t*)data++ = i2c->RXDR;
		length--;
	}

	/* Wait until STOP is cleared by hardware */
	while (i2c->ISR & I2C_ISR_TCR) {}

	_stop(i2c);

    return length;
}

int i2c_read_reg(i2c_t dev, uint8_t address, uint8_t reg, void *data)
{
    return i2c_read_regs(dev, address, reg, data, 1);
}

int i2c_read_regs(i2c_t dev, uint8_t address, uint8_t reg, void *data, int length)
{
    if ((unsigned int)dev >= I2C_NUMOF) {
        return -1;
    }

    I2C_TypeDef *i2c = i2c_config[dev].dev;

	/* Check to see if the bus is busy */
	while((i2c->ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY);

	_start(i2c, address<<1, 1, 0, I2C_FLAG_WRITE);

    i2c->TXDR = reg;

	/* Wait for transfer to be completed */
	while((I2C1->ISR & I2C_ISR_TC) == 0);

	return i2c_read_bytes(dev, address, data, length);
}

int i2c_write_byte(i2c_t dev, uint8_t address, uint8_t data)
{
    return i2c_write_bytes(dev, address, &data, 1);
}

int i2c_write_bytes(i2c_t dev, uint8_t address, const void *data, int length)
{
    if ((unsigned int)dev >= I2C_NUMOF) {
        return -1;
    }

    I2C_TypeDef *i2c = i2c_config[dev].dev;

    /* Start transmission and send slave address */
    _start(i2c, address, 1, 0, I2C_FLAG_WRITE);
    /* Send out data bytes */
    _write(i2c, data, length);
    /* End transmission */
    _stop(i2c);
    return length;
}

int i2c_write_reg(i2c_t dev, uint8_t address, uint8_t reg, uint8_t data)
{
    return i2c_write_regs(dev, address, reg, &data, 1);
}

int i2c_write_regs(i2c_t dev, uint8_t address, uint8_t reg, const void *data, int length)
{
    if ((unsigned int)dev >= I2C_NUMOF) {
        return -1;
    }

    I2C_TypeDef *i2c = i2c_config[dev].dev;

	/* Check to see if the bus is busy */
	while((i2c->ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY);

    /* Start transmission and send slave address */
	_start(i2c, address<<1, 1+length, 0, I2C_FLAG_WRITE);

    /* Send register address and wait for complete transfer to be finished*/
    _write(i2c, &reg, 1);
    /* Write data to register */
    _write(i2c, data, length);
    /* Finish transfer */
    _stop(i2c);
    /* Return number of bytes send */
    return length;
}

void i2c_poweron(i2c_t dev)
{
	if ((unsigned int)dev < I2C_NUMOF) {
        periph_clk_en(APB1, (RCC_APB1ENR_I2C1EN << dev));
    }
}

void i2c_poweroff(i2c_t dev)
{
    if ((unsigned int)dev < I2C_NUMOF) {
        while (i2c_config[dev].dev->ISR & I2C_ISR_BUSY) {}
        periph_clk_dis(APB1, (RCC_APB1ENR_I2C1EN << dev));
    }
}

static void _start(I2C_TypeDef *i2c, uint8_t address, uint8_t nbyte, uint8_t mode, uint8_t rw_flag)
{
	gpio_toggle(GPIO_PIN(PORT_C, 9));
	gpio_toggle(GPIO_PIN(PORT_C, 9));

	uint32_t tmpreg= 0;
	/* Get the CR2 register value */
	tmpreg = i2c->CR2;

	/* Clear tmpreg specific bits */
	tmpreg &= (uint32_t)~((uint32_t)(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP));

	/* Update tmpreg */
	tmpreg |= (uint32_t)(((uint32_t)address & I2C_CR2_SADD) | (((uint32_t)nbyte << 16 ) & I2C_CR2_NBYTES) | (uint32_t)(mode << I2C_CR2_RD_WRN_Pos) | (uint32_t)(I2C_CR2_START | (rw_flag << I2C_CR2_RD_WRN_Pos)));
	/* Update CR2 register */
	i2c->CR2 = tmpreg;

	/* Wait for the start followed by the address to be sent */
    while (!(i2c->CR2 & I2C_CR2_START)) {}
}

static inline void _write(I2C_TypeDef *i2c, const uint8_t *data, int length)
{
    for (int i = 0; i < length; i++) {
        /* Write data to data register */
        i2c->TXDR = data[i];

        /* Wait for transfer to finish */
        while (!(i2c->ISR & I2C_ISR_TXE)) {}
    }
}

static inline void _stop(I2C_TypeDef *i2c)
{
    /* Make sure last byte was send */
    while (!(i2c->ISR & I2C_ISR_TXE)) {}

    /* Send STOP condition */
    i2c->CR2 |= I2C_CR2_STOP;
}

//#if I2C_0_EN
//void I2C_0_ERR_ISR(void)
//{
//    unsigned state = I2C1->SR1;
//    DEBUG("\n\n### I2C1 ERROR OCCURED ###\n");
//    DEBUG("status: %08x\n", state);
//    if (state & I2C_SR1_OVR) {
//        DEBUG("OVR\n");
//    }
//    if (state & I2C_SR1_AF) {
//        DEBUG("AF\n");
//    }
//    if (state & I2C_SR1_ARLO) {
//        DEBUG("ARLO\n");
//    }
//    if (state & I2C_SR1_BERR) {
//        DEBUG("BERR\n");
//    }
//    if (state & I2C_SR1_PECERR) {
//        DEBUG("PECERR\n");
//    }
//    if (state & I2C_SR1_TIMEOUT) {
//        DEBUG("TIMEOUT\n");
//    }
//    if (state & I2C_SR1_SMBALERT) {
//        DEBUG("SMBALERT\n");
//    }
//    while (1) {}
//}
//#endif /* I2C_0_EN */

//#if I2C_1_EN
//void I2C_1_ERR_ISR(void)
//{
//    unsigned state = I2C2->SR1;
//    DEBUG("\n\n### I2C2 ERROR OCCURED ###\n");
//    DEBUG("status: %08x\n", state);
//    if (state & I2C_SR1_OVR) {
//        DEBUG("OVR\n");
//    }
//    if (state & I2C_SR1_AF) {
//        DEBUG("AF\n");
//    }
//    if (state & I2C_SR1_ARLO) {
//        DEBUG("ARLO\n");
//    }
//    if (state & I2C_SR1_BERR) {
//        DEBUG("BERR\n");
//    }
//    if (state & I2C_SR1_PECERR) {
//        DEBUG("PECERR\n");
//    }
//    if (state & I2C_SR1_TIMEOUT) {
//        DEBUG("TIMEOUT\n");
//    }
//    if (state & I2C_SR1_SMBALERT) {
//        DEBUG("SMBALERT\n");
//    }
//    while (1) {}
//}
//#endif /* I2C_1_EN */

#endif /* I2C_NUMOF */
