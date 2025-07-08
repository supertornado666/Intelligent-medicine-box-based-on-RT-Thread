/*
 * Copyright (c) 2006-2025, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025/02/01     Rbb666       Add license info
 */
#include "llm.h"
#include "shell.h"
#include "demo/ui/ui.h"
#include "event.h"
#include <cJSON.h>

extern rt_uint8_t authHeader[128];

// Create JSON payload
cJSON *responseRoot;
cJSON *requestRoot;
cJSON *model;
cJSON *messages;
cJSON *systemMessage;
cJSON *userMessage;
cJSON *historyMessage;

static struct llm_obj handle = {0};
static int index;
static int length;
static rt_bool_t history_init;
//static rt_err_t result;
//struct rt_semaphore call_deepseek_sem;

llm_shared_data_t llm_answer = {0};
//extern struct rt_semaphore show_sem;
rt_uint8_t msg[600];
rt_uint8_t m_buf[500];
extern rt_uint8_t u1_buf[500];
//extern struct rt_semaphore info_sem;
//static rt_uint8_t prio;

static rt_bool_t llm_handle_history(const char *prompt)
{
    rt_kprintf("\033[2K\r");
    rt_kprintf("%s%s", prompt, handle.line);
    return RT_FALSE;
}

static void llm_push_history(void)
{
    if (handle.line_position > 0)
    {
        if (handle.history_count >= LLM_HISTORY_LINES)
        {
            if (rt_memcmp(&handle.llm_history[LLM_HISTORY_LINES - 1], handle.line, PKG_LLM_CMD_BUFFER_SIZE))
            {
                for (index = 0; index < FINSH_HISTORY_LINES - 1; index ++)
                {
                    rt_memcpy(&handle.llm_history[index][0], &handle.llm_history[index + 1][0], PKG_LLM_CMD_BUFFER_SIZE);
                }
                rt_memset(&handle.llm_history[index][0], 0, PKG_LLM_CMD_BUFFER_SIZE);
                rt_memcpy(&handle.llm_history[index][0], handle.line, handle.line_position);

                handle.history_count = LLM_HISTORY_LINES;
            }
        }
        else
        {
            if (handle.history_count == 0 || rt_memcmp(&handle.llm_history[handle.history_count - 1], handle.line, PKG_LLM_CMD_BUFFER_SIZE))
            {
                handle.history_current = handle.history_count;
                rt_memset(&handle.llm_history[handle.history_count][0], 0, PKG_LLM_CMD_BUFFER_SIZE);
                rt_memcpy(&handle.llm_history[handle.history_count][0], handle.line, handle.line_position);

                handle.history_count++;
            }
        }
    }

    handle.history_current = handle.history_count;
}

static void llm_run(rt_thread_t *parameter)
{
    while (1)
    {
        rt_event_recv(speak_event, EVENT_CALL_DEEPSEEK,
                                   RT_EVENT_FLAG_CLEAR | RT_EVENT_FLAG_AND,
                                   RT_WAITING_FOREVER, NULL);
        //rt_kprintf("ready to call deepseek\n");

        //strcpy(msg, "给我讲个故事");
        strcat(msg, m_buf);
        m_buf[0] = '\0';
//        length = strlen(msg);
        rt_kprintf("%s\n",msg);

        _ui_screen_change(&ui_emotion, LV_SCR_LOAD_ANIM_FADE_ON, 500, 100, &ui_emotion_screen_init);
        lv_obj_add_flag(ui_sad, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_question, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui_smile, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui_Spinner1, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(ui_responsetext, "分析中...");
        char *showtext = handle.get_answer(msg);

        /*
        rt_mutex_take(&llm_answer.lock, RT_WAITING_FOREVER);
        rt_strncpy(llm_answer.response, showtext, PKG_WEB_SORKET_BUFSZ-1);
        llm_answer.response_ready = RT_TRUE;
        rt_sem_release(&show_sem);
        rt_mutex_release(&llm_answer.lock);
        */

        if (showtext != RT_NULL)
        {
            rt_free(showtext);
            showtext = RT_NULL;
        }

        rt_memset(msg, 0, sizeof(msg));
        rt_thread_mdelay(10000);
        //return;
    }
}

int llm2rtt_init(void)
{
    history_init = RT_FALSE;

    if (history_init == RT_FALSE)
    {
        rt_memset(&handle, 0x00, sizeof(struct llm_obj));
        history_init = RT_TRUE;
    }
    else
    {
        handle.stat = LLM_WAIT_NORMAL;
        handle.argc = 0;
        rt_memset(handle.line, 0x00, PKG_LLM_CMD_BUFFER_SIZE);
        handle.line_position = 0;
        handle.line_curpos = 0;
        handle.device = RT_NULL;
        handle.rx_indicate = RT_NULL;
    }

    //rt_sem_init(&call_deepseek_sem, "callAI_sem", 0, RT_IPC_FLAG_FIFO);
    rt_mutex_init(&llm_answer.lock, "llm_mutex", RT_IPC_FLAG_FIFO);

    //prio = rt_thread_self()->current_priority + 1;
    handle.get_answer = get_llm_answer;

    rt_thread_init(&handle.thread,
                             "llm_td",
                             llm_run, RT_NULL,
                             &handle.thread_stack[0], sizeof(handle.thread_stack),
                             11, 10);
    rt_thread_startup(&handle.thread);

    requestRoot = cJSON_CreateObject();
    model = cJSON_CreateString(PKG_LLM_MODEL_NAME);
    messages = cJSON_CreateArray();
    systemMessage = cJSON_CreateObject();
    userMessage = cJSON_CreateObject();
    historyMessage = cJSON_CreateObject();

    cJSON_AddItemToObject(requestRoot, "model", model);
    cJSON_AddItemToObject(requestRoot, "messages", messages);
#ifdef PKG_LLMCHAT_STREAM
    cJSON_AddBoolToObject(requestRoot, "stream", RT_TRUE);
#else
    cJSON_AddBoolToObject(requestRoot, "stream", RT_FALSE);
#endif
    cJSON_AddItemToArray(messages, systemMessage);
    cJSON_AddItemToArray(messages, userMessage);
    cJSON_AddItemToArray(messages, historyMessage);

    cJSON_AddStringToObject(systemMessage, "role", "system");
    cJSON_AddStringToObject(systemMessage, "content", "回复限制在200字之内");

    cJSON_AddStringToObject(userMessage, "role", "user");
    cJSON_AddStringToObject(historyMessage, "role", "assistant");

#ifdef PKG_LLM_USING_QWEN_CLOUD
    cJSON_AddBoolToObject(historyMessage, "partial", RT_TRUE);
#elif defined(PKG_LLM_USING_DEEPSEEK_CLOUD)
    cJSON_AddBoolToObject(historyMessage, "prefix", RT_TRUE);
#endif

    // Prepare authorization header
    rt_snprintf(authHeader, sizeof(authHeader), "Authorization: Bearer %s\r\n", PKG_LLM_API_KEY);

    return RT_EOK;
}
