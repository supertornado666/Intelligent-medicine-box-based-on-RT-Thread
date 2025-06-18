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
#include "medication_process.h"
#include "infrared.h"
#include "pwm.h"

#define DBG_TAG "main"
#define DBG_LVL         DBG_LOG
#include <rtdbg.h>

#define PIN_KEY0 GET_PIN(C, 0)
#define PIN_KEY1 GET_PIN(C, 1)

int main(void)
{
    wifi_connect();

    rt_pin_mode(SPI_CS_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(SPI_CS_PIN, PIN_HIGH);
    rt_pin_mode(SCAN_PIN, PIN_MODE_OUTPUT);

    rtc_init();
    zw101_init();
    openmv_init();
    uart4_init();
    can_init();
    //spi1_init();
    medication_event_init();
    led_init();
    servo_pwm_init();
    INFRA_Init();
    set_screen_time();

    rt_thread_mdelay(15000);
    myaht10_init();
    aht_mqtt_init();
    rt_thread_mdelay(5000);
    identity_mqtt_change();

    //rt_pin_write(SPI_CS_PIN, PIN_LOW);
    //spi1_write(buf, 500);
    //rt_thread_mdelay(1000);
    //spi1_write("rubbish data", 400);
    //rt_pin_write(SPI_CS_PIN, PIN_HIGH);

    can_send("init_end", 8);

    while (1)
    {
      set_screen_time();
      rt_thread_mdelay(15000);
//      spi1_write(buf, 500);
//      rt_thread_mdelay(5000);
//      rt_device_write(u4_dev, 0, "rubbish$", strlen("rubbish$"));
    }

    return 0;
}

