/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-04-20     廖钟涛       the first version
 */
#ifndef APPLICATIONS_SYN6288_C_
#define APPLICATIONS_SYN6288_C_

#include "syn8086.h"
//#include "string.h"

static rt_uint8_t Voice_data[130];

void remove_newlines(rt_uint32_t *str)
{
    if (str == RT_NULL) return;

    rt_uint32_t *src = str;
    rt_uint32_t *dst = str;

    while (*src)
    {
        if (*src != '\n')
        {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';  // 终止新字符串
}


//Music:选择背景音乐。0:无背景音乐，1~15：选择背景音乐
void SYN_FrameInfo(const rt_uint32_t *HZdata)
{
    rt_strncpy(Voice_data, HZdata, sizeof(Voice_data) - 1);
    Voice_data[sizeof(Voice_data) - 1] = '\0';
    remove_newlines(Voice_data);

  /****************需要发送的文本**********************************/
    rt_uint8_t  Frame_Info[120];
    rt_uint8_t  HZ_Length;
  //unsigned  char  ecc  = 0;             //定义校验字节
  //unsigned  int i = 0;
  HZ_Length = rt_strlen((rt_uint8_t *)Voice_data);            //需要发送文本的长度
  //rt_kprintf("%d", HZ_Length);

  /*****************帧固定配置信息**************************************/
  Frame_Info[0] = 0xFD ;            //构造帧头FD
  Frame_Info[1] = 0x00 ;            //构造数据区长度的高字节
  Frame_Info[2] = HZ_Length + 2;        //构造数据区长度的低字节
  Frame_Info[3] = 0x01 ;            //构造命令字：合成播放命令
  Frame_Info[4] = 0x05 ; //构造命令参数：背景音乐设定

  /*******************校验码计算***************************************/
//  for(i = 0; i < 5; i++)                //依次发送构造好的5个帧头字节
//  {
//    ecc = ecc ^ (Frame_Info[i]);        //对发送的字节进行异或校验
//  }
//
//  for(i = 0; i < HZ_Length; i++)        //依次发送待合成的文本数据
//  {
//    ecc = ecc ^ (HZdata[i]);                //对发送的字节进行异或校验
//  }
  /*******************发送帧信息***************************************/
  rt_memcpy(&Frame_Info[5], Voice_data, HZ_Length);
  //Frame_Info[5 + HZ_Length] = ecc;
  //USART3_SendString(Frame_Info, 5 + HZ_Length + 1);
  rt_device_write(u4_dev, 0, Frame_Info, 5 + HZ_Length);
}


/***********************************************************
* 名    称： YS_SYN_Set(u8 *Info_data)
* 功    能： 主函数   程序入口
* 入口参数： *Info_data:固定的配置信息变量
* 出口参数：
* 说    明：本函数用于配置，停止合成、暂停合成等设置 ，默认波特率9600bps。
* 调用方法：通过调用已经定义的相关数组进行配置。
**********************************************************/
void YS_SYN_Set(rt_uint8_t *Info_data)
{
  rt_uint8_t Com_Len;
  Com_Len = strlen((rt_uint8_t*)Info_data);
  //USART3_SendString(Info_data, Com_Len);
  rt_device_write(u4_dev, 0, Info_data, Com_Len);
}

#endif /* APPLICATIONS_SYN6288_C_ */
