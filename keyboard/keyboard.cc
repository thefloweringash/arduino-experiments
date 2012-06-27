#include <avr/io.h>
#include <avr/interrupt.h>

#include <HardwareSerial.h>

#include "cpp-prelude.h"
#include "cpp-prelude/digitalpin.h"
#include "cpp-prelude/digitalpinpulledup.h"
#include "cpp-prelude/ps2keyboard.h"

#include "timing.h"

// Simple digital output

extern "C" {
  void INT0_vect();
}

extern DigitalPin<PORTB_t, DDRB_t, PORTB5> led;

// Program

static const int reset_value = 15625;
bool ledval = true;

char cmd_buffer[256] = {0};
int cmd_head = 0;

typedef DigitalPinPulledUp<PIND_t, DDRD_t, PORTD3> kbd_data_pin;
typedef DigitalPinPulledUp<PIND_t, DDRD_t, PORTD2> kbd_clock_pin;

PS2Keyboard<kbd_data_pin, kbd_clock_pin> keyboard;

static bool process_command(const char* command);

uint8_t led_state;

int main() {
  led_state = 0;
  keyboard.begin();

  Serial.begin(115200);
  Serial.println("Hello");

  led.setDDR();

  EICRA = _BV(ISC01); // falling edge only
  EIMSK = _BV(INT0);

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

    if (keyboard.available()) {
      uint8_t byte = keyboard.read();
      Serial.print("keyboard data: 0x");
      Serial.println(byte, 16);
    }
  }
}

static bool process_command(const char* command) {
  int nl, sl, cl;
  if (strcmp(command,"led on") == 0) {
    Serial.println("enabling led");
    led.set();
  }
  else if (strcmp(command, "led off") == 0) {
    Serial.println("disabling led");
    led.clear();
  }
  else if ((sl = strcmp(command, "sl")) == 0 ||
	   (nl = strcmp(command, "nl")) == 0 ||
	   (cl = strcmp(command, "cl")) == 0) {
    led_state ^= 1 << (sl == 0 ? 0 :
		       nl == 0 ? 1 :
		       cl == 0 ? 2 :
		       0);

    keyboard.send(0xED);
    while (keyboard.sending());
    keyboard.send(led_state);
    while (keyboard.sending());
  }
  else if (strcmp(command, "reset") == 0) {
    Serial.println("resetting keyboard");
    keyboard.send(0xFF);
    while (keyboard.sending());
  }
  else if (strcmp(command, "") == 0 || strcmp(command, "ping") == 0) {
    // ping
    Serial.println("pong");
  }
  else {
    // no command handler matched
    return false;
  }

  return true;
}

ISR(INT0_vect) {
  keyboard.interrupt();
}
