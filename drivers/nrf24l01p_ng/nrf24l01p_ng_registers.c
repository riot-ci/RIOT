/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup drivers_nrf24l01p_ng
 * @{
 *
 * @file
 * @brief   Implementation of register modification functions
 *          for the NRF24L01+ (NG) transceiver
 *
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */
#include "nrf24l01p_ng_communication.h"
#include "nrf24l01p_ng_registers.h"

uint8_t nrf24l01p_ng_reg8_set(const nrf24l01p_ng_t *dev, uint8_t reg_addr,
                              uint8_t *reg_val)
{
    uint8_t reg_val_old;
    nrf24l01p_ng_read_reg(dev, reg_addr, &reg_val_old, sizeof(reg_val_old));
    *reg_val = reg_val_old | *reg_val;
    return nrf24l01p_ng_write_reg(dev, reg_addr, reg_val, sizeof(*reg_val));
}

uint8_t nrf24l01p_ng_reg8_clear(const nrf24l01p_ng_t *dev, uint8_t reg_addr,
                                uint8_t *reg_val)
{
    uint8_t reg_val_old;
    nrf24l01p_ng_read_reg(dev, reg_addr, &reg_val_old, sizeof(reg_val_old));
    *reg_val = reg_val_old &= ~(*reg_val);
    return nrf24l01p_ng_write_reg(dev, reg_addr, reg_val, sizeof(*reg_val));
}

uint8_t nrf24l01p_ng_reg8_mod(const nrf24l01p_ng_t *dev, uint8_t reg_addr,
                              uint8_t mask, uint8_t *reg_val)
{
    uint8_t reg_val_old;
    nrf24l01p_ng_read_reg(dev, reg_addr, &reg_val_old, sizeof(reg_val_old));
    reg_val_old &= ~mask;
    *reg_val = reg_val_old | *reg_val;
    return nrf24l01p_ng_write_reg(dev, reg_addr, reg_val, sizeof(*reg_val));
}
