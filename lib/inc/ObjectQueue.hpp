

#ifndef __OBJECT_QUEUE_HPP__
#define __OBJECT_QUEUE_HPP__


#include <sstream>
#include <atomic>
#include <cassert>
#include <stddef.h>

#include <iostream>


template <typename TObject, std::size_t KObjectCount>
class ObjectQueue;

template <typename TObject, std::size_t KObjectCount>
void to_string(const ObjectQueue<TObject, KObjectCount> &val, std::ostringstream &ss);

#define MOD(val, base) ((val) & (base-1))




/**
 * @brief ObjectStack
 * This class implements a lock-free, wait-free, and thread-safe object Queue (FIFO)
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
class ObjectQueue
{
public:
    friend void to_string <> (const ObjectQueue<TObject, KObjectCount> &val, std::ostringstream &ss);
    ObjectQueue(ObjectQueue const &)              = delete;
    ObjectQueue &operator=(ObjectQueue const &)   = delete;

    ObjectQueue();

    bool PushBack(TObject const &data);
    bool Pop(TObject &data);

private:

    struct Slot
    {
        std::atomic<std::size_t>    _seq;
        TObject                     _data;
    };

    Slot                        _buffer[KObjectCount];
    const std::size_t           _idxMod;

    std::atomic<std::size_t>    _idxPush    = 0UL;
    std::atomic<std::size_t>    _idxPop     = 0UL;
};


template <typename TObject, std::size_t KObjectCount>
void to_string(const ObjectQueue<TObject, KObjectCount> &val, std::ostringstream &ss)
{
    ss  << "_idxMod:"    << val._idxMod
        << ", _idxPush:" << val._idxPush
        << ", _idxPop:"  << val._idxPop
        << ", _buffer:"  << val._buffer[0]._seq;
    for (std::size_t i = 1; i != KObjectCount; i += 1)
        ss << ", " << val._buffer[i]._seq << ":" << val._buffer[i]._data;
}


template <typename TObject, std::size_t KObjectCount>
std::string to_string(const ObjectQueue<TObject, KObjectCount> &val)
{
    std::ostringstream ss;
    to_string(val, ss);
    return ss.str();
}


template <typename TObject, std::size_t KObjectCount>
ObjectQueue<TObject, KObjectCount>::ObjectQueue()
    : _idxMod(KObjectCount - 1)
{
    static_assert((KObjectCount >= 2) && ((KObjectCount & (KObjectCount - 1)) == 0), "KObjectCount should be >1 and power of 2 ");

    for (std::size_t i = 0; i != KObjectCount; i += 1)
        _buffer[i]._seq.store(i, std::memory_order_relaxed);
}


template <typename TObject, std::size_t KObjectCount>
bool ObjectQueue<TObject, KObjectCount>::PushBack(TObject const &data)
{
    Slot *slot = nullptr;

    while (true)
    {
        std::size_t pos = _idxPush.load(std::memory_order_relaxed);

        slot = &_buffer[pos & _idxMod];
        std::size_t seq = slot->_seq.load(std::memory_order_acquire);

         long dif = (long)seq - (long)pos;
        if (dif == 0)
        {
            if (_idxPush.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
            {
                slot->_data = data;
                slot->_seq.store(pos + 1, std::memory_order_release);
                return true;
            }
        }
        else if (dif < 0)
        {
            return false;
        }
    }

    return false;
}


template <typename TObject, std::size_t KObjectCount>
bool ObjectQueue<TObject, KObjectCount>::Pop(TObject &data)
{
    Slot *slot = nullptr;

    while (true)
    {
        std::size_t pos = _idxPop.load(std::memory_order_relaxed);

        slot = &_buffer[pos & _idxMod];
        std::size_t seq = slot->_seq.load(std::memory_order_acquire);

        long dif = (long)seq - (long)(pos + 1);
        if (dif == 0)
        {
            if (_idxPop.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
            {
                data = slot->_data;
                slot->_seq.store(pos + _idxMod + 1, std::memory_order_release);
                return true;
            }
        }
        else if (dif < 0)
        {
            return false;
        }
    }

    return false;
}



#endif // __OBJECT_QUEUE_HPP__
