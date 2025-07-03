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
static int medicine_count = 0;

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
int add_medicine(const char *bar_code, int times_per_day, int amount);
int del_medicine(int number);
// find_medicine 函数原型
matched_medicine* find_medicine(const char *time, int *found_count);

// get_medicine_info 函数原型
Medicine* get_medicine_info(int *count);


#endif /* SRC_MEDICATION_MANAGEMENT_H_ */
// 3. 查找在 12:00 时需要服用的药物
/*int found = 0;
matched_medicine *m_list = find_medicine("12:00", &found);
rt_kprintf("---- 12:00 had (%d ) ----\n", found);
rt_thread_mdelay(3);
for (int i = 0; i < found; i++) {
rt_kprintf("  name:%s number: %d time:%s amount:%d\n",
                                    m_list[i].name,
                                    m_list[i].number,
                                    m_list[i].time,
                                    m_list[i].amount
                                    );
rt_thread_mdelay(3);
                     }

free(m_list);

                     // 4. 删除格子号为 2 的药物
                     if (del_medicine(2) == 0) {
                         rt_kprintf("have deleted 2 \n");
                         rt_thread_mdelay(3);}
                     if (del_medicine(1) == 0) {
                         rt_kprintf("have deleted 1 \n");
                         rt_thread_mdelay(3);}
                     if (del_medicine(3) == 0) {
                         rt_kprintf("have deleted 3 \n");
                         rt_thread_mdelay(3);}

                     // 5. 再次打印当前药物列表
                     info = get_medicine_info(&count);
                     rt_kprintf("---- remain (%d ) ----\n", count);
                     rt_thread_mdelay(3);
                     for (int i = 0; i < count; i++) {
                         rt_kprintf("name:%s  %d /day,%d /having number:%d\n",
                                    //i,
                                    info[i].name,
                                    info[i].times_per_day,
                                    info[i].amount,
                                    info[i].number);
                         rt_thread_mdelay(3);
                     }*/
