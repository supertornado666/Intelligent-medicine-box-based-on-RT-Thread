/*
 * Copyright (c) 2006-2025, RT-Thread Development Team
 *
 * SPDX-License-Identifier: MIT
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025/02/01     Rbb666       Add license info
 * 2025/02/03     Rbb666       Unified Adaptive Interface
 * 2025/02/06     Rbb666       Add http stream support
 */
#include "llm.h"
#include "webclient.h"
#include <cJSON.h>
#include "llm_show.h"
#include "demo/ui/ui.h"
#include "syn8086.h"

#define MAX_RETRY_COUNT 3
#define RETRY_DELAY_MS 10000  // 每次重试间隔（毫秒）

extern cJSON *responseRoot;
extern cJSON *requestRoot;
extern cJSON *model;
extern cJSON *messages;
extern cJSON *systemMessage;
extern cJSON *userMessage;
extern cJSON *historyMessage;

//extern rt_device_t u4_dev;
extern rt_uint8_t playback_queue_head, playback_queue_tail;
extern rt_thread_t voice_td;
extern rt_bool_t speech_thread_running;

extern rt_uint8_t u4_buf[16];
extern struct rt_semaphore voice_sem;

static rt_bool_t success = RT_TRUE;

#define LLM_API_KEY PKG_LLM_API_KEY
#if defined(PKG_LLM_QWEN_API_URL)
    #define LLM_API_URL PKG_LLM_QWEN_API_URL
#elif defined(PKG_LLM_DOUBAO_API_URL)
    #define LLM_API_URL PKG_LLM_DOUBAO_API_URL
#elif defined(PKG_LLM_DEEPSEEK_API_URL)
    #define LLM_API_URL PKG_LLM_DEEPSEEK_API_URL
#endif
#define LLM_MODEL_NAME PKG_LLM_MODEL_NAME
#define WEB_SOCKET_BUF_SIZE PKG_WEB_SORKET_BUFSZ

char authHeader[128] = {0};
static char responseBuffer[WEB_SOCKET_BUF_SIZE] = {0};
static char contentBuffer[WEB_SOCKET_BUF_SIZE] = {0};

char *get_llm_answer(const char *inputText)
{
    success = RT_TRUE;

    struct webclient_session *webSession = NULL;
    char *allContent = NULL;
    int bytesRead, responseStatus;

    int retryCount = 0;
    rt_bool_t reconnected = RT_FALSE;


//    // Create web session
//    webSession = webclient_session_create(WEB_SOCKET_BUF_SIZE);
//    if (webSession == NULL)
//    {
//        rt_kprintf("Failed to create webclient session.\n");
//        goto cleanup;
//    }

    cJSON_AddStringToObject(userMessage, "content", inputText);
    cJSON_AddStringToObject(historyMessage, "content", "");

    char *payload = cJSON_PrintUnformatted(requestRoot);
    if (payload == NULL)
    {
        rt_kprintf("Failed to create JSON payload.\n");
        goto cleanup;
    }

//    // Add headers
//    webclient_header_fields_add(webSession, "Content-Type: application/json\r\n");
//    webclient_header_fields_add(webSession, authHeader);
//    webclient_header_fields_add(webSession, "Content-Length: %d\r\n", rt_strlen(payload));
//    webclient_header_fields_add(webSession, "Connection: keep-alive\r\n");

//    LLM_DBG("HTTP Header: %s\n", webSession->header->buffer);
//    LLM_DBG("HTTP Payload: %s\n", payload);

    while (retryCount < MAX_RETRY_COUNT)
    {
        rt_kprintf("Attempting to post... (%d/%d)\n", retryCount + 1, MAX_RETRY_COUNT);
        // 关闭旧 session
        if (webSession){
            webclient_close(webSession);
            webSession = NULL;
        }

        // 重建 web session
        webSession = webclient_session_create(WEB_SOCKET_BUF_SIZE);
        if (webSession == NULL)
        {
            rt_kprintf("Failed to recreate webclient session.\n");
            retryCount++;
            rt_thread_mdelay(RETRY_DELAY_MS);
            continue;
        }

        // 重新设置 header
        webclient_header_fields_add(webSession, "Content-Type: application/json\r\n");
        webclient_header_fields_add(webSession, authHeader);
        webclient_header_fields_add(webSession, "Content-Length: %d\r\n", rt_strlen(payload));
        //webclient_header_fields_add(webSession, "Connection: keep-alive\r\n");

        // 重新 POST 请求
        responseStatus = webclient_post(webSession, LLM_API_URL, payload, rt_strlen(payload));
        if (responseStatus == 200)
        {
            rt_kprintf("Post successfully.\n");
            reconnected = RT_TRUE;
            break;
        }
        else
        {
            rt_kprintf("Post attempt %d failed, status: %d\n", retryCount + 1, responseStatus);
        }
        retryCount++;
        rt_thread_mdelay(RETRY_DELAY_MS);
    }
    if (!reconnected)
    {
        success = RT_FALSE;
        rt_kprintf("All post attempts failed.\n");
        goto cleanup;
    }

    rt_size_t contentLen = 0;

    while (1){
        // Read and process response
        bytesRead = webclient_read(webSession, responseBuffer, WEB_SOCKET_BUF_SIZE);

        if (bytesRead > 0){
            int inContent = 0;

            for (int i = 0; i < bytesRead; i++)
            {
                if (inContent)
                {
                    if (responseBuffer[i] == '"')
                    {
                        inContent = 0;

                        // Append content to allContent
//                        char *oldAllContent = allContent;
//                        size_t oldLen = oldAllContent ? rt_strlen(oldAllContent) : 0;
//                        size_t newLen = rt_strlen(contentBuffer);
//                        size_t totalLen = oldLen + newLen + 1;
                        rt_size_t oldLen = allContent ? rt_strlen(allContent) : 0;
                        rt_size_t newLen = rt_strlen(contentBuffer);

                        rt_uint8_t *newAllContent = (rt_uint8_t *)web_malloc(oldLen + newLen + 1);
                        if (newAllContent)
                        {
//                            newAllContent[0] = '\0';
//                            //web_free(newAllContent);
//                            if (oldAllContent)
//                            {
//                                rt_strcpy(newAllContent, oldAllContent);
//                            }
                            if (allContent) rt_strcpy(newAllContent, allContent); // 拷贝旧内容
                            strcat(newAllContent, contentBuffer);              // 拼接新内容

                            web_free(allContent);   // 释放旧内存
                            allContent = newAllContent; // 指针指向新内存
                            if (rt_strlen(contentBuffer) >= 110){

                                //show_response_segmented(contentBuffer);
                                //rt_kprintf("%s", contentBuffer);
                                //rt_thread_mdelay(6000);

                                enqueue_to_playback_queue(contentBuffer);
                                contentBuffer[0] = '\0';
                            }
                            //rt_kprintf("%s", contentBuffer);
                            //rt_free(oldAllContent);
                        }
                        else
                        {
                            rt_kprintf("Memory allocation failed, content truncated!\n");
                        }

                        //contentBuffer[0] = '\0'; // Reset content buffer
                    }
                    else
                    {
                        strncat(contentBuffer, &responseBuffer[i], 1);
                    }
                }
                else if (responseBuffer[i] == '"' && i > 8 &&
                         rt_strncmp(&responseBuffer[i - 10], "\"content\":\"", 10) == 0)
                {
                    inContent = 1;
                }
            }
        }
        else if (bytesRead < 0){
            contentBuffer[0] = '\0';
            retryCount = 0;
            reconnected = RT_FALSE;

            rt_kprintf("Connection lost, attempting to reconnect...\n");

            // 重建 JSON 请求
//            if (requestRoot)
//                cJSON_Delete(requestRoot);
//            requestRoot = cJSON_CreateObject();

            //cJSON_AddStringToObject(userMessage, "content", inputText);
            //cJSON_AddStringToObject(historyMessage, "content", "");

            // ✅ 加入断线前已收到内容作为 assistant 的历史
            if (allContent && rt_strlen(allContent) > 0)
            {
                cJSON_AddStringToObject(historyMessage, "content", allContent);
            }

            // 重新生成 payload
            if (payload){
                cJSON_free(payload);
                payload = RT_NULL;
            }
            payload = cJSON_PrintUnformatted(requestRoot);
            if (payload == RT_NULL)
            {
                rt_kprintf("Failed to recreate JSON payload.\n");
                goto cleanup;
            }

            while (retryCount < MAX_RETRY_COUNT)
            {
                rt_kprintf("Attempting to reconnect... (%d/%d)\n", retryCount + 1, MAX_RETRY_COUNT);
//                // 关闭旧 session
                if (webSession){
                    webclient_close(webSession);
                    webSession = RT_NULL;
                }

                // 重建 web session
                webSession = webclient_session_create(WEB_SOCKET_BUF_SIZE);
                if (webSession == RT_NULL)
                {
                    rt_kprintf("Failed to recreate webclient session.\n");
                    retryCount++;
                    rt_thread_mdelay(RETRY_DELAY_MS);
                    continue;
                }

                // 重新设置 header
                webclient_header_fields_add(webSession, "Content-Type: application/json\r\n");
                webclient_header_fields_add(webSession, authHeader);
                webclient_header_fields_add(webSession, "Content-Length: %d\r\n", rt_strlen(payload));
                //webclient_header_fields_add(webSession, "Connection: keep-alive\r\n");

                // 重新 POST 请求
                responseStatus = webclient_post(webSession, LLM_API_URL, payload, rt_strlen(payload));
                if (responseStatus == 200)
                {
                    rt_kprintf("Reconnected successfully.\n");
                    reconnected = RT_TRUE;
                    break;
                }
                else
                {
                    rt_kprintf("Reconnection attempt %d failed, status: %d\n", retryCount + 1, responseStatus);
                }
                retryCount++;
                rt_thread_mdelay(RETRY_DELAY_MS);
            }

            if (!reconnected)
            {
                success = RT_FALSE;
                rt_kprintf("All reconnection attempts failed.\n");
                goto cleanup;
            }
        }
        else{
            rt_kprintf("receive successfully\n");
            break;
        }
    }
    rt_kprintf("\n\nbytesRead:%d\n", bytesRead);

    if (strlen(contentBuffer) > 0){
        //show_response_segmented(contentBuffer);
        //rt_kprintf("%s", contentBuffer);
        enqueue_to_playback_queue(contentBuffer);
        contentBuffer[0] = '\0';
    }
    start_show();
    while(playback_queue_head != playback_queue_tail) rt_thread_mdelay(3000);
    rt_thread_mdelay(5000);
    speech_thread_running = RT_FALSE;
    while(u4_buf[0] != 'O' && rt_pin_read(BUSY_PIN) == PIN_HIGH) rt_thread_mdelay(3000);
    rt_thread_mdelay(3000);
    while(u4_buf[0] != 'O' && rt_pin_read(BUSY_PIN) == PIN_HIGH) rt_thread_mdelay(3000);
    rt_thread_mdelay(5000);
    _ui_screen_change(&ui_mainscreen, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_mainscreen_screen_init);
    lv_label_set_text(ui_responsetext, "");
    rt_sem_detach(&voice_sem);
    voice_td = RT_NULL;

    //rt_kprintf("\n");

cleanup:
    // Cleanup resources
    if (webSession){
        webclient_close(webSession);
        webSession = RT_NULL;
    }

//    if (requestRoot){
//        cJSON_Delete(requestRoot);
//        requestRoot = RT_NULL;
//    }

    if (responseRoot){
        cJSON_Delete(responseRoot);
        responseRoot = RT_NULL;
    }

    if (payload){
        cJSON_free(payload);
        payload = RT_NULL;
    }

    if (!success) {
        SYN_FrameInfo("网络不稳定，请重试");
        while (playback_queue_head != playback_queue_tail){
            playback_queue_head = (playback_queue_head + 1) % PLAYBACK_QUEUE_SIZE;
        }
    }

    _ui_screen_change(&ui_mainscreen, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_mainscreen_screen_init);
    lv_label_set_text(ui_responsetext, "");

    return allContent;
}
