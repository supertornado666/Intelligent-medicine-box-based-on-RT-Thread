/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-05-25     huge       the first version
 */
#ifndef SRC_PWM_H_
#define SRC_PWM_H_
void servo_pwm_init(void);
void servo_set_angle(int angle);
void servo_toggle(void);
#endif /* SRC_PWM_H_ */
