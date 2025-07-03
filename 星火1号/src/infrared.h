/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-06     廖钟涛       the first version
 */

#ifndef APPLICATIONS_INFRARED_H_
#define APPLICATIONS_INFRARED_H_
#include "medication_management.h"
void INFRA_Init(void);
int INFRA_Read(int num, ...);
void wait_for_medicine_pickup(matched_medicine *list,int count);
void text();

#endif /* APPLICATIONS_INFRARED_H_ */
