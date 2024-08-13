
#ifndef __TRHEAD_HPP__
#define __TRHEAD_HPP__

#include "TimeFrame.h"

#include <condition_variable>
#include <mutex>
#include <thread>
#include <atomic>
#include <unistd.h>





/**
 * @brief
 * Thread base for Threading/Queueing interfaces.
 *
 */

class Thread
{
    public:
        void            Start()             {   _th  = std::thread( [this]() { Run(); } );                  }
        void            Join()              {   if (_th.joinable()) _th.join();                             }
        void            Yield()             {   std::this_thread::yield();                                  }
        void            NSleep(uint32_t ns);

        virtual void    Run()  = 0;
    private:
        std::thread     _th;
        timespec        _ts {};
};





/**
 * @brief
 * A timer-like frequently triggered Thread interface.
 *
 */

class TickThread : public Thread
{
    public:

        void            Stop();
        void            SetInterval(std::size_t val);

        virtual void    Tick() = 0;

        uint64_t        TickCount()         { return _tickCount; }
        TimeFrame       TickTimeFrame()     { return _tickTs;    };

    protected:
        virtual void    Run() override;

    private:
        std::chrono::milliseconds   _interval {1000};
        std::condition_variable     _cv;
        std::mutex                  _mtx;
        std::atomic_bool            _quit {false};

        std::atomic<uint64_t>       _tickCount  {0};
        TimeFrame                   _tickTs     {};
};


#endif // __TRHEAD_HPP__
