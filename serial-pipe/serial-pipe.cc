#include <SoftwareSerial.h>
#include <HardwareSerial.h>

SoftwareSerial softSerial(10, 11);

extern "C" {
  void setup();
  void loop();
}

void setup() {
  Serial.begin(57600);
  softSerial.begin(19200);

  Serial.println("Serial-pipe online");
}

template <typename T, typename U>
void serial_pipe(T& from, U& to) __attribute__((always_inline));

template <typename T, typename U>
void serial_pipe(T& from, U& to) {
  if (from.available())
    to.write(from.read());
}

void loop() {
  serial_pipe(Serial, softSerial);
  serial_pipe(softSerial, Serial);
}
