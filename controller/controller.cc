#include <Arduino.h>
#include <RemoteReceiver.h>
#include <RemoteTransmitter.h>
#include <SerialCommandProcessor.h>

RemoteTransmitterID tx(8);

class ControllerCommands;
SerialCommandProcessor<ControllerCommands> commandProcessor;

class ControllerCommands {
public:
	static void handleCommand(char *cmd, uint8_t len);
};

static const char set_id_cmd[] = "set_id";
static const char send_code_cmd[] = "send_code";
static const char send_raw_cmd[] = "send_raw";
static const char button_cmd[] = "button";

enum LightSwitch {
	Light, Switch
};

enum Button {
	Button1 = 6,
	Button2 = 4,
	Button3 = 2,
	All = 0
};

uint8_t composeButtons(enum LightSwitch ls, Button b, bool on) {
	if (b == All) {
		return on ? 9 : 1;
	}
	else {
		return (ls == Light) | b | (((uint8_t)on) << 3);
	}
}

const char *nameButton(Button b) {
	switch (b) {
	case Button1: return "1";
	case Button2: return "2";
	case Button3: return "3";
	case All: return "a";
	default: return "";
	}
}

Button readButton(char buttonChar) {
	static const Button buttons[] = {Button1, Button2, Button3};
	if (buttonChar >= '1' && buttonChar <= '3')
		return buttons[buttonChar - '1'];
	else
		return All;
}

void ControllerCommands::handleCommand(char *rawCommand,
                                       uint8_t commandLen)
{
	char *tag = rawCommand;
	char *command = strchr(tag, ' ');
	if (!command)
		return;
	*command = 0;
	command++;
	commandLen -= (command - tag);

	if (strcmp(command, "echo off") == 0) {
		commandProcessor.echo = false;
	}
	else if (strcmp(command, "echo on") == 0) {
		commandProcessor.echo = true;
	}
	else if (strcmp(command, "") == 0 || strcmp(command, "ping") == 0) {
		Serial.print(tag);
		Serial.print(' ');
		Serial.println("pong");
	}
	else if (commandLen >= sizeof(set_id_cmd) &&
	         memcmp(command, set_id_cmd, sizeof(set_id_cmd) - 1) == 0)
	{
		uint32_t id = atol(command + sizeof(set_id_cmd));
		Serial.print(tag);
		Serial.print(' ');
		Serial.print("id = ");
		Serial.println(id);
		tx.setID(id);
	}
	else if (commandLen >= sizeof(send_code_cmd) &&
	         memcmp(command, send_code_cmd, sizeof(send_code_cmd) - 1) == 0)
	{
		int code = atoi(command + sizeof(send_code_cmd));
		Serial.print(tag);
		Serial.print(' ');
		Serial.print("Sending code:");
		Serial.println(code);
		tx.sendCode(code);
	}
	else if (commandLen >= sizeof(send_raw_cmd) &&
	         memcmp(command, send_raw_cmd, sizeof(send_raw_cmd) - 1) == 0)
	{
		uint32_t raw = atol(command + sizeof(send_raw_cmd));
		Serial.print(tag);
		Serial.print(' ');
		Serial.print("sending = ");
		Serial.println(raw);
		tx.send(raw);
	}
	else if (commandLen >= sizeof(button_cmd) + 3 &&
	         memcmp(command, button_cmd, sizeof(button_cmd) - 1) == 0)
	{
		const char *switchCmd = command + sizeof(button_cmd);
		LightSwitch ls = switchCmd[0] == 'l' ? Light : Switch;
		Button button = readButton(switchCmd[1]);
		bool on = switchCmd[2] == 't' ? true : false;

		tx.sendCode(composeButtons(ls, button, on));
	}
	else {
		Serial.print(tag);
		Serial.println(" END 1");
		return;
	}
	Serial.print(tag);
	Serial.println(" END 0");
}


extern "C" {
	void setup();
	void loop();
}

void setup() {
	Serial.begin(57600);
	tx.begin();
	RemoteReceiver::start(0);
}

void loop() {
	commandProcessor.step(Serial);
	if (RemoteReceiver::dataReady()) {
		uint32_t data = RemoteReceiver::getData();
		uint32_t remoteID = data >> 4;
		uint8_t code = data & 0xf;
		uint8_t buttonPart = code & 0x6;

		bool on = code & 0x8;
		LightSwitch ls = code & 0x1 ? Light : Switch;
		Button b = buttonPart == 0 ? All : (Button) buttonPart;

		Serial.print("OOB BUTTON ");
		Serial.print(remoteID);
		Serial.print(' ');
		Serial.print(ls == Light ? 'l' : 's');
		Serial.print(nameButton(b));
		Serial.println(on ? 't' : 'f');
	}
}
