#Quellcode als Antwort auf Kundenfrage
#Gerne erstelle ich auf Anfrage mehr solcher Beispiele

import pbc
import framebuf

#Alle verfügbaren Schriften:
import NotoSans_32
import NotoSansMono_32
import vga1_8x8
import vga1_16x16
import vga1_bold_16x16

pb = pbc.PBC()

# Einige Textelemente zeichnen
pb.tft.write(NotoSans_32, "NotoSans_32", 10, 110)
pb.tft.write(NotoSansMono_32, "NotoSansMono_32", 10, 140)
pb.tft.text(vga1_8x8, "vga1_8x8", 10, 20)
pb.tft.text(vga1_16x16, "vga1_16x16", 10, 50)
pb.tft.text(vga1_bold_16x16, "vga1_bold_16x16", 10, 80)

#Leider können die Schriftarten nicht verwendet werden, um in den Buffer zu schreiben, da der MicroPython-Buffer
#dies nicht unterstützt. Auf ein aus dem Buffer gezeichnetes Bild können sie natürlich angewendet werden.