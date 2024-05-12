from micropython import const
import utime as time
import random
from machine import Pin, SPI
#import st7789
from st7789 import *

class PBC():
    LED_RED = Pin(5, Pin.OUT)#7
    LED_YELLOW = Pin(6, Pin.OUT)#6
    LED_GREEN = Pin(7, Pin.OUT)#5
    
    JOY_UP = Pin(1, Pin.IN, Pin.PULL_UP)#0
    JOY_DOWN = Pin(3, Pin.IN, Pin.PULL_UP)#3
    JOY_LEFT = Pin(4, Pin.IN, Pin.PULL_UP)#4
    JOY_RIGHT = Pin(2, Pin.IN, Pin.PULL_UP)#2
    JOY_CENTER = Pin(0, Pin.IN, Pin.PULL_UP)#1
    #BUTTON_A =
    #BUTTON_B =
    
    
    SPEAKER = Pin(15, Pin.OUT)#15
    
    def __init__(self,baudrate=31250000):
        BACKLIGHT_PIN = 26
        RESET_PIN = 9
        DC_PIN = 8
        CS_PIN = 10
        CLK_PIN = 18
        DIN_PIN = 19
        spi = SPI(0, baudrate, sck=Pin(CLK_PIN), mosi=Pin(DIN_PIN))
        self.tft = ST7789(spi, 240, 280,  #st7789
            reset=Pin(RESET_PIN, Pin.OUT),
            cs=Pin(CS_PIN, Pin.OUT),
            dc=Pin(DC_PIN, Pin.OUT),
            backlight=Pin(BACKLIGHT_PIN, Pin.OUT),
            rotation=2)
        self.tft.init()
        
    def __getattr__(self, attr):
        if hasattr(self.tft, attr):
            return getattr(self.tft, attr)
        else:
            raise AttributeError("'PicoBoyColor' object has no attribute '{}'".format(attr))
    
    def pressedUp(self):
        return self.JOY_UP.value()==0
            
    def pressedDown(self):
        return self.JOY_DOWN.value()==0
    
    def pressedLeft(self):
        return self.JOY_LEFT.value()==0
    
    def pressedRight(self):
        return self.JOY_RIGHT.value()==0
    
    def pressedCenter(self):
        return self.JOY_CENTER.value()==0
    
    def delay(self,millis):
        time.sleep_ms(millis)

    def reset(self):
        super().reset(self.res)   
