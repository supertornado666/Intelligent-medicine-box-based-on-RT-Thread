/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-03-15     廖钟涛       the first version
 */
#ifndef SRC_UART1_H_
#define SRC_UART1_H_

#include <rtthread.h>
#include <rtdevice.h>
#include "hal_data.h"

extern rt_device_t u1_dev;

rt_uint8_t uart1_init(void);

#endif /* SRC_UART1_H_ */
