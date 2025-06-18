/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-04-25     廖钟涛       the first version
 */
#include "medication_management.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "rtc.h"
#include "led.h"
#include "check_take_medicine.h"
// 最大支持药品数量
#define MAX_MEDICINE 5
static int alarm_set[10]={0};
//0-7:00  1-8:00  2-10:00  3-12:00  4-13:00  5-16:00  6-17:00  7-19:00  8-20:00  9-22:00
// 条码——药名映射表
const char *code_name_list[][2] = {
    {"6938588802324", "阿莫西林胶囊"},
    {"6938588802325", "test1"},
    {"6938588802326", "test2"},
    {"6971779430036", "孟鲁斯特"},
    {"6906952090028", "芙朴感冒颗粒"}
};

// 核心药物信息结构体


static Medicine medicine_list[MAX_MEDICINE];


// 默认时间表
const char *default_time[][5] = {

    {"08:00"},
    {"08:00","20:00"},
    {"07:00","12:00","19:00"},
    {"07:00","12:00","17:00","22:00"},
    {"07:00","10:00","13:00","16:00","19:00"}
};

/***********************************************************
* 名    称： add_medicine
* 功    能： 添加药物到数组，最多 MAX_MEDICINE 个
* 入口参数： bar_code: 条形码
*          times_per_day: 一天几次（1~5）
*          amount: 单次用药量
*          number: 存放格子序号
* 出口参数： 成功返回0，失败返回-1
* 说    明： 查不到条码时，药名为 "未知药物"
***********************************************************/
int add_medicine(const char *bar_code, int times_per_day, int amount)
{
    if (medicine_count >= MAX_MEDICINE ||times_per_day < 1 || times_per_day > 5) {
        rt_kprintf("add_medicine_fail\n");
        rt_thread_mdelay(3);
        return -1;
    }

    // 查找药名
    const char *med_name = "未知药物";
    int map_len = sizeof(code_name_list)/sizeof(code_name_list[0]);
    for (int i = 0; i < map_len; i++) {
        if (strcmp(code_name_list[i][0], bar_code) == 0) {
            med_name = code_name_list[i][1];
            break;
        }
    }
    rt_kprintf("name：%s\n", med_name);rt_thread_mdelay(3);

    // 填充数组
    Medicine *m = &medicine_list[medicine_count];
    snprintf(m->bar_code, sizeof(m->bar_code), "%s", bar_code);
    snprintf(m->name,     sizeof(m->name),     "%s", med_name);
    m->times_per_day = times_per_day;
    m->amount = amount;
    medicine_count++;
    m->number =medicine_count;
    //led_on_st(medicine_count);
    rt_kprintf("add to  %d",medicine_count);
    //servo_set_angle(0);
    // 设置默认时间
    for (int i = 0; i < times_per_day; i++) {
        snprintf(m->take_time[i], sizeof(m->take_time[i]), "%s", default_time[times_per_day-1][i]);
        alarm_add(m->take_time[i]);
    }
    medicine_mqtt_add(*m);
    //if(rt_pin_read(FINGER_PIN) == PIN_HIGH)
    //{
     //   led_off_all();
      //  servo_set_angle(0);
    //}
    return 0;
}


/***********************************************************
* 名    称： del_medicine
* 功    能： 从数组中删除指定格子号的药物
* 入口参数： number: 要删除的药物格子号
* 出口参数： 成功返回0，失败返回-1
***********************************************************/
int del_medicine(int number)
{
    if (medicine_count == 0) {
        rt_kprintf("No medicines to delete.\n");rt_thread_mdelay(3);
        return -1;
    }

    for (int i = 0; i < medicine_count; i++) {
        if (medicine_list[i].number == number) {
            // 后面的往前移
            for (int j = i; j < medicine_count -1; j++) {
                medicine_list[j] = medicine_list[j+1];
            }
            medicine_count--;
            have_take[number - 1]=0;
            rt_kprintf("delete ok\n");rt_thread_mdelay(3);
            return 0;
        }
    }

    rt_kprintf("found_fail\n");rt_thread_mdelay(3);
    return -1;
}



/***********************************************************
* 名    称： find_medicine
* 功    能： 查找在指定时间需要服用的药物
* 入口参数： time: 时间字符串 "HH:MM"
*          found_count: 输出找到的数量
* 出口参数： 动态分配的 matched_medicine 数组指针
* 说    明： 调用者用完后需 free()
***********************************************************/
matched_medicine* find_medicine(const char *time, int *found_count)
{
    matched_medicine *res = malloc(MAX_MEDICINE * sizeof(matched_medicine));
    if (!res) {
        rt_kprintf("Memory allocation failed\n");rt_thread_mdelay(3);
        *found_count = 0;
        return NULL;
    }

    int cnt = 0;
    for (int i = 0; i <medicine_count; i++) {
        Medicine *m = &medicine_list[i];
        for (int j = 0; j < m->times_per_day; j++) {
            if (strcmp(m->take_time[j], time) == 0) {
                // 填充匹配结果
                snprintf(res[cnt].name, sizeof(res[cnt].name), "%s", m->name);
                snprintf(res[cnt].time, sizeof(res[cnt].time), "%s", m->take_time[j]);
                res[cnt].amount = m->amount;
                res[cnt].number = m->number;

                cnt++;
                break;
            }
        }
    }

    *found_count = cnt;
    return res;
}

// 用于展示的结构体


/***********************************************************
* 名    称： get_medicine_info
* 功    能： 获取所有药物的展示信息
* 入口参数： count: 输出药物总数
* 出口参数： 指向静态数组的指针，无需 free()
***********************************************************/
MedicineDisplayInfo* get_medicine_info(int *count)
{
    static MedicineDisplayInfo list[MAX_MEDICINE];

    for (int i = 0; i <medicine_count; i++) {
        snprintf(list[i].name, sizeof(list[i].name), "%s", medicine_list[i].name);
        list[i].times_per_day = medicine_list[i].times_per_day;
        list[i].amount = medicine_list[i].amount;
        list[i].number = medicine_list[i].number;
    }

    *count = medicine_count;
    return list;
}
// 2. 获取并打印所有药物的展示信息
                             /*   int count=0;
                                MedicineDisplayInfo *info = get_medicine_info(&count);
                                rt_kprintf("------  (%d ) ------\n", count);
                                rt_thread_mdelay(3);
                                for (int i = 0; i < count; i++) {
                                    rt_kprintf(" number:%d ,name %s,times_per_day %d,amount %d\n",
                                               info[i].number,
                                               info[i].name,
                                               info[i].times_per_day,
                                               info[i].amount
                                               );

                                    rt_thread_mdelay(3);
                                }*/
/*matched_medicine *m_list = find_medicine("08:00", &count);
      rt_kprintf("-------08:00  %d-------\n",count);*/
