/*
#include <zw101.h>
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-04-24     huge       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "string.h"
#include "zw101.h"
#include "pwm.h"
#define DBG_TAG "main"
#define DBG_LVL         DBG_LOG
#include <rtdbg.h>

static struct rt_semaphore u3_sem;
static rt_thread_t zw101_th;
static rt_size_t rx3_len = 0;
uint8_t buf[256] = {0};
rt_size_t len = 0;
int wait=0;
static rt_err_t rx3_callback(rt_device_t dev, rt_size_t size){
    //中断接收
   // rt_sem_release(&u3_sem);

    //DMA接收
    rx3_len = size;
    rt_sem_release(&u3_sem);

    return RT_EOK;
}

static void zw101_thread_entry(void *parameter){
    //中断接收
//    char buf;
//    while (1){
//        while(rt_device_read(u3_dev, 0, &buf, 1) != 1){
//            rt_sem_take(&u3_sem, RT_WAITING_FOREVER);
//        }
//        rt_kprintf("%c", buf);
//    }

    //DMA接收

    while (1){
        rt_sem_take(&u3_sem, RT_WAITING_FOREVER);
        rt_memset(buf, 0, sizeof(buf));
        len = rt_device_read(u3_dev, 0, buf, rx3_len);
        rt_thread_mdelay(50);
        /*if(rx3_len==len){rt_kprintf("rv_ok\n");}*/
        /*for (int i = 0; i < len; i++) {
                   rt_kprintf("%02X ", buf[i]);  // 按十六进制打印每个字节
               }*/

    }
}

int zw101_init(void)
{
    rt_err_t ret = 0;

    u3_dev = rt_device_find("uart3");
    if (u3_dev == RT_NULL){
        LOG_E("rt_device_find[uart3] failed...\n");
        return -1;
    }
    LOG_D("rt_device_find[uart3] succeeded...\n");

    //中断接收
    //ret = rt_device_open(u3_dev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    //DMA接收
    ret = rt_device_open(u3_dev, RT_DEVICE_FLAG_DMA_RX);

    if (ret < 0){
        LOG_E("rt_device_open[uart3] failed...\n");
        return ret;
    }
    LOG_D("rt_device_open[uart3] succeeded...\n");
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; // 默认配置
    config.baud_rate = BAUD_RATE_57600;
    config.data_bits = DATA_BITS_8;
    config.stop_bits = STOP_BITS_1;
    config.parity = PARITY_NONE;
    rt_device_control(u3_dev, RT_DEVICE_CTRL_CONFIG, &config);

    rt_device_set_rx_indicate(u3_dev, rx3_callback);

    rt_sem_init(&u3_sem, "rx3_sem", 0, RT_IPC_FLAG_FIFO);

    zw101_th = rt_thread_create("zw101_recv", zw101_thread_entry, NULL, 1024, 10, 5);
    rt_thread_startup(zw101_th);

    return 0;
}


uint8_t Med_Zw101_IdentifyFinger (void)
{
    uint8_t flag = 0;
    uint8_t data[17] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x32,0x02,0xFF,0xFF,0x00,0x05,0x02,0x40};

    // 发送指令
    rt_device_write(u3_dev, 0, data,17);

    // 等待接收完应答数据
    /*while (rx3_len==len)
    {
        timeOut ++;
        if (timeOut >= 2000)
        {
        rt_kprintf("timeout");
        break;
        }
        rt_thread_mdelay(1);
    }*/
    int wait_count = 0;
           while (rx3_len < 12 && wait_count++ < 100)
           {
               rt_thread_mdelay(10);  // 等待数据接收
           }
    if (rx3_len==len)
    {
        if (rx3_len >= 11 &&buf[9] ==00)
            flag = 1;

    }
    return flag;
}

void build_auto_enroll_packet(uint16_t id, uint8_t *packet_out) {
    uint8_t data[] = {
        0x01,             // 包标识
        0x00, 0x08,       // 包长度
        0x31,             // 指令码
        (id >> 8) & 0xFF, // ID高字节
        id & 0xFF,        // ID低字节
        0x03,             // 录入次数固定为3
        0x00, 0x1A        // 参数 0x001A
    };

    uint16_t sum = 0;
    for (int i = 0; i < sizeof(data); i++) {
        sum += data[i];
    }

    // 构造最终发送包
    packet_out[0]  = 0xEF;
    packet_out[1]  = 0x01;
    packet_out[2]  = 0xFF;
    packet_out[3]  = 0xFF;
    packet_out[4]  = 0xFF;
    packet_out[5]  = 0xFF;

    for (int i = 0; i < sizeof(data); i++) {
        packet_out[6 + i] = data[i];
    }

    packet_out[15] = (sum >> 8) & 0xFF; // 校验和高位
    packet_out[16] = sum & 0xFF;        // 校验和低位
}
uint8_t Med_Zw101_AddFinger (uint8_t id,uint8_t cunt)
{
    uint8_t flag = 0;
    uint8_t data[17]  ;
    build_auto_enroll_packet(id, data);
    // 发送指令
    rt_device_write(u3_dev, 0, data,17);
    int wait_count = 0;
           while (rx3_len < 12 && wait_count++ < 100)
           {
               rt_thread_mdelay(10);  // 等待数据接收
           }
    if (rx3_len>11)
    {
        if (buf[6] == 0x07&&buf[9] == 0x00)
        {
            flag = 1;
        rt_kprintf("buffer %d ok\n",cunt);
        }
        else
        {
            rt_kprintf("buffer %d fail\n",cunt);
        }
    }
    else rt_kprintf("rv_nothing\n");
    return flag;
}
int zw101_add_fingerprint_auto(rt_uint16_t id)
{
    for (int i = 1; i <= 3; i++)
    {
        rt_kprintf("put finger %d \n", i);

        // 一直循环直到当前次录入成功或超时
        int try_count = 0;
        while (try_count++ < 30)
        {
            int res = Med_Zw101_AddFinger(id, i);
            if (res ==1)
            {
                rt_kprintf(" %d success\n", i);
                rt_thread_mdelay(1500);  // 等待用户松手
                break;
            }
            rt_thread_mdelay(500);
        }

        if (try_count >= 30)
        {
            rt_kprintf(" %d add_fail\n", i);
            return -RT_ERROR;
        }
    }

    rt_kprintf(" ID=%d add_OK\n", id);
    return RT_EOK;
}


uint8_t Med_Zw101_DeleteFinger (uint8_t id)
{
    uint8_t flag = 0;

    uint8_t data[16] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x0C,0x00,0x00,0x00,0x01,0x00,0x15};

    data[11] = id;
    data[15] = 0x15 + id;

    // 发送指令
    rt_device_write(u3_dev, 0, data,16);
    int wait_count = 0;
           while (rx3_len < 12 && wait_count++ < 100)
           {
               rt_thread_mdelay(10);  // 等待数据接收
           }
    // 等待接收完应答数据
    if (rx3_len>11)
    {
       if (buf[9] == 0x00 ) flag = 1;
       rt_kprintf("delete ok\n");
    }
    else rt_kprintf("rv_nothing\n");

    return flag;
}

