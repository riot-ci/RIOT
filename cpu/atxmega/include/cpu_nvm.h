/*
 * Copyright (C) 2021 Gerson Fernando Budke <nandojve@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atxmega
 * @brief       Non Volatile Memory (NVM) internal API
 * @{
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 */

#ifndef CPU_NVM_H
#define CPU_NVM_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Get offset of calibration bytes in the production signature row
 *
 * @param regname Name of register within the production signature row
 * @retval Offset of register into the production signature row
 */
#define nvm_get_production_signature_row_offset(regname) \
    offsetof(NVM_PROD_SIGNATURES_t, regname)

/**
 * @brief Read one byte from the production signature row
 *
 * This function reads one byte from the production signature row of the device
 * at the given address.
 *
 * @note This function is modifying the NVM.CMD register.
 *       If the application are using program space access in interrupts
 *       (__flash pointers in IAR EW or pgm_read_byte in GCC) interrupts
 *       needs to be disabled when running EEPROM access functions. If not
 *       the program space reads will be corrupted.
 *
 * @param address Byte offset into the signature row
 */
uint8_t nvm_read_production_signature_row(uint8_t address);

#ifdef __cplusplus
}
#endif

#endif /* CPU_NVM_H */
/** @} */
