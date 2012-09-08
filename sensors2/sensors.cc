#include <SoftwareSerial.h>
#include <OneWire.h>
#include <SerialCommandProcessor.h>

#define INT16_MIN (-32768)

SoftwareSerial mySerial(0, 1); // receive pin, transmit pin

OneWire ds(2);  // on pin 2

void setup(void) {
	// set the data rate for the SoftwareSerial port
	mySerial.begin(19200);
}

int16_t readTemperature(const char *tag){
	if(!ds.reset()){
		mySerial.print(tag);
		mySerial.print(' ');
		mySerial.println("No devices detected!");
		return INT16_MIN;
	}
	ds.skip();
	ds.write(0x44, 1);         // start conversion, with parasite power on at the end

	delay(1000);     // maybe 750ms is enough, maybe not
	// we might do a ds.depower() here, but the reset will take care of it.

	if(!ds.reset()){
		mySerial.print(tag);
		mySerial.print(' ');
		mySerial.println("Device vanished during read-wait!");
		return INT16_MIN;
	}
	ds.skip();
	ds.write(0xBE);         // Read Scratchpad

	uint8_t data[9];
	ds.read_bytes(&data[0], 9);
	if(OneWire::crc8(&data[0], 9) != 0){
		mySerial.print(tag);
		mySerial.print(' ');
		mySerial.println("bad crc!");
		return INT16_MIN;
	}
	ds.reset();

	// ok, got good data:

	// convert the data to actual temperature
	uint16_t raw = (data[1] << 8) | data[0];

	byte cfg = (data[4] & 0x60);
	if (cfg == 0x00) raw = raw << 3;  // 9 bit resolution, 93.75 ms
	else if (cfg == 0x20) raw = raw << 2; // 10 bit res, 187.5 ms
	else if (cfg == 0x40) raw = raw << 1; // 11 bit res, 375 ms
	// default is 12 bit resolution, 750 ms conversion time

	//mySerial.println(raw);
	return raw;
}

class Commands;
SerialCommandProcessor<Commands> commandProcessor;

class Commands {
public:
	static void handleCommand(char *cmd, uint8_t len);
};


void Commands::handleCommand(char* rawCommand, uint8_t commandLen) {
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
	if (strcmp(command, "") == 0 || strcmp(command, "ping") == 0) {
		// ping
		mySerial.print(tag);
		mySerial.print(' ');
		mySerial.println("pong");
	}
	/*  else if (strcmp(command, "rs") == 0) {
		uint16_t ps = ds.reset();
		mySerial.println("get presence:");
		mySerial.println(ps);
		}
	*/
	else if (strcmp(command, "get_temp") == 0) {
		uint16_t raw = readTemperature(tag);
		mySerial.print(tag);
		mySerial.print(' ');
		mySerial.println(raw);
	}
	else {
		// no command handler matched
		mySerial.print(tag);
		mySerial.println(" END 1");
		return;
	}
	mySerial.print(tag);
	mySerial.println(" END 0");
}

void loop(void) {
	commandProcessor.step(mySerial);
}
