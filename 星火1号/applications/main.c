/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-5-10      ShiHao       first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "can.h"
#include "commands_def.h"
#include "wlan.h"
#include "rtc.h"
#include "uart2.h"
#include "uart3.h"
#include "uart4.h"
#include "spi1.h"

#define DBG_TAG "main"
#define DBG_LVL         DBG_LOG
#include <rtdbg.h>

time_t now;

int main(void)
{
    rtc_init();
    uart2_init();
    uart3_init();
    uart4_init();
    can_init();
    spi1_init();
    wifi_init();

    char tx_buf[400] = "@message from master$";
    int len = strlen(tx_buf);  // 实际内容长度
    memset(tx_buf + len, 'F', sizeof(tx_buf) - len - 1);
    tx_buf[399] = '\0';
    while (1)
    {
        spi1_write(tx_buf, sizeof(tx_buf));
        rt_thread_mdelay(2000);
    }

    return 0;
}

