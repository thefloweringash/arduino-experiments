#include <scheduler.h>

void Scheduler::init(ThreadBase **t, int count) {
    mThreadsCount = count;
    mThreads = t;
}

void Scheduler::run() {
    uint8_t currentThread = mCurrentThread;
    while (!mThreads[currentThread]->runnable()) {
	currentThread = (currentThread + 1) % mThreadsCount;

	if (currentThread == mCurrentThread)
	    return;
    }

    mCurrentThread = currentThread;

    ThreadBase& thread = *mThreads[currentThread];

    if (!thread.mSetUp) {
	setupThread(thread, currentThread);
    }

    if (setjmp(mSchedulerJmpbuf) == 0) {
	if (thread.mSleeping) {
	    thread.mSleeping = false;
	}

	longjmp(thread.mJmpBuf, currentThread);
    }
    else {
	mCurrentThread = (mCurrentThread + 1) % mThreadsCount;
    }
}

void Scheduler::setupThread(ThreadBase& t, uint8_t id) {
    setjmp(t.mJmpBuf);

    jmp_buf_contents& buf = *reinterpret_cast<jmp_buf_contents*>(t.mJmpBuf);
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

    t.mSetUp = true;
}

void Scheduler::yield() {
    ThreadBase& thread = *mThreads[mCurrentThread];
    if (setjmp(thread.mJmpBuf) == 0) {
	longjmp(mSchedulerJmpbuf, mCurrentThread);
    }
}

void Scheduler::sleep(int ms) {
    ThreadBase& t = *mThreads[mCurrentThread];
    t.mSleeping = true;
    t.mSleepEndMillis = millis() + ms;
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
