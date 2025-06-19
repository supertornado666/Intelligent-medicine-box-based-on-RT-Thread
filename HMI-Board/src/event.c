/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-04-01     廖钟涛       the first version
 */
#include "event.h"

#include <rtdbg.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG

rt_event_t speak_event;

rt_uint8_t speak_event_init(void){
    speak_event = rt_event_create("spaek_event", RT_IPC_FLAG_FIFO);
    if (speak_event == RT_NULL){
        LOG_E("rt_event_create failed...\n");
        return -RT_ERROR;
    }
    LOG_D("rt_event_create succeeded...\n");
}
