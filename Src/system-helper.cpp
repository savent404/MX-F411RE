#include "textHelper.h"
#include "mbed.h"
#include <queue>

static Semaphore event_sem;
static Mutex event_mutex;
static queue<uint32_t> event_queue;

bool defaultEventSender(uint32_t message)
{
    event_mutex.lock();
    event_queue.push(message);
    event_mutex.unlock();
    event_sem.release();
    return true;
}

bool defaultEventReciver(uint32_t& message, uint32_t timeout)
{
    if (event_sem.wait() != 0)
        return false;
    event_mutex.lock();
    message = event_queue.front();
    event_queue.pop();
    event_mutex.unlock();
    return true;
}

void mDebug(int level, const char* str, ...)
{
    static char buffer[512];
    va_list aptr;
    int cnt;

    va_start(aptr, str);
    cnt = sprintf(buffer, ":%d", level);
    vsprintf(buffer + cnt, str, aptr);
    va_end(aptr);
}
