#Quellcode als Antwort auf Kundenfrage
#Gerne erstelle ich auf Anfrage mehr solcher Beispiele

import pbc
import framebuf
import vga1_16x16

pb = pbc.PBC()

# PWM für Hintergrundbeleuchtung
backlight = machine.PWM(machine.Pin(26))
backlight.freq(1000)  # PWM-Frequenz einstellen

# Funktion zur Einstellung der Helligkeit (0-100%)
def set_backlight(brightness):
    duty_cycle = int((brightness / 100) * 65535)  # Berechnet Duty Cycle basierend auf der Helligkeit
    backlight.duty_u16(duty_cycle)

# Kreis ist jetzt im Puffer, den Puffer auf das Display übertragen
pb.tft.text(vga1_16x16, "Backlight", 45, 124)

# Helligkeit anpassen
set_backlight(50)  # 50% Helligkeit

#Eine Sekunde warten
pb.delay(1000)

#Spielereien
for i in range(0,50):
        set_backlight(50-i)
        pb.delay(40)
        
while True:
    for i in range(0,100):
        set_backlight(i)
        pb.delay(40)