## Die Turtle-Demo setzt auch die Dateien turtle.py und pbc.py auf dem Picoboy Color voraus 

import pbc    
from turtle import Turtle

pb = pbc.PBC()

t = Turtle(pb)

t.penDown()
t.setPenColor("purple")
t.moveTo(100,100)
t.moveTo(0,100)
t.moveTo(0,0)

t.setPenColor("red")
t.setDirection(0)
t.forward(50)
t.setPenColor("green")
t.left(90)
t.forward(50)
t.setPenColor("blue")
t.left(90)
t.forward(50)
t.setPenColor("yellow")
t.left(90)
t.forward(50)

t.penUp()
t.forward(50)
t.penDown()

t.setPenColor("white")
t.hide()
for i in range(0,360,3):
    t.left(3)
    t.forward(2)
t.show()

t.clear("green")
t.setPenColor("red")
t.setDirection(0)
t.setPosition(0,0)
#t.hide()
while True:
    if pb.pressedLeft():
        t.left(3)
    if pb.pressedRight():
        t.right(3)
    t.forward(1)
        
