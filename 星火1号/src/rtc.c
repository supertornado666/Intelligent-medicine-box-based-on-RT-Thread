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
    time_t now;
    int hour, minute;
    struct tm timeinfo;
    char str[6];

    while (1){
        rt_sem_take(&alarm_sem, RT_WAITING_FOREVER);

        now = time(RT_NULL);
        gmtime_r(&now, &timeinfo);   // 转为结构体形式（UTC时间）

        hour = timeinfo.tm_hour;
        minute = timeinfo.tm_min;
        sprintf(str, "%02d:%02d", hour, minute);
        find_medicine(str);
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
    struct rt_alarm *alarm = RT_NULL;
    int hour = 0, min = 0;

    // 解析 "00:00" 形式的时间字符串
    if (sscanf(tim, "%d:%d", &hour, &min) != 2)
    {
        rt_kprintf("时间格式错误，应为 HH:MM\n");
        return;
    }

    // 设置为每天重复的闹钟
    setup.flag = RT_ALARM_DAILY;
    setup.wktime.tm_hour = hour;
    setup.wktime.tm_min = min;
    setup.wktime.tm_sec = 0;

//    // 其余字段可以不填或设为0，因为每天重复时只看时分秒
//    setup.wktime.tm_year = 0;
//    setup.wktime.tm_mon  = 0;
//    setup.wktime.tm_mday = 0;
//    setup.wktime.tm_wday = 0;

    alarm = rt_alarm_create(user_alarm_callback, &setup);
    if (alarm != RT_NULL)
    {
        rt_alarm_start(alarm);
        //rt_kprintf("每天 %02d:%02d 的闹钟已设置\n", hour, min);
    }
    else
    {
        //rt_kprintf("闹钟创建失败！\n");
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
    //rt_kprintf("%s", str);
    can_send(str, 8);
    rt_memcpy(str, t + 2, 8);
    //rt_kprintf("%s", str);
    rt_thread_mdelay(500);
    can_send(str, 8);
}
