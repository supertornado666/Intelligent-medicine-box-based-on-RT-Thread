/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-04-24     廖钟涛       the first version
 */
#ifndef SRC_RTC_H_
#define SRC_RTC_H_

#define RTC_NAME       "rtc"

int rtc_init();
int alarm_add(char *tim);
void alarm_delete(char *tim);
void alarm_clear_all(void);
void set_screen_time(void);

#endif /* SRC_RTC_H_ */
