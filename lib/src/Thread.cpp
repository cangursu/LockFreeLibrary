
#include "Thread.hpp"




/**
 *
 *
 *
 */

void Thread::NSleep(uint32_t ns)
{
    clock_gettime(CLOCK_MONOTONIC, &_ts);

    _ts.tv_nsec += ns;
    if(_ts.tv_nsec >= SEC_TO_NS(1))
    {
        _ts.tv_nsec -= SEC_TO_NS(1);
        _ts.tv_sec++;
    }
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &_ts, NULL);
}



/**
 *
 *
 */

void TickThread::SetInterval(std::size_t val)
{
    std::unique_lock<std::mutex> lock(_mtx);
    _interval = std::chrono::milliseconds(val);
}


void TickThread::Stop()
{
    {
        std::unique_lock<std::mutex> lock(_mtx);
        _quit.store(true);
        _cv.notify_all();
    }
    Join();
}


void TickThread::Run()
{
    _tickCount = 0;
    _tickTs.Reset();

    std::unique_lock<std::mutex> lock(_mtx);
    while (!_cv.wait_for(lock, _interval, [this]{return _quit.load();}))
    {
        _tickTs.Step();
        ++_tickCount;

        Tick();
    }
}



