/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-03-15     廖钟涛       the first version
 */
#include "uart4.h"
#include <rtthread.h>
#include <rtdbg.h>
#include <rtdevice.h>
#include "hal_data.h"
#include "commands_def.h"
#include "standby_timer.h"
#include "string.h"
#include "event.h"

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG

rt_device_t u4_dev;
static struct rt_semaphore u4_sem;
static rt_thread_t u4_th;
static rt_size_t rx_len = 0;

char msg[512] = {0};
//extern struct rt_semaphore read_sem, show_sem;
//extern bool inback_flag;
//extern uint32_t last_touch_time;

//extern struct rt_semaphore call_deepseek_sem;
//extern struct rt_semaphore info_sem;

/*
static int is_ni_hao(const char* arr) {
    const char ni_hao_utf8[] = {0xE4, 0xBD, 0xA0, 0xE5, 0xA5, 0xBD}; // "你好" 的 UTF-8 编码
    return memcmp(arr, ni_hao_utf8, 6) == 0;
}
*/

static rt_err_t rx_callback(rt_device_t dev, rt_size_t size){
    //DMA接收
    rx_len = size;
    rt_sem_release(&u4_sem);
    /*
    if (!inback_flag) {
        rt_sem_release(&read_sem);
    }
    */

    return RT_EOK;
}

static void serial_thread_entry(void *parameter){
    //DMA接收
    static rt_size_t len = 0;

    while (1){
        rt_sem_take(&u4_sem, RT_WAITING_FOREVER);
        len = rt_device_read(u4_dev, 0, msg, rx_len);
        msg[len] = '\0';

        if (rt_strstr(msg, "病情") != RT_NULL){
            rt_event_send(speak_event, EVENT_GET_INFO);
            //rt_sem_release(&call_deepseek_sem);
            can_send(MEDICINE_GET_INFO, 1);
        }
        else{
            rt_event_send(speak_event, EVENT_GET_INFO);
            rt_event_send(speak_event, EVENT_CALL_DEEPSEEK);

            //rt_sem_release(&call_deepseek_sem);
            //rt_sem_release(&info_sem);
        }
        /*
        if (command[0] >= 'E' && command[0] <= 'H') {
            rt_sem_release(&show_sem);
            //rt_kprintf("buf:%s\n", command);
        }
        */

        backlight_on();
        //rt_kprintf("buf:%s\n", command);
    }
}

int uart4_init(void){
    static rt_err_t ret = 0;
    static const struct serial_configure u4_configs = RT_SERIAL_CONFIG_DEFAULT;

    u4_dev = rt_device_find("uart4");
    if (u4_dev == RT_NULL){
        LOG_E("rt_device_find[uart4] failed...\n");
        return -1;
    }

    ret = rt_device_open(u4_dev, RT_DEVICE_FLAG_DMA_RX);
    if (ret < 0){
        LOG_E("rt_device_open[uart4] failed...\n");
        return -2;
    }

    rt_device_control(u4_dev, RT_DEVICE_CTRL_CONFIG, (void *)&u4_configs);
    rt_device_set_rx_indicate(u4_dev, rx_callback);
    rt_sem_init(&u4_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    u4_th = rt_thread_create("u4_recv", serial_thread_entry, NULL, 512, 22, 5);
    rt_thread_startup(u4_th);

    return 0;
}
//INIT_APP_EXPORT(uart4_init);
