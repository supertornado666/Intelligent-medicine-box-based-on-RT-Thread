/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-04-20     廖钟涛       the first version
 */
#ifndef APPLICATIONS_SYN6288_H_
#define APPLICATIONS_SYN6288_H_

#include <rtthread.h>
#include "hal_data.h"
#include <rtdevice.h>

extern rt_device_t u4_dev;

#define BUSY_PIN    BSP_IO_PORT_00_PIN_08

void SYN_FrameInfo(const char *HZdata);
void YS_SYN_Set(uint8_t *Info_data);

#endif /* APPLICATIONS_SYN6288_H_ */
