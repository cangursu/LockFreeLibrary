

#include "ObjectQueue.hpp"

#include <iostream>
#include <unistd.h>

namespace lfl=LockFreeLib;


void main_ObjectQueue_1()
{
    lfl::ObjectQueue<int, 4> queue;

    std::cout << "Stat\n";
    std::cout << lfl::to_string(queue) << std::endl;

    std::cout << "PushBack\n";
    bool b = false;
    b = queue.PushBack(-1);
    std::cout << "b  : " << (b?"true":"false") << std::endl;
    std::cout << lfl::to_string(queue) << std::endl;

    b = queue.PushBack(-2);
    std::cout << "b  : " << (b?"true":"false") << std::endl;
    std::cout << lfl::to_string(queue) << std::endl;

    b = queue.PushBack(-3);
    std::cout << "b  : " << (b?"true":"false") << std::endl;
    std::cout << lfl::to_string(queue) << std::endl;


    std::cout << "Pop\n";

    int val = 0;
    b = queue.Pop(val);
    std::cout << "val  : " << val  << ", b  : " << (b?"true":"false") << std::endl;
    std::cout << lfl::to_string(queue) << std::endl;

    b = queue.Pop(val);
    std::cout << "val  : " << val  << ", b  : " << (b?"true":"false") << std::endl;
    std::cout << lfl::to_string(queue) << std::endl;

    b = queue.Pop(val);
    std::cout << "val  : " << val  << ", b  : " << (b?"true":"false") << std::endl;
    std::cout << lfl::to_string(queue) << std::endl;
}




void main_ObjectQueue_2()
{
    lfl::ObjectQueue<int, 4> queue;

    std::cout << "Stat\n";
    std::cout << lfl::to_string(queue) << std::endl;

    std::cout << "PushBack\n";
    bool b = false;
    b = queue.PushBack(-1);
    std::cout << "b  : " << (b?"true":"false") << std::endl;
    std::cout << lfl::to_string(queue) << std::endl;

    b = queue.PushBack(-2);
    std::cout << "b  : " << (b?"true":"false") << std::endl;
    std::cout << lfl::to_string(queue) << std::endl;

    b = queue.PushBack(-3);
    std::cout << "b  : " << (b?"true":"false") << std::endl;
    std::cout << lfl::to_string(queue) << std::endl;

    b = queue.PushBack(-4);
    std::cout << "b  : " << (b?"true":"false") << std::endl;
    std::cout << lfl::to_string(queue) << std::endl;

    b = queue.PushBack(-5);
    std::cout << "b  : " << (b?"true":"false") << std::endl;
    std::cout << lfl::to_string(queue) << std::endl;


    std::cout << "Pop\n";

    int val = 0;
    b = queue.Pop(val);
    std::cout << "val  : " << val  << ", b  : " << (b?"true":"false") << std::endl;
    std::cout << lfl::to_string(queue) << std::endl;

    b = queue.Pop(val);
    std::cout << "val  : " << val  << ", b  : " << (b?"true":"false") << std::endl;
    std::cout << lfl::to_string(queue) << std::endl;

    b = queue.Pop(val);
    std::cout << "val  : " << val  << ", b  : " << (b?"true":"false") << std::endl;
    std::cout << lfl::to_string(queue) << std::endl;

    b = queue.Pop(val);
    std::cout << "val  : " << val  << ", b  : " << (b?"true":"false") << std::endl;
    std::cout << lfl::to_string(queue) << std::endl;

    b = queue.Pop(val);
    std::cout << "val  : " << val  << ", b  : " << (b?"true":"false") << std::endl;
    std::cout << lfl::to_string(queue) << std::endl;

    std::cout << "PushBack\n";
    queue.PushBack(-6);
    std::cout << lfl::to_string(queue) << std::endl;
    queue.PushBack(-7);
    std::cout << lfl::to_string(queue) << std::endl;
    queue.PushBack(-8);
    std::cout << lfl::to_string(queue) << std::endl;
    queue.PushBack(-9);
    std::cout << lfl::to_string(queue) << std::endl;
    queue.PushBack(-10);
    std::cout << lfl::to_string(queue) << std::endl;


    std::cout << "Pop\n";
    b = queue.Pop(val);
    std::cout << "val  : " << val << ", b  : " << (b?"true":"false") << std::endl;
    std::cout << lfl::to_string(queue) << std::endl;

    b = queue.Pop(val);
    std::cout << "val  : " << val  << ", b  : " << (b?"true":"false") << std::endl;
    std::cout << lfl::to_string(queue) << std::endl;

    b = queue.Pop(val);
    std::cout << "val  : " << val << ", b  : " << (b?"true":"false") << std::endl;
    std::cout << lfl::to_string(queue) << std::endl;

    b = queue.Pop(val);
    std::cout << "val  : " << val << ", b  : " << (b?"true":"false") << std::endl;
    std::cout << lfl::to_string(queue) << std::endl;

    b = queue.Pop(val);
    std::cout << "val  : " << val << ", b  : " << (b?"true":"false") << std::endl;
    std::cout << lfl::to_string(queue) << std::endl;
}






static constexpr int g_lenQue  = 16 * 16;
static constexpr int g_lenPush = 20 * 16;
static constexpr int g_lenTh   = 10 * 16;

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
    lfl::ObjectQueue<int64_t, g_lenQue> * _queue = nullptr;
};


static void *Produce(void *p)
{
    Prm *prm = (Prm *)p;
    lfl::ObjectQueue<int64_t, g_lenQue> *queue = prm->_queue;

    for (int i = 0; i < g_lenPush; ++i )
    {
        while (false == queue->PushBack((10000 * prm->_idx) + i))
            ;// std::cout << "Produce : unable to push\n";
        std::cout << "PushBack : " << prm->_idx << "," << i << " = " << ((10000 * prm->_idx) + i) << std::endl;
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
            ;//std::cout << "Consume : unable to Pop\n";

//        std::cout << "Pop  : " << prm->_idx << "," << i << " = " << val << std::endl;

        int idx = int(val/10000.0) - 1;
        int ii   = val % 10000;
        if (-1 == g_val [idx][ii])
            g_val [idx] [ii] = val;
        else
            std::cout << "..................ERROR : val=" << val << ", " << __LINE__ << std::endl;

        usleep(rand() % 10);
    }

    return nullptr;
}


void main_ObjectQueue_3()
{
    std::cout << "Hello  " << std::endl;

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
//            std::cout << idx << "," << i << "\t:" << g_val[idx][i] << std::endl;
            if (((10000 * (idx + 1)) + i) != g_val[idx][i])
                std::cout << "__________________ERROR : " << idx << "," << i << ": " << g_val[idx][i] <<  std::endl;
        }
    }
}

