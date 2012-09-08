#include <scheduler.h>

#include <Arduino.h>

#include <stdint.h>

// Sample program follows

Scheduler sched;
Scheduler *Scheduler::gScheduler(0);

class BlinkyThread : public Thread<40> {
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

#ifdef SCHEDULER_REPORT_STACK_USAGE
class ThreadReport : public Thread<128> {
public:
	virtual void run() {
		while (true) {
			sched.reportStackUsage();
			sched.sleep(5000);
		}
	}
};
#endif

BlinkyThread pin3(3, 1000, false);
BlinkyThread pin4(4, 500, true);
#ifdef SCHEDULER_REPORT_STACK_USAGE
ThreadReport report;
#endif

static ThreadBase* all_threads[] = { &pin3, &pin4,
#ifdef SCHEDULER_REPORT_STACK_USAGE
                                     &report
#endif
};

void setup() {
#ifdef SCHEDULER_REPORT_STACK_USAGE
	Serial.begin(57600);
#endif

	pin3.init();
	pin4.init();
	sched.init(all_threads, sizeof(all_threads) / sizeof(*all_threads));
}

void loop() {
	sched.run();
}

