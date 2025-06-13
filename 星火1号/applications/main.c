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
#include "openmv.h"
#include "zw101.h"
#include "uart4.h"
#include "spi1.h"
#include "myaht10.h"
#include "mqtt.h"
#include "led.h"
#include "medication_management.h"
#define DBG_TAG "main"
#define DBG_LVL         DBG_LOG
#include <rtdbg.h>
#include "pwm.h"
#define PIN_KEY0 GET_PIN(C, 0)
#define PIN_KEY1 GET_PIN(C, 1)
#include "medication_process.h"
#include "infrared.h"


int main(void)
{
    rt_pin_write(GET_PIN(A, 4), PIN_HIGH);
    rt_pin_mode(SCAN_PIN, PIN_MODE_OUTPUT);
    rtc_init();
    zw101_init();
    openmv_init();
    uart4_init();
    can_init();
    spi1_init();
    medication_event_init();
    wifi_connect();
    rt_thread_mdelay(20000);
    led_init();
    set_screen_time();
    myaht10_init();
    aht_mqtt_init();
    rt_thread_mdelay(50);
    identity_mqtt_change();
    servo_pwm_init();
    INFRA_Init();

    while (1)
    {
      set_screen_time();
      rt_thread_mdelay(3000);
    }

    return 0;
}

