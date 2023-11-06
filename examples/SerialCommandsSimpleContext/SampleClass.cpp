#include "SampleClass.hpp"
#include <SerialCommands.h>
#include <Arduino.h>

char serial_command_buffer_[32];
SerialCommands serial_commands_(&Serial, serial_command_buffer_,
                                sizeof(serial_command_buffer_), "\r\n", " ");

// This is the default handler, and gets called when no other command matches.
void cmd_unrecognized(SerialCommands *sender, const char *cmd) {
  sender->GetSerial()->print("Unrecognized command [");
  sender->GetSerial()->print(cmd);
  sender->GetSerial()->println("]");
}

// called for ON command
void cmd_led_on(SerialCommands *sender) {

  SampleClass *parent = static_cast<SampleClass *>(sender->context);
  parent->ledOnCounter++;
  sender->GetSerial()->print("Led was on ");
  sender->GetSerial()->print(parent->ledOnCounter);
  sender->GetSerial()->println("times");

  digitalWrite(parent->ledPin, HIGH);
  sender->GetSerial()->println("Led is on");
}

// called for OFF command
void cmd_led_off(SerialCommands *sender) {
  SampleClass *parent = static_cast<SampleClass *>(sender->context);
  digitalWrite(parent->ledPin, LOW);
  sender->GetSerial()->println("Led is off");
}

SampleClass::SampleClass(char ledPin) : ledPin(ledPin) {
  
    // Configure the LED for output and sets the intial state to off
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  ledOnCounter = 0;
  serial_commands_.context = this;
  serial_commands_.SetDefaultHandler(cmd_unrecognized);

  SerialCommand cmd_led_on_("ON", cmd_led_on);
  SerialCommand cmd_led_off_("OFF", cmd_led_off);

  serial_commands_.AddCommand(&cmd_led_on_);
  serial_commands_.AddCommand(&cmd_led_off_);
}

void SampleClass::loop() { serial_commands_.ReadSerial(); }
