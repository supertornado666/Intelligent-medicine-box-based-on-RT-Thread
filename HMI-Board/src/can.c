/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-03-21     廖钟涛       the first version
 */

#include "can.h"
#include <rtthread.h>
#include <rtdbg.h>
#include "hal_data.h"
#include <rtdevice.h>
#include "standby_timer.h"
#include "commands_def.h"

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG

static struct rt_semaphore can_rx_sem;     /* 用于接收消息的信号量 */
static rt_device_t can_dev;            /* CAN 设备句柄 */

char command[9] = {0};
extern struct rt_semaphore read_sem, show_sem;
extern bool inback_flag;

/* 接收数据回调函数 */
static rt_err_t can_rx_callback(rt_device_t dev, rt_size_t size)
{
    /* CAN 接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    rt_sem_release(&can_rx_sem);

    if (!inback_flag) {
        rt_sem_release(&read_sem);
    }

    return RT_EOK;
}

static void can_rx_thread(void *parameter)
{
    static rt_err_t res;
    static struct rt_can_msg rxmsg = {0};

#ifdef RT_CAN_USING_HDR
    struct rt_can_filter_item items[5] =
    {
        RT_CAN_FILTER_ITEM_INIT(0x100, 0, 0, 0, 0x700, RT_NULL, RT_NULL), /* std,match ID:0x100~0x1ff，hdr 为 - 1，设置默认过滤表 */
        RT_CAN_FILTER_ITEM_INIT(0x300, 0, 0, 0, 0x700, RT_NULL, RT_NULL), /* std,match ID:0x300~0x3ff，hdr 为 - 1 */
        RT_CAN_FILTER_ITEM_INIT(0x211, 0, 0, 0, 0x7ff, RT_NULL, RT_NULL), /* std,match ID:0x211，hdr 为 - 1 */
        RT_CAN_FILTER_STD_INIT(0x486, RT_NULL, RT_NULL),                  /* std,match ID:0x486，hdr 为 - 1 */
        {0x555, 0, 0, 0, 0x7ff, 7,}                                       /* std,match ID:0x555，hdr 为 7，指定设置 7 号过滤表 */
    };
    struct rt_can_filter_config cfg = {5, 1, items}; /* 一共有 5 个过滤表 */
    /* 设置硬件过滤表 */
    res = rt_device_control(can_dev, RT_CAN_CMD_SET_FILTER, &cfg);
    RT_ASSERT(res == RT_EOK);
#endif

    while (1)
    {
        /* hdr 值为 - 1，表示直接从 uselist 链表读取数据 */
        //rxmsg.hdr_index = -1;
        /* 阻塞等待接收信号量 */
        rt_sem_take(&can_rx_sem, RT_WAITING_FOREVER);
        /* 从 CAN 读取一帧数据 */
        rt_device_read(can_dev, 0, &rxmsg, sizeof(rxmsg));
        sprintf(command, "%.*s", rxmsg.len, rxmsg.data);

        //rt_kprintf("buf:%s\n", command);
        if (strcmp(command, SHOW_SMILE) >= 0 && strcmp(command, SHOW_EMO_END) <= 0) {
            rt_sem_release(&show_sem);
            //rt_kprintf("buf:%s\n", command);
        }

        backlight_on();
    }
}

int can_send(rt_uint8_t *p_buff, rt_uint32_t len)
{
    static struct rt_can_msg msg = {0};
    static rt_size_t  size;

    msg.id  = 0x555;              /* ID 为 0x03 */
    msg.ide = RT_CAN_STDID;     /* 标准格式 */
    msg.rtr = RT_CAN_DTR;       /* 数据帧 */
    msg.len = len;                /* 数据长度为 8 */
    rt_memcpy(&msg.data[0], p_buff, len);
    /* 发送一帧 CAN 数据 */
    size = rt_device_write(can_dev, 0, &msg, sizeof(msg) );
    if (size == 0)
    {
        rt_kprintf("CAN send failed: no ack or bus error\n");
        return -1; // 发送失败
    }
    return 0;
}

int can_init(void){
    static rt_err_t ret;
    static rt_thread_t can_th;

    /* 查找 CAN 设备 */
    can_dev = rt_device_find("can0");
    if (!can_dev)
    {
        LOG_E("find can0 failed!\n");
        return -RT_ERROR;
    }

    /* 以中断接收及发送方式打开 CAN 设备 */
    ret = rt_device_open(can_dev, RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX);
    if (ret < 0){
        LOG_E("rt_device_open[can0] failed...\n");
        return -2;
    }

    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(can_dev, can_rx_callback);
    /* 初始化 CAN 接收信号量 */
    rt_sem_init(&can_rx_sem, "can_rx_sem", 0, RT_IPC_FLAG_FIFO);

    /* 设置 CAN 的工作模式为正常工作模式 */
    rt_device_control(can_dev, RT_CAN_CMD_SET_MODE, (void *)RT_CAN_MODE_NORMAL);
    /* 设置 CAN 通信的波特率为 100kbit/s*/
    //res = rt_device_control(can_dev, RT_CAN_CMD_SET_BAUD, (void *)CAN100kBaud);

    /* 创建数据接收线程 */
    can_th = rt_thread_create("can_rx", can_rx_thread, RT_NULL, 768, 21, 5);
    if (can_th != RT_NULL)
    {
        rt_thread_startup(can_th);
    }
    else
    {
        rt_kprintf("create can_rx thread failed!\n");
    }

    return ret;
}
//INIT_APP_EXPORT(can_init);
