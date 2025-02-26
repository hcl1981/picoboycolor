// Serial example
// Written by seeseekey (https://seeseekey.net)
// Licensed under MIT License

// Flash via port UF2_Board
// After flashing switch to port /dev/cu.usbmodem12345 or similar
// Start serial monitor via Tools menu

// Colors
//
// Red: Waiting for serial connection
// Yellow: Send serial message
// Green: Sended serial message 

#define LED_RED 14
#define LED_YELLOW 13
#define LED_GREEN 12

void setup() {

  // Setup pins
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  // Set waiting led
  digitalWrite(LED_RED, HIGH);

  // Starts serial communication via USB
  Serial.begin(115200);

  // Waits for serial connection
  while (!Serial);

  // Deactivate waiting for connection led
  digitalWrite(LED_RED, LOW);

  // Send message via serial
  Serial.println("Picoboy Color serial connection successful.");
}

void loop() {

  // Send message via serial
  Serial.println("Hello from Picoboy Color. (" + String(millis()) + ")");

  // Deativate sending led
  digitalWrite(LED_YELLOW, LOW);

  // Show activity
  digitalWrite(LED_GREEN, HIGH);
  delay(250);
  digitalWrite(LED_GREEN, LOW);

  // Activate sending led
  digitalWrite(LED_YELLOW, HIGH);

  // Wait for one second
  delay(1000);
}