#include <TFT_eSPI.h>
#define LEDR          14
#define LEDY          13
#define LEDG          12
#define KEY_RIGHT      2
#define KEY_DOWN       3
#define KEY_LEFT       4
#define KEY_UP         1
#define KEY_CENTER     0
#define SPEAKER       15

TFT_eSPI tft = TFT_eSPI();


void setup() {
 
  pinMode(26, OUTPUT);
  analogWrite(26, 100);

  pinMode(KEY_RIGHT, INPUT_PULLUP);
  pinMode(KEY_DOWN, INPUT_PULLUP);
  pinMode(KEY_LEFT, INPUT_PULLUP);
  pinMode(KEY_UP, INPUT_PULLUP);
  pinMode(KEY_CENTER, INPUT_PULLUP);

  pinMode(SPEAKER, OUTPUT);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDY, OUTPUT);

  Serial.begin(9600);

  tft.init();
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("Hello, World!", 50, 50, 2);
}

void loop() {}
