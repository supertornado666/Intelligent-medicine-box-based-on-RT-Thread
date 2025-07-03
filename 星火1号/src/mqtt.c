/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-05-05     廖钟涛       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <stdbool.h>

#include "mqtt_api.h"
#include "cJSON.h"

#include "myaht10.h"
#include "medication_management.h"

char DEMO_PRODUCT_KEY[IOTX_PRODUCT_KEY_LEN + 1];
char DEMO_DEVICE_NAME[IOTX_DEVICE_NAME_LEN + 1];
char DEMO_DEVICE_SECRET[IOTX_DEVICE_SECRET_LEN + 1];
static char mqtt_aht_stack[4096];
static struct rt_thread mqtt_aht;

extern bool finger_flag;
extern int have_take[5];

#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)

void example_event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    HAL_Printf("msg->event_type : %d\n", msg->event_type);
}

//example_message_arrive: 云端消息到达时的回调函数。
//
//解析云端下发的JSON格式的消息，并根据消息内容控制LED状态和LED矩阵显示模式。
//使用CJSON库解析JSON字符串，并根据cmd字段的内容执行相应的操作。
static void example_message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    //rt_kprintf("something arrived\n");
    iotx_mqtt_topic_info_t     *topic_info = (iotx_mqtt_topic_info_pt) msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            /* print topic name and topic message */
            EXAMPLE_TRACE("Message Arrived:");
            EXAMPLE_TRACE("Topic  : %.*s", topic_info->topic_len, topic_info->ptopic);
            EXAMPLE_TRACE("Payload: %.*s", topic_info->payload_len, topic_info->payload);
            EXAMPLE_TRACE("\n");
            // 解析json数据
            cJSON *root = cJSON_Parse(topic_info->payload);
            if (root == NULL) {
                EXAMPLE_TRACE("json parse error");
                return;
            }

            cJSON *cmd = RT_NULL;
            if ((cmd = cJSON_GetObjectItem(root, "AlarmSwitch"))){
                if (cmd->valueint == 1) rt_kprintf("AlarmOn\n");
                else if (cmd->valueint == 0) rt_kprintf("AlarmOff\n");
            }

            cmd = cJSON_GetObjectItem(root, "cmd");
            if (cmd && !rt_strcmp(cJSON_GetStringValue(cmd), "DeleteMedicineInfo")) {
                cJSON *data = cJSON_GetObjectItem(root, "data");
                if (data) {
                    cJSON *num_item = cJSON_GetObjectItem(data, "number");
                    if (num_item && cJSON_IsNumber(num_item)) {
                        del_medicine(num_item->valueint);
                    }
                }
            }
            else if (cmd && !rt_strcmp(cJSON_GetStringValue(cmd), "RequestMedicineInfo")){
                int count = 0;
                Medicine* med_list = get_medicine_info(&count);//medicine  类型
                for (int i = 0; i < count; i++){
                   // medicine_mqtt_add(med_list[i]);
                    rt_thread_mdelay(100);
                }
            }
            else if (cmd && !rt_strcmp(cJSON_GetStringValue(cmd), "SyncMedicineInfo")){
                cJSON *data = cJSON_GetObjectItem(root, "data");
                Medicine *new_info = (Medicine *)rt_calloc(1, sizeof(Medicine));
                if (!new_info) {
                    rt_kprintf("Memory allocation failed!\n");
                    cJSON_Delete(root);
                    return;
                }
                if (data) {
                    cJSON *name_item = cJSON_GetObjectItem(data, "name");
                    cJSON *taketime_item = cJSON_GetObjectItem(data, "take_time");
                    cJSON *amount_item = cJSON_GetObjectItem(data, "amount");
                    cJSON *times_item = cJSON_GetObjectItem(data, "taken_times");
                    cJSON *num_item = cJSON_GetObjectItem(data, "number");

                    if (name_item && cJSON_IsString(name_item)) {
                        rt_strcpy(new_info->name, name_item->valuestring);
                    }

                    if (taketime_item && cJSON_IsArray(taketime_item)) {
                        int count = cJSON_GetArraySize(taketime_item);
                        for (int i = 0; i < count && i < 5; i++) {
                            cJSON *item = cJSON_GetArrayItem(taketime_item, i);
                            if (item && cJSON_IsString(item)) {
                                rt_strcpy(new_info->take_time[i], item->valuestring);
                            }
                        }
                    }

                    if (amount_item && cJSON_IsNumber(amount_item))
                        new_info->amount = amount_item->valueint;
                    if (times_item && cJSON_IsNumber(times_item))
                        new_info->times_per_day = times_item->valueint;
                    if (num_item && cJSON_IsNumber(num_item))
                        new_info->number = num_item->valueint;
                    //update_medicine(new_info);
                }
            }
            else if (cmd == NULL) {
                EXAMPLE_TRACE("json parse error");
                cJSON_Delete(root);
                return;
            }
            // 打印解析后的数据
            //EXAMPLE_TRACE("cmd: %s",cmd->valuestring);


//            if (strcmp(cmd->valuestring, "1") == 0) {
//                rt_kprintf("AlarmOn\n");
//            }
//            else if (strcmp(cmd->valuestring, "0") == 0) {
//                rt_kprintf("AlarmOff\n");
//            }


            break;
        default:
            break;
    }
}

//订阅云端主题的函数。
//
//根据设备的产品密钥和设备名称构建主题。
//使用IOT_MQTT_Subscribe函数订阅主题，并指定回调函数example_message_arrive。
static int example_subscribe(void *handle)
{
    int res = 0;
    const char *fmt = "/%s/%s/user/get";
    char *topic = NULL;
    int topic_len = 0;

    topic_len = strlen(fmt) + strlen(DEMO_PRODUCT_KEY) + strlen(DEMO_DEVICE_NAME) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return -1;
    }
    memset(topic, 0, topic_len);
    HAL_Snprintf(topic, topic_len, fmt, DEMO_PRODUCT_KEY, DEMO_DEVICE_NAME);

    res = IOT_MQTT_Subscribe(handle, topic, IOTX_MQTT_QOS0, example_message_arrive, NULL);
    if (res < 0) {
        EXAMPLE_TRACE("subscribe failed");
        HAL_Free(topic);
        return -1;
    }

    HAL_Free(topic);
    return 0;
}


//mqtt_tranfer_sensor: 从消息队列读取传感器数据，并将其发布到云端的函数。
//
//从消息队列中读取温湿度、光照强度和接近检测数据。
//构建JSON格式的负载，并使用IOT_MQTT_Publish_Simple函数将数据发布到云端。
static void mqtt_tranfer_sensor(void *pclient)
{
    float humidity;
    float temperature;
    char *payload = NULL;
    int payload_len = 0,res = 0;

    const char     *fmt = "/sys/%s/%s/thing/event/property/post";
    char           *topic = NULL;
    int topic_len = 0;

    topic_len = strlen(fmt) + strlen(DEMO_PRODUCT_KEY) + strlen(DEMO_DEVICE_NAME) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return;
    }
    memset(topic, 0, topic_len);
    HAL_Snprintf(topic, topic_len, fmt, DEMO_PRODUCT_KEY, DEMO_DEVICE_NAME);

    rt_mq_recv(mq_hum, &humidity, sizeof(humidity), RT_WAITING_NO);
    // LOG_D("humidity   : %d.%d %%", (int)humidity, (int)(humidity * 10) % 10);
    /* 从队列中读取温度并打印 */
    rt_mq_recv(mq_tem, &temperature, sizeof(temperature), RT_WAITING_NO);
    // LOG_D("temperature: %d.%d", (int)temperature, (int)(temperature * 10) % 10);

    // 判断数据是否合法
    if (humidity < 0 || humidity > 100 || temperature < -40 || temperature > 85) {
        EXAMPLE_TRACE("data error");
        return;
    }

    //计算payload所需长度，温度湿度数据保留一位小数上传，"params":{"CurrentTemperature":16.5,"CurrentHumidity":56.3,"LightValue":1000.0,"DetectDistance":1000.0}
    payload_len = strlen("{\"params\":{\"CurrentTemperature\":16.5,\"CurrentHumidity\":56.3}}") + 5;
    payload = HAL_Malloc(payload_len);
    if (payload == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return;
    }
    memset(payload, 0, payload_len);
    HAL_Snprintf(payload, payload_len, "{\"params\":{\"CurrentTemperature\":%d.%d,\"CurrentHumidity\":%d.%d}}", (int)temperature, (int)(temperature * 10) % 10, (int)humidity, (int)(humidity * 10) % 10);
    //rt_kprintf("aaa%d,   %d\n", temperature, humidity);

    res = IOT_MQTT_Publish_Simple(0, topic, IOTX_MQTT_QOS0, payload, strlen(payload));
    if (res < 0) {
        EXAMPLE_TRACE("publish failed, res = %d", res);
        HAL_Free(topic);
        HAL_Free(payload);
        return;
    }
    HAL_Free(topic);
    HAL_Free(payload);
}

static void mqtt_aht_main(void *parameter)
{
    // 获取WiFi连接成功信号量，等待WiFi连接成功后再进行mqtt连接
//    rt_sem_take(wifi_connect_sem, RT_WAITING_FOREVER);
//    rt_sem_take(ap3216_connect_sem, RT_WAITING_FOREVER);
//    rt_sem_take(aht32_connect_sem, RT_WAITING_FOREVER);

    void                   *pclient = NULL;
    int                     res = 0;
    //int                     loop_cnt = 0;
    iotx_mqtt_param_t       mqtt_params;

    HAL_GetProductKey(DEMO_PRODUCT_KEY);
    HAL_GetDeviceName(DEMO_DEVICE_NAME);
    HAL_GetDeviceSecret(DEMO_DEVICE_SECRET);

    EXAMPLE_TRACE("mqtt example");
    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));
    mqtt_params.handle_event.h_fp = example_event_handle;
    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == pclient) {
        EXAMPLE_TRACE("MQTT construct failed");
        return;
    }

    res = example_subscribe(pclient);
    if (res < 0) {
        IOT_MQTT_Destroy(&pclient);
        return;
    }

    while (1) {
        mqtt_tranfer_sensor(pclient);

        IOT_MQTT_Yield(pclient, 200);

        rt_thread_mdelay(9800);
    }
    return;
}

void alarm_on(void){
    char *payload = NULL;
    int payload_len = 0,res = 0;

    const char     *fmt = "/sys/%s/%s/thing/event/property/post";
    char           *topic = NULL;
    int topic_len = 0;

    topic_len = strlen(fmt) + strlen(DEMO_PRODUCT_KEY) + strlen(DEMO_DEVICE_NAME) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return;
    }
    memset(topic, 0, topic_len);
    HAL_Snprintf(topic, topic_len, fmt, DEMO_PRODUCT_KEY, DEMO_DEVICE_NAME);

    //计算payload所需长度，温度湿度数据保留一位小数上传，"params":{"CurrentTemperature":16.5,"CurrentHumidity":56.3,"LightValue":1000.0,"DetectDistance":1000.0}
    payload_len = strlen("{\"params\":{\"alarmStatus\":1}}") + 5;
    payload = HAL_Malloc(payload_len);
    if (payload == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return;
    }
    memset(payload, 0, payload_len);
    HAL_Snprintf(payload, payload_len, "{\"params\":{\"alarmStatus\":%d}}", 1);

    res = IOT_MQTT_Publish_Simple(0, topic, IOTX_MQTT_QOS0, payload, strlen(payload));
    if (res < 0) {
        EXAMPLE_TRACE("publish failed, res = %d", res);
        HAL_Free(topic);
        HAL_Free(payload);
        return;
    }
    HAL_Free(topic);
    HAL_Free(payload);
}

void identity_mqtt_change(void){
    char *payload = NULL;
    int payload_len = 0,res = 0;

    const char     *fmt = "/sys/%s/%s/thing/event/property/post";
    char           *topic = NULL;
    int topic_len = 0;

    topic_len = strlen(fmt) + strlen(DEMO_PRODUCT_KEY) + strlen(DEMO_DEVICE_NAME) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return;
    }
    memset(topic, 0, topic_len);
    HAL_Snprintf(topic, topic_len, fmt, DEMO_PRODUCT_KEY, DEMO_DEVICE_NAME);

    //计算payload所需长度，温度湿度数据保留一位小数上传，"params":{"CurrentTemperature":16.5,"CurrentHumidity":56.3,"LightValue":1000.0,"DetectDistance":1000.0}
    payload_len = strlen("{\"params\":{\"fingerPrintValue\":1}}") + 5;
    payload = HAL_Malloc(payload_len);
    if (payload == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return;
    }
    memset(payload, 0, payload_len);
    HAL_Snprintf(payload, payload_len, "{\"params\":{\"fingerPrintValue\":%d}}", finger_flag);

    res = IOT_MQTT_Publish_Simple(0, topic, IOTX_MQTT_QOS0, payload, strlen(payload));
    if (res < 0) {
        EXAMPLE_TRACE("publish failed, res = %d", res);
        HAL_Free(topic);
        HAL_Free(payload);
        return;
    }
    HAL_Free(topic);
    HAL_Free(payload);
}

void medicine_mqtt_add(Medicine med){
    char *payload = NULL;
    int payload_len = 0,res = 0;

    const char     *fmt = "/sys/%s/%s/thing/event/property/post";
    char           *topic = NULL;
    int topic_len = 0;

    topic_len = strlen(fmt) + strlen(DEMO_PRODUCT_KEY) + strlen(DEMO_DEVICE_NAME) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return;
    }
    memset(topic, 0, topic_len);
    HAL_Snprintf(topic, topic_len, fmt, DEMO_PRODUCT_KEY, DEMO_DEVICE_NAME);

    //计算payload所需长度，温度湿度数据保留一位小数上传，"params":{"CurrentTemperature":16.5,"CurrentHumidity":56.3,"LightValue":1000.0,"DetectDistance":1000.0}
    payload_len = strlen("{\"params\":{\"medicineInfo\":{"
            "\"drugName\":\"啊啊啊啊啊啊啊啊啊啊啊啊\","
            "\"drugTaketime\":\"\\\"00:00\\\"\\\"00:00\\\"\\\"00:00\\\"\\\"00:00\\\"\\\"00:00\\\"\","
            "\"singleAmount\":5,"
            "\"takenTime\":5,"
            "\"Location\":1"
            "}}}") + 5;
    payload = HAL_Malloc(payload_len);
    if (payload == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return;
    }
    memset(payload, 0, payload_len);

    HAL_Snprintf(payload, payload_len,
        "{\"params\":{\"medicineInfo\":{"
        "\"drugName\":\"%s\","
        "\"drugTaketime\":\"\\\"%s\\\"\\\"%s\\\"\\\"%s\\\"\\\"%s\\\"\\\"%s\\\"\","
        "\"singleAmount\":%d,"
        "\"takenTime\":%d,"
        "\"Location\":%d"
        "}}}",
        med.name,
        med.take_time[0], med.take_time[1], med.take_time[2], med.take_time[3], med.take_time[4],
        med.amount,
        have_take[med.number - 1],
        med.number
    );

    res = IOT_MQTT_Publish_Simple(0, topic, IOTX_MQTT_QOS0, payload, strlen(payload));
    if (res < 0) {
        EXAMPLE_TRACE("publish failed, res = %d", res);
        HAL_Free(topic);
        HAL_Free(payload);
        return;
    }
    HAL_Free(topic);
    HAL_Free(payload);
}

void aht_mqtt_init(void)
{
    // 提示信息
    rt_kprintf("creating mqtt aht thread\n");

    rt_thread_init(&mqtt_aht, "mqtt_aht", mqtt_aht_main, RT_NULL, mqtt_aht_stack, sizeof(mqtt_aht_stack), 20, 10);
    rt_thread_startup(&mqtt_aht);

    // 提示信息
    rt_kprintf("creating mqtt aht rt_thread_startup\n");

}
