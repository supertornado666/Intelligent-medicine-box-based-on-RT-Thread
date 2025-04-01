/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-03-19     廖钟涛       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <time.h>

#define DBG_LEVEL DBG_LOG
#define DBG_SECTION_NAME "rtc"
#include <rtdbg.h>

#define RTC_NAME "rtc"

static rt_sem_t rtc_init_sem = RT_NULL;
//static struct tm *nowtime;
//static time_t now;
//static struct tm safe_tm;

int user_rtc_init(void)
{
    static rt_err_t ret = RT_EOK;
    static rt_device_t device = RT_NULL;

    /*创建初始化完成信号量*/
    rtc_init_sem = rt_sem_create("rtc init flag", 0, 0);
    if(rtc_init_sem == RT_NULL)
    {
        rt_kprintf("rtc sem init failed!");
        return RT_ERROR;
    }
    /*寻找设备*/
    device = rt_device_find(RTC_NAME);
    if (!device)
    {
        rt_kprintf("find %s failed!", RTC_NAME);
        return RT_ERROR;
    }
    /*初始化RTC设备*/
    if(rt_device_open(device, 0) != RT_EOK)
    {
        rt_kprintf("open %s failed!", RTC_NAME);
        return RT_ERROR;
    }
    /* 设置日期 */
    ret = set_date(2025, 03, 20);
    if (ret != RT_EOK)
    {
        rt_kprintf("set RTC date failed\n");
        return ret;
    }

    /* 设置时间 */
    ret = set_time(23, 58, 50);
    if (ret != RT_EOK)
    {
        rt_kprintf("set RTC time failed\n");
        return ret;
    }

    rt_sem_release(rtc_init_sem);
    /* 获取时间 */
    /*
    now = time(RT_NULL);
    //rt_kprintf("RTC device init success,now time is %s\n", ctime(&now));
    if (localtime_r(&now, &safe_tm)){
        rt_kprintf("rtc init success\n");
        nowtime = &safe_tm;
    }
    else{
        rt_kprintf("rtc init fail\n");
    }
    */

    return ret;
}
//INIT_APP_EXPORT(user_rtc_init);

