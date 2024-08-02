
#include "ObjectPool.hpp"

#include <iostream>


class Object
{
    public:
        Object()
        {
        }

        //Object(int val) : _val(val)
        //{
        //}

        bool Init(int val);
        void Release();

        int _val = -1;
        int _idx = -1;
};


bool Object::Init(int val)
{
    static int idx = 0;
    _idx = idx++;
    _val = val;
    return true;
}

void Object::Release()
{
    _val = -2;
}


ObjectPoolItem<Object> *Acquire(ObjectPool<Object, 8> &pool)
{
    ObjectPoolItem<Object> *p = pool.ObjectAcquire();
    if (nullptr == p)
    {
        std::cout << "Free slot not found" << std::endl;
    }
    else
    {
        std::cout << "Aquire : " << (*p)->_idx << std::endl;
    }

    return p;
}


void main_ObjectPool()
{
    ObjectPool<Object, 8> pool;
    if (false == pool.Init(5))
    {
        std::cout << "Failed to init pool" << std::endl;
        return;
    }

    ObjectPoolItem<Object> *p1 = Acquire(pool);
    ObjectPoolItem<Object> *p2 = Acquire(pool);

    pool.ObjectRelease(p2);

    ObjectPoolItem<Object> *p3 = Acquire(pool);

    pool.ObjectRelease(p3);
    pool.ObjectRelease(p1);

    /*ObjectPoolItem<Object> *p4 = */Acquire(pool);

    pool.Release();
}


void main_ObjectPool2()
{
    ObjectPool<Object, 8> pool;

    ObjectPoolLock <Object, 8> guard1(pool);
    guard1->_val = -19;

    ObjectPoolLock <Object, 8> guard2(pool);
    guard2->_val = -29;
}

