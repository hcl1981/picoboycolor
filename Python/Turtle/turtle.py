## Die Turtle setzt auch die Datei pbc.py auf dem Picoboy Color voraus 

import framebuf
import math
import pbc

class Turtle:
    def __init__(self, pbc_):
        self.pbc = pbc_
        self.position = (0, 0)
        self.previous_position = (0, 0)
        self.color = pbc.color565(255, 255, 255)
        self.first_draw = True
        
        self.draw = True
        self.color = pbc.color565(255,255,255)
        self.speed = 100
        self.angle = 270
        self.positionX = 120.0
        self.positionY = 140.0
        self.visible = True
        
        self.colors = {
            "blue": pbc.color565(0, 0, 255),
            "green": pbc.color565(0, 255, 0),
            "red": pbc.color565(255, 0, 0),
            "yellow": pbc.color565(255, 255, 0),
            "cyan": pbc.color565(0, 255, 255),
            "magenta": pbc.color565(255, 0, 255),
            "white": pbc.color565(255, 255, 255),
            "black": pbc.color565(0, 0, 0),
            "orange": pbc.color565(255, 165, 0),
            "purple": pbc.color565(128, 0, 128)
        }
        
        self.backup_size = 26
        self.canvas_backup_buffer = bytearray(self.backup_size * self.backup_size * 2)
        self.canvas_backup = framebuf.FrameBuffer(
            self.canvas_backup_buffer,
            self.backup_size,
            self.backup_size,
            framebuf.RGB565
        )
        
    def swap_bytes(self,value):
        return ((value & 0xFF) << 8) | ((value >> 8) & 0xFF)
        
    def setPenColor(self, color):
        self.color = self.colors.get(color, pbc.color565(255,255,255))
        self.drawTurtle()
        
    def spc(self, color):
        self.setPenColor(color)

    def left(self,angle):
        self.angle = (self.angle - angle) % 360
        self.drawTurtle()
        
    def lt(self,angle):
        self.left(angle)
        
    def right(self,angle):
        self.angle = (self.angle + angle) % 360
        self.drawTurtle()
        
    def rt(self,angle):
        self.right(angle)
        
    def penUp(self):
        self.draw=False
        
    def pu(self):
        self.penUp()
        
    def penDown(self):
        self.draw=True
        
    def pd(self):
        self.penDown()
    
    def hide(self):
        self.visible=False
        self.drawTurtle()
        
    def lt(self):
        self.hide()
        
    def show(self):
        self.visible=True
        self.drawTurtle()
        
    def st(self):
        self.show()
    
    def setDirection(self,angle):
        angle = angle + 270
        self.angle = angle % 360
        self.drawTurtle()
        
    def sd(self,angle):
        self.setAngle(angle)
        
    def setPosition(self,x,y):
        self.positionX = x + 120
        self.positionY = 140 - y
        self.drawTurtle()
        
    def sp(self,x,y):
        self.setPosition(x,y)
        
    def moveTo(self, x, y):
        backupDirection = self.angle
        dx = (x + 120) - self.positionX
        dy = (140 - y) - self.positionY
        angle = math.degrees(math.atan2(dy, dx)) + 90
        self.setDirection(angle)
        distance = math.sqrt(dx**2 + dy**2)
        self.forward(distance)
        self.setDirection(backupDirection)
        self.drawTurtle()
        
    def mt(self,x,y):
        moveTo(x,y)
    
    def home(self):
        self.setPosition(0,0)
        
    def h(self):
        self.home()
        
    def clear(self,color=pbc.color565(0,0,0)):
        self.pbc.canvas.fill(self.colors.get(color, pbc.color565(0,0,0)))
        self.canvas_backup.fill(self.colors.get(color, pbc.color565(0,0,0)))
        self.pbc.tft.fill(self.colors.get(color, pbc.color565(0,0,0)))
        self.drawTurtle()
        
    def c(self,color=pbc.color565(0,0,0)):
        self.clear(color)

    def forward(self,dist):
        for i in range(int(dist)):
            rad = math.radians(self.angle)
            self.positionX = self.positionX + math.cos(rad)
            self.positionY = self.positionY + math.sin(rad)
            self.drawTurtle()
            if(self.draw):
                self.pbc.canvas.pixel(int(self.positionX),int(self.positionY),self.color)
                self.pbc.tft.pixel(int(self.positionX),int(self.positionY),self.color)
                
    def fd(self,dist):
        self.forward(dist)
                
    def back(self,dist):
        for i in range(int(dist)):
            rad = math.radians(self.angle)
            self.positionX = self.positionX - math.cos(rad)
            self.positionY = self.positionY - math.sin(rad)
            self.drawTurtle()
            if(self.draw):
                self.pbc.canvas.pixel(int(self.positionX),int(self.positionY),self.color)
                self.pbc.tft.pixel(int(self.positionX),int(self.positionY),self.color)
                
    def bk(self,dist):
        self.back(dist)

#################################################################################################

    def drawTurtle(self):
        self.drawTriangle(int(self.positionX),int(self.positionY),self.angle-120)
   
    def drawTriangle(self, x, y, angle=0):
        half_size = self.backup_size // 2

        # Bildschirmdimensionen (Passe diese Werte an dein Display an)
        DISPLAY_WIDTH = self.pbc.BUFFER_WIDTH
        DISPLAY_HEIGHT = self.pbc.BUFFER_HEIGHT

        # Funktion zur Begrenzung der Koordinaten
        def clamp(value, min_value, max_value):
            return max(min_value, min(value, max_value))

        # Alte Position wiederherstellen
        if not self.first_draw:
            px, py = self.previous_position
            restore_x = clamp(px - half_size, 0, DISPLAY_WIDTH - self.backup_size)
            restore_y = clamp(py - half_size, 0, DISPLAY_HEIGHT - self.backup_size)

            self.pbc.tft.blit_buffer(
                self.pbc.rgb2bgr565(self.canvas_backup_buffer),  # Pixel des alten Hintergrunds
                restore_x,
                restore_y,
                self.backup_size,
                self.backup_size
            )

        # Neuen Bereich ins Backup sichern
        backup_x = clamp(x - half_size, 0, DISPLAY_WIDTH - self.backup_size)
        backup_y = clamp(y - half_size, 0, DISPLAY_HEIGHT - self.backup_size)
        self.canvas_backup.blit(self.pbc.canvas, -backup_x, -backup_y)

        # Neues Dreieck direkt ins Display malen
        if self.visible:
            self.draw_triangle_direct(x, y, angle)

        # Positionen merken
        self.previous_position = (x, y)
        self.position = (x, y)
        self.first_draw = False

    def draw_triangle_direct(self, x, y, angle):
        side_length = 8
        rad = math.radians(angle)

        # 3 Eckpunkte berechnen
        p1_x = x + int(side_length * math.cos(rad))
        p1_y = y + int(side_length * math.sin(rad))

        p2_x = x + int(side_length * math.cos(rad + 2.0944))  # +120°
        p2_y = y + int(side_length * math.sin(rad + 2.0944))

        p3_x = x + int(side_length * math.cos(rad - 2.0944))  # -120°
        p3_y = y + int(side_length * math.sin(rad - 2.0944))

        # Nun direkt aufs Display zeichnen:
        tftline = self.pbc.tft.line
        c = self.color

        tftline(p1_x, p1_y, p2_x, p2_y, c)
        tftline(p2_x, p2_y, p3_x, p3_y, c)
        #tftline(p3_x, p3_y, p1_x, p1_y, c)
