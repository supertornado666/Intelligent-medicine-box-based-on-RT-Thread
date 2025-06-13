/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-06     廖钟涛       the first version
 */
#include "LED.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <stdarg.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

/******根据星火一号实际修改******/

/******根据星火一号实际修改******/

static rt_base_t leds[5];

void led_init()
{
    leds[0] = rt_pin_get("PE.11");
    leds[1] = rt_pin_get("PE.13");
    leds[2] = rt_pin_get("PE.15");
    leds[3] = rt_pin_get("PD.9");
    leds[4] = rt_pin_get("PG.7");

    for (int i = 0; i < 5; i++)
    {
        rt_pin_mode(leds[i], PIN_MODE_OUTPUT);
        rt_pin_write(leds[i], PIN_LOW); // 默认关闭（高电平）
    }
}

void led_on_st(int number)
{
    if (number >= 1 && number <= 5)
        rt_pin_write(leds[number - 1], PIN_HIGH);
}

void led_off_st(int number)
{
    if (number >= 1 && number <= 5)
        rt_pin_write(leds[number - 1], PIN_LOW);
}
void led_on_all()
{
    for(int i=0;i<5;i++)
    rt_pin_write(leds[i], PIN_HIGH);
}
void led_off_all()
{
    for(int i=0;i<5;i++)
    rt_pin_write(leds[i], PIN_LOW);
}
