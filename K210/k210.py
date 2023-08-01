import sensor
import image
import lcd
import time
from machine import UART,Timer
from fpioa_manager import fm

lcd.init()
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_hmirror(False)
sensor.set_vflip(True)

#映射串口引脚
fm.register(6, fm.fpioa.UART1_RX, force=True)
fm.register(7, fm.fpioa.UART1_TX, force=True)

#初始化串口
uart = UART(UART.UART1, 115200, read_buf_len=4096)
#uart.write('Hello 01Studio!')

sensor.run(1)
green_threshold   = (0,   80,  -70,   -10,   -0,   30)
red_threshold = (30, 100, 15, 127, 15, 127)
while True:
    img=sensor.snapshot()
    blobs = img.find_blobs([green_threshold])
    #if(blobs != []):

        #print(blobs[0].w())
    if blobs != []:
        pixel = []
        for b in blobs:
            pixel.append(b.pixels())
        max_index = pixel.index(max(pixel))
        b = blobs[max_index]
        tmp=img.draw_rectangle(b[0:4])
        tmp=img.draw_cross(b[5], b[6])
        c=img.get_pixel(b[5], b[6])
        #print(b.x(),b.y(),b.w(),b.h())
        uart.write('!'+str(b.x()+round(b.w()/2))+','+str(b.y()+round(b.h()/2))+'@')
    lcd.display(img)
