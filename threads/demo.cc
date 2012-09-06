#include <scheduler.h>

#include <Arduino.h>

#include <stdint.h>

// Sample program follows

Scheduler sched;
Scheduler *Scheduler::gScheduler(0);

class BlinkyThread : public Thread<32> {
    int mPin;
    int mSleepMS;
    bool mInitialState;
public:
    BlinkyThread(int pin, int sleepMS, bool initialState)
	: mPin(pin)
	, mSleepMS(sleepMS)
	, mInitialState(initialState)
    {}

    void init() {
	pinMode(mPin, OUTPUT);
    }

    virtual void run() {
	bool state = mInitialState;
	while (true) {
	    digitalWrite(mPin, state);
	    sched.sleep(mSleepMS);
	    state = !state;
	}
    }

};

BlinkyThread pin3(3, 1000, false);
BlinkyThread pin4(4, 500, true);

static ThreadBase* all_threads[] = { &pin3, &pin4 };

void setup() {
    pin3.init();
    pin4.init();
    sched.init(all_threads, 2);
}

void loop() {
    sched.run();
}

// pretend to be wiring
int main() {
    init(); // wiring init
    setup();
    while (true) {
	loop();
    }
}
