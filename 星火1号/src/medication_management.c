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
// 最大支持药品数量
#define MAX_MEDICINE 5

// 条码——药名映射表
const char *code_name_list[][2] = {
    {"6938588802324", "a_mo_xi_lin"}
};

// 核心药物信息结构体


static Medicine medicine_list[MAX_MEDICINE];
static int medicine_count = 0;

// 默认时间表
const char *default_time[][5] = {
    {},
    {"08:00"},
    {"08:00","20:00"},
    {"07:00","12:00","19:00"},
    {"07:00","12:00","17:00","22:00"},
    {"07:00","10:00","13:00","16:00","19:00"}
};

static char number_list[5] = {0, 0, 0, 0, 0};

int find_blank(void){
    for (int i = 0; i < 5; i++){
        if (number_list[i] == 0){
            return i+1;
        }
    }

    return 0;
}

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
int add_medicine(const char *bar_code, int times_per_day, int amount, int number)
{
    if (medicine_count >= MAX_MEDICINE || times_per_day < 1 || times_per_day > 5) {
        rt_kprintf("add_medicine_fail\n");
        return -1;
    }

    // 查找药名
    const char *med_name = "unknown_medicine";
    int map_len = sizeof(code_name_list)/sizeof(code_name_list[0]);
    for (int i = 0; i < map_len; i++) {
        if (strcmp(code_name_list[i][0], bar_code) == 0) {
            med_name = code_name_list[i][1];
            break;
        }
    }
    rt_kprintf("name：%s\n", med_name);

    // 填充数组
    Medicine *m = &medicine_list[medicine_count];
    snprintf(m->bar_code, sizeof(m->bar_code), "%s", bar_code);
    snprintf(m->name,     sizeof(m->name),     "%s", med_name);
    m->times_per_day = times_per_day;
    m->amount = amount;
    m->number = number;

    // 设置默认时间
    for (int i = 0; i < times_per_day; i++) {
        snprintf(m->take_time[i], sizeof(m->take_time[i]), "%s", default_time[times_per_day][i]);
    }
    medicine_count++;
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
        rt_kprintf("No medicines to delete.\n");
        return -1;
    }

    for (int i = 0; i < medicine_count; i++) {
        if (medicine_list[i].number == number) {
            // 后面的往前移
            for (int j = i; j < medicine_count - 1; j++) {
                medicine_list[j] = medicine_list[j+1];
            }
            medicine_count--;
            rt_kprintf("delete ok\n");
            return 0;
        }
    }

    rt_kprintf("found_fail\n");
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
        rt_kprintf("Memory allocation failed\n");
        *found_count = 0;
        return NULL;
    }

    int cnt = 0;
    for (int i = 0; i < medicine_count; i++) {
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

    for (int i = 0; i < medicine_count; i++) {
        snprintf(list[i].name, sizeof(list[i].name), "%s", medicine_list[i].name);
        list[i].times_per_day = medicine_list[i].times_per_day;
        list[i].amount = medicine_list[i].amount;
        list[i].number = medicine_list[i].number;
    }

    *count = medicine_count;
    return list;
}

