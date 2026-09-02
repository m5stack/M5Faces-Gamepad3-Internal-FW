/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "user_scan_handle.h"
#include "user_sys.h"
#include "user_int.h"

__IO uint8_t s_key = 0xFF;

__IO uint8_t key_press = 0;

static uint8_t s_key_candidate   = 0xFF;
static uint32_t s_debounce_start = 0U;

void request_event(void)
{
    i2c1_set_send_data(&s_key, 1);
    key_press = 0;
    IRQ_CLR;
}

void keyboard_update(void)
{
    uint8_t key_raw = KEY_READ();
    uint32_t now    = HAL_GetTick();

    if (key_raw != s_key_candidate) {
        s_key_candidate  = key_raw;
        s_debounce_start = now;
        return;
    }

    if ((now - s_debounce_start) < KEY_DEBOUNCE_TIME_MS) {
        return;
    }

    if (s_key_candidate != s_key) {
        s_key     = s_key_candidate;
        key_press = 1;
        IRQ_SET;
    }
}