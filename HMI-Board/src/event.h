/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-04-01     廖钟涛       the first version
 */
#ifndef SRC_EVENT_H_
#define SRC_EVENT_H_

#include <rtdef.h>

extern rt_event_t speak_event;
#define EVENT_GET_INFO      (0x1 << 0)
#define EVENT_CALL_DEEPSEEK (0x1 << 1)

int speak_event_init(void);

#endif /* SRC_EVENT_H_ */
