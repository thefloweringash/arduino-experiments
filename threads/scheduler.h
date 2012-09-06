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
    enum State {
	NEW,
	SLEEPING,
	BLOCKED,
	READY
    };

    State state;
    union {
	long sleepEndMillis;
    } stateData;

     jmp_buf jmpBuf;

    virtual uint8_t *getStackTop();
    virtual uint8_t *getStackBottom();

    friend class Scheduler;

public:
    ThreadBase()
	: state(NEW)
    {}

    virtual void run() = 0;
};

template <size_t stackSize>
    class Thread : public ThreadBase {
public:
    uint8_t mStack[stackSize];

    virtual uint8_t* getStackTop() {
	return &mStack[stackSize];
    }

    virtual uint8_t* getStackBottom() {
	return &mStack[0];
    }
};


class Scheduler {
    jmp_buf mSchedulerJmpbuf;
    uint8_t mCurrentThread;
    ThreadBase **mThreads;
    uint8_t mThreadsCount;

    static Scheduler *gScheduler;

    void setupThread(ThreadBase& t, uint8_t id);
    bool isRunnable(ThreadBase& t);
public:
    Scheduler() {
	gScheduler = this;
    }

    void init(ThreadBase **t, int count);
    void run();

    void yield();
    void sleep(int ms);

#ifdef SCHEDULER_REPORT_STACK_USAGE
    void reportStackUsage();
#endif

    static void threadRunner(int dummy, ...);
};
