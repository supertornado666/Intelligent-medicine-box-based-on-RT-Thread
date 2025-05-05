/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-05-02     廖钟涛       the first version
 */
#include "myaht10.h"
#include "rtthread.h"

#define DBG_TAG "main"
#define DBG_LVL         DBG_LOG
#include <rtdbg.h>

static aht10_device_t aht_dev;
static rt_thread_t aht_th;

float humidity, temperature;
rt_mq_t mq_hum = RT_NULL;
rt_mq_t mq_tem = RT_NULL;

static void aht10_thread_entry(void *parameter){
    char str[9];
    while (1){
        /* 读取湿度 */
        humidity = aht10_read_humidity(aht_dev);
        sprintf(str, "hum:%.1f", humidity);
        can_send(str, 8);
        rt_mq_urgent(mq_hum, &humidity, sizeof(humidity));
        //LOG_D("humidity   : %d.%d %%", (int)humidity, (int)(humidity * 10) % 10);

        rt_thread_mdelay(200);

        /* 读取温度 */
        temperature = aht10_read_temperature(aht_dev);
        sprintf(str, "tem:%.1f", temperature);
        can_send(str, 8);
        rt_mq_urgent(mq_tem, &temperature, sizeof(temperature));
        //LOG_D("temperature: %d.%d℃", (int)temperature, (int)(temperature * 10) % 10);

        rt_thread_mdelay(10000);
    }
}

int myaht10_init(void){
    /* 总线名称 */
    const char *i2c_bus_name = "i2c3";
    int count = 0;

    /* 等待传感器正常工作 */
    rt_thread_mdelay(2000);

    /* 初始化 aht10 */
    aht_dev = aht10_init(i2c_bus_name);
    if (aht_dev == RT_NULL)
    {
        LOG_E(" The sensor initializes failure");
        return -1;
    }

    // 初始化消息队列
    mq_hum = rt_mq_create("mq_hum", 10, sizeof(humidity), RT_IPC_FLAG_FIFO);
    mq_tem = rt_mq_create("mq_tem", 10, sizeof(temperature), RT_IPC_FLAG_FIFO);

    aht_th = rt_thread_create("aht_th", aht10_thread_entry, NULL, 1024, 10, 5);
    rt_thread_startup(aht_th);

    return 0;
}
