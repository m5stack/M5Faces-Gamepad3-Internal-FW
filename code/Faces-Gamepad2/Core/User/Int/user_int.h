/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef __USER_INT_H__
#define __USER_INT_H__

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IRQ_CLR (LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1))
#define IRQ_SET (LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1))

#ifdef __cplusplus
}
#endif

#endif /* __USER_INT_H__ */
