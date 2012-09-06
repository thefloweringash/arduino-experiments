#include <scheduler.h>

void Scheduler::init(ThreadBase **t, int count) {
    mThreadsCount = count;
    mThreads = t;
}

bool Scheduler::isRunnable(ThreadBase& t) {
    switch (t.state) {
    case ThreadBase::NEW:
	return true;
    case ThreadBase::SLEEPING:
	return millis() >= t.stateData.sleepEndMillis;
    case ThreadBase::BLOCKED:
	return false;
    }
}

void Scheduler::run() {
    uint8_t currentThread = mCurrentThread;
    while (!isRunnable(*mThreads[currentThread])) {
	currentThread = (currentThread + 1) % mThreadsCount;

	if (currentThread == mCurrentThread)
	    return;
    }

    mCurrentThread = currentThread;

    ThreadBase& thread = *mThreads[currentThread];

    if (thread.state == ThreadBase::NEW) {
	setupThread(thread, currentThread);
	thread.state = ThreadBase::READY;
    }
    else if (thread.state == ThreadBase::SLEEPING) {
	thread.state = ThreadBase::READY;
    }

    if (setjmp(mSchedulerJmpbuf) == 0) {
	longjmp(thread.jmpBuf, currentThread);
    }
    else {
	mCurrentThread = (mCurrentThread + 1) % mThreadsCount;
    }
}

void Scheduler::setupThread(ThreadBase& t, uint8_t id) {
    setjmp(t.jmpBuf);

    jmp_buf_contents& buf = *reinterpret_cast<jmp_buf_contents*>(t.jmpBuf);
    buf.frame_pointer = 0;
    buf.return_address = reinterpret_cast<uint16_t>(&threadRunner);

    uint8_t *stackTop = t.getStack();

    // I can't find any description of the calling convention. I'll
    // look harder later, but for now, we look and see what
    // happens. Then mimic.

    // This is how we find where to put it.:

    // const uint8_t pattern[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };
    // stackTop -= sizeof(pattern);
    // memcpy(stackTop, pattern, sizeof(pattern));

    constexpr int padding = 5;
    stackTop -= padding + sizeof(int);
    *reinterpret_cast<int*>(stackTop + padding) = id;
    buf.stack_pointer = reinterpret_cast<uint16_t>(stackTop);
}

void Scheduler::yield() {
    ThreadBase& thread = *mThreads[mCurrentThread];
    if (setjmp(thread.jmpBuf) == 0) {
	longjmp(mSchedulerJmpbuf, mCurrentThread);
    }
}

void Scheduler::sleep(int ms) {
    ThreadBase& t = *mThreads[mCurrentThread];
    t.state = ThreadBase::SLEEPING;
    t.stateData.sleepEndMillis = millis() + ms;
    yield();
}

void Scheduler::threadRunner(int dummy, ...) {
    uint8_t threadID;
    {
	va_list l;
	va_start(l, dummy);
	threadID = va_arg(l, int);
	va_end(l);
    }
    ThreadBase& thread = *gScheduler->mThreads[threadID];
    thread.run();

    // Thread has terminated. Not currently gracefully handled.
}
