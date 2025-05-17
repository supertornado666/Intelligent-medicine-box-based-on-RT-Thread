/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-05-10     廖钟涛       the first version
 */
#include "medication_process.h"
#include <rtthread.h>
#include <rtdbg.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG

rt_event_t medication_event;

int medication_event_init(void){
    medication_event = rt_event_create("medication_event", RT_IPC_FLAG_FIFO);
    if (medication_event == RT_NULL){
        LOG_E("rt_event_create failed...\n");
        return -3;
    }
    LOG_D("rt_event_create succeeded...\n");
}
