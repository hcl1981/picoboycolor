import pbc
import framebuf

pb = pbc.PBC()

# Buffer (Zwischenspeicher) für das Zeichnen erstellen
buffer_width = 240
buffer_height = 280
buffer = bytearray(buffer_width * buffer_height * 2)                                         # 2 Bytes pro Pixel für RGB565 Format
framebuf_buffer = framebuf.FrameBuffer(buffer, buffer_width, buffer_height, framebuf.RGB565) # FrameBuffer erstellen (RGB565 Format)

# Einige Farben definieren
black = pbc.color565(0,0,0)                                                                  #color565 erwartet 3 Argumente
red = pbc.color565(0,31,0)                                                                   #(Blauanteil, Rotanteil, Gruenanteil)
                                                                                             #(0..31, 0..31, 0..63)
# Startposition festlegen
x = 120
y = 140

while True:                                                                                  # Wiederhole fuer immer
    if pb.pressedUp():                                                                       # Falls Joystick hoch gedrueckt
        y = y - 3                                                                            # y-Position verringern
    if pb.pressedDown():                                                                     # Falls Joystick runter gedrueckt
        y = y + 3                                                                            # y-Position erhoehen
    if pb.pressedLeft():                                                                     # Falls Joystick nach links gedrueckt
        x = x - 3                                                                            # x-Position verringern
    if pb.pressedRight():                                                                    # Falls Joystick nach rechts gedrueckt
        x = x + 3                                                                            # x-Position erhoehen

    framebuf_buffer.fill(black)                                                              # Buffer mit Schwarz füllen
    framebuf_buffer.ellipse(x, y, 10, 10, red, True)                                         # Kreis an Position x,y zeichnen
    pb.tft.blit_buffer(buffer, 0, 0, buffer_width, buffer_height)                            # Buffer zum Display uebertragen
        