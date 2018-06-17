/*--------------------------------------------------------------------
Author		: Pedro Pereira
License		: BSD
Repository	: https://github.com/ppedro74/Arduino-SerialCommands

Modified    : Fitzterra <fitzterra@icave.net>
            : to demonstrate usage of the one_key commands

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
// Note: It does not get called for one_key commands that do not match
void cmd_unrecognized(SerialCommands* sender, const char* cmd)
{
  sender->GetSerial()->print("Unrecognized command [");
  sender->GetSerial()->print(cmd);
  sender->GetSerial()->println("]");
}

//called for ON command, or one_key '1' command
void cmd_led_on(SerialCommands* sender)
{
  digitalWrite(kLedPin, HIGH);  
  sender->GetSerial()->println("Led is on");
}

//called for OFF command, or one_key '0' command
void cmd_led_off(SerialCommands* sender)
{
  digitalWrite(kLedPin, LOW);  
  sender->GetSerial()->println("Led is off");
}

//Note: Commands are case sensitive
SerialCommand cmd_led_on_("ON", cmd_led_on);
SerialCommand cmd_led_off_("OFF", cmd_led_off);
// Add one_key commands to call the same on and off function but by simply
// pressing '1' or '0' without needing a terminating key stroke.
// One_key commands are ONLY tested when they are the first key pressed on
// startup, or the first key after a previous terminating keystroke.
SerialCommand cmd_led_on_ok_("1", cmd_led_on, true);
SerialCommand cmd_led_off_ok_("0", cmd_led_off, true);


void setup() 
{
  Serial.begin(115200);

  //Configure the LED for output and sets the intial state to off
  pinMode(kLedPin, OUTPUT);
  digitalWrite(kLedPin, LOW);

  serial_commands_.SetDefaultHandler(cmd_unrecognized);
  serial_commands_.AddCommand(&cmd_led_on_);
  serial_commands_.AddCommand(&cmd_led_off_);
  serial_commands_.AddCommand(&cmd_led_on_ok_);
  serial_commands_.AddCommand(&cmd_led_off_ok_);

  Serial.println("Ready!");
  Serial.println("\nLED On : type: 'ON' + press Enter, or only '1' without enter\n"
                 "LED Off: type 'OFF' + press Enter, or only '0' without enter.\n");
}

void loop() 
{
  serial_commands_.ReadSerial();
}
