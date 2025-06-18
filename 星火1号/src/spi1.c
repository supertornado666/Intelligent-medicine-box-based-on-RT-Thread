/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-04-26     廖钟涛       the first version
 */
#include <rtthread.h>
#include "drv_spi.h"
#include "spi1.h"

static struct rt_spi_device *spi1_dev = RT_NULL;
char tx_buf[400];

/**
 * @brief  interface spi bus init
 * @return status code
 *         - 0 success
 *         - 1 spi init failed
 * @note   none
 */
uint8_t spi1_init(void) {
    static rt_bool_t initialnized = RT_FALSE;
    rt_err_t err;
    if (!initialnized) {
        initialnized = RT_TRUE;
        err = rt_hw_spi_device_attach("spi1", "spi10", GPIOA, GPIO_PIN_4);
        if (err) {
            rt_kprintf("attach device error\r\n");
            return err;
        }
    }
    spi1_dev = (struct rt_spi_device *) rt_device_find("spi10");
    if (spi1_dev == RT_NULL) {
        rt_kprintf("find %s error\r\n", "spi10");
        return err;
    }
    struct rt_spi_configuration cfg = {
            .mode = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB,
            .data_width = 8,
            .max_hz = 1 * 1000
    };
    err = rt_spi_configure(spi1_dev, &cfg);
    if (err != RT_NULL) {
        rt_kprintf("spi configurate error\r\n");
        return err;
    }

    return 0;
}

/**
 * @brief     interface spi bus write
 * @param[in] *buf points to a data buffer
 * @param[in] len is the length of data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t spi1_write(uint8_t *buf, uint16_t len) {
    int s_len = strlen(buf);  // 实际内容长度
    memset(buf + s_len, 'F', len - s_len - 1);
    buf[499] = '\0';
    //rt_kprintf("%s\n", buf);

//    uint8_t send_buf[len];
//
//    // 初始化发送缓冲区
//    for (uint16_t i = 0; i < len; i++) {
//        send_buf[i] = buf[i];
//    }

    rt_kprintf("%s\n", buf);
    //rt_pin_write(SPI_CS_PIN, PIN_LOW);
    //rt_thread_mdelay(500);
    rt_spi_send(spi1_dev, buf, len);
    //rt_thread_mdelay(500);
    //rt_pin_write(SPI_CS_PIN, PIN_HIGH);
//    rt_pin_mode(GET_PIN(A, 7), PIN_MODE_OUTPUT);
//    rt_pin_write(GET_PIN(A, 7), PIN_HIGH);
//    rt_pin_write(GET_PIN(A, 7), PIN_LOW);
//    spi1_init();

    return 0;
}

uint8_t spi1_read(uint8_t *buf, uint16_t len) {
    rt_spi_recv(spi1_dev, buf, len);

    return 0;
}
