/*
#include <fingerprint_reader.h>
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-05-17     廖钟涛       the first version
 */
#include "check_identity.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "medication_process.h"
#include "can.h"
#include "commands_def.h"

#define DBG_TAG "main"
#define DBG_LVL         DBG_LOG
#include <rtdbg.h>

rt_thread_t id_th;

static void check_identity_thread(void *parameter){
    while (1){
        if (Med_Zw101_IdentifyFinger()){
            rt_event_send(medication_event, EVENT_CHECK_IDENTITY);
        }
        else{
            can_send(IDENTITY_WRONG, 1);
            continue;
        }
        id_th = RT_NULL;
        rt_thread_delete(rt_thread_self());
    }
}

int check_identity_start(void){
    id_th = rt_thread_create("ck_id", check_identity_thread, NULL, 1024, 10, 5);
    rt_thread_startup(id_th);
    return 0;
}
