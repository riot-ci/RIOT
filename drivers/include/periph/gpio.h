/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef PERIPH_GPIO_H
#define PERIPH_GPIO_H

#ifndef MODULE_PERIPH_GPIO_EXT

/* PLEASE NOTE:
 * The part of `gpio.h` in `#ifndef MODULE_PERIPH_GPIO_EXT ... #else` is the
 * legacy GPIO. It will be removed from the file, once all platforms implement
 * the new GPIO API which allows to implement the API for any kind of
 * GPIO hardware.
 */

/**
 * @defgroup    drivers_periph_gpio GPIO
 * @ingroup     drivers_periph
 * @brief       Low-level GPIO peripheral driver
 *
 * This is a basic GPIO (General-purpose input/output) interface to allow
 * platform independent access to a MCU's input/output pins. This interface is
 * intentionally designed to be as simple as possible, to allow for easy
 * implementation and maximum portability.
 *
 * The interface provides capabilities to initialize a pin as output-,
 * input- and interrupt pin. With the API you can basically set/clear/toggle the
 * digital signal at the hardware pin when in output mode. Configured as input you can
 * read a digital value that is being applied to the pin externally. When initializing
 * an external interrupt pin, you can register a callback function that is executed
 * in interrupt context once the interrupt condition applies to the pin. Usually you
 * can react to rising or falling signal flanks (or both).
 *
 * In addition the API provides to set standard input/output circuit modes such as
 * e.g. internal push-pull configurations.
 *
 * All modern micro controllers organize their GPIOs in some form of ports,
 * often named 'PA', 'PB', 'PC'..., or 'P0', 'P1', 'P2'..., or similar. Each of
 * these ports is then assigned a number of pins, often 8, 16, or 32. A hardware
 * pin can thus be described by its port/pin tuple. To access a pin, the
 * @p GPIO_PIN(port, pin) macro should be used. For example: If your platform has
 * a pin PB22, it will be port=1 and pin=22. The @p GPIO_PIN macro should be
 * overridden by a MCU, to allow for efficient encoding of the the port/pin tuple.
 * For example, on many platforms it is possible to `OR` the pin number with the
 * corresponding ports base register address. This allows for efficient decoding
 * of pin number and base address without the need of any address lookup.
 *
 * In case the driver does not define it, the below macro definition is used to
 * simply map the port/pin tuple to the pin value. In that case, predefined GPIO
 * definitions in `RIOT/boards/ * /include/periph_conf.h` will define the selected
 * GPIO pin.
 *
 * @warning The scalar GPIO pin type `gpio_t` is deprecated and will be
 * replaced by a structured GPIO pin type in a future GPIO API. Therefore,
 * don't use the direct comparison of GPIO pins anymore. Instead, use the
 * inline comparison functions @ref gpio_is_equal and @ref gpio_is_undef.
 * 
 * # (Low-) Power Implications
 *
 * On almost all platforms, we can only control the peripheral power state of
 * full ports (i.e. groups of pins), but not for single GPIO pins. Together with
 * CPU specific alternate function handling for pins used by other peripheral
 * drivers, this can make it quite complex to keep track of pins that are
 * currently used at a certain moment. To simplify the implementations (and ease
 * the memory consumption), we expect ports to be powered on (e.g. through
 * peripheral clock gating) when first used and never be powered off again.
 *
 * GPIO driver implementations **should** power on the corresponding port during
 * gpio_init() and gpio_init_int().
 *
 * For external interrupts to work, some platforms may need to block certain
 * power modes (although this is not very likely). This should be done during
 * gpio_init_int().
 *
 * @{
 * @file
 * @brief       Low-level GPIO peripheral driver interface definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <limits.h>

#include "periph_cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_GPIO_T
/**
 * @brief   GPIO type identifier
 */
typedef unsigned int gpio_t;
#endif

#ifndef GPIO_PIN
/**
 * @brief   Convert (port, pin) tuple to @c gpio_t value
 */
/* Default GPIO macro maps port-pin tuples to the pin value */
#define GPIO_PIN(x,y)       ((gpio_t)((x & 0) | y))
#endif

#ifndef GPIO_UNDEF
/**
 * @brief   GPIO pin not defined
 */
#define GPIO_UNDEF          ((gpio_t)(UINT_MAX))
#endif

/**
 * @brief   Available pin modes
 *
 * Generally, a pin can be configured to be input or output. In output mode, a
 * pin can further be put into push-pull or open drain configuration. Though
 * this is supported by most platforms, this is not always the case, so driver
 * implementations may return an error code if a mode is not supported.
 */
#ifndef HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN ,               /**< configure as input without pull resistor */
    GPIO_IN_PD,             /**< configure as input with pull-down resistor */
    GPIO_IN_PU,             /**< configure as input with pull-up resistor */
    GPIO_OUT,               /**< configure as output in push-pull mode */
    GPIO_OD,                /**< configure as output in open-drain mode without
                             *   pull resistor */
    GPIO_OD_PU              /**< configure as output in open-drain mode with
                             *   pull resistor enabled */
} gpio_mode_t;
#endif

/**
 * @brief   Definition of possible active flanks for external interrupt mode
 */
#ifndef HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING = 0,       /**< emit interrupt on falling flank */
    GPIO_RISING = 1,        /**< emit interrupt on rising flank */
    GPIO_BOTH = 2           /**< emit interrupt on both flanks */
} gpio_flank_t;
#endif

/**
 * @brief   Signature of event callback functions triggered from interrupts
 *
 * @param[in] arg       optional context for the callback
 */
typedef void (*gpio_cb_t)(void *arg);

/**
 * @brief   Default interrupt context for GPIO pins
 */
#ifndef HAVE_GPIO_ISR_CTX_T
typedef struct {
    gpio_cb_t cb;           /**< interrupt callback */
    void *arg;              /**< optional argument */
} gpio_isr_ctx_t;
#endif

/**
 * @brief   Initialize the given pin as general purpose input or output
 *
 * When configured as output, the pin state after initialization is undefined.
 * The output pin's state **should** be untouched during the initialization.
 * This behavior can however **not be guaranteed** by every platform.
 *
 * @param[in] pin       pin to initialize
 * @param[in] mode      mode of the pin, see @c gpio_mode_t
 *
 * @return              0 on success
 * @return              -1 on error
 */
int gpio_init(gpio_t pin, gpio_mode_t mode);

#if defined(MODULE_PERIPH_GPIO_IRQ) || defined(DOXYGEN)
/**
 * @brief   Initialize a GPIO pin for external interrupt usage
 *
 * The registered callback function will be called in interrupt context every
 * time the defined flank(s) are detected.
 *
 * The interrupt is activated automatically after the initialization.
 *
 * @note    You have to add the module `periph_gpio_irq` to your project to
 *          enable this function
 *
 * @param[in] pin       pin to initialize
 * @param[in] mode      mode of the pin, see @c gpio_mode_t
 * @param[in] flank     define the active flank(s)
 * @param[in] cb        callback that is called from interrupt context
 * @param[in] arg       optional argument passed to the callback
 *
 * @return              0 on success
 * @return              -1 on error
 */
int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg);

/**
 * @brief   Enable pin interrupt if configured as interrupt source
 *
 * @note    You have to add the module `periph_gpio_irq` to your project to
 *          enable this function
 *
 * @param[in] pin       the pin to enable the interrupt for
 */
void gpio_irq_enable(gpio_t pin);

/**
 * @brief   Disable the pin interrupt if configured as interrupt source
 *
 * @note    You have to add the module `periph_gpio_irq` to your project to
 *          enable this function
 *
 * @param[in] pin       the pin to disable the interrupt for
 */
void gpio_irq_disable(gpio_t pin);

#endif /* defined(MODULE_PERIPH_GPIO_IRQ) || defined(DOXYGEN) */

/**
 * @brief   Get the current value of the given pin
 *
 * @param[in] pin       the pin to read
 *
 * @return              0 when pin is LOW
 * @return              >0 for HIGH
 */
int gpio_read(gpio_t pin);

/**
 * @brief   Set the given pin to HIGH
 *
 * @param[in] pin       the pin to set
 */
void gpio_set(gpio_t pin);

/**
 * @brief   Set the given pin to LOW
 *
 * @param[in] pin       the pin to clear
 */
void gpio_clear(gpio_t pin);

/**
 * @brief   Toggle the value of the given pin
 *
 * @param[in] pin       the pin to toggle
 */
void gpio_toggle(gpio_t pin);

/**
 * @brief   Set the given pin to the given value
 *
 * @param[in] pin       the pin to set
 * @param[in] value     value to set the pin to, 0 for LOW, HIGH otherwise
 */
void gpio_write(gpio_t pin, int value);

/**
 * @brief   Test if a GPIO pin is equal to another GPIO pin
 *
 * @param[in] gpio1 First GPIO pin to check
 * @param[in] gpio2 Second GPIO pin to check
 */
static inline int gpio_is_equal(gpio_t gpio1, gpio_t gpio2)
{
    return (gpio1 == gpio2);
}

/**
 * @brief   Test if a GPIO pin is undefined
 *
 * @param[in] gpio GPIO pin to check
 */
static inline int gpio_is_undef(gpio_t gpio)
{
    return (gpio == GPIO_UNDEF);
}

#ifdef __cplusplus
}
#endif

/** @} */

#else /* MODULE_PERIPH_GPIO_EXT */

/* PLEASE NOTE:
 * The part of `gpio.h` in `#ifndef MODULE_PERIPH_GPIO_EXT ... #else` is the
 * legacy GPIO. It will be removed from the file, once all platforms implement
 * the following GPIO API which allows to implement the API for any kind of
 * GPIO hardware.
 */

/**
 * @defgroup    drivers_periph_gpio_ext GPIO
 * @ingroup     drivers_periph
 * @brief       GPIO peripheral driver
 *
 * This is a basic GPIO (General-purpose input/output) interface to allow
 * platform-independent access to the input/output pins provided by the MCU
 * or any other hardware component such as GPIO expanders. This interface is
 * intentionally designed to be as simple as possible, to allow for easy
 * implementation and maximum portability.
 *
 * The interface provides capabilities to initialize a pin as output-,
 * input- and interrupt pin. With the API you can basically set/clear/toggle
 * the digital signal at the hardware pin when in output mode. Configured as
 * input you can read a digital value that is being applied to the pin
 * externally. When initializing an external interrupt pin, you can register
 * a callback function that is executed in interrupt context once the interrupt
 * condition applies to the pin. Usually you can react to rising or falling
 * signal flanks (or both).
 *
 * In addition the API provides to set standard input/output circuit modes
 * such as e.g. internal push-pull configurations.
 *
 * All modern micro controllers organize their GPIOs in some form of ports,
 * often named 'PA', 'PB', 'PC'..., or 'P0', 'P1', 'P2'..., or similar. Each
 * of these ports is then assigned a number of pins, often 8, 16, or 32. A
 * hardware pin can thus be described by its port/pin tuple. To access a pin,
 * the @ref GPIO_PIN(port, pin) macro should be used. For example: If your
 * platform has a pin PB22, it will be port=1 and pin=22.
 *
 * ## (Low-) Power Implications
 *
 * On almost all platforms, we can only control the peripheral power state of
 * full ports (i.e. groups of pins), but not for single GPIO pins. Together with
 * CPU specific alternate function handling for pins used by other peripheral
 * drivers, this can make it quite complex to keep track of pins that are
 * currently used at a certain moment. To simplify the implementations (and ease
 * the memory consumption), we expect ports to be powered on (e.g. through
 * peripheral clock gating) when first used and never be powered off again.
 *
 * GPIO driver implementations **should** power on the corresponding port during
 * gpio_init() and gpio_init_int().
 *
 * For external interrupts to work, some platforms may need to block certain
 * power modes (although this is not very likely). This should be done during
 * gpio_init_int().
 *
 * ## Implementation
 *
 * The interface is divided into a low-level API and a high-level API.
 *
 * The low-level API provides functions for port-oriented access to the GPIOs
 * and has to be implemented by any hardware component that provides GPIOs. The
 * functions of the low-level API are used via a driver of type gpio_driver_t.
 * This driver defines the interfaces of the low-level functions and contains
 * references to these functions of the respective hardware component. The
 * low-level API must be implemented by the MCU in the `gpio_cpu_*` functions.
 * These functions are used for the @ref gpio_cpu_driver driver for the access
 * to GPIO ports of the MCU.
 *
 * The high-level API is used by the application and provides pin-oriented
 * access to the GPIO pins.  It uses the functions of the low-level API for
 * this purpose.
 *
 * @{
 * @file
 * @brief       Low-level GPIO peripheral driver interface definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <limits.h>

#include "gpio_arch.h"  /* include architecture specific GPIO definitions */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_GPIO_PIN_T
/**
 * @brief   GPIO pin number type
 */
typedef unsigned int gpio_pin_t;
#endif

/**
 * @brief   Register address type for GPIO ports of the MCU
 *
 * The size of this type must match the size of a pointer to distinguish
 * between MCU GPIO register addresses and pointers on GPIO devices.
 */
#ifndef HAVE_GPIO_REG_T
typedef uint32_t gpio_reg_t;
#endif

/**
 * @brief   GPIO mask type that corresponds to the supported GPIO port width
 *
 * This type is used to mask the pins of a GPIO port in various low-level GPIO
 * functions. Its size must therefore be the maximum width of all different
 * GPIO ports used in the system. For this purpose, each component that
 * provides GPIO ports must activate the corresponding pseudo module that
 * specifies the width of its GPIO ports.
 */
#if defined(MODULE_GPIO_MASK_32BIT)
typedef uint32_t gpio_mask_t;
#elif defined(MODULE_GPIO_MASK_16BIT)
typedef uint16_t gpio_mask_t;
#else
typedef uint8_t gpio_mask_t;
#endif

/**
 * @brief   Convert (port, pin) tuple to @ref gpio_t structure
 */
#ifndef GPIO_PIN
#define GPIO_PIN(x,y)       ((gpio_t){ .port = &gpio_ports[x], .pin = y })
#endif

/**
 * @brief   GPIO pin not defined
 */
#ifndef GPIO_PIN_UNDEF
#define GPIO_PIN_UNDEF      ((gpio_pin_t)(UINT_MAX))
#endif

/**
 * @brief   GPIO not defined
 */
#ifndef GPIO_UNDEF
#define GPIO_UNDEF          ((gpio_t){ .port = NULL, .pin = GPIO_PIN_UNDEF })
#endif

/**
 * @brief   Available GPIO modes
 *
 * Generally, a GPIO can be configured to be input or output. In output mode, a
 * pin can further be put into push-pull or open drain configuration. Though
 * this is supported by most platforms, this is not always the case, so driver
 * implementations may return an error code if a mode is not supported.
 */
#ifndef HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN ,               /**< configure as input without pull resistor */
    GPIO_IN_PD,             /**< configure as input with pull-down resistor */
    GPIO_IN_PU,             /**< configure as input with pull-up resistor */
    GPIO_OUT,               /**< configure as output in push-pull mode */
    GPIO_OD,                /**< configure as output in open-drain mode without
                             *   pull resistor */
    GPIO_OD_PU              /**< configure as output in open-drain mode with
                             *   pull resistor enabled */
} gpio_mode_t;
#endif

/**
 * @brief   Definition of possible active flanks for external interrupt mode
 */
#ifndef HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING = 0,       /**< emit interrupt on falling flank */
    GPIO_RISING = 1,        /**< emit interrupt on rising flank */
    GPIO_BOTH = 2           /**< emit interrupt on both flanks */
} gpio_flank_t;
#endif

/**
 * @brief   Signature of event callback functions triggered from interrupts
 *
 * @param[in] arg       optional context for the callback
 */
typedef void (*gpio_cb_t)(void *arg);

/**
 * @brief   Default interrupt context for GPIO pins
 */
#ifndef HAVE_GPIO_ISR_CTX_T
typedef struct {
    gpio_cb_t cb;           /**< interrupt callback */
    void *arg;              /**< optional argument */
} gpio_isr_ctx_t;
#endif

/** forward declaration of GPIO port type */
typedef union gpio_port gpio_port_t;

/**
 * @brief   GPIO device driver type
 *
 * GPIO device drivers are used for port-oriented access to GPIO ports.
 * It defines the function prototypes of the low-level API and contains
 * the references to these functions implemented by a hardware component
 * that provides GPIO ports.
 */
typedef struct {
    /**
     * @brief   Initialize the given pin as general purpose input or output
     *
     * @param[in] port  port of the GPIO pin to initialize
     * @param[in] pin   GPIO pin to initialize
     * @param[in] mode  mode of the pin, see @ref gpio_mode_t
     *
     * @return    0 on success
     * @return    -1 on error
     *
     * @see @ref #gpio_init
     */
    int (*init)(const gpio_port_t *port, gpio_pin_t pin, gpio_mode_t mode);

#ifdef MODULE_PERIPH_GPIO_IRQ

    /**
     * @brief   Initialize a GPIO pin for external interrupt usage
     *
     * @param[in] port  port of the GPIO pin to initialize
     * @param[in] pin   GPIO pin to initialize
     * @param[in] mode  mode of the pin, see @ref gpio_mode_t
     * @param[in] flank define the active flank(s)
     * @param[in] cb    callback that is called from interrupt context
     * @param[in] arg   optional argument passed to the callback
     *
     * @return    0 on success
     * @return    -1 on error
     *
     * @see @ref #gpio_init_int
     */
    int (*init_int)(const gpio_port_t *port, gpio_pin_t pin, gpio_mode_t mode,
                    gpio_flank_t flank, gpio_cb_t cb, void *arg);

    /**
     * @brief   Enable GPIO pin interrupt if configured as interrupt source
     *
     * @param[in] port  port of the GPIO pin
     * @param[in] pin   GPIO pin
     *
     * @see @ref #gpio_irq_enable
     */
    void (*irq_enable)(const gpio_port_t *port, gpio_pin_t pin);

    /**
     * @brief   Disable GPIO pin interrupt if configured as interrupt source
     *
     * @param[in] port  port of the GPIO pin
     * @param[in] pin   GPIO pin
     *
     * @see @ref #gpio_irq_disable
     */
    void (*irq_disable)(const gpio_port_t *port, gpio_pin_t pin);

#endif /* MODULE_PERIPH_GPIO_IRQ */

    /**
     * @brief   Get current values of all pins of the given GPIO port
     *
     * @param[in] port  GPIO port
     *
     * @return    value of width @ref gpio_mask_t where the bit positions
     *            represent the current value of the according pin
     *            (0 when pin is LOW and >0 when pin is HIGH)
     */
    gpio_mask_t (*read)(const gpio_port_t *port);

    /**
     * @brief   Set the pins of a port defined by the pin mask to HIGH
     *
     * @param[in] port  GPIO port
     * @param[in] pins  mask of the pins to set
     *
     * @see @ref #gpio_set
     */
    void (*set)(const gpio_port_t *port, gpio_mask_t pins);

    /**
     * @brief   Set the pins of a port defined by the pin mask to LOW
     *
     * @param[in] port  GPIO port
     * @param[in] pins  mask of the pins to clear
     *
     * @see @ref #gpio_set
     */
    void (*clear)(const gpio_port_t *port, gpio_mask_t pins);

    /**
     * @brief   Toggle the value the pins of a port defined by the pin mask
     *
     * @param[in] port  GPIO port
     * @param[in] pins  mask of the pins to toggle
     *
     * @see @ref #gpio_set
     */
    void (*toggle)(const gpio_port_t *port, gpio_mask_t pins);

    /**
     * @brief   Set the values of all pins of the given GPIO port
     *
     * @param[in] port  GPIO port
     * @param[in] pins  values of the pins
     *
     * @see @ref #gpio_write
     */
    void (*write)(const gpio_port_t *port, gpio_mask_t values);

} gpio_driver_t;

/**
 * @brief   GPIO device type
 *
 * A GPIO device is a hardware component that provides a number of GPIO
 * pins, e.g. a GPIO expander. It is defined by a device descriptor that
 * contains the state and parameters of the device, as well as an associated
 * driver for using the device.
 *
 * @note The GPIO device type isn't used for MCU GPIO ports.
 */
typedef struct {
    void *dev;                      /**< device descriptor */
    const gpio_driver_t *driver;    /**< associated device driver */
} gpio_dev_t;

/**
 * @brief   GPIO port type
 *
 * A GPIO port allows the access to a certain number of GPIO pins. It is either
 *
 * - a register address in the case of MCU GPIO ports or
 * - a pointer to a device of type `gpio_dev_t` which provides a number
 *   of GPIO pins, e.g. a GPIO expander.
 */
typedef union gpio_port {
    const gpio_reg_t reg;   /**< register address of a MCU GPIO port */
    const gpio_dev_t* dev;  /**< pointer to a device that provides the GPIO port */
} gpio_port_t;

/**
 * @brief   GPIO pin type definition
 *
 * A GPIO pin is defined by a port that provides the access to the pin and
 * the pin number at this port.
 */
typedef struct {
    const gpio_port_t *port;   /**< pointer to the port */
    gpio_pin_t pin;            /**< pin number */
} gpio_t;

/**
 * @brief   GPIO device driver for MCU GPIO ports.
 *
 * The GPIO device driver @c gpio_cpu_driver contains the references to the
 * low-level functions of the MCU implementation for accessing GPIO pins
 * of the MCU GPIO ports.
 */
extern const gpio_driver_t gpio_cpu_driver;

/**
 * @brief   Table of existing MCU and GPIO expander device ports
 *
 * This table is created from `GPIO_CPU_PORTS`, as defined by the MCU, and
 * `GPIO_EXT_PORTS`, as defined in the GPIO expander configuration.
 */
extern const gpio_port_t gpio_ports[];

/**
 * @name    Low-level versions of the MCU GPIO functions
 *
 * The following functions define the prototypes for the low-level GPIO
 * functions that have to be implemented for MCU GPIO ports in
 * `cpu/.../periph/gpio.c`.
 *
 * These functions are used by high-level GPIO functions `gpio_*`.
 * They should only be called directly if several pins of a GPIO port are
 * to be changed simultaneously using the definition of GPIO pin masks.
 *
 * The GPIO device driver @ref gpio_cpu_driver contains references to these
 * low-level functions of the MCU implementation.
 *
 * @see See function prototypes in @ref gpio_driver_t for detailed information
 * about these functions.
 * @{
 */
int  gpio_cpu_init(const gpio_port_t *port, gpio_pin_t pin, gpio_mode_t mode);
int  gpio_cpu_init_int(const gpio_port_t *port, gpio_pin_t pin, gpio_mode_t mode,
                       gpio_flank_t flank, gpio_cb_t cb, void *arg);
void gpio_cpu_irq_enable(const gpio_port_t *port, gpio_pin_t pin);
void gpio_cpu_irq_disable(const gpio_port_t *port, gpio_pin_t pin);

gpio_mask_t gpio_cpu_read(const gpio_port_t *port);
void gpio_cpu_set(const gpio_port_t *port, gpio_mask_t pins);
void gpio_cpu_clear(const gpio_port_t *port, gpio_mask_t pins);
void gpio_cpu_toggle(const gpio_port_t *port, gpio_mask_t pins);
void gpio_cpu_write(const gpio_port_t *port, gpio_mask_t values);
/** @} */

/**
 * @brief    Get the driver for a GPIO port
 */
static inline const gpio_driver_t *gpio_driver_get(const gpio_port_t *port)
{
#if MODULE_EXTEND_GPIO
    if ((port->reg & GPIO_CPU_REG_MASK) == GPIO_CPU_REG_GPIO) {
        return &gpio_cpu_driver;
    }
    else {
        assert(port->dev);
        assert(port->dev->driver);
        return port->dev->driver;
    }
#else
    (void)port;
    return &gpio_cpu_driver;
#endif
}

/**
 * @brief   Initialize the given pin as general purpose input or output
 *
 * When configured as output, the pin state after initialization is undefined.
 * The output pin's state **should** be untouched during the initialization.
 * This behavior can however **not be guaranteed** by every platform.
 *
 * @param[in] gpio  GPIO pin to initialize
 * @param[in] mode  mode of the pin, see @ref gpio_mode_t
 *
 * @return    0 on success
 * @return    -1 on error
 */
static inline int gpio_init(gpio_t gpio, gpio_mode_t mode)
{
    const gpio_driver_t *driver = gpio_driver_get(gpio.port);
    return driver->init(gpio.port, gpio.pin, mode);
}

#if defined(MODULE_PERIPH_GPIO_IRQ) || defined(DOXYGEN)
/**
 * @brief   Initialize a GPIO pin for external interrupt usage
 *
 * The registered callback function will be called in interrupt context every
 * time the defined flank(s) are detected.
 *
 * The interrupt is activated automatically after the initialization.
 *
 * @note    You have to add the module `periph_gpio_irq` to your project to
 *          enable this function
 *
 * @param[in] gpio  GPIO pin to initialize
 * @param[in] mode  mode of the pin, see @ref gpio_mode_t
 * @param[in] flank define the active flank(s)
 * @param[in] cb    callback that is called from interrupt context
 * @param[in] arg   optional argument passed to the callback
 *
 * @return    0 on success
 * @return   -1 on error
 */
static inline int gpio_init_int(gpio_t gpio, gpio_mode_t mode, gpio_flank_t flank,
                                gpio_cb_t cb, void *arg)
{
    const gpio_driver_t *driver = gpio_driver_get(gpio.port);
    return driver->init_int(gpio.port, gpio.pin, mode, flank, cb, arg);
}

/**
 * @brief   Enable GPIO pin interrupt if configured as interrupt source
 *
 * @note    You have to add the module `periph_gpio_irq` to your project to
 *          enable this function
 *
 * @param[in] gpio  GPIO pin to enable the interrupt for
 */
static inline void gpio_irq_enable(gpio_t gpio)
{
    const gpio_driver_t *driver = gpio_driver_get(gpio.port);
    driver->irq_enable(gpio.port, gpio.pin);
}

/**
 * @brief   Disable the GPIO pin interrupt if configured as interrupt source
 *
 * @note    You have to add the module `periph_gpio_irq` to your project to
 *          enable this function
 *
 * @param[in] gpio  GPIO pin to disable the interrupt for
 */
static inline void gpio_irq_disable(gpio_t gpio)
{
    const gpio_driver_t *driver = gpio_driver_get(gpio.port);
    driver->irq_disable(gpio.port, gpio.pin);
}

#endif /* defined(MODULE_PERIPH_GPIO_IRQ) || defined(DOXYGEN) */

/**
 * @brief   Get the current value of the given GPIO pin
 *
 * @param[in] gpio  GPIO pin to read
 *
 * @return    0 when pin is LOW
 * @return    >0 for HIGH
 */
static inline int gpio_read(gpio_t gpio)
{
    const gpio_driver_t *driver = gpio_driver_get(gpio.port);
    return driver->read(gpio.port) & (1 << gpio.pin);
}

/**
 * @brief   Set the given GPIO pin to HIGH
 *
 * @param[in] gpio  GPIO pin to set
 */
static inline void gpio_set(gpio_t gpio)
{
    const gpio_driver_t *driver = gpio_driver_get(gpio.port);
    driver->set(gpio.port, (1 << gpio.pin));
}

/**
 * @brief   Set the given GPIO pin to LOW
 *
 * @param[in] gpio  GPIO pin to clear
 */
static inline void gpio_clear(gpio_t gpio)
{
    const gpio_driver_t *driver = gpio_driver_get(gpio.port);
    driver->clear(gpio.port, (1 << gpio.pin));
}

/**
 * @brief   Toggle the value of the given GPIO pin
 *
 * @param[in] gpio  GPIO pin to toggle
 */
static inline void gpio_toggle(gpio_t gpio)
{
    const gpio_driver_t *driver = gpio_driver_get(gpio.port);
    driver->toggle(gpio.port, (1 << gpio.pin));
}

/**
 * @brief   Set the given GPIO pin to the given value
 *
 * @param[in] gpio  GPIO pin to set
 * @param[in] value value to set the pin to, 0 for LOW, HIGH otherwise
 */
static inline void gpio_write(gpio_t gpio, int value)
{
    const gpio_driver_t *driver = gpio_driver_get(gpio.port);
    driver->write(gpio.port, value ? driver->read(gpio.port) | (1 << gpio.pin)
                                   : driver->read(gpio.port) & ~(1 << gpio.pin));
}

/**
 * @brief   Test if a GPIO pin is equal to another GPIO pin
 *
 * @param[in] gpio1 First GPIO pin to check
 * @param[in] gpio2 Second GPIO pin to check
 */
static inline int gpio_is_equal(gpio_t gpio1, gpio_t gpio2)
{
    return (gpio1.port == gpio2.port) && (gpio1.pin == gpio2.pin);
}

/**
 * @brief   Test if a GPIO pin is undefined
 *
 * @param[in] gpio GPIO pin to check
 */
static inline int gpio_is_undef(gpio_t gpio)
{
    return gpio_is_equal(gpio, GPIO_UNDEF);
}

/**
 * @brief   Returns the total number of GPIO ports (MCU and other GPIO ports)
 *
 * return   number of GPIO ports
 */
int gpio_port_numof(void);

/**
 * @brief   Returns the port number of a given GPIO pin (MCU and other
 *          GPIO ports)
 *
 * @param[in] gpio  given GPIO pin
 *
 * return     port number for the given GPIO pin
 */
int gpio_port(gpio_t gpio);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* MODULE_PERIPH_GPIO_EXT */
#endif /* PERIPH_GPIO_H */
