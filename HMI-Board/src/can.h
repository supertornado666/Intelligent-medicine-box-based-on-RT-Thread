/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-03-21     廖钟涛       the first version
 */
#ifndef SRC_CAN_TEST_H_
#define SRC_CAN_H_

#include <rtthread.h>
#include "hal_data.h"
#include <rtdevice.h>

extern rt_uint8_t command[9];

rt_uint8_t can_send(rt_uint8_t *p_buff, rt_uint32_t len);

#endif /* SRC_CAN_TEST_H_ */
