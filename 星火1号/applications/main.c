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
#include "myaht10.h"
#include "mqtt.h"

#define DBG_TAG "main"
#define DBG_LVL         DBG_LOG
#include <rtdbg.h>

#define PIN_KEY0 GET_PIN(C, 0)
#define PIN_KEY1 GET_PIN(C, 1)

int main(void)
{
    rt_pin_mode(SCAN_PIN, PIN_MODE_OUTPUT);

    rtc_init();
    uart2_init();
    uart3_init();
    uart4_init();
    can_init();
    spi1_init();
    wifi_connect();

    set_screen_time();
    myaht10_init();
    aht_mqtt_init();
    rt_thread_mdelay(500);
    identity_mqtt_change();

    while (1) {
        rt_thread_mdelay(30000);
        set_screen_time();
    }

    return 0;
}

