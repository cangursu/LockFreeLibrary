
#ifndef __OBJECT_STACK_HPP__
#define __OBJECT_STACK_HPP__


#include <atomic>
#include <cstddef>




/**
 * @brief ObjectStack
 * This class implements a lock-free, wait-free, and thread-safe object Stack (LIFO)
 * using C++11's compare-and-swap (CAS) feature.
 *
 * The Objects should be stored and initialized before use this infrastructure for performance reasons.
 *
 * @tparam ObjectStack Object type
 * @tparam KObjectCount Number of objects in stack
 *
 *
 */


template <typename TObject, std::size_t KObjectCount>
class ObjectStack
{
public:
    ObjectStack(ObjectStack const &)                = delete;
    ObjectStack &operator=(ObjectStack const &)     = delete;

    ObjectStack()
    {
    }

    int  Push(const TObject &data);
    int  Pop(TObject &data);
    void Reset();

private:
    TObject                     _buffer[KObjectCount];
    std::atomic<std::size_t>    _idx    = 0UL;
};


template <typename TObject, std::size_t KObjectCount>
int ObjectStack<TObject, KObjectCount>::Push(const TObject &data)
{
    for (int countTry = 0; countTry < 250; ++countTry)
    {
        std::size_t idx = _idx.load(std::memory_order_relaxed);
        if (idx < KObjectCount)
        {
            if (_idx.compare_exchange_weak(idx, idx + 1, std::memory_order_release, std::memory_order_relaxed))
            {
                _buffer[idx] = data;
                return idx;
            }
        }
        else
        {
            return -1; //Full
        }
    }


    return -2; //Not Ready
}


template <typename TObject, std::size_t KObjectCount>
int ObjectStack<TObject, KObjectCount>::Pop(TObject &data)
{
    for (int countTry = 0; countTry < 250; ++countTry)
    {
        std::size_t idx = _idx.load(std::memory_order_relaxed);
        if (idx > 0)
        {
            if (_idx.compare_exchange_weak(idx, idx - 1, std::memory_order_release, std::memory_order_acquire))
            {
                data = _buffer[idx - 1];
                return idx - 1;
            }
        }
        else if (idx == 0)
        {
            return -1; //Empty
        }
        else
        {
        }
    }

    return -2; //Not Ready
}


template <typename TObject, std::size_t KObjectCount>
void ObjectStack<TObject, KObjectCount>::Reset()
{
    _idx.store(0, std::memory_order_release);
}



#endif // __OBJECT_STACK_HPP__
