
#include "WorkQueue.hpp"

#include <gtest/gtest.h>

namespace lfl=LockFreeLib;


template <std::size_t KObjectCount = 128>
class QueTest : public lfl::WorkQueue<int, KObjectCount, Thread>
{
    public :
        int Pop(int *pData)
        {
            _list.emplace_back(*pData);
            if (0 < _sleepInt)
                usleep(_sleepInt);
            return 0;
        }

        uint32_t _sleepInt = 20;
        std::vector<int> _list;
};


TEST(WorkQueue, PushBack)
{
    QueTest<> que;
    EXPECT_EQ(0, que.Init(lfl::WQ_QUEUE_STATE::WORKING, "PushBackTestLF"));

    constexpr int countItem = 10;
    for (int i = 0; i < countItem; ++i)
        que.PushBack(i+1);
    que.Release();

    EXPECT_EQ(countItem, que._list.size());
    for (int i = 0; i < countItem; ++i)
        EXPECT_EQ(i+1, que._list[i]);
}


TEST(WorkQueue, PushFresh)
{
    QueTest que;
    que._sleepInt = 50;
    EXPECT_EQ(0, que.Init(lfl::WQ_QUEUE_STATE::WORKING, "PushFreshTestLF"));

    int i = 1;
    que.PushFresh(i);
    usleep(15);
    while(++i < 10)
        que.PushFresh(i);
    que.Release();

    EXPECT_EQ(2, que._list.size());
    EXPECT_EQ(1, que._list[0]);
    EXPECT_EQ(9, que._list[1]);
}


TEST(WorkQueue, PushBackTh)
{
    QueTest<128> que;
    EXPECT_EQ(0, que.Init(lfl::WQ_QUEUE_STATE::WORKING, "PushBackTestLF"));

    constexpr int countTh   =  4;
    constexpr int countItem = 32;

    class PushTh : public Thread
    {
        public :

            void Que(QueTest<> *que)    { _que = que;   }
            void Idx(int idx)           { _idx = idx;   }

            virtual void Run() override
            {
                for (int j = 0; j < countItem; ++j)
                {
                    _que->PushBack((_idx+1) * 10000 + j);
                    usleep(rand() % 10);
                }
            }

            QueTest<>   *_que = nullptr;
            int         _idx = 0;
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

    EXPECT_EQ(countTh * countItem, que._list.size());
}


TEST(WorkQueue, PushBackTh2)
{
    QueTest<64> que;
    EXPECT_EQ(0, que.Init(lfl::WQ_QUEUE_STATE::WORKING, "PushBackTestLF"));

    constexpr int countTh   =  4;
    constexpr int countItem = 32;

    class PushTh : public Thread
    {
        public :

            void Que(QueTest<64> *que)  { _que = que;   }
            void Idx(int idx)           { _idx = idx;   }

            virtual void Run() override
            {
                for (int j = 0; j < countItem; ++j)
                {
                    if (true == _que->PushBack((_idx+1) * 10000 + j))
                        ++_pushed;
                    usleep(rand() % 10);
                }
            }

            QueTest<64>*_que = nullptr;
            int         _pushed = 0;
            int         _idx = 0;
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

    int pushed = 0;
    for (int idxTh = 0; idxTh < countTh; ++idxTh)
        pushed += th[idxTh]._pushed;

    EXPECT_EQ(pushed, que._list.size());
}
