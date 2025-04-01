/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-03-15     廖钟涛       the first version
 */
#ifndef SRC_UART4_H_
#define SRC_UART4_H_

#include <rtdevice.h>

extern rt_device_t u4_dev;
//extern char command[8];

int uart4_init(void);

#endif /* SRC_UART4_H_ */
