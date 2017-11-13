/**
 * @addtogroup      cpu_stm32f1
 * @{
 *
 * @file
 * @brief           Memory definitions for the STM32F103ZE
 *
 * @author          George Hopkins <george-hopkins@null.net>
 *
 * @}
 */

MEMORY
{
    rom (rx)        : ORIGIN = 0x08000000, LENGTH = 512K
    ram (xrw)       : ORIGIN = 0x20000000, LENGTH = 64K
}

INCLUDE cortexm_base.ld
