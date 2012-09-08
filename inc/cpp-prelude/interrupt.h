#ifndef CPP_PRELUDE_INTERRUPT_H
#define CPP_PRELUDE_INTERRUPT_H

#include <avr/interrupt.h>

class AutoDisableInterrupts {
	uint8_t oldSREG;
public:
	AutoDisableInterrupts() {
		oldSREG = SREG;
		cli();
	}
	~AutoDisableInterrupts() {
		SREG = oldSREG;
	}
};

#endif
