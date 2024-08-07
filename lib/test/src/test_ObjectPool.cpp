

#include "ObjectPool.hpp"

#include <gtest/gtest.h>
#include <atomic>
#include <list>
#include <mutex>

//#define LOG std::cout
#define LOG if(false) std::cout

class ObjectTest
{
    public:
        bool Init()
        {
            return true;
        }

        void Release()
        {
        }

        int _val = -1;
        static std::atomic<int> _countCurAquired;
        static std::atomic<int> _countTotalAquired;
        static std::atomic<int> _countTotalNotAquired;
};

std::atomic<int> ObjectTest::_countCurAquired       = 0;
std::atomic<int> ObjectTest::_countTotalAquired     = 0;
std::atomic<int> ObjectTest::_countTotalNotAquired  = 0;


TEST(ObjectPool, Basic)
{
    ObjectPool<ObjectTest, 4> pool;

    EXPECT_TRUE(pool.Init());

    ObjectPoolItem<ObjectTest> *item1 = pool.ObjectAcquire();
    EXPECT_NE(nullptr, item1);
    item1->Obj()._val = 1;

    ObjectPoolItem<ObjectTest> *item2 = pool.ObjectAcquire();
    EXPECT_NE(nullptr, item2);
    item2->Obj()._val = 2;

    ObjectPoolItem<ObjectTest> *item3 = pool.ObjectAcquire();
    EXPECT_NE(nullptr, item3);
    item3->Obj()._val = 3;

    ObjectPoolItem<ObjectTest> *item4 = pool.ObjectAcquire();
    EXPECT_NE(nullptr, item4);
    item4->Obj()._val = 4;


    ObjectPoolItem<ObjectTest> *item = nullptr;
    item = pool.ObjectAcquire();
    EXPECT_EQ(nullptr, item);

    EXPECT_TRUE(pool.ObjectRelease(item1));
    EXPECT_TRUE(pool.ObjectRelease(item4));

    EXPECT_TRUE(pool.ObjectRelease(item2));
    EXPECT_TRUE(pool.ObjectRelease(item3));

    //
    item = pool.ObjectAcquire();
    EXPECT_NE(nullptr, item);
    EXPECT_EQ(3, item->Obj()._val);

    EXPECT_TRUE(pool.ObjectRelease(item));
    item = pool.ObjectAcquire();
    EXPECT_NE(nullptr, item);
    EXPECT_EQ(3, item->Obj()._val);

    //
    item = pool.ObjectAcquire();
    EXPECT_NE(nullptr, item);
    EXPECT_EQ(2, item->Obj()._val);

    EXPECT_TRUE(pool.ObjectRelease(item));
    item = pool.ObjectAcquire();
    EXPECT_NE(nullptr, item);
    EXPECT_EQ(2, item->Obj()._val);

    pool.Release();
}




static constexpr int        g_lenPoolTest  = 16;

using PoolTest = ObjectPool<ObjectTest, g_lenPoolTest>;


struct Prm
{
    PoolTest    *_pool      = nullptr;
    int          _idxTh     = 0;
    int          _lenPool   = 0;
    int          _lenLoop   = 0;
};


static void *ThLimited(void *p)
{
    Prm *prm = (Prm *)p;
    PoolTest *pool = prm->_pool;

    for (int idxPush = 0, idxMaxPush = prm->_lenPool * prm->_lenLoop; idxPush < idxMaxPush; ++idxPush)
    {
        EXPECT_LE(0,                ObjectTest::_countCurAquired.load(std::memory_order_seq_cst));
        EXPECT_GE(prm->_lenPool,    ObjectTest::_countCurAquired.load(std::memory_order_seq_cst));

        ObjectPoolItem<ObjectTest> *pObj = pool->ObjectAcquire();
        EXPECT_NE(nullptr, pObj);

        ++ObjectTest::_countCurAquired;
        usleep(rand() % 10);

        --ObjectTest::_countCurAquired;
        pool->ObjectRelease(pObj);

        EXPECT_LE(0,                ObjectTest::_countCurAquired.load(std::memory_order_seq_cst));
        EXPECT_GE(prm->_lenPool,    ObjectTest::_countCurAquired.load(std::memory_order_seq_cst));
    }

    return nullptr;
}


static void *ThUnlimited(void *p)
{
    Prm *prm = (Prm *)p;
    PoolTest *pool = prm->_pool;

    for (int idxPush = 0; idxPush < prm->_lenPool * prm->_lenLoop; ++idxPush)
    {
        EXPECT_LE(0,                ObjectTest::_countCurAquired.load(std::memory_order_seq_cst));
        EXPECT_GE(prm->_lenPool,    ObjectTest::_countCurAquired.load(std::memory_order_seq_cst));

        ObjectPoolItem<ObjectTest> *pObj = pool->ObjectAcquire();
        if (nullptr == pObj)
        {
            ++ObjectTest::_countTotalNotAquired;
        }
        else
        {
            ++ObjectTest::_countTotalAquired;
            ++ObjectTest::_countCurAquired;
            usleep(rand() % 10);

            --ObjectTest::_countCurAquired;
            pool->ObjectRelease(pObj);
        }

        EXPECT_LE(0,                ObjectTest::_countCurAquired.load(std::memory_order_seq_cst));
        EXPECT_GE(prm->_lenPool,    ObjectTest::_countCurAquired.load(std::memory_order_seq_cst));
    }

    return nullptr;
}


TEST(ObjectPool, MTLimited)
{
    PoolTest pool;

    EXPECT_TRUE(pool.Init());

    // Thread count is less then the size of the pool
    constexpr int lenTh =  int (pool.Size() * 3.0/ 4.0);

    Prm       prm[lenTh];
    pthread_t th[lenTh] {};

    for (std::size_t idx = 0; idx < lenTh; ++idx)
    {
        prm[idx]._idxTh     = idx + 1;
        prm[idx]._pool      = &pool;
        prm[idx]._lenPool   = pool.Size();
        prm[idx]._lenLoop   = 32;

        pthread_create(&th[idx], nullptr, *ThLimited, (void *)(&prm[idx]));
    }

    for (std::size_t idx = 0; idx < lenTh; ++idx)
    {
        pthread_join(th[idx], nullptr);
    }
}


TEST(ObjectPool, MTUnlimited)
{
    PoolTest pool;

    EXPECT_TRUE(pool.Init());

    // Thread count is grater then the size of the pool
    constexpr int lenTh =  int (pool.Size() * 2.0/*4.0 / 3.0*/);

    Prm       prm[lenTh];
    pthread_t th[lenTh] {};

    for (std::size_t idx = 0; idx < lenTh; ++idx)
    {
        prm[idx]._idxTh     = idx + 1;
        prm[idx]._pool      = &pool;
        prm[idx]._lenPool   = g_lenPoolTest;
        prm[idx]._lenLoop   = 32;

        pthread_create(&th[idx], nullptr, *ThUnlimited, (void *)(&prm[idx]));
    }

    for (std::size_t idx = 0; idx < lenTh; ++idx)
    {
        pthread_join(th[idx], nullptr);
    }


    EXPECT_EQ(0, ObjectTest::_countCurAquired);
    EXPECT_EQ(ObjectTest::_countTotalAquired + ObjectTest::_countTotalNotAquired, prm->_lenPool * prm->_lenLoop * lenTh);
}
