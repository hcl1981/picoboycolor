from micropython import const
import utime as time
import random
from machine import Pin, SPI
from st7789 import *
import framebuf
import os
import math


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
    
    FOREGROUND = color565(31,31,63)
    BACKGROUND = color565(0,0,0) 
    MARKER = color565(0,31,0) 

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
        
        x = x % self.BUFFER_WIDTH
        y = y % self.BUFFER_HEIGHT

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
        if self.canvas: 
            self.tft.blit_buffer(self.canvas, 0, 0, self.BUFFER_WIDTH, self.BUFFER_HEIGHT)
        else:
            # if canvas not set (e.g. when some file was executed via the menu), we have to create it again
            self.__init__()
            self.tft.blit_buffer(self.canvas, 0, 0, self.BUFFER_WIDTH, self.BUFFER_HEIGHT)

        
    def menu(self, menuFile=""):
        self.programList = [".".join(program.split('.')[:-1]) for program in os.listdir("/") if program.endswith('.py')]
        
        # remove pbc class from menu and the menuFile if given by the menu() call
        self.programList = [program for program in self.programList if not (program == 'pbc') | (program == menuFile)]
    

        self.selectorProgram = 0
        selectorVisible = 0
        self.paginate = 0
        maxVisible = 10
        
        self.drawPrograms(0, maxVisible)

        while True:
            if self.pressedUp():
                self.selectorProgram = (self.selectorProgram - 1) % len(self.programList)
                selectorVisible = self.selectorProgram % maxVisible
                self.paginate = self.selectorProgram // maxVisible            
        
            if self.pressedDown():
                self.selectorProgram = (self.selectorProgram + 1) % len(self.programList)
                selectorVisible = self.selectorProgram % maxVisible
                self.paginate = self.selectorProgram // maxVisible
        
            self.drawPrograms(self.paginate, maxVisible)     
                
            if self.pressedA() or self.pressedB():
                self.canvas.fill(self.BACKGROUND)
                self.canvas = None 
                                
                exec_context = {} # this is necessary so that globals and locals use the same context
                exec(open(self.programList[self.selectorProgram] + '.py').read() + \
                     "\npb.delay(2000)\npb.canvas=None", # this deletes the canvas at the end of the script
                     exec_context, exec_context)
        
            self.draw()
            self.delay(100)
                
                
    def drawPrograms(self, paginate, maxPrograms):
    
        self.canvas.fill(self.BACKGROUND)
        self.canvas.text("Programm/Spiel auswaehlen", 20, 60, self.FOREGROUND)
        self.canvas.text("Seite " + str(paginate+1) + '/' + str(math.ceil(len(self.programList)/maxPrograms)), 20, 80 + 12*(maxPrograms+1), self.FOREGROUND)
        for n, program in enumerate(self.programList[paginate*maxPrograms : (paginate*maxPrograms + maxPrograms)]):    
            self.canvas.text(program, 20 + 20, 60 + 10 + (n+1)*12, self.FOREGROUND)

        self.canvas.ellipse(30, 65 + 6 + (self.selectorProgram % maxPrograms + 1)*12, 5, 5, self.MARKER, 1)


