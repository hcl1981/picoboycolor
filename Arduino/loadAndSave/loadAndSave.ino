// Low level save and load example
// from the internal flash of the controller
//
// Written by seeseekey (https://seeseekey.net)
// Licensed under MIT License

// Libraries
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include <Arduino.h>

// Display libraries
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// Memory area for flash
#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - 2 * FLASH_SECTOR_SIZE)

// Structure for flash storage
struct FlashData {
  int counter;
  int checksum;
};

// Set pins on the microcontroller
#define BACKLIGHT 26

#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8

#define LED_RED 14
#define LED_YELLOW 13
#define LED_GREEN 12

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 280

#define SIGN_WIDTH 7
#define SIGN_HEIGHT 6

// Display object
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Counter
int counter = 0;

// Function for saving the counter in the flash memory
void saveCounter(int value) {

  // Fill flash data
  FlashData data;
  data.counter = value;
  data.checksum = value ^ 0xA5A5A5A5;  // XOR checksum

  uint8_t buffer[FLASH_PAGE_SIZE] = { 0 };
  memcpy(buffer, &data, sizeof(data));

  uint32_t ints = save_and_disable_interrupts();
  flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
  flash_range_program(FLASH_TARGET_OFFSET, buffer, FLASH_PAGE_SIZE);
  restore_interrupts(ints);
}

// Function for loading the counter from the flash memory
int loadCounter() {
  const FlashData* stored_data = (const FlashData*)(XIP_BASE + FLASH_TARGET_OFFSET);

  // Validate checksum
  if (stored_data->checksum != (stored_data->counter ^ 0xA5A5A5A5)) {
    return 0;  // If invalid, start with 0
  }

  return stored_data->counter;
}

void initAndDisplay() {

  // Init display
  tft.init(DISPLAY_WIDTH, DISPLAY_HEIGHT);

  // Set SPI speed
  tft.setSPISpeed(125000000);

  // Set the display backlight to full brightness
  pinMode(BACKLIGHT, OUTPUT);
  analogWrite(BACKLIGHT, 255);

  // Report successful initialization
  Serial.println("Picoboy Color serial connection successful.");

  // Clear screen
  tft.fillScreen(0x8410);

  // Draw circle
  tft.fillCircle(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, 25, 0xFC00);

  // Draw counter value
  tft.setCursor(DISPLAY_WIDTH / 2 - SIGN_WIDTH, DISPLAY_HEIGHT / 2 - SIGN_HEIGHT / 2);
  tft.print(counter);
}

void setup() {

  // Start serial
  Serial.begin(115200);

  // Set pins
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  // Initialization (red LED on)
  digitalWrite(LED_RED, HIGH);

  // Short wait time, to see if an error
  delay(1000);

  // Load counter from Flash
  counter = loadCounter();
  Serial.print("Counter vorher: ");
  Serial.println(counter);

  // Increase and save counter
  counter++;
  saveCounter(counter);

  Serial.print("Counter nachher: ");
  Serial.println(counter);

  // Initialization completed (red LED off)
  digitalWrite(LED_RED, LOW);

  // Show counter in display
  initAndDisplay();

  // Everything is okay
  digitalWrite(LED_GREEN, HIGH);
}

void loop() {

  // Do nothing, no display refresh is needed

  // Serial ping
  Serial.println("Ping from Picoboy Color. (" + String(millis()) + ")");
  Serial.println("Current startup counter: " + String(counter));

  delay(1000);
}