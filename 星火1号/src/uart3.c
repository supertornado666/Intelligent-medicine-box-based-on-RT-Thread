/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-04-24     huge       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "uart3.h"

#define DBG_TAG "main"
#define DBG_LVL         DBG_LOG
#include <rtdbg.h>

static struct serial_configure u3_configs = RT_SERIAL_CONFIG_DEFAULT;
static struct rt_semaphore u3_sem;
static rt_thread_t u3_th;
static rt_size_t rx3_len = 0;

static rt_err_t rx3_callback(rt_device_t dev, rt_size_t size){
    //中断接收
    rt_sem_release(&u3_sem);

    //DMA接收
    rx3_len = size;
    rt_sem_release(&u3_sem);

    return RT_EOK;
}

static void serial3_thread_entry(void *parameter){
    //中断接收
//    char buf;
//    while (1){
//        while(rt_device_read(u3_dev, 0, &buf, 1) != 1){
//            rt_sem_take(&u3_sem, RT_WAITING_FOREVER);
//        }
//        rt_kprintf("%c", buf);
//    }

    //DMA接收
    rt_size_t len = 0;
    char buf[512] = {0};
    while (1){
        rt_sem_take(&u3_sem, RT_WAITING_FOREVER);
        len = rt_device_read(u3_dev, 0, buf, rx3_len);
        buf[len] = '\0';

        rt_kprintf("buf:%s\n", buf);
    }
}

int uart3_init(void)
{
    rt_err_t ret = 0;

    u3_dev = rt_device_find("uart3");
    if (u3_dev == RT_NULL){
        LOG_E("rt_device_find[uart3] failed...\n");
        return -1;
    }
    LOG_D("rt_device_find[uart3] succeeded...\n");

    //中断接收
    //ret = rt_device_open(u3_dev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    //DMA接收
    ret = rt_device_open(u3_dev, RT_DEVICE_FLAG_DMA_RX);

    if (ret < 0){
        LOG_E("rt_device_open[uart3] failed...\n");
        return ret;
    }
    LOG_D("rt_device_open[uart3] succeeded...\n");

    rt_device_control(u3_dev, RT_DEVICE_CTRL_CONFIG, (void *)&u3_configs);

    rt_device_set_rx_indicate(u3_dev, rx3_callback);

    rt_sem_init(&u3_sem, "rx3_sem", 0, RT_IPC_FLAG_FIFO);

    u3_th = rt_thread_create("u3_recv", serial3_thread_entry, NULL, 1024, 10, 5);
    rt_thread_startup(u3_th);

    return 0;
}

