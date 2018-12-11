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
* Optional _OneKey_ commands that are single characters without the need for
    additional command termination

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

### OneKey commands

When sending commands from a keyboard, it is often useful to be able to use
single keypresses to perform a specific action, like adjusting a motor speed up
or down, for example.

To make any command a _OneKey_ command, instantiate the command with a **single
character** command string, and add `true` as a third argument:

```c++
SerialCommand cmd_faster("+", cmd_faster, true);
SerialCommand cmd_slower("-", cmd_slower, true);
```

The `cmd_faster` and `cmd_slower` command functions has the same signature as
for multiple character commands, except that arguments are not allowed for
_OneKey_ commands (they don't make sense), so calling `sender->Next()` is not
supported.

_OneKey_ commands are only active when they come in as the _first_ character,
either on startup, or after processing a previous command. In other words, if
another command string contains a `+` like `set+go` for example, the command
callback for the one key `+` command will not be called when `set+go` is
entered, and the `set+go` command callback will be called as expected.

Any multi character command that _starts_ with the same character as a one key
command, will however never be activated as the one key command will always
consume the single character command first, i.e:

Multi char command: `up`  
OneKey command: `u`

The `up` command will never be executed since the `u` will be consumed by the
one key command first, which leaves the `p` as the remainder for the command
which will not match (unless of course there *is* in fact a `p` one key or multi char
command).

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
