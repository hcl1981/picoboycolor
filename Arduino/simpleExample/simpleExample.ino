// Beispielprogramm zum PicoBoy Color

// Benoetigte Bibliotheken einbinden
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

//Gewuenschte Schriftart einbinden
//Liste unter https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
#include <Fonts/FreeSansBold12pt7b.h>

//Pins am Mikrocontroller festlegen
#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8

#define KEY_RIGHT 1
#define KEY_DOWN 2
#define KEY_LEFT 3
#define KEY_UP 4
#define KEY_CENTER 0
#define KEY_A 27
#define KEY_B 28

#define BACKLIGHT 26
#define SPEAKER 15

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 280

// Objekt erzeugen, dass spaeter das Display repraesentiert
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Canvas anlegen, auf das gezeichnet werden soll.
// Es wird zuerst auf eine "Leinwand" gezeichnet, die anschließend zum Display übertragen wird.
// So ist der Zeichenvorgang selbst nicht sichtbar.
// Hier Canvas der Groesse 240x280 angeleget. Waehle es immer nur so gross wie noetig.
GFXcanvas16 canvas(DISPLAY_WIDTH, DISPLAY_HEIGHT);

int x = 120;
int y = 140;

void setup() {

  // Display initialisieren
  tft.init(DISPLAY_WIDTH, DISPLAY_HEIGHT);

  // Uebertragung zum Display in den schnellstmoeglichen Modus schalten.
  tft.setSPISpeed(125000000);

  //Hintergrundbeleuchtung des Displays auf volle Helligkeit einstellen.
  pinMode(BACKLIGHT, OUTPUT);
  analogWrite(BACKLIGHT, 255);

  // Verwenete Pins auf Ein- bzw. Ausgang schalten
  pinMode(KEY_RIGHT, INPUT_PULLUP);
  pinMode(KEY_DOWN, INPUT_PULLUP);
  pinMode(KEY_LEFT, INPUT_PULLUP);
  pinMode(KEY_UP, INPUT_PULLUP);
  pinMode(KEY_CENTER, INPUT_PULLUP);
  pinMode(KEY_A, INPUT_PULLUP);
  pinMode(KEY_B, INPUT_PULLUP);

  // Bildschirm schwarz fuellen
  tft.fillScreen(0x0000);

  // Schriftart (für tft) festlegen, vollstaendige Liste s.o.
  // canvas.setFont(...), um sie fuer das Canvas festzulegen.
  tft.setFont(&FreeSansBold12pt7b);

  // Cursor an Position 30,30 setzen
  tft.setCursor(30, 30);

  // Textausgabe auf dem Display, ohne es vorher auf ein Canvas zu zeichnen
  tft.print("Hallo Welt!");

  // Eine Sekunde warten
  delay(1000);
}

void loop() {

  // Beim Druecken des Joysticks die x- bzw y-Position entsprechend aendern
  if (digitalRead(KEY_UP) == LOW) {
    y = y - 2;
  }

  if (digitalRead(KEY_DOWN) == LOW) {
    y = y + 2;
  }

  if (digitalRead(KEY_LEFT) == LOW) {
    x = x - 2;
  }

  if (digitalRead(KEY_RIGHT) == LOW) {
    x = x + 2;
  }

  // Canvas schwarz fuellen.
  canvas.fillScreen(0x0000);

  // Lila Kreis mit Radius 10 bei Position x,y zueichnen
  // Weitere Farbcodes unter: https://github.com/newdigate/rgb565_colors
  // Weitere Grafikfunktionen unter: https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives
  canvas.fillCircle(x, y, 10, 0x897B);

  // Canvas auf das display Uebertragen
  tft.drawRGBBitmap(0, 0, canvas.getBuffer(), canvas.width(), canvas.height());
}