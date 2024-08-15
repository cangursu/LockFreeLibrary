
#include "ObjectQueue.hpp"

#include <gtest/gtest.h>


//#define LOG std::cout
#define LOG if(false) std::cout

namespace lfl=LockFreeLib;

TEST(ObjectQueue, Basic)
{
    lfl::ObjectQueue<int, 4> queue;
    int val = 0;
    EXPECT_TRUE(queue.PushBack(-1));

    EXPECT_TRUE(queue.Pop(val));
    EXPECT_EQ(val, -1);

    EXPECT_TRUE(queue.PushBack(-2));
    EXPECT_TRUE(queue.PushBack(-3));
    EXPECT_TRUE(queue.PushBack(-4));
    EXPECT_TRUE(queue.PushBack(-5));
    EXPECT_FALSE(queue.PushBack(-6));

    EXPECT_TRUE(queue.Pop(val));
    EXPECT_EQ(val, -2);
    EXPECT_TRUE(queue.Pop(val));
    EXPECT_EQ(val, -3);
    EXPECT_TRUE(queue.Pop(val));
    EXPECT_EQ(val, -4);
    EXPECT_TRUE(queue.Pop(val));
    EXPECT_EQ(val, -5);

    EXPECT_FALSE(queue.Pop(val));

    EXPECT_TRUE(queue.PushBack(-6));
    EXPECT_TRUE(queue.Pop(val));
    EXPECT_EQ(val, -6);

    EXPECT_TRUE(queue.PushBack(-7));
    EXPECT_TRUE(queue.PushBack(-8));

    EXPECT_TRUE(queue.Pop(val));
    EXPECT_EQ(val, -7);
    EXPECT_TRUE(queue.Pop(val));
    EXPECT_EQ(val, -8);

    EXPECT_FALSE(queue.Pop(val));
}



static constexpr int g_lenQue  = 32;
static constexpr int g_lenPush = 16;
static constexpr int g_lenTh   =  8;


static int64_t g_val [g_lenTh][g_lenPush] {};

static void ResetVal(int64_t v [g_lenTh][g_lenPush])
{
    for (int idx = 0; idx < g_lenTh; ++idx)
        for (int i = 0; i < g_lenPush; ++i)
            v[idx][i] = -1;
}


struct Prm
{
    int _idx = 0;
    lfl::ObjectQueue<int64_t, g_lenQue> *   _queue = nullptr;
};


static void *Produce(void *p)
{
    Prm *prm = (Prm *)p;
    lfl::ObjectQueue<int64_t, g_lenQue> *queue = prm->_queue;

    for (int i = 0; i < g_lenPush; ++i )
    {
        while (false == queue->PushBack((10000 * prm->_idx) + i))
            ;// LOG << "Produce : unable to push\n";
        LOG << "PushBack : " << prm->_idx << "," << i << " = " << ((10000 * prm->_idx) + i) << std::endl;
        usleep(rand() % 10);
    }

    return nullptr;
}


static void *Consume(void *p)
{
    Prm *prm = (Prm *)p;
    lfl::ObjectQueue<int64_t, g_lenQue> *queue = prm->_queue;

    for (int i = 0; i < g_lenPush; ++i )
    {
        int64_t val = -1;
        while (false == queue->Pop(val))
            ;//LOG << "Consume : unable to Pop\n";

//        LOG << "Pop  : " << prm->_idx << "," << i << " = " << val << std::endl;

        int idx = int(val/10000.0) - 1;
        int ii  = val % 10000;

        EXPECT_EQ(-1, g_val [idx][ii]);
        g_val [idx] [ii] = val;
        usleep(rand() % 10);
    }

    return nullptr;
}


TEST(ObjectQueue, MT)
{
    srand(time(nullptr));
    ResetVal(g_val);

    lfl::ObjectQueue<int64_t, g_lenQue> queue;
    Prm       prm[g_lenTh];
    pthread_t thProd[g_lenTh] {};
    pthread_t thCons[g_lenTh] {};

    for (std::size_t idx = 0; idx < g_lenTh; ++idx)
    {
        prm[idx]._queue  = &queue;
        prm[idx]._idx    = idx + 1;

        pthread_create(&thProd[idx], nullptr, *Produce, (void *)(&prm[idx]));
        pthread_create(&thCons[idx], nullptr, *Consume, (void *)(&prm[idx]));
    }

    for (std::size_t idx = 0; idx < g_lenTh; ++idx)
    {
        pthread_join(thProd[idx], nullptr);
        pthread_join(thCons[idx], nullptr);
    }

    for (int idx = 0; idx < g_lenTh; ++idx)
    {
        for (int i = 0; i < g_lenPush; ++i )
        {
            EXPECT_EQ( ((10000 * (idx + 1)) + i), g_val[idx][i]) ;
//            LOG << idx << "," << i << "\t:" << g_val[idx][i] << std::endl;
            if (((10000 * (idx + 1)) + i) != g_val[idx][i])
                LOG << "ERROR : " << idx << "," << i << ": " << g_val[idx][i] <<  std::endl;
        }
    }
}

