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

#define MAX_LINES 3
#define CHARS_PER_LINE 15
static char full_response[PKG_WEB_SORKET_BUFSZ];
static int response_offset = 0;

extern struct rt_semaphore show_sem;

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

        if ((unsigned char)*p < 0x80) {
            *d++ = *p++;
            chars++;
        } else if ((unsigned char)*p >= 0xE0) {
            if (*(p + 1) && *(p + 2)) {
                *d++ = *p++;
                *d++ = *p++;
                *d++ = *p++;
                chars++;
            } else {
                break;
            }
        } else {
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
    char formatted_text[PKG_WEB_SORKET_BUFSZ];

    int step = format_multiline_text(full_response + response_offset, formatted_text, MAX_LINES, CHARS_PER_LINE);
    lv_label_set_text(ui_responsetext, formatted_text);

    if (step > 0) {
        response_offset += step;

        // 如果还有剩余文本，延迟继续显示下一段
        if (response_offset < strlen(full_response)) {
            lv_timer_t *timer = lv_timer_create_basic();
            lv_timer_set_period(timer, 6000); // 每 3 秒更新一次
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
    strncpy(full_response, text, sizeof(full_response) - 1);
    full_response[sizeof(full_response) - 1] = '\0';
    response_offset = 0;
    update_label_cb(NULL);
}
