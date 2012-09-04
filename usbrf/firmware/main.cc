/* Name: main.c
 * Project: custom-class, a basic USB example
 * Author: Christian Starkjohann
 * Creation Date: 2008-04-09
 * Tabsize: 4
 * Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
 * This Revision: $Id$
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */

#include <avr/pgmspace.h>   /* required by usbdrv.h */

extern "C" {
#include "usbdrv.h"
#include "oddebug.h"        /* This is also an example for using debug macros */
}

#include "requests.h"       /* The custom request numbers we use */

#include "cpp-prelude.h"

#include <RemoteTransmitter.h>
#include <RemoteReceiver.h>

template <typename i_reg_t,
	  unsigned char bit>
class DigitalInput {
public:
    static inline bool get() {
	return i_reg_t::template get_bit<bit>() != 0;
    }
};

RemoteTransmitter tx(0);
DigitalInput<PINB_t, 4> rxPin;

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
    usbRequest_t    *rq = reinterpret_cast<usbRequest_t *>(data);
    static uchar    dataBuffer[4];  /* buffer must stay valid when usbFunctionSetup returns */

    if(rq->bRequest == USBRF_RQ_ECHO){ /* echo -- used for reliability tests */
	dataBuffer[0] = rq->wValue.bytes[0];
	dataBuffer[1] = rq->wValue.bytes[1];
	dataBuffer[2] = rq->wIndex.bytes[0];
	dataBuffer[3] = rq->wIndex.bytes[1];
	usbMsgPtr = dataBuffer;         /* tell the driver which data to return */
	return 4;
    }
    else if (rq->bRequest == USBRF_RQ_SEND_CODE) {
	// In a more "proper" system, the remote code would be a
	// following data packet. However, since the setup request has
	// enough space for our entire 32-bit code, it's just extra
	// hassle.
	uint32_t remoteCode = (((uint32_t) rq->wValue.word) << 16) | rq->wIndex.word;
	tx.send(remoteCode);
    }
    
    return 0;   /* default for not implemented requests: return no data back to host */
}

/* ------------------------------------------------------------------------- */

template <typename T>
static void send_interrupt(const T val) {
    static T txdata;
    txdata = val;
    usbSetInterrupt((unsigned char*) &txdata, sizeof(txdata));
}

int __attribute__((noreturn)) main(void)
{
uchar   i;

    wdt_enable(WDTO_1S);
    /* Even if you don't use the watchdog, turn it off here. On newer devices,
     * the status of the watchdog (on/off, period) is PRESERVED OVER RESET!
     */

    /* RESET status: all port bits are inputs without pull-up.
     * That's the way we need D+ and D-. Therefore we don't need any
     * additional hardware initialization.
     */
    odDebugInit();
    DBG1(0x00, 0, 0);       /* debug output: main starts */
    usbInit();
    usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
    i = 0;
    while(--i){             /* fake USB disconnect for > 250 ms */
        wdt_reset();
        _delay_ms(1);
    }
    usbDeviceConnect();
    sei();
    tx.begin();

    RemoteReceiver::start();
    init(); // wiring init

    DBG1(0x01, 0, 0);       /* debug output: main loop starts */

    static unsigned long last_ping;
    last_ping = -1;

    for(;;){                /* main event loop */
        DBG1(0x02, 0, 0);   /* debug output: main loop iterates */
        wdt_reset();
        usbPoll();

	if (RemoteReceiver::dataReady()) {
	    if (usbInterruptIsReady()) {
		send_interrupt<uint32_t>(RemoteReceiver::getData());
	    }
	}
	else {
	    bool last = 0;
	    for (unsigned long block_end = millis() + 100; millis() < block_end;) {
		bool current  = rxPin.get();
		if (last != current) {
		    RemoteReceiver::interrupt();
		    if (RemoteReceiver::dataReady())
			break;
		    last = current;
		}
	    }
	}
    }
}

/* ------------------------------------------------------------------------- */
