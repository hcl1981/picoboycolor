import pbc

pb = pbc.PBC()

# Einige Farben definieren
black = pbc.color565(0, 0, 0)
red   = pbc.color565(0, 31, 0)

# Startposition festlegen
x = 120
y = 140

while True:
    # Joystick-Abfragen
    if pb.pressedUp():
        y -= 3
    if pb.pressedDown():
        y += 3
    if pb.pressedLeft():
        x -= 3
    if pb.pressedRight():
        x += 3

    # Canvas (= interner Buffer) mit Schwarz füllen und roten Kreis zeichnen
    pb.canvas.fill(black)
    pb.canvas.ellipse(x, y, 10, 10, red, True)

    # Canvas auf das Display übertragen
    pb.draw()
