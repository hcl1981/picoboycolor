from micropython import const
import utime as time
import random
from machine import Pin, SPI
from st7789 import *
import framebuf

class PBC():
    LED_RED = Pin(5, Pin.OUT)
    LED_YELLOW = Pin(6, Pin.OUT)
    LED_GREEN = Pin(7, Pin.OUT)
    
    JOY_UP = Pin(4, Pin.IN, Pin.PULL_UP)
    JOY_DOWN = Pin(2, Pin.IN, Pin.PULL_UP)
    JOY_LEFT = Pin(3, Pin.IN, Pin.PULL_UP)
    JOY_RIGHT = Pin(1, Pin.IN, Pin.PULL_UP)
    JOY_CENTER = Pin(0, Pin.IN, Pin.PULL_UP)
    BUTTON_A = Pin(27, Pin.IN, Pin.PULL_UP)
    BUTTON_B = Pin(28, Pin.IN, Pin.PULL_UP)
    
    SPEAKER = Pin(15, Pin.OUT)
    
    BUFFER_WIDTH = 240
    BUFFER_HEIGHT = 280
    

    def __init__(self,baudrate=31250000):
        BACKLIGHT_PIN = 26
        RESET_PIN = 9
        DC_PIN = 8
        CS_PIN = 10
        CLK_PIN = 18
        DIN_PIN = 19
        spi = SPI(0, baudrate, sck=Pin(CLK_PIN), mosi=Pin(DIN_PIN))
        self.tft = ST7789(spi, self.BUFFER_WIDTH, self.BUFFER_HEIGHT,  #st7789
            reset=Pin(RESET_PIN, Pin.OUT),
            cs=Pin(CS_PIN, Pin.OUT),
            dc=Pin(DC_PIN, Pin.OUT),
            backlight=Pin(BACKLIGHT_PIN, Pin.OUT),
            rotation=2)
        self.tft.init()
        
        buffer = bytearray(self.BUFFER_WIDTH * self.BUFFER_HEIGHT * 2)
        self.canvas = framebuf.FrameBuffer(buffer, self.BUFFER_WIDTH, self.BUFFER_HEIGHT, framebuf.RGB565)
        
        
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
    
    def pressedA(self):
        return self.BUTTON_A.value()==0
    
    def pressedB(self):
        return self.BUTTON_B.value()==0
    
    def delay(self,millis):
        time.sleep_ms(millis)

    def reset(self):
        super().reset(self.res)
        
        
    def setPeriodicBoundary(self, x, y):
        
        if x < 0:
            x = self.BUFFER_WIDTH + x
            
        if x >= self.BUFFER_WIDTH:
            x = x - self.BUFFER_WIDTH
            
        if y < 0:
            y = self.BUFFER_HEIGHT + y
            
        if y >= self.BUFFER_HEIGHT:
            y  = y - self.BUFFER_HEIGHT
        
        return x, y
    
    def setHardBoundary(self, x, y, radius=0):
        
        if x < (0 + radius):
            x = 0 + radius
            
        if x >= (self.BUFFER_WIDTH - radius):
            x = self.BUFFER_WIDTH - 1 - radius
            
        if y < (0 + radius):
            y = 0 + radius
            
        if y >= (self.BUFFER_HEIGHT - radius):
            y  = self.BUFFER_HEIGHT - 1 - radius
        
        return x, y
    
    def draw(self):
        self.tft.blit_buffer(self.canvas, 0, 0, self.BUFFER_WIDTH, self.BUFFER_HEIGHT)    
        

