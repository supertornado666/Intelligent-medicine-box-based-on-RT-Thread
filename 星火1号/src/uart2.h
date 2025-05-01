/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-04-24     huge       the first version
 */
#ifndef SRC_UART2_H_
#define SRC_UART2_H_

rt_device_t u2_dev;
extern struct rt_semaphore pill_freq;

int uart2_init(void);

#endif /* SRC_UART2_H_ */
