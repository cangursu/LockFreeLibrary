
// clang-format off


#ifndef __WORK_QUEUE_H__
#define __WORK_QUEUE_H__

#include "ObjectQueue.hpp"
//#include "TimeFrame.h"
#include "Thread.hpp"

#include <thread>
#include <sstream>
#include <condition_variable>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <deque>
#include <list>
#include <vector>
#include <iostream>


namespace LockFreeLib
{


/**
 * @brief
 *
 */

enum class WQ_QUEUE_STATE
{
    NA              = 0,
    WORKING         = 1,
    PAUSE           = 2,
    EXITING_WAIT    = 3,
    EXITING_FORCE   = 4,
};

std::string WQ_QUEUE_STATE_text(WQ_QUEUE_STATE value);


/**
 * @brief
 * WorkQueue, A Queue implementation which is attached with a Worker thread.
 *
 * @tparam TObject The Data structure to queued
 * @tparam TThread The Thread Implamantation
 */

template <typename TObject, std::size_t KObjectCount, typename TThread>
class WorkQueue
    : public TThread
    , public ObjectQueue<TObject, KObjectCount>

{
 public:
    virtual ~WorkQueue();
/*
    enum class QUEUE_STATE
    {
        NA      = 0,
        WORKING = 1,
        EXITING = 2,
        PAUSE   = 3,
    };
*/

    int                         Init(WQ_QUEUE_STATE state, const std::string &name = "");

    void                        SetState(WQ_QUEUE_STATE stat);
    WQ_QUEUE_STATE              GetState() const;
    void                        SetWaitTime(const timespec &tmsp);
    const timespec &            GetWaitTime();

    std::size_t                 Size() const ;

    //Form Thread
    virtual void                Run() override;


    virtual void                Begin(__attribute__((unused)) TObject *); //nullptr is passed. Parameter is just for Overloading
    virtual int                 Pop(TObject *data);
    virtual void                End(__attribute__((unused)) TObject *);   //nullptr is passed. Parameter is just for Overloading
    virtual bool                PushBack (TObject &&data);
    virtual bool                PushBack (const TObject &data);
//    virtual std::size_t          PushFront(TObject &&data);
//    virtual std::size_t          PushFront(const TObject &data);
    virtual bool                PushFresh(TObject &&data);
    virtual bool                PushFresh(const TObject &data);

    virtual void*               Listener();
    void                        Release(bool bForce = false);

    const std::string&          Name() const;

 private:

    std::string                 _name;
    mutable std::shared_mutex   _thStatLock;
    WQ_QUEUE_STATE              _thState = WQ_QUEUE_STATE::EXITING_WAIT;
    timespec                    _thWaitTime {1,0};

    std::atomic_size_t          _containerSize = 0;
};


template <typename TObject, std::size_t KObjectCount, typename TThread>
WorkQueue<TObject, KObjectCount, TThread>::~WorkQueue()
{
    Release();
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
int WorkQueue<TObject, KObjectCount, TThread>::Pop(TObject*)
{
    return 0;
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
void WorkQueue<TObject, KObjectCount, TThread>::Begin(TObject *)
{
//    std::cout << "Listen thread will be begun\n";
//    std::cout << "Please overload the virtual member function below\n";
//    std::cout << "WorkQueue::Begin\n";
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
void WorkQueue<TObject, KObjectCount, TThread>::End(TObject *)
{
//    std::cout << "Listen thread will be stop\n";
//    std::cout << "Please overload the virtual member function below\n";
//    std::cout << "WorkQueue::End\n";
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
int WorkQueue<TObject, KObjectCount, TThread>::Init(WQ_QUEUE_STATE state, const std::string &name /*= ""*/)
{
    _name = name;
    SetState(state);
    TThread::Start();
    return 0;
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
void WorkQueue<TObject, KObjectCount, TThread>::Release(bool bForce /*= false*/)
{
    SetState(bForce ? WQ_QUEUE_STATE::EXITING_FORCE : WQ_QUEUE_STATE::EXITING_WAIT);
    TThread::Join();
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
/*typename WorkQueue<TObject, KObjectCount, TThread>::QUEUE_STATE*/
WQ_QUEUE_STATE WorkQueue<TObject, KObjectCount, TThread>::GetState() const
{
    std::shared_lock lck(_thStatLock);
    return _thState;
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
void WorkQueue<TObject, KObjectCount, TThread>::SetState(WQ_QUEUE_STATE stat)
{
    std::scoped_lock lck(_thStatLock);
    _thState = stat;
//    _thCond.notify_one();
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
const timespec  &WorkQueue<TObject, KObjectCount, TThread>::GetWaitTime()
{
    std::shared_lock lck(_thStatLock);
    return _thWaitTime;
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
void WorkQueue<TObject, KObjectCount, TThread>::SetWaitTime(const timespec &tmsp)
{
    std::scoped_lock lck(_thStatLock);
    _thWaitTime = tmsp;
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
std::size_t WorkQueue<TObject, KObjectCount, TThread>::Size() const
{
    return _containerSize;
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
const std::string& WorkQueue<TObject, KObjectCount, TThread>::Name() const
{
    return _name;
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
bool WorkQueue<TObject, KObjectCount, TThread>::PushBack(const TObject &data)
{
    bool b = false;
    switch (GetState())
    {
        case WQ_QUEUE_STATE::WORKING :
        {
            if (true == (b = ObjectQueue<TObject, KObjectCount>::PushBack(data)))
                ++_containerSize;
            break;
        }

        default :
            break;
    }

    return b;
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
bool WorkQueue<TObject, KObjectCount, TThread>::PushBack(TObject &&data)
{
    return PushBack(data);
}


/*
template <typename TObject, std::size_t KObjectCount, typename TThread>
std::size_t WorkQueue<TObject, KObjectCount, TThread>::PushFront(const TObject &data)
{
    switch (GetState())
    {
        case WQ_QUEUE_STATE::WORKING :
        {
            ObjectQueue<TObject, KObjectCount>::PushBack(data);
            ++_containerSize;
        }

        default :
            break;
    }

    return _containerSize;
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
std::size_t WorkQueue<TObject, KObjectCount, TThread>::PushFront(TObject &&data)
{
    return PushFront(data);
}
*/


template <typename TObject, std::size_t KObjectCount, typename TThread>
bool WorkQueue<TObject, KObjectCount, TThread>::PushFresh(const TObject &data)
{
    ObjectQueue<TObject, KObjectCount>::Reset();
    _containerSize = 0;
    return PushBack(data);
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
bool WorkQueue<TObject, KObjectCount, TThread>::PushFresh(TObject &&data)
{
    return PushFresh(data);
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
void WorkQueue<TObject, KObjectCount, TThread>::Run()
{
//    std::cout << "WorkQueue thread : " << _name << " : Entering\n";
    Begin(nullptr);
    Listener();
    End(nullptr);
//    std::cout << "WorkQueue thread : " << _name << " : Quiting \n";
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
void* WorkQueue<TObject, KObjectCount, TThread>::Listener()
{
    bool doExit = false;
    for(/*int count = 0*/; true != doExit; /*count++*/)
    {
        switch(GetState())
        {
            case WQ_QUEUE_STATE::WORKING:
            case WQ_QUEUE_STATE::EXITING_WAIT:
            {
                std::list<TObject> listBuff;

                {
                    TObject obj;
                    bool    b = false;
                    for (int iTry = 0; iTry < 1000 && (false == b); ++iTry)
                    {
                        while (true == ObjectQueue<TObject, KObjectCount>::Pop(obj))
                        {
                            b = true;
                            listBuff.push_back(obj);
                            --_containerSize;
                        }
                    }


                    switch (GetState())
                    {
                        case WQ_QUEUE_STATE::EXITING_FORCE :
                            doExit = true;
                            break;

                        case WQ_QUEUE_STATE::EXITING_WAIT :
                            if ((0 == _containerSize) /*|| (false == b)*/)
                            {
                                doExit = true;
                                break;
                            }

                        default:
                            ;
                    }


                    if (false == b)
                    {
                        TThread::NSleep(1); //TODO: Find a more precise way.
                        //TThread::Yield();
                        continue;
                    }
                }

                for(auto &item : listBuff)
                {
                    if (GetState() != WQ_QUEUE_STATE::EXITING_FORCE)
                        Pop(&item);
                }

                break;
            }

            case WQ_QUEUE_STATE::PAUSE:
                clock_nanosleep(CLOCK_MONOTONIC, 0, &GetWaitTime(), NULL);
                break;

            case WQ_QUEUE_STATE::EXITING_FORCE:
                doExit = true;
                break;

//            case WQ_QUEUE_STATE::EXITING_WAIT:
//                if (0 == _containerSize)
//                    doExit = true;
//                break;

            default :
                break;
        }
    }
    SetState(WQ_QUEUE_STATE::NA);

    return NULL;
}







/**
 * @brief
 * WorkQueuePool : A pool of WorkQueue
 *
 * @tparam TObject
 * @tparam TThread
 */

template <typename TObject, std::size_t KObjectCount, typename TThread>
class WorkQueuePool
{
    class WorkQueuePoolItem : public WorkQueue<TObject, KObjectCount, TThread>
    {
        public:
            void SetPool(WorkQueuePool *pool)
            {
                _pPool = pool;
            }
            virtual void Begin() override
            {
                if (nullptr == _pPool)
                {
                    std::cerr << "ERROR: invalid _pPool" << std::endl;
                    return;
                }

                return _pPool->Begin();
            }
            virtual int Pop(TObject *data) override
            {
                if (nullptr == _pPool)
                {
                    std::cerr << "ERROR: invalid _pPool" << std::endl;
                    return -1;
                }

                return _pPool->Pop(data);
            }
            virtual void End() override
            {
                if (nullptr == _pPool)
                {
                    std::cerr << "ERROR: invalid _pPool" << std::endl;
                    return;
                }

                return _pPool->End();
            }
        private:
            WorkQueuePool *_pPool = nullptr;
    };

    public :

        using WorkQueuePoolList =  std::vector<WorkQueuePoolItem>;

        WorkQueuePool(std::size_t queCount)
            : _queCount(queCount)
            , _pool(queCount)
        {
        }

        int             Init(/*WorkQueue<TObject, KObjectCount, TThread>::QUEUE_STATE*/WQ_QUEUE_STATE state, const std::string &name = "");
        void            Release();

        virtual int     Pop(TObject *data);
        virtual void    Begin();
        virtual void    End();
        virtual int     PushBack (TObject &&data);
        virtual int     PushFront(TObject &&data);

        std::size_t          QueCount() const;
        std::size_t          Size(std::vector<int> &sizeList);

    private :
        int             MaxIdx();
        int             MinIdx();

        std::string         _name;
        std::size_t         _queCount = 16;
        WorkQueuePoolList   _pool;
};


template <typename TObject, std::size_t KObjectCount, typename TThread>
int WorkQueuePool<TObject, KObjectCount, TThread>::Init(/*WorkQueue<TObject, KObjectCount, TThread>::QUEUE_STATE*/WQ_QUEUE_STATE state, const std::string &name /*= ""*/)
{
    _name       = name;

    for (std::size_t idx = 0; idx < _queCount; ++idx)
    {
        _pool[idx].SetPool(this);
        _pool[idx].Init(state, name + ":" + std::to_string(idx));
    }
    return 0;
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
void WorkQueuePool<TObject, KObjectCount, TThread>::Release()
{
    for (std::size_t idx = 0; idx < _queCount; ++idx)
        _pool[idx].Release();
}

template <typename TObject, std::size_t KObjectCount, typename TThread>
int WorkQueuePool<TObject, KObjectCount, TThread>::MaxIdx()
{
    std::size_t  sizeMax = 0;
    std::size_t  idxMax  = (std::size_t)-1;

    for (std::size_t idx = 0; idx < _queCount; ++idx)
    {
        std::size_t size = _pool[idx].Size();
        if (size >= sizeMax)
        {
            sizeMax = size;
            idxMax  = idx;
        }
    }
    return idxMax;
}

template <typename TObject, std::size_t KObjectCount, typename TThread>
int WorkQueuePool<TObject, KObjectCount, TThread>::MinIdx()
{
    std::size_t  sizeMin = (std::size_t)-1;
    std::size_t  idxMin  = (std::size_t)-1;

    for (std::size_t idx = 0; idx < _queCount; ++idx)
    {
        std::size_t size = _pool[idx].Size();
        if (size < sizeMin)
        {
            sizeMin = size;
            idxMin  = idx;
        }
    }

    return idxMin;
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
std::size_t WorkQueuePool<TObject, KObjectCount, TThread>::QueCount() const
{
    return  _queCount;
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
std::size_t WorkQueuePool<TObject, KObjectCount, TThread>::Size(std::vector<int> &sizeList)
{
    std::size_t sum = 0;
    for (std::size_t idx = 0; idx < _queCount; ++idx)
    {
        std::size_t sz = _pool[idx].Size();
        sum += sz;
        sizeList.push_back(sz);
    }
    return sum;
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
int WorkQueuePool<TObject, KObjectCount, TThread>::PushBack (TObject &&data)
{
    int idx = MinIdx();
    if (idx > -1)
        _pool[idx].PushBack(std::move(data));

    return idx;
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
int WorkQueuePool<TObject, KObjectCount, TThread>::PushFront(TObject &&data)
{
    int idx = MinIdx();
    if (idx > -1)
        _pool[idx].PushFront(std::move(data));

    return idx;
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
int WorkQueuePool<TObject, KObjectCount, TThread>::Pop(TObject * /*data*/)
{
    std::cout << "ERROR\n";
    std::cout << "Please overload the virtual member function below\n";
    std::cout << "WorkQueuePool::Pop\n";
    return 0;
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
void WorkQueuePool<TObject, KObjectCount, TThread>::Begin()
{
//    std::cout << "Listen thread will be begun\n";
//    std::cout << "Please overload the virtual member function below\n";
//    std::cout << "WorkQueuePool::Begin\n";
}


template <typename TObject, std::size_t KObjectCount, typename TThread>
void WorkQueuePool<TObject, KObjectCount, TThread>::End()
{
//    std::cout << "Listen thread will be stop\n";
//    std::cout << "Please overload the virtual member function below\n";
//    std::cout << "WorkQueuePool::End\n";
}


} // namespace LockFreeLib


#endif // __WORK_QUEUE_H__

// clang-format on

