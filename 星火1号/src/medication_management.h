/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-04-25     廖钟涛       the first version
 */
#ifndef SRC_MEDICATION_MANAGEMENT_H_
#define SRC_MEDICATION_MANAGEMENT_H_
/*
typedef struct medicine_info {
    char name[20];          //药名
    char take_time[3][6];   //服用时间，最多一天3次
    int amount;             //单次用药量
    int taken_times;        //已服用次数
    int number;             //对应放入格子序号

    struct medicine_info *next;
} medicine_info;*/
// 用于返回匹配时间的临时结构
#include <rtthread.h>   // rt_kprintf、rt_snprintf
#include <rtdevice.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_MEDICINE 5

// 核心药物信息结构体（在 .c 里存储）
typedef struct {
    char bar_code[20];
    char name[50];
    char take_time[5][6];
    int times_per_day;
    int amount;
    int number;
} Medicine;

// 用于 find_medicine 返回
typedef struct {
    char name[50];
    char time[6];
    int amount;
    int number;
} matched_medicine;

// 用于 get_medicine_info 返回
typedef struct {
    char name[50];
    int times_per_day;
    int amount;
    int number;
} MedicineDisplayInfo;

int find_blank(void);
int add_medicine(const char *bar_code, int times_per_day, int amount, int number);
int del_medicine(int number);
// find_medicine 函数原型
matched_medicine* find_medicine(const char *time, int *found_count);

// get_medicine_info 函数原型
MedicineDisplayInfo* get_medicine_info(int *count);


#endif /* SRC_MEDICATION_MANAGEMENT_H_ */
