/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-05-10     廖钟涛       the first version
 */
#ifndef SRC_MEDICATION_PROCESS_H_
#define SRC_MEDICATION_PROCESS_H_

#include <rtdef.h>

extern rt_event_t medication_event;
#define EVENT_CHECK_IDENTITY        (0x1 << 0)
#define EVENT_TAKE_MEDICINE_END     (0x1 << 1)
#define EVENT_BOX_CLOSED            (0x1 << 2)

int medication_event_init(void);

#endif /* SRC_MEDICATION_PROCESS_H_ */
