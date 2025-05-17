/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-05-17     廖钟涛       the first version
 */
#include "check_afterprocess.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "medication_process.h"
#include "can.h"
#include "commands_def.h"

#define DBG_TAG "main"
#define DBG_LVL         DBG_LOG
#include <rtdbg.h>

rt_thread_t after_th;

static void after_process_thread(void *parameter){
    while (1){
        //等待用药结束
        rt_event_recv(medication_event, EVENT_TAKE_MEDICINE_END, RT_EVENT_FLAG_AND, RT_WAITING_FOREVER, NULL);

        /*检查盖上*/


        rt_event_send(medication_event, EVENT_BOX_CLOSED);
        after_th = RT_NULL;
        rt_thread_delete(rt_thread_self());
    }
}

int after_process(void){
    after_th = rt_thread_create("after_th", after_process_thread, NULL, 1024, 10, 5);
    rt_thread_startup(after_th);
    return 0;
}
