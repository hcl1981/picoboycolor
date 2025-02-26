// Test Pattern
// Written by seeseekey (https://seeseekey.net)
// Licensed under MIT License

// Test programm to test the controller and the display
// 
// Controls
// Left / Right: Change background color
// Up / Down: Change brightness of backlight
// Left button (A): Flash display
// Right button (B): Tone test

// Display libararies
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// Multicore library
#include "pico/multicore.h"

// Define pins
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

#define LED_RED 14
#define LED_YELLOW 13
#define LED_GREEN 12

#define BACKLIGHT 26
#define SPEAKER 15

// Other defines

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 280

#define SIGN_WIDTH 7
#define SIGN_HEIGHT 6

// Definition of an array with 10 common colors in RGB565 format
uint16_t rgb565_colors[10] = {
  0xF800,  // Red
  0x07E0,  // Green
  0x001F,  // Blue
  0xFFFF,  // White
  0x0000,  // Black
  0xFFE0,  // Yellow
  0xF81F,  // Magenta
  0x07FF,  // Cyan
  0x8410,  // Gray
  0xFC00   // Orange
};

// Accent colors in RGB565 format
uint16_t rgb565_accent_colors[10] = {
  0x7BEF, // Light gray
  0x2D6B, // Dark green
  0xA145, // Brown
  0x5ACB, // Turquoise
  0x911A, // Violet
  0xFCA0, // Apricot
  0xF81F, // Pink
  0x04B0, // Mint green
  0xC618, // Medium gray
  0x3D8E // Olive green
};

// Define display and canvas object
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16 canvas(DISPLAY_WIDTH, DISPLAY_HEIGHT);

// Brightness of the backlight
int backlight_brightness = 255;

// Color of the background
int background_color_index = 0;

void setup() {

  // Init display
  tft.init(DISPLAY_WIDTH, DISPLAY_HEIGHT);

  // Set SPI speed
  tft.setSPISpeed(125000000);

  // Set the display backlight to full brightness
  pinMode(BACKLIGHT, OUTPUT);
  analogWrite(BACKLIGHT, backlight_brightness);

  // Switch used pins to input or output
  pinMode(KEY_RIGHT, INPUT_PULLUP);
  pinMode(KEY_DOWN, INPUT_PULLUP);
  pinMode(KEY_LEFT, INPUT_PULLUP);
  pinMode(KEY_UP, INPUT_PULLUP);
  pinMode(KEY_CENTER, INPUT_PULLUP);

  pinMode(KEY_A, INPUT_PULLUP);
  pinMode(KEY_B, INPUT_PULLUP);

  pinMode(SPEAKER, OUTPUT);

  // Start core 1 one process
  multicore_launch_core1(core1Loop);
}

void core1Loop() {

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  while (true) {
    digitalWrite(LED_RED, HIGH);
    delay(500);
    digitalWrite(LED_RED, LOW);

    digitalWrite(LED_YELLOW, HIGH);
    delay(500);
    digitalWrite(LED_YELLOW, LOW);

    digitalWrite(LED_GREEN, HIGH);
    delay(500);
    digitalWrite(LED_GREEN, LOW);
  }
}

void handleInputForBacklightBrightness() {

  if (digitalRead(KEY_DOWN) == LOW) {

    if (backlight_brightness > 0) {
      backlight_brightness -= 5;
      analogWrite(BACKLIGHT, backlight_brightness);
    }
  }

  if (digitalRead(KEY_UP) == LOW) {

    if (backlight_brightness < 255) {
      backlight_brightness += 5;
      analogWrite(BACKLIGHT, backlight_brightness);
    }
  }
}

void handleInputForBackground() {

  if (digitalRead(KEY_LEFT) == LOW) {
    if (background_color_index > 0) {
      background_color_index--;
    }

    delay(50);  // Debouncing
  }

  if (digitalRead(KEY_RIGHT) == LOW) {
    if (background_color_index < 10) {
      background_color_index++;
    }

    delay(50);  // Debouncing
  }
}

void flash() {

  for (int i = 0; i < 10; i++) {
    canvas.fillScreen(rgb565_colors[i]);
    tft.drawRGBBitmap(0, 0, canvas.getBuffer(), canvas.width(), canvas.height());
    delay(75);
  }
}

void loop() {

  // Background
  canvas.fillScreen(rgb565_colors[background_color_index]);

  // Test circle
  canvas.fillCircle(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, DISPLAY_WIDTH / 2, rgb565_accent_colors[9]);

  canvas.fillCircle(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, 25, rgb565_accent_colors[0]);

  // Middle lines
  canvas.drawLine(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, rgb565_accent_colors[4]);
  canvas.drawLine(DISPLAY_WIDTH, 0, 0, DISPLAY_HEIGHT, rgb565_accent_colors[4]);

  // Edges
  int edgeSize = 30;
  canvas.fillRect(0, 0, edgeSize, edgeSize, rgb565_accent_colors[5]);
  canvas.fillRect(DISPLAY_WIDTH - edgeSize, 0, edgeSize, edgeSize, rgb565_accent_colors[6]);

  canvas.fillRect(0, DISPLAY_HEIGHT - edgeSize, edgeSize, edgeSize, rgb565_accent_colors[7]);
  canvas.fillRect(DISPLAY_WIDTH - edgeSize, DISPLAY_HEIGHT - edgeSize, edgeSize, edgeSize, rgb565_accent_colors[8]);

  canvas.setCursor(edgeSize / 2, edgeSize / 2);
  canvas.print("TL");

  // Background brightness
  canvas.setCursor(DISPLAY_WIDTH / 2 - SIGN_WIDTH, DISPLAY_HEIGHT / 2 - SIGN_HEIGHT / 2);
  canvas.print(backlight_brightness);

  // Show millis since boot
  canvas.setCursor(DISPLAY_WIDTH / 2 + 50, 0 + DISPLAY_HEIGHT / 2 - SIGN_HEIGHT / 2);
  canvas.print(millis());

  // Handle input
  handleInputForBacklightBrightness();

  handleInputForBackground();

  // Handle input for flash
  if (digitalRead(KEY_A) == LOW) {
    flash();
  }

  // Handle input for tone
  if (digitalRead(KEY_B) == LOW) {
    tone(SPEAKER, 2500, 10);
  }

  // Canvas to tft shift test
  canvas.fillCircle(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 75, 25, rgb565_accent_colors[2]);

  // Draw to tft
  tft.drawRGBBitmap(0, 0, canvas.getBuffer(), canvas.width(), canvas.height());

  // Tft to canvas shift test
  tft.fillCircle(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 75, 25, rgb565_accent_colors[3]);

  // Delay
  delay(10);
}