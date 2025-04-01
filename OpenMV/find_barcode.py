# 条形码识别例程
#
# 这个例子展示了使用OpenMV Cam M7来检测条形码是多么容易。条形码检测不适用于M4相机。
START_SCAN = 'I'
FINISH_SCAN = 'J'

import sensor, image, time, math
from pyb import UART
import ustruct
import pyb

sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.VGA) # High Res!
sensor.set_windowing((640, 480)) # V Res of 80 == less work (40 for 2X the speed).
sensor.set_auto_gain(False)  # 必须关闭此功能，以防止图像冲洗…
sensor.set_auto_whitebal(False)  # 必须关闭此功能，以防止图像冲洗…
clock = time.clock()

uart = UART(3,115200,bits=8, parity=None, stop=1, timeout_char = 1000)#初始化串口三、波特率115200 TXD:P4\PB10 RXD:P5\PB11

def send_codenum(codenum):#功能发送五个无符号字符（unsigned char）
    global uart;
    #data = ustruct.pack("<B", codenum)

    uart.write(codenum);
    print(codenum)#通过 print(data) 打印发送的数据到串行终端，方便调试和确认发送的内容。

def start_scan():
    sensor.skip_frames(time = 2000)

    while True:
        clock.tick()
        img = sensor.snapshot()
        codes = img.find_barcodes()
        if codes:
            #img.draw_rectangle(code.rect())
            send_codenum(codes[0].payload())
            break
        if uart.any():
            command = uart.read()
            if command == FINISH_SCAN:
                break

# 条形码检测可以在OpenMV Cam的OV7725相机模块的640x480分辨率下运行。
# 条码检测也将在RGB565模式下工作，但分辨率较低。 也就是说，
# 条形码检测需要更高的分辨率才能正常工作，因此应始终以640x480的灰度运行。

while True:
    if uart.any():
        command = uart.read()
        if command == START_SCAN:
            command = ""
            start_scan()

        time.sleep_ms(100)
        continue

    pyb.stop()
