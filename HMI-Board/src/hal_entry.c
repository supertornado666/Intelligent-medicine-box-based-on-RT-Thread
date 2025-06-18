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
#include "uart1.h"
#include "standby_timer.h"
#include "can.h"
#include "wlan_mgnt.h"
#include "llm.h"
#include "spi1.h"
#include "event.h"
#include "syn8086.h"
#include "demo/ui/ui.h"
#include "llm_show.h"

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG

extern int user_rtc_init(void);
extern void user_font_init(void);

extern rt_bool_t f;

void hal_entry(void)
{
    rt_wlan_connect("spark", "12345678");

    speak_event_init();
    //spi1_init();
    user_rtc_init();
    user_font_init();
    uart4_init();
    uart1_init();
    //speech_system_init();
    timer_init();
    can_init();
    llm2rtt_init();

    //rt_event_send(speak_event, EVENT_GET_INFO);
    while (f) rt_thread_mdelay(100);
    lv_obj_clear_flag(ui_loginoutbutton, LV_OBJ_FLAG_HIDDEN);
    SYN_FrameInfo("[v2]欢迎使用");

}
