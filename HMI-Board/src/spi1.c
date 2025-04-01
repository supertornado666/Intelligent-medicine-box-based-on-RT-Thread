/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-03-31     廖钟涛       the first version
 */

#include "spi1.h"
#include <rtthread.h>
#include <rtdbg.h>
#include <rtdevice.h>
#include "hal_data.h"
#include "event.h"

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG

static rt_device_t spi1_dev;
static struct rt_semaphore spi1_rx_sem;
static rt_thread_t spi1_th;

static rt_size_t spi_rx_len = 0;
static char raw_buf[400];
char valid_data[400];
//struct rt_semaphore info_sem;

static rt_err_t spi_rx_callback(rt_device_t dev, rt_size_t size)
{
    rt_uint8_t data;

    // 从 SPI 读取 1 字节数据（SPI 是全双工，必须发送才能接收）
    rt_spi_recv(spi1_dev, &data, 1);

    // 存入缓冲区（简单示例，未做缓冲区满检查）
    if (spi_rx_len < 400)
    {
        raw_buf[spi_rx_len++] = data;
    }
    if (data == '$'){
        // 释放信号量，通知线程有新数据
        rt_sem_release(&spi1_rx_sem);
    }

    return RT_EOK;
}

static void spi_thread_entry(void *param)
{
    while (1)
    {
        // 等待信号量（阻塞，直到 SPI 中断释放信号量）
        rt_sem_take(&spi1_rx_sem, RT_WAITING_FOREVER);

        //rt_kprintf("Received %d bytes: ", spi_rx_len);
        for (int i = 0; i < spi_rx_len; i++)
        {
            if (raw_buf[i] == '@' && raw_buf[spi_rx_len-1] == '$')
            {
                // 提取有效数据（跳过头字段）
                memcpy(valid_data, &raw_buf[i+1], spi_rx_len - i - 2);

                rt_event_send(speak_event, EVENT_CALL_DEEPSEEK);
                //rt_sem_release(&info_sem);
                // 处理数据...
                //rt_kprintf("Valid data: ");
                //for (int j = 0; j < data_len; j++)
                    //rt_kprintf("%02X ", valid_data[j]);
                //rt_kprintf("\n");
                break;
            }
        }

        // 清空缓冲区，准备下一次接收
        spi_rx_len = 0;
    }
}

int spi1_init(void){
    rt_err_t spi_mont;

    struct rt_spi_configuration cfg;
    spi1_dev = rt_device_find("spi1");
    if(!spi1_dev)
    {
        rt_kprintf("spi no find\r\n");
    }
    else {
        rt_kprintf("spi get ready\r\n");
    }
    spi1_dev = (struct rt_spi_device *)rt_malloc(sizeof(struct rt_spi_device));
    spi_mont = rt_spi_bus_attach_device_cspin(spi1_dev,"spi10","spi1",RT_NULL,RT_NULL);
    if(spi_mont != RT_EOK)
    {
        LOG_E("mount spi error\r\n");
    }
    else {
        LOG_E("spi success\r\n");
        cfg.data_width = 8;
        cfg.mode = RT_SPI_SLAVE | RT_SPI_MODE_0 | RT_SPI_MSB;          //低低模式
        cfg.max_hz = 8 *1000 *1000;                                    //10M
        rt_spi_configure(spi1_dev, &cfg);
//        rt_spi_send(spi_device, test, 4);
    }

    // 初始化信号量
    rt_sem_init(&spi1_rx_sem, "spi1_rx_sem", 0, RT_IPC_FLAG_FIFO);
    //rt_sem_init(&info_sem, "info_sem", 0, RT_IPC_FLAG_FIFO);

    // 设置 SPI 接收中断回调
    rt_device_set_rx_indicate(spi1_dev, spi_rx_callback);
    rt_device_control(spi1_dev, RT_DEVICE_CTRL_SET_INT, (void *)RT_DEVICE_FLAG_INT_RX);
    spi1_th = rt_thread_create("spi1_recv", spi_thread_entry, NULL, 1024, 22, 5);
    rt_thread_startup(spi1_th);

    return 0;
}
