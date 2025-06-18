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
#include "medication_process.h"
#include "mqtt.h"
#include "commands_def.h"
#include "medication_management.h"
#include "check_identity.h"
#include "check_take_medicine.h"
#include "check_afterprocess.h"
#include "led.h"
#include "pwm.h"
#include "infrared.h"
#define EVENT_MASK (EVENT_CHECK_IDENTITY | EVENT_TAKE_MEDICINE_END | EVENT_BOX_CLOSED)

static struct rt_semaphore alarm_sem;
static rt_thread_t alarm_th;

#define MAX_ALARMS 30

struct alarm_entry
{
    struct rt_alarm *alarm;
    int hour;
    int min;
};

static struct alarm_entry alarm_list[MAX_ALARMS] = {0};
static int alarm_count = 0;

//闹钟到时进入下述线程
static void alarm_thread_entry(void *parameter){
    time_t now;
    int hour, minute;
    struct tm timeinfo;
    char str[6];

    while (1){
        rt_sem_take(&alarm_sem, RT_WAITING_FOREVER);
        rt_kprintf("alarm_On\n");
        now = time(RT_NULL);
        gmtime_r(&now, &timeinfo);   // 转为结构体形式（UTC时间）

        hour = timeinfo.tm_hour;
        minute = timeinfo.tm_min;
        sprintf(str, "%02d:%02d", (hour + 8) % 24, minute);

        int count;
        find_medicine(str, &count);
        if (count == 0)
        continue;
        can_send(MEDICINE_TIME_ON, 1);

        //所有子线程接收单独事件时不加RT_EVENT_FLAG_CLEAR
        //在某线程中验证指纹，成功后发送事件EVENT_CHECK_IDENTITY
        rt_kprintf("put  finger\n");
        check_identity_start();
        //在某线程中接收事件EVENT_CHECK_IDENTITY，然后检查所有待服药物是否被拿起过，完成后发送事件EVENT_TAKE_MEDICINE_END
        //要传入待服用药物信息
        take_medicine_start(str);

        //rt_kprintf("lock_on\n");
        //在某线程中接收事件EVENT_TAKE_MEDICINE_END，然后检查盒子是否盖上，盖上后发送事件EVENT_BOX_CLOSED
        after_process();

        int result=rt_event_recv(medication_event,
                              EVENT_BOX_CLOSED,
                              RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,
                              60 * RT_TICK_PER_SECOND,  // 或者设置超时
                              NULL);
         led_off_all();
        //接下来传给屏幕
         rt_kprintf("take_finish\n");
         servo_toggle();
        if (result == -RT_ETIMEOUT){
            //删除前面所有线程
            if (id_th != RT_NULL) rt_thread_delete(id_th);
            if (m_take_th != RT_NULL) rt_thread_delete(m_take_th);
            if (after_th != RT_NULL) rt_thread_delete(after_th);

            // 循环清除所有可能已置位的事件标志，防止影响后续
            while (rt_event_recv(medication_event, EVENT_MASK,
                                RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                                0, NULL) == RT_EOK);

            //超时报警
            led_off_all();
            can_send(MEDICINE_TIME_TIMEOUT, 1);

            alarm_on();
        }
        //身份验证成功，取药完毕，盖子合上，上锁
        else if (result == RT_EOK){
            //锁上
            servo_set_angle(0);
        }
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
    ret = set_date(2025, 6, 16);
    if (ret != RT_EOK)
    {
        rt_kprintf("set RTC date failed\n");
        return ret;
    }

    /* 设置时间 */
    ret = set_time(7, 57, 45);
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
    alarm_th = rt_thread_create("ala_th", alarm_thread_entry, NULL, 2536, 10, 5);
    rt_thread_startup(alarm_th);

    return ret;
}

static void user_alarm_callback(rt_alarm_t alarm, time_t timestamp)
{
    rt_sem_release(&alarm_sem);
    rt_kprintf("user alarm callback function.\n");
}

static rt_bool_t alarm_time_exists(int hour, int min)
{
    for (int i = 0; i < alarm_count; i++)
    {
        if (alarm_list[i].hour == hour && alarm_list[i].min == min)

            return RT_TRUE;
    }
    return RT_FALSE;
}

int alarm_add(char *tim)//添加闹钟
{
    int hour = 0, min = 0;
    struct tm p_tm;
    struct rt_alarm_setup setup = {0};

    if (sscanf(tim, "%d:%d", &hour, &min) != 2)
    {
        rt_kprintf("时间格式错误，应为 HH:MM\n");
        return;
    }
    hour -= 8;
    if (hour < 0) hour += 24;

    if (alarm_time_exists(hour, min)) {
        rt_kprintf("闹钟 %02d:%02d 已存在，忽略重复设置\n", hour, min);
        return -1;
    }
    // 找空位
    for (int i = 0; i < MAX_ALARMS; i++)
    {
        if (alarm_list[i].alarm == RT_NULL)
        {
            setup.flag = RT_ALARM_DAILY;
            setup.wktime.tm_hour = hour;
            setup.wktime.tm_min = min;
            setup.wktime.tm_sec = 0;

            struct rt_alarm *alarm = rt_alarm_create(user_alarm_callback, &setup);
            if (alarm == RT_NULL)
            {
                rt_kprintf("第 %d 个闹钟创建失败！\n", i);
                return -1;
            }

            if (rt_alarm_start(alarm) != RT_EOK)
            {
                rt_kprintf("rt_alarm_start failed\n");
                return -1;
            }
            alarm_list[i].alarm = alarm;
            alarm_list[i].hour = hour;
            alarm_list[i].min = min;
            alarm_count++;

            rt_kprintf(" id%d alarm_set- %02d:%02d\n", i, (hour + 8) % 24, min);
            return i; // 返回下标作唯一ID
        }
    }

    rt_kprintf("闹钟已满，最多支持 %d 个！\n", MAX_ALARMS);
    return -1;
}

static void alarm_delete_by_id(int id)
{
    if (id < 0 || id >= MAX_ALARMS || alarm_list[id].alarm == RT_NULL)
    {
        rt_kprintf("无效 ID 或该闹钟不存在！\n");
        return;
    }

    rt_alarm_stop(alarm_list[id].alarm);
    rt_alarm_delete(alarm_list[id].alarm);
    alarm_list[id].alarm = RT_NULL;

    rt_kprintf("已删除第 %d 个闹钟（%02d:%02d）\n", id, alarm_list[id].hour, alarm_list[id].min);
}

void alarm_delete(char *tim)
{
    int hour = 0, min = 0;
    if (sscanf(tim, "%d:%d", &hour, &min) != 2)
    {
        rt_kprintf("时间格式错误，应为 HH:MM\n");
        return;
    }
    hour -= 8;
    if (hour < 0) hour += 24;

    for (int i = 0; i < MAX_ALARMS; i++)
    {
        if (alarm_list[i].alarm != RT_NULL &&
            alarm_list[i].hour == hour &&
            alarm_list[i].min == min)
        {
            alarm_delete_by_id(i);
        }
    }
}

void alarm_clear_all(void)
{
    for (int i = 0; i < MAX_ALARMS; i++)
    {
        if (alarm_list[i].alarm != RT_NULL)
        {
            alarm_delete_by_id(i);
        }
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
