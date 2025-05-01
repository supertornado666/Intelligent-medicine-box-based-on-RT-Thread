/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-04-24     廖钟涛       the first version
 */
#include "rtc.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "can.h"

static struct rt_semaphore alarm_sem;
static rt_thread_t alarm_th;

static void alarm_thread_entry(void *parameter){
    while (1){
        rt_sem_take(&alarm_sem, RT_WAITING_FOREVER);


    }
}

int rtc_init()
{
    rt_err_t ret = RT_EOK;
    time_t now;
    rt_device_t device = RT_NULL;

    /* 寻找设备 */
    device = rt_device_find(RTC_NAME);
    if (!device)
    {
        rt_kprintf("find %s failed!", RTC_NAME);
        return RT_ERROR;
    }

    /* 初始化RTC设备 */
    if(rt_device_open(device, 0) != RT_EOK)
    {
        rt_kprintf("open %s failed!", RTC_NAME);
        return RT_ERROR;
    }

    /* 设置日期 */
    //ret = set_date(2025, 4, 26);
    if (ret != RT_EOK)
    {
        rt_kprintf("set RTC date failed\n");
        return ret;
    }

    /* 设置时间 */
    //ret = set_time(12, 42, 30);
    if (ret != RT_EOK)
    {
        rt_kprintf("set RTC time failed\n");
        return ret;
    }

    /* 获取时间 */
    now = time(RT_NULL);
    rt_kprintf("%s\n", ctime(&now));

    /* 延时1秒 */
    rt_thread_mdelay(1000);

    /* 获取时间 */
    now = time(RT_NULL);
    rt_kprintf("%s\n", ctime(&now));

    rt_sem_init(&alarm_sem, "ala_sem", 0, RT_IPC_FLAG_FIFO);
    alarm_th = rt_thread_create("ala_th", alarm_thread_entry, NULL, 1024, 10, 5);
    rt_thread_startup(alarm_th);

    return ret;
}

static void user_alarm_callback(rt_alarm_t alarm, time_t timestamp)
{
    rt_sem_release(&alarm_sem);
    rt_kprintf("user alarm callback function.\n");
}

void alarm_set(char *tim)
{
    struct rt_alarm_setup setup;
    struct rt_alarm * alarm = RT_NULL;
    static time_t now;
    struct tm p_tm;

    if (alarm != RT_NULL)
        return;

    /* 获取当前时间戳，并把下5秒时间设置为闹钟时间 */
    now = time(RT_NULL) + 5;
    gmtime_r(&now,&p_tm);

    setup.flag = RT_ALARM_ONESHOT;
    setup.wktime.tm_year = p_tm.tm_year;
    setup.wktime.tm_mon = p_tm.tm_mon;
    setup.wktime.tm_mday = p_tm.tm_mday;
    setup.wktime.tm_wday = p_tm.tm_wday;
    setup.wktime.tm_hour = p_tm.tm_hour;
    setup.wktime.tm_min = p_tm.tm_min;
    setup.wktime.tm_sec = p_tm.tm_sec;

    alarm = rt_alarm_create(user_alarm_callback, &setup);
    if(RT_NULL != alarm)
    {
        rt_alarm_start(alarm);
    }
}

void set_screen_time(void){

    rt_thread_mdelay(8000);
    time_t now;
    now = time(RT_NULL);
    char t[11];
    sprintf(t, "%ld", now);
    char str[8] = "times:";
    rt_memcpy(str + 6, t, 2);
    rt_kprintf("%s", str);
    can_send(str, 8);
    rt_memcpy(str, t + 2, 8);
    rt_kprintf("%s", str);
    rt_thread_mdelay(500);
    can_send(str, 8);
}
