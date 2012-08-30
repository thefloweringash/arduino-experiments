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

// #include "cpp-prelude.h"

// borrow my previously written arduino library
#include <RemoteTransmitter.h>

class RepeatingTransmitter {
    uint8_t mTimes;
    uint32_t mCode;
    RemoteTransmitter mTx;

public:
    RepeatingTransmitter(uint8_t pin)
	: mTimes(0)
	, mTx(pin)
    {}
    void begin() {
	mTx.begin();
    }
    void step() {
	if (mTimes) {
	    mTx.sendOnce(mCode);
	    mTimes--;
	}
    }
    void send(uint32_t code) {
	mTimes = 4;
	mCode = code;
	mTx.sendOnce(mCode);
    }
};
RepeatingTransmitter rtx(0);


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
	rtx.send(remoteCode);
    }
    
    return 0;   /* default for not implemented requests: return no data back to host */
}

/* ------------------------------------------------------------------------- */

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
    DBG1(0x01, 0, 0);       /* debug output: main loop starts */
    rtx.begin();
    for(;;){                /* main event loop */
        DBG1(0x02, 0, 0);   /* debug output: main loop iterates */
        wdt_reset();
        usbPoll();
	rtx.step();
    }
}

/* ------------------------------------------------------------------------- */
