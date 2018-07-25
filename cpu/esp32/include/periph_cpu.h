/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       CPU specific definitions and functions for peripheral handling
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <stdint.h>

/* we first include board.h since board-specific configurations can override
   the default CPU peripheral configurations and have precedence over them */
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Power management configuration
 * @{
 */
#define PROVIDES_PM_SET_LOWEST
#define PROVIDES_PM_RESTART
#define PROVIDES_PM_OFF
/** @} */

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (7U)

/**
 * @brief   Available ports on the ESP32
 * @{
 */
#define PORT_GPIO 0       /**< port GPIO */
/** @} */

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF (0xff)

/**
 * @brief   Define CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)  ((x << 4) | y)

/**
 * @brief   Define CPU specific number of GPIO pins
 * @{
 */
#define GPIO_PIN_NUMOF  40
#ifndef GPIO_PIN_COUNT
#define GPIO_PIN_COUNT  GPIO_PIN_NUMOF
#endif
/** @} */

/**
 * @name   Predefined GPIO names
 * @{
 */
#define GPIO0       (GPIO_PIN(PORT_GPIO,0))
#define GPIO1       (GPIO_PIN(PORT_GPIO,1))
#define GPIO2       (GPIO_PIN(PORT_GPIO,2))
#define GPIO3       (GPIO_PIN(PORT_GPIO,3))
#define GPIO4       (GPIO_PIN(PORT_GPIO,4))
#define GPIO5       (GPIO_PIN(PORT_GPIO,5))
#define GPIO6       (GPIO_PIN(PORT_GPIO,6))
#define GPIO7       (GPIO_PIN(PORT_GPIO,7))
#define GPIO8       (GPIO_PIN(PORT_GPIO,8))
#define GPIO9       (GPIO_PIN(PORT_GPIO,9))
#define GPIO10      (GPIO_PIN(PORT_GPIO,10))
#define GPIO11      (GPIO_PIN(PORT_GPIO,11))
#define GPIO12      (GPIO_PIN(PORT_GPIO,12))
#define GPIO13      (GPIO_PIN(PORT_GPIO,13))
#define GPIO14      (GPIO_PIN(PORT_GPIO,14))
#define GPIO15      (GPIO_PIN(PORT_GPIO,15))
#define GPIO16      (GPIO_PIN(PORT_GPIO,16))
#define GPIO17      (GPIO_PIN(PORT_GPIO,17))
#define GPIO18      (GPIO_PIN(PORT_GPIO,18))
#define GPIO19      (GPIO_PIN(PORT_GPIO,19))
/* GPIO 20 is not available */
#define GPIO21      (GPIO_PIN(PORT_GPIO,21))
#define GPIO22      (GPIO_PIN(PORT_GPIO,22))
#define GPIO23      (GPIO_PIN(PORT_GPIO,23))
/* GPIO 24 is not available */
#define GPIO25      (GPIO_PIN(PORT_GPIO,25))
#define GPIO26      (GPIO_PIN(PORT_GPIO,26))
#define GPIO27      (GPIO_PIN(PORT_GPIO,27))
/* GPIOs 28 ...32 are not available */
#define GPIO32      (GPIO_PIN(PORT_GPIO,32))
#define GPIO33      (GPIO_PIN(PORT_GPIO,33))
/* GPIOs 34 ... 39 can only be used as inputs and do not have pullups/pulldowns */
#define GPIO34      (GPIO_PIN(PORT_GPIO,34))
#define GPIO35      (GPIO_PIN(PORT_GPIO,35))
#define GPIO36      (GPIO_PIN(PORT_GPIO,36))
#define GPIO37      (GPIO_PIN(PORT_GPIO,37))
#define GPIO38      (GPIO_PIN(PORT_GPIO,38))
#define GPIO39      (GPIO_PIN(PORT_GPIO,39))
/** @} */

/**
 * @brief   Override mode flank selection values
 *
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_NONE    = 0,
    GPIO_RISING  = 1,        /**< emit interrupt on rising flank  */
    GPIO_FALLING = 2,        /**< emit interrupt on falling flank */
    GPIO_BOTH    = 3,        /**< emit interrupt on both flanks   */
    GPIO_LOW     = 4,        /**< emit interrupt on low level     */
    GPIO_HIGH    = 5         /**< emit interrupt on low level     */
} gpio_flank_t;

/** @} */

/**
 * @brief   Override GPIO modes
 *
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN,        /**< input */
    GPIO_IN_PD,     /**< input with pull-down */
    GPIO_IN_PU,     /**< input with pull-up */
    GPIO_OUT,       /**< output */
    GPIO_OD,        /**< open-drain output */
    GPIO_OD_PU,     /**< open-drain output with pull-up */
    GPIO_IN_OUT,    /**< input and output */
    GPIO_IN_OD,     /**< input and open-drain output */
    GPIO_IN_OD_PU   /**< input and open-drain output */
} gpio_mode_t;
/** @} */

/**
 * @name   ADC configuration
 *
 * ESP32 integrates two 12-bit ADCs (ADC1 and ADC2) capable of measuring up to
 * 18 analog signals in total. Most of these ADC channels are either connected
 * to a number of intergrated sensors like a Hall sensors, touch sensors and a
 * temperature sensor or can be connected with certain GPIOs. Integrated sensors
 * are disabled in RIOT's implementation and are not accessible. Thus, up to 18
 * GPIOs, the RTC GPIOs, can be used as ADC inputs:
 *
 * - ADC1 supports 8 channels: GPIO 32-39
 * - ADC2 supports 10 channels: GPIO 0, 2, 4, 12-15, 25-27
 *
 * Please note: GPIO37 and GPIO38 are normally not broken out on ESP32
 * modules and are therefore not usable.
 *
 * @{
 */
/**
 * @brief   Possible ADC resolution settings
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_9BIT = 0,       /**< ADC resolution: 9 bit */
    ADC_RES_10BIT,          /**< ADC resolution: 10 bit */
    ADC_RES_11BIT,          /**< ADC resolution: 11 bit */
    ADC_RES_12BIT,          /**< ADC resolution: 12 bit */
} adc_res_t;

/** @} */

/**
 * @brief  Number of ADC cahnnels that could be used at maximum.
 */
#define ADC_NUMOF_MAX   16

/**
 * @brief  Declaration of GPIOs that can be used as ADC channels
 *
 * ADC_GPIOS declares the GPIOs that could be used as ADC channels by
 * the application. The order of the listed GPIOs determines the mapping
 * between the RIOT ADC lines and the GPIOs. The definition of ADC_NUMOF is
 * derived automatically from ADC_GPIOS and must not be changed.
 *
 * ADC_GPIOS can be defined in *board.h* to override the default configuration
 * with a board-specific definition.
 *
 * PLEASE NOTE: ADC_GPIOS must not be empty. If no ADC pins are to be used,
 * just omit the ADC_GPIOS declaration and declare ADC_GPIOS_NOT_AVAILABLE
 * instead.
 *
 * The following standard declaration of ADC_GPIOS contains all GPIOs that
 * might be used as ADC channels. Some of the GPIOs (e.g. GPIO0, GPIO2,
 * GPIO15, GPIO33, GPIO36) have special meanings on some boards and cannot
 * be used freely. GPIO37 and GPIO38 are normally not broken out on ESP32
 * modules. Override ADC_GPIOS for ADC channels according to your board
 * configuration.
 *
 * PLEASE NOTE: As long as the GPIOs listed in ADC_GPIOS are not initialized
 * as ADC channels with the *adc_init* function, they can be used for all
 * other uses of GPIOs.
 */
#if !defined(ADC_GPIOS) && !defined(ADC_GPIOS_NOT_AVAILABLE)
#define ADC_GPIOS   { GPIO0 , GPIO2 , GPIO4 , GPIO12, GPIO13, GPIO14, \
                      GPIO15, GPIO25, GPIO26, GPIO27, GPIO32, GPIO33, \
                      GPIO34, GPIO35, GPIO36, GPIO39 }
#endif

/** Map of RIOT ADC lines to GPIOs */
#ifdef ADC_GPIOS
static const uint32_t ADC_PINS[] = ADC_GPIOS;
#endif

/**
 * @brief  Number of ADC intputs determined from GPIOs declared in ADC_GPIOS
 * This define must not be changed.
 */
#ifdef ADC_GPIOS
#define ADC_NUMOF   (sizeof(ADC_PINS)/sizeof(uint32_t))
#else
#define ADC_NUMOF   (0)
#endif
/** @} */

/**
 * @name   DAC configuration
 *
 * ESP32 supports 2 DAC lines at GPIO25 and GPIO26. These DACs have a width of
 * 8 bits and produce voltages in the range from 0 V to 3.3 V (VDD_A). The 16
 * bits DAC values given as parameter of function *dac_set* are down-scaled
 * to 8 bit.
 *
 * @{
 */

#if !defined(DAC_GPIOS) && !defined(DAC_GPIOS_NOT_AVAILABLE)
/**
 * @brief  Declaration of GPIOs that can be used as DAC channels
 *
 * DAC_GPIOS declares the GPIOs that could be used as DAC channels by
 * the application. The order of the listed GPIOs determines the mapping
 * between the RIOT DAC lines and the GPIOs. The definition of DAC_NUMOF is
 * derived automatically from DAC_GPIOs and must not be changed.
 *
 * DAC_GPIOS can be defined in *board.h* to override the default configuration
 * with a board-specific definition.
 *
 * PLEASE NOTE: DAC_GPIOS must not be empty. If no DAC pins are to be used,
 * just omit the DAC_GPIOS declaration and declare DAC_GPIOS_NOT_AVAILABLE.
 *
 * The following standard declaration of DAC_GPIOS contains all GPIOs that can
 * be used as DAC channels.
 *
 * PLEASE NOTE: As long as the GPIOs listed in DACC_GPIOS are not initialized
 * as DAC channels with the *dac_init* function, they can be used for all
 * other uses of GPIOs.
 */
#define DAC_GPIOS   { GPIO25, GPIO26 }
#endif

/** Map of RIOT DAC lines to GPIO */
#ifdef DAC_GPIOS
static const uint32_t DAC_PINS[] = DAC_GPIOS;
#endif

/**
 * @brief  Number of DAC intputs determined from GPIOs declared in ADC_GPIOS
 * This define must not be changed.
 */
#ifdef DAC_GPIOS
#define DAC_NUMOF   (sizeof(DAC_PINS)/sizeof(uint32_t))
#else
#define DAC_NUMOF   (0)
#endif
/** @} */

/**
 * @name   I2C configuration
 *
 * ESP32 has two built-in I2C interfaces. The configuration determines
 * which GPIO pins are used for these interfacese. The default configuration
 * below can be overriden in *board.h* with a board-specific definition. If
 * a board does not provide I2C interfaces define I2C0_NOT_AVAILABLE and
 * I2C1_NOT_AVAILABLE, respectively in board definition.
 * @{
 */
#if !defined(I2C0_NOT_AVAILABLE) && !defined(I2C0_SCL)
#define I2C0_SCL   GPIO22
#define I2C0_SDA   GPIO21
#endif

/**
 * @brief   Number of I2C devices determined from IC2 pin declarations.
 */
#if defined(I2C0_SCL) && defined(I2C1_SCL)
#define I2C_NUMOF   2
#elif defined(I2C0_SCL) || defined(I2C1_SCL)
#define I2C_NUMOF   1
#else
#define I2C_NUMOF   0
#endif

/** @} */

/**
 * @name   PWM configuration
 * @{
 *
 * PWM implementation uses ESP32's high-speed MCPWM modules. ESP32 has 2 such
 * modules, each with up to 6 channels. Thus, the maximum number of PWM
 * devices is 2 and the maximum total number of PWM channels is 12.
 */

/**
 * @brief   Maximum number of channels per PWM device.
 */
#define PWM_CHANNEL_NUM_DEV_MAX (6)

/**
 * @brief   Declaration of GPIOs that can be used as PWM channels.
 *
 * The defines PWM0_GPIOS and PWM1_GPIOS declare which GPIOs are used as PWM
 * channels by PWM_DEV(0) and PWM_DEV(1), respectively. As long as the
 * respective PWM device is not initialized with the *pwm_init* function these
 * GPIOs can be used for other purposes.
 *
 * The default configuration below just serves as an example showing the 2 PWM
 * devices PWM0 and PWM1. This configuration should be overridden by the board
 * definition in file *board.h*.
 *
 * PLEASE NOTE: The definition of PWM0_GPIOS and PWM1_GPIOS can be omitted or
 * empty. In the latter case, they must at least contain the curly braces.
 * The corresponding PWM device can not be used in this case.
 *
 * PLEASE NOTE: As long as a PWM device has not been initialized with the
 * pwm_init* function, the GPIOs declared as PWM channels for this device can
 * be used for all other uses of GPIOs.
 */
#if !defined(PWM0_GPIOS) && !defined(PWM0_GPIOS_NOT_AVAILABLE)
#define PWM0_GPIOS { GPIO25, GPIO26, GPIO27 }
#endif

#if !defined(PWM1_GPIOS) && !defined(PWM1_GPIOS_NOT_AVAILABLE)
#define PWM1_GPIOS { GPIO17, GPIO18 }
#endif

/**
 * @brief   Number of PWM devices determined from PWM0_GPIOS and PWM1_GPIOS.
 */
#if defined(PWM0_GPIOS) && defined(PWM1_GPIOS)
#define PWM_NUMOF (2)
#elif defined(PWM0_GPIOS) || defined(PWM1_GPIOS)
#define PWM_NUMOF (1)
#else
#define PWM_NUMOF (0)
#endif
/** @} */

/**
 * @name   SPI configuration
 *
 * ESP32 has four SPI controllers:
 *
 * - controller SPI0 is reserved for accessing flash memory
 * - controller SPI1 realizes interface FSPI which shares its signals with SPI0
 * - controller SPI2 realizes interface HSPI that can be used for peripherals
 * - controller SPI3 realizes interface VSPI that can be used for peripherals
 *
 * Thus, at most three interfaces can be used:
 *
 * - VSPI: realized by controller SPI3 and mapped to SPI_DEV(0)
 * - HSPI: realized by controller SPI2 and mapped to SPI_DEV(1)
 * - FSPI: realized by controller SPI1 and mapped to SPI_DEV(2).
 *
 * All SPI interfaces could be used in quad SPI mode, but RIOT's low level
 * device driver doesn't support it.
 *
 * PLEASE NOTE:
 * - Since the interface SPI_DEV(2) shares its bus signals with the controller
 *   that implements the flash memory interface, we use the name FSPI for this
 *   interface. In the technical reference, this interface is misleadingly
 *   simply referred to as SPI.
 * - Since the FSPI interface SPI_DEV(2) shares its bus signals with flash
 *   memory interface and optionally other external memories, you can only use
 *   SPI_DEV(2) to attach external memory with same SPI mode and same bus speed
 *   but with a different CS.
 * - Using SPI_DEV(2) for anything else can disturb flash memory access which
 *   causes a number of problems. If not really necessary, you should not use
 *   this interface.
 *
 * Predefined SPI pin configurations below correspond to their direct I/O
 * pin configuration, but can be changed.
 * @{
 */
#define SPI_DEV(x)  (x)
#define SPI_NUMOF   3

/* pin configuration for SPI_DEV(0), the VSPI interface, can be changed */
#if defined(SPI0_NOT_AVAILABLE)
#define SPI0_SCK    GPIO_UNDEF
#define SPI0_MISO   GPIO_UNDEF
#define SPI0_MOSI   GPIO_UNDEF
#define SPI0_CS0    GPIO_UNDEF
#elif !defined(SPI0_SCK) && !defined(SPI0_MISO) && !defined(SPI0_MOSI) && !defined(SPI0_CS0)
#define SPI0_SCK    GPIO18  /* direct I/O pin VSPICLK */
#define SPI0_MISO   GPIO19  /* direct I/O pin VSPIQ */
#define SPI0_MOSI   GPIO23  /* direct I/O pin VSPID */
#define SPI0_CS0    GPIO5   /* direct I/O pin VSPICS0 */
#endif

/* pin configuration for SPI_DEV(1), the HSPI interface, can be changed */
#if defined(SPI1_NOT_AVAILABLE)
#define SPI1_SCK    GPIO_UNDEF
#define SPI1_MISO   GPIO_UNDEF
#define SPI1_MOSI   GPIO_UNDEF
#define SPI1_CS0    GPIO_UNDEF
#elif !defined(SPI1_SCK) && !defined(SPI1_MISO) && !defined(SPI1_MOSI) && !defined(SPI1_CS0)
#define SPI1_SCK    GPIO14  /* direct I/O pin HSPICLK */
#define SPI1_MISO   GPIO12  /* direct I/O pin HSPIQ */
#define SPI1_MOSI   GPIO13  /* direct I/O pin HSPID */
#define SPI1_CS0    GPIO15  /* direct I/O pin HSPICS0 */
#endif

/* pin configuration for SPI_DEV(2) (the FSPI interface), CANNOT be changed
   PLEASE NOTE: Be careful when you use this interface. Please use another CS
   signal in your application since GPIO11 is used to select the built-in flash
   memory. */
#if !defined(SPI2_SCK) && !defined(SPI2_MISO) && !defined(SPI2_MOSI) && !defined(SPI2_CS0)
#define SPI2_SCK    GPIO6   /* direct I/O pin FSPICLK (SD_CLK    / CLK) */
#define SPI2_MISO   GPIO7   /* direct I/O pin FSPIQ   (SD_DATA_0 / SD0) */
#define SPI2_MOSI   GPIO8   /* direct I/O pin FSPID   (SD_DATA_1 / SD1) */
#define SPI2_CS0    GPIO11  /* direct I/O pin FSPICS0 (SD_CMD    / CMD) */
#if defined(FLASH_MODE_QIO) || defined(FLASH_MODE_QOUT)
/* two additional pins are required in quad SPI flash modes */
#define SPI2_HD     GPIO9   /* direct I/O pin FSPICS0 (SD_DATA_2 / SD2) */
#define SPI2_WP     GPIO10  /* direct I/O pin FSPICS0 (SD_DATA_3 / SD3) */
#endif
#endif

#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

#if defined(MODULE_MTD) || defined(DOXYGEN)
#include "mtd.h"
/**
 * @name   SPIFFS configuration
 *
 * Using the MTD implementation with the SPIFFS module, the SPI built-in
 * flash memory can be used as the SPIFFS drive. The only configurable part
 * in board definitions is the declaration of the system MTD device and its
 * name.
 *
 * @{
 */

#ifndef MTD_0
/** Declare the system MTD device name. */
#define MTD_0 mtd0

/** Pointer to the system MTD device. */
extern mtd_dev_t *mtd0;
#endif /* MTD_0 */
/** @} */
#endif /* defined(MODULE_MTD) || defined(DOXYGEN) */

#if defined(MODULE_SPIFFS) && !defined(DOXYGEN)
#define SPIFFS_ALIGNED_OBJECT_INDEX_TABLES 1
#define SPIFFS_READ_ONLY 0
#define SPIFFS_SINGLETON 0
#define SPIFFS_HAL_CALLBACK_EXTRA 1
#define SPIFFS_CACHE 1

#if SPIFFS_SINGLETON == 1
#define SPIFFS_CFG_PHYS_SZ(ignore)        (0x70000)
#define SPIFFS_CFG_PHYS_ERASE_SZ(ignore)  (4096)
#define SPIFFS_CFG_PHYS_ADDR(ignore)      (0)
#define SPIFFS_CFG_LOG_PAGE_SZ(ignore)    (256)
#define SPIFFS_CFG_LOG_BLOCK_SZ(ignore)   (65536)
#endif /* SPIFFS_SINGLETON */
#endif /* defined(MODULE_SPIFFS) && !defined(DOXYGEN) */

#if SPIFFS_HAL_CALLBACK_EXTRA == 0
/** Declare the SPIFFS MTD device name. */
#define SPIFFS_MTD_DEV (MTD_0)
#endif

/**
 * @name   Timer configuration depenend on which implementation is used
 * @{
 */
#ifdef HW_COUNTER_USED
/** hardware ccount/ccompare registers are used for timer implementation */
#define TIMER_NUMOF     (2)
#define TIMER_CHANNELS  (1)
#else
/** hardware timer modules are used for timer implementation (default) */
#define TIMER_NUMOF     (3)
#define TIMER_CHANNELS  (1)
#endif

/** Timer used for system time */
#define TIMER_SYSTEM    TIMERG0.hw_timer[0]

/** @} */

/**
 * @name   UART configuration
 *
 * ESP32 supports up to three UART devices UART_DEV(0) pin configuration is
 * fix. All ESP32 boards use it as standard configuration.
 *
 * UART_DEV(0).TXD      GPIO1
 * UART_DEV(0).RXD      GPIO3
 *
 * The pin configuration of UART_DEV(1) and UART_DEV(2) can be changed.
 * If the according UART interfaces are not to be used, declare
 * UART1_NOT_AVAILABLE and/or UART2_NOT_AVAILABLE instead.
 *
 * @{
 */

#if !defined(UART1_NOT_AVAILABLE) && !defined(UART1_TXD) && !defined(UART1_RXD)
#define UART1_TXD   GPIO10 /* direct I/O pin for UART_DEV(1), can be changed */
#define UART1_RXD   GPIO9  /* direct I/O pin for UART_DEV(1), can be changed */
#endif

#if !defined(UART2_NOT_AVAILABLE) && !defined(UART2_TXD) && !defined(UART2_RXD)
#define UART2_TXD   GPIO17 /* direct I/O pin for UART_DEV(2), can be changed */
#define UART2_RXD   GPIO16 /* direct I/O pin for UART_DEV(2), can be changed */
#endif

#if !defined(UART1_NOT_AVAILABLE) && !defined(UART2_NOT_AVAILABLE)
/** Number of UART interfaces */
#define UART_NUMOF  3
#elif defined(UART1_NOT_AVAILABLE) && defined(UART2_NOT_AVAILABLE)
#define UART_NUMOF  1
#else
#define UART_NUMOF  2
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
