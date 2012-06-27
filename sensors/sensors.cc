#include "HardwareSerial.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "cpp-prelude.h"
#include "cpp-prelude/digitalpin.h"
#include "cpp-prelude/digitalpinpulledup.h"

#include "timing.h"

// Simple digital output

extern DigitalPin<PORTB_t, DDRB_t, PORTB5> led;
using sensor_pin_t = DigitalPinPulledUp<PINB_t, DDRB_t, PORTB1>;

// Program

char cmd_buffer[256] = {0};
int cmd_head = 0;

static bool process_command(const char* command);

extern "C" int analogRead(uint8_t);

template <typename pin>
class Dallas1Wire {
  static constexpr int recovery = 200;

  static constexpr char writeStartTime = 1;
  static constexpr char writeSlotDuration = 60;

  static constexpr char readStartTime = 1;
  static constexpr char readSlotDuration = 15;
  static constexpr char readSlotOffset = 10;

public:
  static int reset() {
    pin::pull_low();
    delayMicroseconds(480);
    pin::float_high();

    int presence = 0;

    constexpr int sampleInterval = 15;

    int delays = 480 / sampleInterval;
    while (delays) {
      delayMicroseconds(sampleInterval);
      if (!pin::get()) {
	presence++;
      }
      delays--;
    }

    return presence;
  }

  static void writeBit(bool value) {
    delayMicroseconds(recovery); // FUDGE

    pin::pull_low();
    if (!value) {
      delayMicroseconds(writeSlotDuration);
      pin::float_high();
    }
    else {
      delayMicroseconds(writeStartTime);
      pin::float_high();
      delayMicroseconds(writeSlotDuration - writeStartTime);
    }
  }

  static bool readBit() {
    delayMicroseconds(recovery); // FUDGE

    pin::pull_low();
    delayMicroseconds(readStartTime);
    pin::float_high();

    delayMicroseconds(readSlotOffset - readStartTime);
    bool x = pin::get();
    delayMicroseconds((readSlotDuration - readSlotOffset)
		      - readStartTime);

    return x;
  }

  template <typename T>
  static void write(T value) {
    constexpr int total_bits = sizeof(value) * 8;
    int bits = total_bits;
    while (bits) {
      writeBit(value & 1);
      value >>= 1;
      bits--;
    }
  }

  template <typename T>
  static T read()  {
    T value = 0;
    constexpr int total_bits = sizeof(value) * 8;
    for (int bit = 0; bit < total_bits; bit++) {
      if (readBit()) {
	value = value | (1 << bit);
      }
    }
    return value;
  }

  template <typename T, int N>
  static void read(T* array)  {
    for (int i = 0; i < N; i++) {
      array[i] = read<T>();
    }
  }
};

extern Dallas1Wire<sensor_pin_t> temperature_sensor;

int main() {
  Serial.begin(115200);
  Serial.println("Hello");

  led.setDDR();

  sei();

  Serial.write("> ");
  while (true) {
    if (Serial.available()) {
      int byte = Serial.read();
      Serial.write(byte);
      cmd_buffer[cmd_head] = byte;
      if (byte == '\r' || byte == '\n') {
	Serial.println();
	cmd_buffer[cmd_head] = 0;
	cmd_head = 0;
	bool result = process_command(cmd_buffer);
	Serial.println(result ? "ok" : "fail");
	Serial.print("> ");
      }
      else {
	cmd_head++;
      }
    }
    serialEventRun();
  }
}

static bool process_command(const char* command) {
  if (strcmp(command,"led on") == 0) {
    Serial.println("enabling led");
    led.set();
  }
  else if (strcmp(command, "led off") == 0) {
    Serial.println("disabling led");
    led.clear();
  }
  else if (strcmp(command, "") == 0 || strcmp(command, "ping") == 0) {
    // ping
    Serial.println("pong");
  }
  else if (strcmp(command, "rs") == 0) {
    int ps = temperature_sensor.reset();
    Serial.print("get presence:");
    Serial.println(ps);
  }
  else if (memcmp(command, "w8", 2) == 0) {
    uint8_t cmd = atoi(command+3);
    Serial.print("writing ");
    Serial.println(cmd, BIN);
    temperature_sensor.write(cmd);
  }
  else if (strcmp(command, "r8") == 0) {
    Serial.println(temperature_sensor.read<uint8_t>(), BIN);
  }
  else if (strcmp(command, "ra") == 0) {
    while (1) {
      uint8_t val = temperature_sensor.read<uint8_t>();
      Serial.print(val, DEC);
      Serial.print("   0x");
      Serial.print(val, HEX);
      Serial.print("   ");
      Serial.println(val, BIN);
      if (val == 0xff) break;
    }
  }
  else if (strcmp(command, "grc") == 0) {
    int ps = temperature_sensor.reset();
    Serial.print("get presence:");
    Serial.println(ps);

    uint8_t reply[8];
    temperature_sensor.write<uint8_t>(0x33);
    temperature_sensor.read<uint8_t, 8>(&reply[0]);
    Serial.print("family (28h)=");
    Serial.println(reply[0], HEX);

    Serial.print("serial (msb to lsb)=");
    for (int i = 6; i >= 0; i--) {
      Serial.print(reply[i], HEX);
      Serial.print(' ');
    }
    Serial.println();

    Serial.print("crc=");
    Serial.println(reply[7], HEX);
  }
  else if (strcmp(command, "get_temp") == 0) {
    temperature_sensor.reset();
    temperature_sensor.write<uint8_t>(0xCC);
    temperature_sensor.write<uint8_t>(0x44);
    while (temperature_sensor.readBit() == 0)
      ;

    temperature_sensor.reset();
    temperature_sensor.write<uint8_t>(0xCC);
    temperature_sensor.write<uint8_t>(0xBE);

    uint16_t reply = temperature_sensor.read<uint16_t>();
    temperature_sensor.reset(); // discard rest of buffer

    Serial.print("raw=");
    Serial.println(reply, HEX);
  }
  else {
    // no command handler matched
    return false;
  }

  return true;
}
