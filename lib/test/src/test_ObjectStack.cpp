

#include "ObjectStack.hpp"

#include <gtest/gtest.h>
#include <list>
#include <mutex>

//#define LOG std::cout
#define LOG if(false) std::cout

namespace lfl=LockFreeLib;


TEST(ObjectStack, Basic)
{
    lfl::ObjectStack<int, 4> stc;

    EXPECT_NE(-1, stc.Push(-1));

    int val;
    EXPECT_NE(-1, stc.Pop(val));
    EXPECT_EQ(-1, val);


    EXPECT_NE(-1, stc.Push(-1));
    EXPECT_NE(-1, stc.Push(-2));
    EXPECT_NE(-1, stc.Push(-3));

    EXPECT_NE(-1, stc.Pop(val));
    EXPECT_EQ(-3, val);

    EXPECT_NE(-1, stc.Push(-4));

    EXPECT_NE(-1, stc.Pop(val));
    EXPECT_EQ(-4, val);

    EXPECT_NE(-1, stc.Pop(val));
    EXPECT_EQ(-2, val);

    EXPECT_NE(-1, stc.Pop(val));
    EXPECT_EQ(-1, val);
}



using TestType = int64_t;

static constexpr int        g_lenStack = 32 * 8;
static constexpr int        g_lenPush  = 16 * 8;
static constexpr int        g_lenTh    =  8 * 8;

static std::list<TestType>  g_valsPoped;

struct Prm
{
    lfl::ObjectStack<TestType, g_lenStack> *_stack = nullptr;
    std::mutex *_lock  = nullptr;
    int         _idxTh = 0;
};


static void *Produce(void *p)
{
    Prm *prm = (Prm *)p;
    lfl::ObjectStack<TestType, g_lenStack>* stack = prm->_stack;

    for (int idxPush = 0; idxPush < g_lenPush; ++idxPush)
    {
        int idx = -1;
        while (0 < (idx = stack->Push((100000 * prm->_idxTh) + idxPush)))
            ;//LOG << "Produce : unable to push. Retrying\n";
        std::lock_guard lock(*(prm->_lock));
        {
            timespec ts {};
            clock_gettime(CLOCK_REALTIME, &ts);
            LOG << "Push - " << ((int64_t)(ts.tv_sec) * (int64_t)1000000000 + (int64_t)(ts.tv_nsec))
                << " : _idxTh:" << prm->_idxTh
                << ", idxPush:" << idxPush
                << ", val:" << ((100000 * prm->_idxTh) + idxPush)
                << ", idx:" << idx
                << std::endl;
        }
        usleep(rand() % 10);
    }

    return nullptr;
}

static void *Consume(void *p)
{
    Prm *prm = (Prm *)p;
    lfl::ObjectStack<TestType, g_lenStack>* stack = prm->_stack;

    for (int idxPop = 0; idxPop < g_lenPush; ++idxPop )
    {
        TestType val = -1;
        int idx = -1;

        while (0 < (idx = stack->Pop(val)))
            usleep(100);//LOG << "Consume : unable to Pop. Retrying\n";

        if (-1 < idx)
        {
            std::lock_guard lock(*(prm->_lock));
            timespec ts {};
            clock_gettime(CLOCK_REALTIME, &ts);
            LOG << "Pop  - " << ((int64_t)(ts.tv_sec) * (int64_t)1000000000 + (int64_t)(ts.tv_nsec)) << " : "
                << "_idxTh:" << prm->_idxTh
                << ", idxPop :" << idxPop
                << ", val:" << val
                << ", idx:" << idx
                << std::endl;

            g_valsPoped.push_back(val);
        }
        usleep(rand() % 100);
    }

    return nullptr;
}


TEST(ObjectStack, MT)
{
    lfl::ObjectStack<TestType, g_lenStack> stack;
    std::mutex                        lock;

    Prm       prm[g_lenTh];
    pthread_t thProd[g_lenTh] {};
    pthread_t thCons[g_lenTh] {};

    for (std::size_t idx = 0; idx < g_lenTh; ++idx)
    {
        prm[idx]._idxTh = idx + 1;
        prm[idx]._stack = &stack;
        prm[idx]._lock  = &lock;

        pthread_create(&thProd[idx], nullptr, *Produce, (void *)(&prm[idx]));
        pthread_create(&thCons[idx], nullptr, *Consume, (void *)(&prm[idx]));
    }

    for (std::size_t idx = 0; idx < g_lenTh; ++idx)
    {
        pthread_join(thProd[idx], nullptr);
        pthread_join(thCons[idx], nullptr);
    }

    int idx      = 0;
    int idxInner = 0;
    for (auto it = g_valsPoped.begin(), itEnd = g_valsPoped.end(); it != itEnd; ++it, ++idx)
    {
        idxInner = 0;
        for (auto itInner = g_valsPoped.begin(), itInnerEnd = g_valsPoped.end(); itInner != itInnerEnd; ++itInner, ++idxInner)
        {
            if ( (it != itInner) && (*it == *itInner))
            {
                //TODO: Test fails due to an issue of the test routine. Have to be corrected.
                LOG << "Duplicate value " << *it << ", " << idx  << "," << idxInner << std::endl;
                EXPECT_TRUE(false);
            }
        }
    }
}
