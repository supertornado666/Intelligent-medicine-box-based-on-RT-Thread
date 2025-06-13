/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-06     廖钟涛       the first version
 */
#include "infrared.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <stdarg.h>
#include "medication_management.h"

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

//若反了将95行改成FALLING，(可选)并将INFRA_Init()的INPUT改成INPUT_PULLUP
/******根据星火一号实际修改******/
#define INFRA_GRID_1_PIN  GET_PIN(G, 6)
#define INFRA_GRID_2_PIN  GET_PIN(G, 4)
#define INFRA_GRID_3_PIN  GET_PIN(G, 2)
#define INFRA_GRID_4_PIN  GET_PIN(B, 2)
#define INFRA_GRID_5_PIN  GET_PIN(G, 0)
/******根据星火一号实际修改******/

typedef struct {
    rt_base_t pin;
    rt_uint32_t bit;  // 对应事件位
} monitored_pin_t;

// 假设你预先配置的最多8个可用引脚（可根据硬件修改）
static monitored_pin_t all_pins[5] = {
    {INFRA_GRID_1_PIN, 1 << 0},
    {INFRA_GRID_2_PIN, 1 << 1},
    {INFRA_GRID_3_PIN, 1 << 2},
    {INFRA_GRID_4_PIN, 1 << 3},
    {INFRA_GRID_5_PIN, 1 << 4},
};

static struct rt_event infra_event;

static rt_uint32_t target = 0;
void gpio_irq_callback(void *args)
{
    rt_base_t pin = (rt_base_t)args;

    for (int i = 0; i < 5; i++) {
        if (all_pins[i].pin == pin) {
            if (rt_pin_read(pin) == PIN_HIGH) {
                if (target & all_pins[i].bit) {

                    rt_kprintf("find hand on pin[%d]\n", i + 1);
                    rt_event_send(&infra_event, all_pins[i].bit);
                }
                else {
                    // ❗️未授权的引脚也变高了，报警
                    rt_kprintf("[ALERT] Unexpected high level on PIN[%d]\n", i + 1);
                }
            }
            break;
        }
    }
}


void INFRA_Init(void){
    rt_pin_mode(INFRA_GRID_1_PIN, PIN_MODE_INPUT_PULLDOWN);
    rt_pin_mode(INFRA_GRID_2_PIN, PIN_MODE_INPUT_PULLDOWN);
    rt_pin_mode(INFRA_GRID_3_PIN, PIN_MODE_INPUT_PULLDOWN);
    rt_pin_mode(INFRA_GRID_4_PIN, PIN_MODE_INPUT_PULLDOWN);
    rt_pin_mode(INFRA_GRID_5_PIN, PIN_MODE_INPUT_PULLDOWN);
}


int INFRA_Read(int num, ...){
    va_list args;

    va_start(args, num);
    while (num != -1) {
        if (num == 1) target |= (1 << 0);
        else if (num == 2) target |= (1 << 1);
        else if (num == 3) target |= (1 << 2);
        else if (num == 4) target |= (1 << 3);
        else if (num == 5) target |= (1 << 4);

        num = va_arg(args, int);
    }
    va_end(args);

    rt_event_init(&infra_event, "infra_evt", RT_IPC_FLAG_PRIO);

    for (int i = 0; i < 5; i++) {
        rt_pin_attach_irq(all_pins[i].pin, PIN_IRQ_MODE_RISING, gpio_irq_callback, (void *)all_pins[i].pin);
        rt_pin_irq_enable(all_pins[i].pin, PIN_IRQ_ENABLE);
    }

    rt_event_recv(&infra_event,
                  target,
                  RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,
                  RT_WAITING_FOREVER,
                  NULL);
    target = 0;

    for (int i = 0; i < 5; i++) {
        rt_pin_detach_irq(all_pins[i].pin);
    }

    rt_event_detach(&infra_event);

    return 0;
}
void wait_for_medicine_pickup(matched_medicine *list,int count)
{
    // 把所有格子的编号打包传给INFRA_Read
    switch (count) {
        case 1:
            INFRA_Read(list[0].number, -1);
            break;
        case 2:
            INFRA_Read(list[0].number, list[1].number, -1);
            break;
        case 3:
            INFRA_Read(list[0].number, list[1].number, list[2].number, -1);
            break;
        case 4:
            INFRA_Read(list[0].number, list[1].number, list[2].number, list[3].number, -1);
            break;
        case 5:
            INFRA_Read(list[0].number, list[1].number, list[2].number, list[3].number, list[4].number, -1);
            break;
        default:
            rt_kprintf("invalid count\n");
    }
}
