/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-03-29     廖钟涛       the first version
 */
#ifndef SRC_LLM_SHOW_H_
#define SRC_LLM_SHOW_H_

#include <rtthread.h>

#define PLAYBACK_QUEUE_SIZE 20     // 最多缓存10段语音
#define PLAYBACK_SEGMENT_LEN 130   // 每段最多128字符

void show_response_segmented(const rt_uint8_t *text);
rt_bool_t enqueue_to_playback_queue(const rt_uint8_t *segment);
void speech_system_init(void);
void start_show(void);

#endif /* SRC_LLM_SHOW_H_ */
