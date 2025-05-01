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

//条码药名对应列表
const char *code_name_list[][2] = {
     {"6938588802324", "阿莫西林胶囊"}
};

typedef struct medicine_info {
    char name[20];          //药名
    char take_time[3][6];   //服用时间，最多一天3次
    int amount;             //单次用药量
    int taken_times;        //已服用次数
    int number;             //对应放入格子序号

    struct medicine_info *next;
} medicine_info;

int add_medicine(char *bar_code, int times_per_day, int amount);
int del_medicine(int number);
medicine_info *find_medicine(char *time);
char ***get_medicine_info(void);

#endif /* SRC_MEDICATION_MANAGEMENT_H_ */
