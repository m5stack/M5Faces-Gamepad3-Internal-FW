/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef __USER_SCAN_HANDLE_H__
#define __USER_SCAN_HANDLE_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

#define KEY_READ()                                                                                      \
    ((uint8_t)((GPIOA->IDR & 0x00C1U) | ((GPIOA->IDR & 0x0010U) << 1) | ((GPIOA->IDR & 0x0020U) >> 1) | \
               ((GPIOA->IDR & 0x0004U) << 1) | ((GPIOA->IDR & 0x0002U) << 1) | ((GPIOA->IDR & 0x0008U) >> 2)))

#define KEY_DEBOUNCE_TIME_MS (20U)

void request_event(void);
void keyboard_update(void);

#ifdef __cplusplus
}
#endif
#endif /* __USER_SCAN_HANDLE_H__ */