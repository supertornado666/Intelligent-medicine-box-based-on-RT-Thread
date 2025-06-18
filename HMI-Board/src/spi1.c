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

//static rt_thread_t spi1_th;
static struct rt_spi_device *spi1_dev = RT_NULL;
rt_bool_t f = 1;

char rx_buf[500];
extern char m_buf[500];

//static void spi1_thread_entry(void *parameter){
//
//    while (1){
//        rt_event_recv(speak_event, EVENT_GET_INFO,
//                               RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,
//                               RT_WAITING_FOREVER, NULL);
//
//        if (f) {
//            while (rt_pin_read(SPI_CS_PIN) == PIN_HIGH); //rt_thread_mdelay(1);
//            while (rt_pin_read(SPI_CS_PIN) == PIN_LOW); //rt_thread_mdelay(1);
//        }
//        rt_kprintf("%s\n", rx_buf);
////        rt_strcpy(m_buf, rx_buf);
////        rx_buf[0] = '\0';
//        //R_SPI_Read(&g_spi1_ctrl, rx_buf, 400, SPI_BIT_WIDTH_8_BITS);
//        rt_spi_recv(spi1_dev, rx_buf, 400);
//        if (f){
//            f = 0;
//        }
//        else{
//            rt_event_send(speak_event, EVENT_CALL_DEEPSEEK);
//        }
//        rt_thread_mdelay(1000);
//    }
//}

// 中断回调函数（主机拉高CS时触发）
void irq15_callback(external_irq_callback_args_t * p_args)
//void SPI_callback(void *args)
{

//    if (rt_pin_read(SPI_CS_PIN) == PIN_LOW){
//        rt_spi_recv(spi1_dev, rx_buf, 500);
//        //R_SPI_Read(&g_spi1_ctrl, rx_buf, 500, SPI_BIT_WIDTH_8_BITS);
//        rt_kprintf("ready to read\n");
//    }
//    else{
//        rt_kprintf("%s\n", rx_buf);
//        rx_buf[0] = '\0';
//    }
//    rt_interrupt_enter();
//    //volatile int time = 10000000;
//    if (15 == p_args->channel){
        rt_kprintf("%s\n", rx_buf);
        //rt_strcpy(m_buf, rx_buf);
        //rt_kprintf("%s\n", m_buf);
        rx_buf[0] = '\0';
        //while (time--);
        //memset(rx_buf, 0, sizeof(rx_buf));
        R_SPI_Read(&g_spi1_ctrl, rx_buf, 500, SPI_BIT_WIDTH_8_BITS);
//        if (f) {
//            f = 0;
//            //rx_buf[0] = '\0';
//            //R_SPI_Read(&g_spi1_ctrl, rx_buf, 400, SPI_BIT_WIDTH_8_BITS);
//    //        R_BSP_IrqClearPending(ICU_IRQ15_IRQn);
//    //        rt_interrupt_leave();
//    //        return;
//        }
//        else{
//            //rt_event_send(speak_event, EVENT_CALL_DEEPSEEK);
//        }
//    }
//    rt_interrupt_leave();
}

int spi1_init(void){
    if (R_SPI_Open(&g_spi1_ctrl, &g_spi1_cfg) != FSP_SUCCESS) rt_kprintf("spi1 fail\n");

//    R_ICU_ExternalIrqOpen(&g_external_irq15_ctrl, &g_external_irq15_cfg);
////    R_ICU_ExternalIrqCallbackSet(&g_external_irq15_ctrl, irq15_callback,
////                                    NULL, NULL);
//    R_ICU_ExternalIrqEnable(&g_external_irq15_ctrl);

    rx_buf[0] = '\0';
    R_SPI_Read(&g_spi1_ctrl, rx_buf, 500, SPI_BIT_WIDTH_8_BITS);

    rt_pin_mode(SPI_CS_PIN, PIN_MODE_INPUT_PULLUP); // ⭐ 输入 + 上拉
    rt_pin_attach_irq(SPI_CS_PIN, PIN_IRQ_MODE_RISING, irq15_callback, RT_NULL);
    rt_pin_irq_enable(SPI_CS_PIN, PIN_IRQ_ENABLE);
//    R_IOPORT_PinCfg(&g_ioport_ctrl, SPI_CS_PIN, IOPORT_CFG_PORT_DIRECTION_INPUT |
//                                                IOPORT_CFG_PULLUP_ENABLE);

//    static rt_bool_t initialnized = RT_FALSE;
//    rt_err_t err;
//    if (!initialnized) {
//        initialnized = RT_TRUE;
//        err = rt_hw_spi_device_attach("spi1", "spi10", BSP_IO_PORT_02_PIN_05);
//        if (err) {
//            rt_kprintf("attach device error\r\n");
//            return err;
//        }
//    }
//    spi1_dev = (struct rt_spi_device *) rt_device_find("spi10");
//    if (spi1_dev == RT_NULL) {
//        rt_kprintf("find %s error\r\n", "spi10");
//        return err;
//    }
//    struct rt_spi_configuration cfg = {
//            .mode = RT_SPI_SLAVE | RT_SPI_MODE_0 | RT_SPI_MSB,
//            .data_width = 8,
//            .max_hz = 1 * 1000
//    };
//    err = rt_spi_configure(spi1_dev, &cfg);
//    if (err != RT_NULL) {
//        rt_kprintf("spi configurate error\r\n");
//        return err;
//    }
//
//    rt_spi_recv(spi1_dev, rx_buf, 500);

//    spi1_th = rt_thread_create("spi1_recv", spi1_thread_entry, NULL, 2048, 22, 10);
//    rt_thread_startup(spi1_th);

    return 0;
}
