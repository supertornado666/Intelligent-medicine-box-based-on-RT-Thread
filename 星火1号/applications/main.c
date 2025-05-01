/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-5-10      ShiHao       first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "can.h"
#include "commands_def.h"
#include "wlan.h"
#include "rtc.h"
#include "uart2.h"
#include "uart3.h"
#include "uart4.h"
#include "spi1.h"

#define DBG_TAG "main"
#define DBG_LVL         DBG_LOG
#include <rtdbg.h>

#define PIN_KEY0 GET_PIN(C, 0)
#define PIN_KEY1 GET_PIN(C, 1)

int main(void)
{
    rtc_init();
    uart2_init();
    uart3_init();
    uart4_init();
    can_init();
    spi1_init();
    wifi_init();

    set_screen_time();

//    while (1) {
//
//    }

    return 0;
}

