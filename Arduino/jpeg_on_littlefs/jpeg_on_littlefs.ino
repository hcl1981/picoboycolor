#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <JPEGDecoder.h>  //Muss installiert werden
#include <LittleFS.h>     //Ist im Arduino-Core enthalten
//Upload von Daten: https://github.com/earlephilhower/arduino-littlefs-upload

#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  // Serielle Schnittstelle initialisieren
  Serial.begin(9600);

  // Display initialisieren
  tft.init(240, 280);  // Setze die Auflösung entsprechend deines Displays
  tft.fillScreen(ST77XX_BLACK);

  //Hintergrundbelechtung einschalten
  pinMode(26, OUTPUT);
  analogWrite(26, 255);

  // LittleFS initialisieren
  if (!LittleFS.begin()) {
    Serial.println("LittleFS-Mount fehlgeschlagen!");
    return;
  }
}

void drawJPEG(const char* filename, int xpos, int ypos) {
  File jpegFile = LittleFS.open(filename, "r");  // Bilddatei öffnen
  if (!jpegFile) {
    Serial.println("Bilddatei konnte nicht geöffnet werden!");
    return;
  }

  // Bild dekodieren
  JpegDec.decodeFsFile(jpegFile);

  // Bild zeichnen
  uint16_t* pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint16_t mcu_x = 0;
  uint16_t mcu_y = 0;

  while (JpegDec.read()) {
    pImg = JpegDec.pImage;
    for (int y = 0; y < mcu_h; y++) {
      for (int x = 0; x < mcu_w; x++) {
        uint16_t color = *pImg++;
        tft.drawPixel(x + xpos + mcu_x, y + ypos + mcu_y, color);
      }
    }

    mcu_x += mcu_w;
    if (mcu_x >= JpegDec.width) {
      mcu_x = 0;
      mcu_y += mcu_h;
    }
  }
}

void loop() {
  drawJPEG("/1.jpg", 0, 0);
  delay(1000);
  drawJPEG("/2.jpg", 0, 0);
  delay(1000);
}