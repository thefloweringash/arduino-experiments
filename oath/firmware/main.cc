#include <string.h>
#include <stdint.h>

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "cpp-prelude.h"
#include "cpp-prelude/digitalpin.h"
#include "cpp-prelude/eeprom.h"
#include "cpp-prelude/interrupt.h"
#include "cpp-prelude/debouncer.h"

extern "C" {
#include <usbdrv.h>
}

#include "sha1.h"

#include "requests.h"

using ButtonPin_t = DigitalIOPin<PINB_t, PORTB_t, DDRB_t, PORTB4>;
using LEDPin_t    = DigitalIOPin<PIND_t, PORTD_t, DDRD_t, PORTD7>;

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
			return NULL;

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
AutoEEMem<uint8_t> gCodeLen EEMEM;

uint16_t gReadTotalLen;
uint16_t gReadOffset;


struct {
	uint32_t downTime;
	Debouncer<uint8_t, ButtonPin_t> debouncer;
	typedef ButtonPin_t pin;
} gButton;

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
	static uint8_t dataBuffer[4];

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
			dataBuffer[0] = rq->wValue.bytes[0];
			dataBuffer[1] = rq->wValue.bytes[1];
			dataBuffer[2] = rq->wIndex.bytes[0];
			dataBuffer[3] = rq->wIndex.bytes[1];
			usbMsgPtr = dataBuffer;
			return 4;

		case OATH_RQ_TYPE_CODE: {
			uint32_t counter =
			    (((uint32_t) rq->wValue.word) << 16)
			    | rq->wIndex.word;
			uint32_t code = generateCode(counter);
			gSender.sendInteger(code, gCodeLen);
			return 0;
		}

		case OATH_RQ_SET_LENGTH: {
			gCodeLen = rq->wValue.word;
			return 0;
		}

		case OATH_RQ_SET_KEY: {
			// expect a read of specified length
			gReadTotalLen = rq->wLength.word;
			gReadOffset = 0;
			return 0xff;
		}

		case OATH_RQ_SET_TIME: {
			{
				AutoDisableInterrupts _;
				gUnixTime =
				    (((uint32_t) rq->wValue.word) << 16)
				    | rq->wIndex.word;
				TCNT2 = 0;
			}
			return 0;
		}
		case OATH_RQ_GET_TIME: {
			{
				AutoDisableInterrupts _;
				uint32_t time = gUnixTime;
				memcpy(dataBuffer, &time, sizeof(time));
			}
			usbMsgPtr = dataBuffer;
			return sizeof(gUnixTime);
		}
		}
	}

	return 0;
}

uchar usbFunctionWrite(uchar *data, uchar len) {
	if (gReadOffset + len > sizeof(secret))
		return -1; // bounds check failed, results in STALL

	// copy in data
	eeprom_update_block(data, &secret[gReadOffset], len);
	gReadOffset += len;
	if (gReadOffset == gReadTotalLen) {
		// request finished, zero pad the rest
		for (uint16_t off = gReadOffset; off < sizeof(secret); off++) {
			eeprom_update_byte(&secret[off], 0);
		}
		return 1;
	}
	else {
		return 0;
	}
}

ISR(TIMER2_OVF_vect, ISR_NOBLOCK) {
	// ledval = !ledval;
	LEDPin_t::invert();
	gUnixTime++;
}

int __attribute__((noreturn)) main() {
	// turn off features that are never used
	ACSR |= _BV(ACD); // disable analog comparator
	ADCSRA &= ~_BV(ADEN); // disable adc

	wdt_enable(WDTO_1S);

	gSender.init();

	LEDPin_t::setDDR();

	LEDPin_t::set();

	ASSR |= _BV(AS2); // set timer2 to async
	while ((ASSR & 0xf) != _BV(AS2)) {} // wait for async to start

	LEDPin_t::invert();

	// timer ticks per second (32768 / 128 = 256) will overflow an
	// 8-bit timer once per second.
	TCCR2 = _BV(CS22) | _BV(CS20); // prescaler = 128
	TIMSK |= _BV(TOIE2); // enable overflow

	// pin is input by default, engage pullup
	decltype(gButton)::pin::set();

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

		decltype(gButton.debouncer)::Edge edge = gButton.debouncer.step();
		if (edge == gButton.debouncer.Rising) {
			uint32_t time;
			{
				AutoDisableInterrupts _;
				time = gUnixTime;
			}

			gSender.sendInteger(generateCode(time / 30),
			                    gCodeLen);
			gButton.downTime = 0;
		}
		else if (edge == gButton.debouncer.Falling) {
			gButton.downTime = gUnixTime;
		}
		if (gButton.downTime) {
			if (gUnixTime - gButton.downTime > 3) {
				TIMSK &= ~_BV(TOIE2);
				constexpr void *bootladerAddress = (void*) (0x1c00 << 1);
				((void(*)()) bootladerAddress)();
			}
		}
	}
}
