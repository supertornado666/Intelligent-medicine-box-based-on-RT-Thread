/*
#include <openmv.h>
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
#include "medication_management.h"
#include "can.h"
#include "pwm.h"
#include "commands_def.h"
#include "openmv.h"
#include "medication_management.h"
#define DBG_TAG "main"
#define DBG_LVL         DBG_LOG
#include <rtdbg.h>

static struct serial_configure u2_configs = RT_SERIAL_CONFIG_DEFAULT;
static struct rt_semaphore u2_sem;
struct rt_semaphore pill_freq;
static rt_thread_t openmv_th;

static rt_size_t rx2_len = 0;

extern char freq[4];
int ret=0;
static rt_err_t rx2_callback(rt_device_t dev, rt_size_t size){
    //中断接收
    //rt_sem_release(&u2_sem);

    //DMA接收
    rx2_len = size;
    rt_sem_release(&u2_sem);

    return RT_EOK;
}

static void openmv_thread_entry(void *parameter){

    //DMA接收
    rt_size_t len = 0;
    char buf[512] = {0};
    while (1){
        rt_sem_take(&u2_sem, RT_WAITING_FOREVER);
//        while(len != 13)
//        {
            len += rt_device_read(u2_dev, 0, buf + len, rx2_len);
//        }
        if (len == 13) {
            buf[len] = '\0';
            len = 0;
            rt_kprintf("buf:%s\n", buf);
            rt_thread_mdelay(300);

            can_send(MEDICINE_INFOIN_SUCCESS, 1);
            rt_sem_take(&pill_freq, RT_WAITING_FOREVER);
            rt_kprintf("freq:%s",freq);
            int ret=0;
            ret = add_medicine(buf, freq[0] - '0', freq[2] - '0');

            if (ret == 0) {
                rt_thread_mdelay(3);

                can_send(MEDICINE_IN_SUCCESS, 1);
            }
            else {
                rt_kprintf("add  fail\n");
                rt_thread_mdelay(3);
                can_send(MEDICINE_IN_ERROR, 1);
            }
        }
        else{
            can_send(PLEASE_SCAN_AGAIN, 1);
            len = 0;
        }

    }
}

int openmv_init(void)
{
    rt_err_t ret = 0;

    u2_dev = rt_device_find("uart2");
    if (u2_dev == RT_NULL){
        LOG_E("rt_device_find[uart2] failed...\n");
        return -1;
    }
    LOG_D("rt_device_find[uart2] succeeded...\n");

    //中断接收
    //ret = rt_device_open(u2_dev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    //DMA接收
    ret = rt_device_open(u2_dev, RT_DEVICE_FLAG_DMA_RX);

    if (ret < 0){
        LOG_E("rt_device_open[uart2] failed...\n");
        return ret;
    }
    LOG_D("rt_device_open[uart2] succeeded...\n");

    rt_device_control(u2_dev, RT_DEVICE_CTRL_CONFIG, (void *)&u2_configs);

    rt_device_set_rx_indicate(u2_dev, rx2_callback);

    rt_sem_init(&u2_sem, "rx2_sem", 0, RT_IPC_FLAG_FIFO);
    rt_sem_init(&pill_freq, "pill_freq", 0, RT_IPC_FLAG_FIFO);

    openmv_th = rt_thread_create("opmv_recv", openmv_thread_entry, NULL, 2536, 10, 5);
    rt_thread_startup(openmv_th);

    return 0;
}

