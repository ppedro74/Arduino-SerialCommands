/*--------------------------------------------------------------------
Author		: Pedro Pereira
License		: BSD
Repository	: https://github.com/ppedro74/Arduino-SerialCommands

SerialCommands Simple Demo

--------------------------------------------------------------------*/
#include <Arduino.h>
#include <SerialCommands.h>

// Pin 13 has an LED connected on most Arduino boards.
// Pin 11 has the LED on Teensy 2.0
// Pin 6  has the LED on Teensy++ 2.0
// Pin 13 has the LED on Teensy 3.0
const int kLedPin = 13;

char serial_command_buffer_[32];
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");

//This is the default handler, and gets called when no other command matches. 
void cmd_unrecognized(SerialCommands* sender, const char* cmd)
{
  sender->GetSerial()->print("Unrecognized command [");
  sender->GetSerial()->print(cmd);
  sender->GetSerial()->println("]");
}

//called for ON command
void cmd_led_on(SerialCommands* sender)
{
  digitalWrite(kLedPin, HIGH);  
  sender->GetSerial()->println("Led is on");
}

//called for OFF command
void cmd_led_off(SerialCommands* sender)
{
  digitalWrite(kLedPin, LOW);  
  sender->GetSerial()->println("Led is off");
}

//Note: Commands are case sensitive
SerialCommand cmd_led_on_("ON", cmd_led_on);
SerialCommand cmd_led_off_("OFF", cmd_led_off);

void setup() 
{
  Serial.begin(57600);

  //Configure the LED for output and sets the intial state to off
  pinMode(kLedPin, OUTPUT);
  digitalWrite(kLedPin, LOW);

  serial_commands_.SetDefaultHandler(cmd_unrecognized);
  serial_commands_.AddCommand(&cmd_led_on_);
  serial_commands_.AddCommand(&cmd_led_off_);

  Serial.println("Ready!");
}

void loop() 
{
  serial_commands_.ReadSerial();
}
