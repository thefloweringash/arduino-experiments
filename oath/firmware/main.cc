#include <string.h>
#include <stdint.h>

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>


extern "C" {
#include <usbdrv.h>
}

#include "sha1.h"

#include "requests.h"

#define BUTTON_INPUT PINB
#define BUTTON_DDR DDRB
#define BUTTON_OUTPUT PORTB
#define BUTTON_BIT 4

uint8_t secret[64] EEMEM;

// Default keyboard copied from usb.org's HID Descriptor tool, minus
// LEDs
const char usbDescriptorHidReport[45] PROGMEM = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0x95, 0x06,                    //   REPORT_COUNT (6)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0xc0                           // END_COLLECTION
};

struct KeyboardReport {
	uint8_t modifiers;
	uint8_t reserved;
	uint8_t keys[6];
};

template <uint8_t size>
class StringSender {
	KeyboardReport mReport;

	uint8_t mString[size];
	uint8_t mStringLen;

	uint8_t mStringOffset;

public:
	void init() {
		mStringOffset = 0;
		mStringLen = 0;
		memset(&mReport, 0, sizeof(mReport));
	}

	void sendString(const uint8_t *string, uint8_t len) {
		mStringOffset = 0;
		uint8_t bytes = size < len ? size : len;
		mStringLen = bytes;
		memcpy(mString, string, bytes);
	}

	void sendInteger(uint32_t i, uint8_t intSize) {
		uint8_t *p = &mString[size];
		while (intSize-- && p > mString) {
			*--p = (i % 10) + '0';
			i /= 10;
		}
		mStringOffset = p - mString;
		mStringLen = size;
	}

	unsigned char* stepReport() {
		if (mStringOffset == mStringLen)
			return false;

		if (mReport.keys[0]) {
			mReport.keys[0] = 0;
			mStringOffset++;
		}
		else {
			// TODO: this only supports numbers
			uint8_t ch = mString[mStringOffset];
			uint8_t usage;
			if (ch == 0x30) {
				usage = 0x27;
			}
			else {
				usage = ch - (0x31 - 0x1E);
			}

			mReport.keys[0] = usage;
		}
		return reinterpret_cast<unsigned char*>(&mReport);
	}

	void haveInterrupt() {
		uint8_t *r = stepReport();
		if (r) {
			usbSetInterrupt(r, sizeof(mReport));
		}
	}
};

// Global state
// ----------
StringSender<11> gSender;
uint8_t gCodeLen EEMEM;

uint16_t gReadTotalLen;
uint16_t gReadOffset;

uint8_t gButtonState;
bool gButtonDown;

// gUnixTime is written to in TIMER0_OVF, and from the main
// loop. Since this can (only) result in concurrent access when timer0
// overflows, make sure that all accesses from the main loop are done
// with interrupts disabled.
volatile uint32_t gUnixTime;

static uint32_t generateCode(uint32_t counter) {
	sha1_state state;
	{
		constexpr size_t secretLen = sizeof(secret);

		sha1_state inner;
		sha1_block b;

		// do inner hash
		{
			b.init_with_eemem_bytes(secret, secretLen);
			b.apply_xor(0x36);
			sha1_update(inner, b);

			b.reset();
			// TODO: year 2038 (*30) problem right here
			uint32_t *words = reinterpret_cast<uint32_t*>(b.data);
			words[1] = counter;
			b.apply_padding(64, 8);
			sha1_update(inner, b);
		}

		// do outer hash
		{
			b.reset();
			b.init_with_eemem_bytes(secret, secretLen);
			b.apply_xor(0x5c);
			sha1_update(state, b);

			b.reset();
			b.init_with_sha1state(inner);
			b.apply_padding(64, 20);
			sha1_update(state, b);
		}
	}

	return dynamic_truncate(state);
}

usbMsgLen_t usbFunctionSetup(uchar data[8]) {
	usbRequest_t *rq = reinterpret_cast<usbRequest_t *>(data);

	static uint8_t idleRate = 0;

	if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){
		switch (rq->bRequest) {
		case USBRQ_HID_GET_REPORT:
			// wValue: ReportType (highbyte), ReportID (lowbyte)
			// we only have one report type, so don't look at wValue
			usbMsgPtr = gSender.stepReport();
			return sizeof(KeyboardReport);

			// we don't do idle rate, but still persist a value to
			// pretend we do
		case USBRQ_HID_GET_IDLE:
			usbMsgPtr = &idleRate;
			return 1;
		case USBRQ_HID_SET_IDLE:
			idleRate = rq->wValue.bytes[1];
			return 0;
		}
	}
	else {
		switch (rq->bRequest) {
		case OATH_RQ_ECHO:
			static uint8_t dataBuffer[4];
			dataBuffer[0] = rq->wValue.bytes[0];
			dataBuffer[1] = rq->wValue.bytes[1];
			dataBuffer[2] = rq->wIndex.bytes[0];
			dataBuffer[3] = rq->wIndex.bytes[1];
			usbMsgPtr = dataBuffer;
			return 4;

		case OATH_RQ_TYPE_CODE: {
			uint32_t counter = (((uint32_t) rq->wValue.word) << 16) | rq->wIndex.word;
			uint32_t code = generateCode(counter);
			gSender.sendInteger(code, eeprom_read_byte(&gCodeLen));
			return 0;
		}

		case OATH_RQ_SET_LENGTH: {
			uint8_t len = rq->wValue.word;
			if (eeprom_read_byte(&gCodeLen) != len) {
				eeprom_write_byte(&gCodeLen, len);
			}
			return 0;
		}

		case OATH_RQ_SET_KEY: {
			// expect a read of specified length
			gReadTotalLen = rq->wLength.word;
			gReadOffset = 0;
			return 0xff;
		}

		case OATH_RQ_SET_TIME: {
			uint8_t oldSREG = SREG;
			cli();
			gUnixTime = (((uint32_t) rq->wValue.word) << 16) | rq->wIndex.word;
			SREG = oldSREG;
			return 0;
		}
		}
	}
}

uchar usbFunctionWrite(uchar *data, uchar len) {
	if (gReadOffset + len > sizeof(secret))
		return -1; // bounds check failed, results in STALL

	// copy in data
	eeprom_write_block(data, &secret[gReadOffset], len);
	gReadOffset += len;
	if (gReadOffset == gReadTotalLen) {
		// request finished, zero pad the rest
		for (uint16_t off = gReadOffset; off < sizeof(secret); off++) {
			eeprom_write_byte(&secret[off], 0);
		}
		return 1;
	}
	else {
		return 0;
	}
}

ISR(TIMER0_OVF_vect, ISR_NOBLOCK) {
	static_assert(F_CPU == 16500000,
	              "Timer overflow constant matches F_CPU value");
	// 16.5mhz / 1024 / 256 = 62.9hz
	//overflow is ~63 times per second.
	static uint8_t counter = 0;
	if (++counter == 63) {
		counter = 0;
		gUnixTime++;
	}
}

int __attribute__((noreturn)) main() {
	wdt_enable(WDTO_1S);

	gSender.init();

	// pin is input by default, engage pullup
	BUTTON_OUTPUT |= _BV(BUTTON_BIT);

	// Prescaler = 1024, enable interrupt
	TCCR0B |= _BV(CS02) | _BV(CS00);
	TIMSK |= _BV(TOIE0);

	usbInit();

	usbDeviceDisconnect();

	// fake USB disconnect for > 250 ms
	for (unsigned char i = 0; --i;) {
		wdt_reset();
		_delay_ms(1);
	}

	usbDeviceConnect();
	sei();

	while (true) {
		wdt_reset();
		usbPoll();

		if (usbInterruptIsReady()) {
			gSender.haveInterrupt();
		}

		gButtonState = (gButtonState << 1) | !!(BUTTON_INPUT & _BV(BUTTON_BIT));
		if (gButtonState == 0xff && gButtonDown) {
			gButtonDown = false;
		}
		else if (gButtonState == 0x00 && !gButtonDown) {
			gButtonDown = true;

			uint8_t oldSREG = SREG;
			cli();
			uint32_t time = gUnixTime;
			SREG = oldSREG;

			gSender.sendInteger(generateCode(time / 30),
			                    eeprom_read_byte(&gCodeLen));
		}
	}
}
