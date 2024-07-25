#include <EEPROM.h>
#include "hardware/pll.h"
#include "hardware/clocks.h"

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#include "misc/helper.h"
#include "misc/starIntro.h"
#include "tabman/tabman.h"
#include "sclange/sclange.h"
#include "pictris/pictris.h"
#include "poopy/poopy.h"

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void dtob() {
  for (int i = 0; i < 255; i++) {
    analogWrite(26, i);
    delay(3);
  }
}

void btod() {
  for (int i = 0; i < 256; i++) {
    analogWrite(26, 255 - i);
    delay(3);
  }
}

void setup() {
  //set_sys_clock_khz(160000, true);
  tft.init(240, 280);
  //tft.setSPISpeed(125000000);

  tft.setRotation(4);

  //tft.setFont(&Seven_Segment12pt7b);
  tft.setTextColor(ST77XX_WHITE);
  tft.fillScreen(ST77XX_BLACK);

  pinMode(26, OUTPUT);
  analogWrite(26, 255);

  pinMode(KEY_RIGHT, INPUT_PULLUP);
  pinMode(KEY_DOWN, INPUT_PULLUP);
  pinMode(KEY_LEFT, INPUT_PULLUP);
  pinMode(KEY_UP, INPUT_PULLUP);
  pinMode(KEY_CENTER, INPUT_PULLUP);

  pinMode(KEY_A, INPUT_PULLUP);
  pinMode(KEY_B, INPUT_PULLUP);

  pinMode(SPEAKER, OUTPUT);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDY, OUTPUT);

  Serial.begin(9600);

  EEPROM.begin(256);

  if (eepromReadInt(0) >= 65500)
  {
    eepromWriteInt(0, 0);
  }
  if (eepromReadInt(2) >= 65500)
  {
    eepromWriteInt(2, 0);
  }
  if (eepromReadInt(4) >= 65500)
  {
    eepromWriteInt(4, 0);
  }
  if (eepromReadInt(6) >= 65500)
  {
    eepromWriteInt(6, 0);
  }

  //dtob();
  int gameID;

  {
    StarIntro intro(tft);
    gameID = intro.starIntro();
  }

  randomSeed(millis());
  btod();
  if (gameID == 1)
  {
    Pictris pic(tft);
    pic.run();
  }

  else if (gameID == 2)
  {
    Sclange scl(tft);
    scl.run();

  }
  else if (gameID == 3)
  {
    Tabman tab(tft);
    tab.run();
  }
  else if (gameID == 4)
  {
    Poopy pp(tft);
    pp.run();
  }
}

void loop() {}
