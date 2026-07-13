/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef __USER_I2C_CALLBACK_H__
#define __USER_I2C_CALLBACK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern __IO uint8_t i2c_addr_reg;
extern __IO uint8_t tx_prepared;

void read_uid(void);
void Slave_Complete_Callback(uint8_t *rx_data, uint16_t len);
void i2c_timeout_handler(void);

#ifdef __cplusplus
}
#endif

#endif /*__USER_I2C_CALLBACK_H__ */
