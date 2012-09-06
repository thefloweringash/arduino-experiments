#include <stdint.h>
#include <setjmp.h>
#include <stdarg.h>

#include <Arduino.h>

struct jmp_buf_contents {
    uint8_t     call_saved_registers[16];
    uint16_t    frame_pointer;
    uint16_t    stack_pointer;
    uint8_t     status_register;
#if defined(__AVR_3_BYTE_PC__) && __AVR_3_BYTE_PC__
    #error 3-byte PC not supported
#else
    uint16_t    return_address;
#endif
};

static_assert(sizeof(jmp_buf_contents) == sizeof(jmp_buf),
	      "jmp_buf_contents matches jmp_buf in size");

class ThreadBase {
    bool mSetUp;
    bool mSleeping;
    long mSleepEndMillis;
    jmp_buf mJmpBuf;

    inline bool runnable() {
	return !mSleeping || millis() > mSleepEndMillis;
    }

    virtual uint8_t *getStack();

    friend class Scheduler;

public:
    ThreadBase()
	: mSleeping(false)
	, mSetUp(false)
    {}

    virtual void run() = 0;
};

template <size_t stackSize>
    class Thread : public ThreadBase {
public:
    uint8_t mStack[stackSize];

    virtual uint8_t* getStack() {
	return &mStack[stackSize];
    }
};


class Scheduler {
    jmp_buf mSchedulerJmpbuf;
    uint8_t mCurrentThread;
    ThreadBase **mThreads;
    uint8_t mThreadsCount;

    static Scheduler *gScheduler;

    void setupThread(ThreadBase& t, uint8_t id);

public:
    Scheduler() {
	gScheduler = this;
    }

    void init(ThreadBase **t, int count);
    void run();

    void yield();
    void sleep(int ms);

    static void threadRunner(int dummy, ...);
};
