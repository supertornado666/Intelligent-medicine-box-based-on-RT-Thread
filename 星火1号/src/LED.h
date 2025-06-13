/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-06     廖钟涛       the first version
 */
#ifndef APPLICATIONS_LED_H_
#define APPLICATIONS_LED_H_

void led_init(void);
void led_on_all(void);
void led_off_all(void);
//将需要点亮的格子序号输入，以-1结尾
void led_on_st(int number);
//将需要熄灭的格子序号输入，以-1结尾
void led_off_st(int number);

#endif /* APPLICATIONS_LED_H_ */
