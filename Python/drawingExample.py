import pbc

# PicoBoyColor-Objekt instanzieren
pb = pbc.PBC()

# Einige Farben definieren
black = pbc.color565(0, 0, 0)
white = pbc.color565(31, 31, 63)
green = pbc.color565(0, 0, 63)
red   = pbc.color565(0, 31, 0)
blue  = pbc.color565(31, 0, 0)

# Canvas (= interner Buffer der PBC-Klasse) leeren
pb.canvas.fill(black)

# Grafikelemente in den Canvas zeichnen
pb.canvas.text("Hallo Welt!", 20, 20, white)
pb.canvas.ellipse(140, 20, 10, 5, red, True)      # Gefülltes rotes Oval
pb.canvas.ellipse(180, 20, 10, 10, green, False)  # Ungefüllter grüner Kreis
pb.canvas.ellipse(220, 20, 5, 10, blue, True)     # Gefülltes blaues Oval

# Den Canvas auf das Display übertragen
pb.draw()
