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
white = pbc.color565(31,31,63)                                                               #(Blauanteil, Rotanteil, Gruenanteil)
green = pbc.color565(0,0,63)                                                                 #(0..31, 0..31, 0..63)
red = pbc.color565(0,31,0)
blue = pbc.color565(31,0,0)

# Einige Grafikelemente den Puffer zeichnen
framebuf_buffer.fill(black)                                                                  # Buffer mit Schwarz füllen
framebuf_buffer.text("Hallo Welt!",20,20,white)                                              # "Hallo Welt!" oben links in den Buffer schreiben
framebuf_buffer.ellipse(140, 20, 10, 5, red, True)                                           # Rotes, gefuelltes Oval an Position 140, 20 zeichnen 
framebuf_buffer.ellipse(180, 20, 10, 10, green, False)                                       # Gruenen, nicht gefuellten Kreis an Position 180, 20 zeichnen
framebuf_buffer.ellipse(220, 20, 5, 10, blue, True)                                          # Blaues, gefuelltes Oval an Position 220, 20 zeichnen

# Kreis ist jetzt im Puffer, den Puffer auf das Display übertragen
pb.tft.blit_buffer(buffer, 0, 0, buffer_width, buffer_height)