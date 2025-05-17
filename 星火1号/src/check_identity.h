/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-05-17     廖钟涛       the first version
 */
#ifndef SRC_CHECK_IDENTITY_H_
#define SRC_CHECK_IDENTITY_H_

#include <rtthread.h>

extern rt_thread_t id_th;

int check_identity_start(void);

#endif /* SRC_CHECK_IDENTITY_H_ */
