#include "HardwareSerial.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "cpp-prelude.h"
#include "cpp-prelude/digitalpin.h"

#include "timing.h"

// Simple digital output

extern DigitalPin<PORTB_t, DDRB_t, PORTB5> led;

// Program

char cmd_buffer[256] = {0};
int cmd_head = 0;

static bool process_command(const char* command);

uint8_t led_state;

extern "C" int analogRead(uint8_t);

int main() {
  led_state = 0;

  Serial.begin(115200);
  Serial.println("Hello");

  led.setDDR();

  PRR.clear_bit<0>();
  // ADCSRA = ADCSRA | 0x0 | _BV(7);
  ADCSRA = _BV(7) | 0x3;
  ADMUX =
    // (_BV(7) | _BV(6)) | // select 1.1V ref
    // _BV(6) | // select Vcc ref
    // AREF
    0;
    

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
  else if (strcmp(command, "get_temp") == 0) {
    Serial.println("Fetching temperature");

    ADCSRA.set_bit<6>();
    while (ADCSRA.get_bit<6>())
      ;
    int low = ADCL;
    int high = ADCH;

    int reading = (high << 8) | low;
    Serial.write("raw=");
    Serial.println(reading);
    float voltage = reading * 3.3 / 1024.0;
    Serial.write("voltage=");
    Serial.println(voltage);

    float temperature = (reading * 3.3 / 1024.0 - 0.5) * 100;
    Serial.write("temperature=");
    Serial.println(temperature);
  }
  else {
    // no command handler matched
    return false;
  }

  return true;
}
