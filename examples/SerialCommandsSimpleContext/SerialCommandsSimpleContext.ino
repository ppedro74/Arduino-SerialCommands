/*--------------------------------------------------------------------
  Author    : Pedro Pereira
  License   : BSD
  Repository  : https://github.com/ppedro74/Arduino-SerialCommands

  SerialCommands Simple Demo

  --------------------------------------------------------------------*/
#include <Arduino.h>
#include "SampleClass.hpp"


// Pin 13 has an LED connected on most Arduino boards.
// Pin 11 has the LED on Teensy 2.0
// Pin 6  has the LED on Teensy++ 2.0
// Pin 13 has the LED on Teensy 3.0
SampleClass sampleClass(13);

void setup() {
  Serial.begin(57600);

  Serial.println("Ready!");
}

void loop() { sampleClass.loop(); }
