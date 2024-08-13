
#include "WorkQueue.hpp"
#include "TimeFrame.h"



class MyQue : public WorkQueue<int, 4, Thread>
{
    public:
        int Pop(int *pData)
        {
            _tf.Stop();
            std::cout << "Pop - data:" << *pData <<  ", ElapsNs:" << _tf.ElapsNs() << std::endl;
            _list.push_back(*pData);
            if (0 < _sleepInt)
                usleep(_sleepInt);
            return 0;
        }

        uint32_t            _sleepInt = 0;
        std::vector<int>    _list;
        TimeFrame           _tf;
};


void MainWorkQueue_1()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;

    MyQue que;
    que.Init(WQ_QUEUE_STATE::WORKING, "MainPushBack");

    for (int i = 0; i < 1000; ++i)
    {
        que._tf.Start();
        que.PushBack(i * 3);
        usleep(500000);
    }

    //usleep(500);

    que.Release();
}


void MainWorkQueue_2()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;

    MyQue que;
    que.Init(WQ_QUEUE_STATE::WORKING, "MainPushFresh");
    que._sleepInt = 10;

    int i = 1;
    que.PushFresh(i);
    usleep(2);
    while(++i < 10)
        que.PushFresh(i);

    //usleep(500);

    que.Release();
}


void MainWorkQueue_3()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    srand(time(nullptr));

    MyQue que;
    que.Init(WQ_QUEUE_STATE::WORKING, "MainPushBackTH");

    constexpr int countTh   = 4;
    constexpr int countItem = 32;

    class PushTh : public Thread
    {
        public :

            void Que(MyQue *que)    { _que = que;   }
            void Idx(int idx)       { _idx = idx;   }

            virtual void Run() override
            {
                for (int j = 0; j < countItem; ++j)
                {
                    _que->_tf.Start();
                    _que->PushBack((_idx+1) * 10000 + j);
                    usleep(rand() % 10);
                }
            }

            MyQue *_que = nullptr;
            int    _idx = 0;
    };

    PushTh th[countTh];
    for (int idxTh = 0; idxTh < countTh; ++idxTh)
    {
        th[idxTh].Que(&que);
        th[idxTh].Idx(idxTh);
        th[idxTh].Start();
    }

    for (int i = 0; i < countTh; ++i)
        th[i].Join();

    que.Release();
}



