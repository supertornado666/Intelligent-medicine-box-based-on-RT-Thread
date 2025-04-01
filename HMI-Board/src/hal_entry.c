/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2021-10-10     Sherman       first version
 */

#include <rtthread.h>
#include "hal_data.h"
#include <rtdevice.h>
#include <rtdbg.h>
#include "uart4.h"
#include "standby_timer.h"
#include "can.h"
#include "wlan_mgnt.h"
#include "llm.h"
#include "spi1.h"
#include "event.h"

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG

extern int user_rtc_init(void);
extern void user_font_init(void);

void hal_entry(void)
{
    user_rtc_init();
    user_font_init();
    uart4_init();
    timer_init();
    can_init();
    spi1_init();
    speak_event_init();

    rt_wlan_connect("Redmi K60 Pro", "jh15988597685");

    rt_thread_mdelay(5000);
    llm2rtt_init();
}
