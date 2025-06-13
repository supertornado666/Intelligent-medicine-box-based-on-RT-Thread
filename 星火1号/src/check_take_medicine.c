/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-05-17     廖钟涛       the first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "medication_process.h"
#include "can.h"
#include "pwm.h"
#include "commands_def.h"
#include "check_take_medicine.h"
#include "infrared.h"
#include "led.h"
#define DBG_TAG "main"
#define DBG_LVL         DBG_LOG
#include <rtdbg.h>
rt_thread_t m_take_th;
int have_take[5];
char can_send_list[5][8] = {
    "motx:x,x",
    "motx:x,x",
    "motx:x,x",
    "motx:x,x",
    "motx:x,x"
};
static void take_medicine_thread(void *parameter){
    char *str = (char *)parameter;

    while (1){
        rt_event_recv(medication_event,
                              EVENT_CHECK_IDENTITY,
                              RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,
                              RT_WAITING_FOREVER,  // 或者设置超时
                              NULL);
        int count;
        matched_medicine *m_list = find_medicine(str, &count);

        for (int i = 0; i < count; i++) {
                       rt_kprintf(" name:%s number:%d time:%s amount:%d \n",
                                      m_list[i].name,
                                      m_list[i].number,
                                      m_list[i].time,
                                      m_list[i].amount);
                    rt_thread_mdelay(3);
                    can_send_list[i][3] = '0' + count;
                    can_send_list[i][5] = '0' + m_list[i].number;
                    can_send_list[i][7] = '0' + m_list[i].amount;
                    led_on_st(m_list[i].number);

                   }//到时打印药物信息到屏幕，mqtt
        for (int i = 0; i < count; i++) {
            can_send(can_send_list[i], 8);
            rt_thread_mdelay(100);
        }
        can_send("mote", 4);

        servo_toggle();//身份验证成功，开锁
        wait_for_medicine_pickup(m_list,count);
        for(int i=0;i<count;i++)
        {
            have_take[m_list[i].number]++;
        }
        rt_event_send(medication_event, EVENT_TAKE_MEDICINE_END);

        m_take_th = RT_NULL;
        return;
    }
}

int take_medicine_start(char *str){
    m_take_th = rt_thread_create("take_th", take_medicine_thread, str, 1024, 10, 5);
    rt_thread_startup(m_take_th);
    return 0;
}
