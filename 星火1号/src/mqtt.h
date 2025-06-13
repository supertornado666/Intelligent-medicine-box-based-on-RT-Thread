/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-05-05     廖钟涛       the first version
 */
#ifndef SRC_MQTT_H_
#define SRC_MQTT_H_
#include "medication_management.h"
void alarm_on(void);
void identity_mqtt_change(void);
void medicine_mqtt_add(Medicine med);
void aht_mqtt_init(void);

#endif /* SRC_MQTT_H_ */
