/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-03-15     廖钟涛       the first version
 */
#include "uart1.h"

#include "standby_timer.h"
#include "event.h"

#include <rtdbg.h>
#define DBG_TAG "main"
#define DBG_LVL DBG_LOG

rt_device_t u1_dev;
static struct rt_semaphore u1_sem;
static rt_thread_t u1_th;
static rt_size_t rx1_len = 0;

rt_uint8_t u1_buf[500] = {0};
extern rt_uint8_t m_buf[500];

volatile rt_bool_t uart_send_complete_flag = RT_FALSE;

static rt_err_t rx1_callback(rt_device_t dev, rt_size_t size){
//void user_uart1_callback(uart_callback_args_t *p_args){
    //DMA接收
    rx1_len = size;
    rt_sem_release(&u1_sem);

    return RT_EOK;
}

static void serial1_thread_entry(void *parameter){
    //DMA接收
    static rt_size_t len = 0;

    while (1){
//        rt_event_recv(speak_event, EVENT_GET_INFO,
//                                           RT_EVENT_FLAG_CLEAR | RT_EVENT_FLAG_AND,
//                                           RT_WAITING_FOREVER, NULL);
        rt_sem_take(&u1_sem, RT_WAITING_FOREVER);

        len += rt_device_read(u1_dev, 0, u1_buf + len, rx1_len);
        if (u1_buf[len - 1] == '$'){
            u1_buf[len] = '\0';
            len = 0;
            rt_kprintf("%s", u1_buf);
            rt_strcpy(m_buf, u1_buf);

            rt_event_send(speak_event, EVENT_CALL_DEEPSEEK);

            backlight_on();
        }
        //rt_kprintf("buf:%s\n", command);
    }
}

rt_uint8_t uart1_init(void){
    static rt_err_t ret = 0;
    static const struct serial_configure u1_configs = RT_SERIAL_CONFIG_DEFAULT;

    u1_dev = rt_device_find("uart1");
    if (u1_dev == RT_NULL){
        LOG_E("rt_device_find[uart1] failed...\n");
        return -RT_ERROR;
    }

    ret = rt_device_open(u1_dev, RT_DEVICE_FLAG_DMA_RX);
    if (ret < 0){
        LOG_E("rt_device_open[uart1] failed...\n");
        return -RT_ERROR;
    }

    rt_device_control(u1_dev, RT_DEVICE_CTRL_CONFIG, (void *)&u1_configs);
    rt_device_set_rx_indicate(u1_dev, rx1_callback);

//    R_SCI_UART_Open(&g_uart1_cfg, &g_uart1_ctrl);

    rt_sem_init(&u1_sem, "rx1_sem", 0, RT_IPC_FLAG_FIFO);
    u1_th = rt_thread_create("u1_recv", serial1_thread_entry, RT_NULL, 1536, 22, 10);
    rt_thread_startup(u1_th);

    return RT_EOK;
}
//INIT_APP_EXPORT(uart4_init);


/*重定向printf*/
//#if defined __GNUC__ && !defined __clang__
//int _write(int fd, char *pBuffer, int size);
//int _write(int fd, char *pBuffer, int size)
//{
//    (void)fd;
//    R_SCI_UART_Write(&g_uart1_ctrl, (uint8_t *)pBuffer, (uint32_t)size);
//    while (uart_send_complete_flag == false);
//    uart_send_complete_flag = false;
//
//    return size;
//}
//#endif
