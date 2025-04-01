/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-03-20     廖钟涛       the first version
 */
#include "standby_timer.h"

#include <rtthread.h>
#include <r_gpt.h>
#include "hal_data.h"

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

static rt_timer_t tm1;
static uint32_t current_time;
// 全局变量，记录触摸操作的时间
uint32_t last_touch_time = 0;

static void backlight_off(void) {
    R_GPT_DutyCycleSet(&g_timer5_ctrl, 0, GPT_IO_PIN_GTIOCB);
}

void backlight_on(void) {
    last_touch_time = lv_tick_get();
    R_GPT_DutyCycleSet(&g_timer5_ctrl, 5000, GPT_IO_PIN_GTIOCB);
}

// 定时器回调函数，每隔一定时间检查是否需要息屏
static void screen_off_timer(void) {
    current_time = lv_tick_get();  // 获取当前时间

    // 如果已经 60 秒没有触摸操作，关闭屏幕
    if (current_time - last_touch_time > 60000) {
        // 调用硬件接口关闭屏幕或降低背光
        backlight_off();
    }
}

int timer_init(void){
    // 设置 LVGL 定时器（每秒检查一次）
    tm1 = rt_timer_create("stdby_tim", screen_off_timer, NULL, 1000,
                RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);

    if (tm1 == RT_NULL){
        LOG_E("rt_timer_create failed...\n");
        return -1;
    }

    LOG_D("rt_timer_create succeeded...\n");
    rt_timer_start(tm1);
}
//INIT_APP_EXPORT(timer_init);
