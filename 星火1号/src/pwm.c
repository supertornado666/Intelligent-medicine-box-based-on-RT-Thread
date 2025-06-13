/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-05-24     huge       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <pwm.h>
#define PWM_DEV_NAME       "pwm1"     /* PWM 设备名 */
#define PWM_DEV_CHANNEL    4          /* PWM 通道 */

#define SERVO_MIN_PULSE    1000000      /* 实测最小脉宽：800us = 0° */
#define SERVO_MAX_PULSE    5000000     /* 实测最大脉宽：2200us = 180° */
#define SERVO_PERIOD       20000000   /* 舵机 PWM 周期：20ms */

static struct rt_device_pwm *pwm_dev = RT_NULL;
static int current_angle = 0;
/* 初始化 PWM 设备 */
void servo_pwm_init(void)
{
    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
    if (pwm_dev == RT_NULL)
    {
        rt_kprintf("Cannot find %s device\n", PWM_DEV_NAME);
        return;
    }

    rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, SERVO_PERIOD, SERVO_MIN_PULSE); // 初始为0°
    rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);
}

/* 设置舵机角度（0~180°） */
void servo_set_angle(int angle)
{
    rt_uint32_t pulse;

    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;


    pulse = SERVO_MIN_PULSE + ((SERVO_MAX_PULSE - SERVO_MIN_PULSE) * angle) / 180;
    current_angle=angle;

    rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, SERVO_PERIOD, pulse);
}
void servo_toggle(void)//每次调用都使角度在180到0之间反转
{


    if (current_angle == 0)
        current_angle = 180;
    else
        current_angle = 0;

    servo_set_angle(current_angle);

}

