/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-04-24     huge       the first version
 */
#ifndef SRC_UART3_H_
#define SRC_UART3_H_

#include <board.h>

#define FINGER_PIN  GET_PIN(D, 12)

rt_device_t u3_dev;

int uart3_init(void);
uint8_t Med_Zw101_IdentifyFinger (void);
//uint8_t Med_Zw101_AddFinger_Full(uint8_t id);
uint8_t Med_Zw101_DeleteFinger (uint8_t id);
int zw101_add_fingerprint_auto(rt_uint16_t id);
#endif /* SRC_UART2_H_ */
