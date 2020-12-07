/*--------------------------------------------------------------------
Author		: Pedro Pereira
License		: BSD
Repository	: https://github.com/ppedro74/Arduino-SerialCommands
--------------------------------------------------------------------*/

#include "SerialCommands.h"

void SerialCommands::AddCommand(SerialCommand* command)
{
#ifdef SERIAL_COMMANDS_DEBUG
	Serial.print("Adding #");
	Serial.print(commands_count_);
	Serial.print(" cmd=[");
	Serial.print(command->command);
	Serial.print("]");
	if(command->one_key)
	{
		Serial.print(" as one-key");
	}
	Serial.println();
#endif
	// Default linked list is commands_????
	SerialCommand** cmd_head=&commands_head_;
	SerialCommand** cmd_tail=&commands_tail_;
	uint8_t *cmd_count = &commands_count_;

	if(command->one_key)
	{
		// For a one_key command, we switch to the onek_cmds_???? list
		cmd_head = &onek_cmds_head_;
		cmd_tail = &onek_cmds_tail_;
		cmd_count = &onek_cmds_count_;
	}
	command->next = NULL;
	if (*cmd_head == NULL)
	{
		*cmd_head = *cmd_tail = command;
	}
	else
	{
		(*cmd_tail)->next = command;
		*cmd_tail = command;
	}
	(*cmd_count)++;
}

SERIAL_COMMANDS_ERRORS SerialCommands::ReadSerial()
{
	if (serial_ == NULL)
	{
		return SERIAL_COMMANDS_ERROR_NO_SERIAL;
	}

	while (serial_->available() > 0)
	{
		int ch = serial_->read();
#ifdef SERIAL_COMMANDS_DEBUG
		Serial.print("Read: bufLen=");
		Serial.print(buffer_len_);
		Serial.print(" bufPos=");
		Serial.print(buffer_pos_);
		Serial.print(" termPos=");
		Serial.print(term_pos_);
		if (ch<32)
		{
			Serial.print(" ch=#");
			Serial.print(ch);
		}
		else
		{
			Serial.print(" ch=[");
			Serial.print((char)ch);
			Serial.print("]");
		}
		Serial.println();
#endif
		if (ch <= 0)
		{
			continue;
		}

		if (buffer_pos_ < buffer_len_)
		{
			buffer_[buffer_pos_++] = ch;
		}
		else
		{
#ifdef SERIAL_COMMANDS_DEBUG			
			Serial.println("Buffer full");
#endif
			ClearBuffer();
			return SERIAL_COMMANDS_ERROR_BUFFER_FULL;
		}

		if(buffer_pos_==1 && CheckOneKeyCmd())
		{
			return SERIAL_COMMANDS_SUCCESS;
		}

		if (term_[term_pos_] != ch)
		{
			term_pos_ = 0;
			continue;
		}

		if (term_[++term_pos_] == 0)
		{
			buffer_[buffer_pos_ - strlen(term_)] = '\0';

#ifdef SERIAL_COMMANDS_DEBUG
			Serial.print("Received: [");
			Serial.print(buffer_);
			Serial.println("]");
#endif
			char* command = strtok_r(buffer_, delim_, &last_token_);
			if (command != NULL)
			{
				boolean matched = false;
				int cx;
				SerialCommand* cmd;
				for (cmd = commands_head_, cx = 0; cmd != NULL; cmd = cmd->next, cx++)
				{
#ifdef SERIAL_COMMANDS_DEBUG
					Serial.print("Comparing [");
					Serial.print(command);
					Serial.print("] to [");
					Serial.print(cmd->command);
					Serial.println("]");
#endif

					if (strncmp(command, cmd->command, strlen(cmd->command) + 1) == 0)
					{
#ifdef SERIAL_COMMANDS_DEBUG
						Serial.print("Matched #");
						Serial.println(cx);
#endif
						cmd->function(this);
						matched = true;
						break;
					}
				}
				if (!matched && default_handler_ != NULL)
				{
					(*default_handler_)(this, command);
				}
			}

			ClearBuffer();
		}
	}

	return SERIAL_COMMANDS_SUCCESS;
}

bool SerialCommands::CheckOneKeyCmd()
{
#ifdef SERIAL_COMMANDS_DEBUG
	Serial.println("Testing for one_key commands.");
#endif

	int cx;
	SerialCommand* cmd;
	for (cmd = onek_cmds_head_, cx = 0; cmd != NULL; cmd = cmd->next, cx++)
	{
#ifdef SERIAL_COMMANDS_DEBUG
		Serial.print("Testing [");
		Serial.print(buffer_[0]);
		Serial.print("] to [");
		Serial.print(cmd->command[0]);
		Serial.println("]");
#endif
		if (buffer_[0] == cmd->command[0])
		{
#ifdef SERIAL_COMMANDS_DEBUG
			Serial.print("Matched #");
			Serial.println(cx);
#endif
			cmd->function(this);
			ClearBuffer();
			return true;
		}
	}

	return false;
}


Stream* SerialCommands::GetSerial()
{
	return serial_;
}

void SerialCommands::AttachSerial(Stream* serial)
{
	serial_ = serial;
}

void SerialCommands::DetachSerial()
{
	serial_ = NULL;
}

void SerialCommands::SetDefaultHandler(void(*function)(SerialCommands*, const char*))
{
	default_handler_ = function;
}

void SerialCommands::ClearBuffer()
{
	buffer_[0] = '\0';
	buffer_pos_ = 0;
	term_pos_ = 0;
}

char* SerialCommands::Next()
{
	return strtok_r(NULL, delim_, &last_token_);
}

