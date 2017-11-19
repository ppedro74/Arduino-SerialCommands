# SerialCommands
An Arduino library to tokenize and parse commands received over a serial port. 

Inspired by (original version):
https://github.com/scogswell/ArduinoSerialCommand

This library is for handling text commands over a serial port.

Goals:
* Small footprint
* No dynamic memory allocation
* Stream class's Serial Ports
* Multiple Serial Ports callback methods
* Custom Command termination default is `CR & LF`
* Custom Arguments Delimeter default is `SPACE`

## SerialCommand object

SerialCommand creates a binding between a const string token with a callback function.

Note:
SerialCommand is used in a Linked List. Do not share SerialCommand objects between SerialCommands.

```c++
#include <SerialCommands.h>

void cmd_hello(SerialCommands* sender)
{
	//Do not use Serial.Print!
	//Use sender->GetSerial this allows sharing the callback method with multiple Serial Ports
	sender->GetSerial()->println("HELLO from arduino!");
}

SerialCommand cmd_hello_("hello", cmd_hello);
```

## SerialCommands object

SerialCommands is the main object creates a binding between a Serial Port and a list of SerialCommand

```c++
#include <SerialCommands.h>

//Create a 32 bytes static buffer to be used exclusive by SerialCommands object.
//The size should accomodate command token, arguments, termination sequence and string delimeter \0 char.
char serial_command_buffer_[32];

//Creates SerialCommands object attached to Serial
//working buffer = serial_command_buffer_
//command delimeter: Cr & Lf
//argument delimeter: SPACE
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");
```

### Arduino setup

```c++
void setup() 
{
	Serial.begin(57600);
	serial_commands_.AddCommand(&cmd_hello);
}
```
### Default handler

```c++
void cmd_unrecognized(SerialCommands* sender, const char* cmd)
{
	sender->GetSerial()->print("ERROR: Unrecognized command [");
	sender->GetSerial()->print(cmd);
	sender->GetSerial()->println("]");
}
void setup() 
{
	serial_commands_.SetDefaultHandler(&cmd_unrecognized);
}
```
### Arduino loop

```c++
void loop() 
{
	serial_commands_.ReadSerial();
}
```

### Multiple Serial Ports
```c++
#include <SerialCommands.h>

char serial_commands_0_buffer_[32];
SerialCommands serial_commands_0_(&Serial, serial_commands_0_buffer_, sizeof(serial_commands_0_buffer_));

char serial_commands_1_buffer_[32];
SerialCommands serial_commands_1_(&Serial1, serial_commands_1_buffer_, sizeof(serial_commands_1_buffer_));

void cmd_hello(SerialCommands* sender)
{
	sender->GetSerial()->println("HELLO from arduino!");
}

void cmd_unrecognized(SerialCommands* sender, const char* cmd)
{
	sender->GetSerial()->print("ERROR: Unrecognized command [");
	sender->GetSerial()->print(cmd);
	sender->GetSerial()->println("]");
}

SerialCommand cmd_hello_0_("hello", cmd_hello);
SerialCommand cmd_hello_1_("hello", cmd_hello);

void setup() 
{
    Serial.begin(57600);
	serial_commands_0_.AddCommand(&cmd_hello_0_);
	serial_commands_0_.SetDefaultHandler(&cmd_hello);
	
    Serial1.begin(57600);
	serial_commands_1_.AddCommand(&cmd_hello_1_);
	serial_commands_1_.SetDefaultHandler(&cmd_hello);
}

void loop() 
{
	serial_commands_0_.ReadSerial();
	serial_commands_1_.ReadSerial();
}
```