import pbc
import random
from st7789 import color565

pb = pbc.PBC()

##### Quellcode ueber dieser Linie bitte nicht aendern! #####


while True:
    
    pb.canvas.fill(0)
    pb.canvas.text("Hallo Welt", 20, 120,
                   color565(random.randrange(0,31), random.randrange(0,31), random.randrange(0,63)))
    pb.draw()
    
    if pb.pressedB():
        break
    
    pb.delay(200)

