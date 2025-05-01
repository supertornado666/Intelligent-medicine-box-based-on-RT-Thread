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

static rt_thread_t spi1_th;

char rx_buf[400];

static void spi1_thread_entry(void *parameter){

    while (1){
        rt_event_recv(speak_event, EVENT_GET_INFO,
                               RT_EVENT_FLAG_CLEAR |RT_EVENT_FLAG_AND,
                               RT_WAITING_FOREVER, NULL);

        R_SPI_Read(&g_spi1_ctrl, rx_buf, sizeof(rx_buf), SPI_BIT_WIDTH_8_BITS);
    }
}

// 中断回调函数（主机拉高CS时触发）
void irq15_callback(external_irq_callback_args_t * p_args)
{
//    rt_kprintf("%s\n", rx_buf);
//    R_SPI_Read(&g_spi1_ctrl, rx_buf, sizeof(rx_buf), SPI_BIT_WIDTH_8_BITS);
    rt_event_send(speak_event, EVENT_CALL_DEEPSEEK);
}

int spi1_init(void){
    if (R_SPI_Open(&g_spi1_ctrl, &g_spi1_cfg) != FSP_SUCCESS) rt_kprintf("spi1 fail\n");
    R_SPI_Read(&g_spi1_ctrl, rx_buf, sizeof(rx_buf), SPI_BIT_WIDTH_8_BITS);

//    rt_pin_mode(SPI_CS_PIN, PIN_MODE_INPUT_PULLUP); // ⭐ 输入 + 上拉
//    rt_pin_attach_irq(SPI_CS_PIN, PIN_IRQ_MODE_RISING, irq15_callback, RT_NULL);
//    rt_pin_irq_enable(SPI_CS_PIN, PIN_IRQ_ENABLE);

    spi1_th = rt_thread_create("spi1_recv", spi1_thread_entry, NULL, 1024, 22, 5);
    rt_thread_startup(spi1_th);

    return 0;
}
