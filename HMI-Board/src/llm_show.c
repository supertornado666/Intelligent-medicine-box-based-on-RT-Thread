/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-03-29     廖钟涛       the first version
 */
#include "llm_show.h"
#include <rtthread.h>
#include "demo/ui/ui.h"
#include "syn8086.h"

#define MAX_LINES 3
#define CHARS_PER_LINE 15

volatile rt_bool_t speech_thread_running = RT_FALSE;

static char full_response[130];
static int response_offset = 0;

static char playback_queue[PLAYBACK_QUEUE_SIZE][PLAYBACK_SEGMENT_LEN];
int playback_queue_head = 0;
int playback_queue_tail = 0;
//static rt_mutex_t playback_queue_mutex;

extern char u4_buf[16];
struct rt_semaphore voice_sem;

rt_thread_t voice_td;

// 每次格式化最多显示 3 行 15 字符，并返回这次处理了多少字节
static int format_multiline_text(const char *src, char *dst, int max_lines, int chars_per_line) {
    int lines = 0, chars = 0;
    const char *p = src;
    char *d = dst;
    const char *start = src;

    while (*p && lines < max_lines) {
        // 过滤掉文本中的 \\n
        if (*p == '\\' && *(p + 1) == 'n') {
            p += 2;
            continue;
        }

        // 跳过连续两个星号 **
        if (*p == '*' && *(p + 1) == '*') {
            p += 2;
            continue;
        }

        // 处理ASCII字符
        if ((unsigned char)*p < 0x80) {
            *d++ = *p++;
            chars++;
        }
        // 处理UTF-8三字节中文
        else if ((unsigned char)*p >= 0xE0) {
            if (*(p + 1) && *(p + 2)) {
                *d++ = *p++;
                *d++ = *p++;
                *d++ = *p++;
                chars++;
            } else {
                break; // 防止越界
            }
        }
        // 其他非法或不支持编码，跳过
        else {
            p++;
        }

        if (chars == chars_per_line) {
            *d++ = '\n';
            chars = 0;
            lines++;
        }
    }

    *d = '\0';
    return (int)(p - start);
}



// 分段更新 label 的回调函数
static void update_label_cb(void *param) {
    char formatted_text[130];

    int step = format_multiline_text(full_response + response_offset, formatted_text, MAX_LINES, CHARS_PER_LINE);

    rt_sem_take(&voice_sem, RT_WAITING_FOREVER);
    rt_thread_mdelay(2000);
    SYN_FrameInfo(formatted_text);
    backlight_on();
    lv_label_set_text(ui_responsetext, formatted_text);
    //rt_thread_delay(2);

    if (step > 0) {
        response_offset += step;

        // 如果还有剩余文本，延迟继续显示下一段
        if (response_offset < strlen(full_response)) {
            lv_timer_t *timer = lv_timer_create_basic();
            //lv_timer_set_period(timer, 5000); // 每 3 秒更新一次
            lv_timer_set_repeat_count(timer, 1); // 执行一次
            backlight_on();
            lv_timer_set_cb(timer, update_label_cb);
        } else {
            response_offset = 0; // 显示完毕，重置偏移量
            //rt_thread_mdelay(5000);
            //_ui_screen_change(&ui_mainscreen, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_mainscreen_screen_init);
        }
    }
    /*
    else{
        response_offset = 0; // 显示完毕，重置偏移量
        rt_thread_mdelay(5000);
        _ui_screen_change(&ui_mainscreen, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_mainscreen_screen_init);
    }
    */
}

// 初始化显示（首次调用）
void show_response_segmented(const char *text) {
    rt_strncpy(full_response, text, sizeof(full_response) - 1);
    full_response[sizeof(full_response) - 1] = '\0';
    response_offset = 0;
    update_label_cb(NULL);
}

/* 入队函数 */
rt_bool_t enqueue_to_playback_queue(const char *segment)
{
    //rt_mutex_take(playback_queue_mutex, RT_WAITING_FOREVER);

    int next_tail = (playback_queue_tail + 1) % PLAYBACK_QUEUE_SIZE;
    if (next_tail == playback_queue_head)
    {
        // 队列满，丢弃或阻塞等待
        //rt_mutex_release(playback_queue_mutex);
        rt_kprintf("Playback queue full, dropping segment!\n");
        return RT_FALSE;
    }

    rt_strncpy(playback_queue[playback_queue_tail], segment, PLAYBACK_SEGMENT_LEN - 1);
    playback_queue[playback_queue_tail][PLAYBACK_SEGMENT_LEN - 1] = '\0';
    playback_queue_tail = next_tail;

    //rt_mutex_release(playback_queue_mutex);
    return RT_TRUE;
}

/* 出队函数 */
rt_bool_t dequeue_from_playback_queue(char *buffer, int buf_len)
{
    //rt_mutex_take(playback_queue_mutex, RT_WAITING_FOREVER);

    if (playback_queue_head == playback_queue_tail)
    {
        // 队列空
        //rt_mutex_release(playback_queue_mutex);
        return RT_FALSE;
    }

    rt_strncpy(buffer, playback_queue[playback_queue_head], buf_len - 1);
    buffer[buf_len - 1] = '\0';
    playback_queue_head = (playback_queue_head + 1) % PLAYBACK_QUEUE_SIZE;

    //rt_mutex_release(playback_queue_mutex);
    return RT_TRUE;
}

static void speech_playback_thread(void *parameter)
{
    char segment[PLAYBACK_SEGMENT_LEN];

    while (speech_thread_running)
    {
        if (dequeue_from_playback_queue(segment, sizeof(segment)))
        {
            //rt_thread_mdelay(6000);
            // 发送语音串口数据
            show_response_segmented(segment);
        }
        else
        {
            rt_thread_mdelay(6000);  // 队列空，短暂休眠
        }
    }
}

void speech_system_init(void)
{
    //playback_queue_mutex = rt_mutex_create("pb_queue", RT_IPC_FLAG_FIFO);
//    voice_td = rt_thread_create("speech_play",
//                                   speech_playback_thread,
//                                   RT_NULL,
//                                   4096, 28, 1);
//    rt_thread_startup(voice_td);
}

void start_show(void){
    speech_thread_running = RT_TRUE;

    rt_sem_init(&voice_sem, "vc_sem", 0, RT_IPC_FLAG_FIFO);

    voice_td = rt_thread_create("speech_play",
                               speech_playback_thread,
                               RT_NULL,
                               3584, 20, 10);
    rt_thread_startup(voice_td);
    rt_sem_release(&voice_sem);
}


