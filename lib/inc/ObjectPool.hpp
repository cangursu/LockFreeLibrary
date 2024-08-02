
#ifndef __OBJECT_POOL_HPP__
#define __OBJECT_POOL_HPP__


#include <ObjectStack.hpp>

#include <map>
#include <string>
#include <atomic>
#include <cassert>

//#include <iostream>
//#define  LLOG_ERROR if(false) std::cout


using ParamList  = std::map<std::string, std::string>;


/**
 * @brief ObjectPoolItem
 * A note that store the Object of ObjectPool with its index to increase the access performance.
 *
 * @tparam TObject Object type
 * @tparam int _idx the index of the object
 *
 *
 */


template <typename TObject>
struct ObjectPoolItem
{
    TObject & Obj()                     { return _obj;    }

    //TObject & operator*()               { return Obj(); }
    //const TObject & operator*() const   { return Obj(); }
    TObject * operator->()              { return &Obj();  }
    const TObject * operator->() const  { return &Obj();  }


private:
    template <typename TObject2, std::size_t KObjectCount>
    friend class ObjectPool;

    TObject     _obj;
    int         _idx;
};



/**
 * @brief ObjectPool
 * This class implements a lock-free, wait-free, and thread-safe object pool using
 * using C++11's compare-and-swap (CAS) feature.
 *
 * The Objects should be stored and initialized before use this infrastructure for performance reasons.
 *
 * @tparam TObject Object type
 * @tparam KObjectCount Max number of objects in pool
 *
 *
 */


template <typename TObject, std::size_t KObjectCount>
class ObjectPool
{
public:

    ObjectPool() = default;

    template <typename... Args>
    bool    Init(Args... args);
    void    Release();


    ObjectPoolItem<TObject> *   ObjectAcquire();
    bool                        ObjectRelease(ObjectPoolItem<TObject> *p);

    void                        Reset();
    constexpr std::size_t       Size() const;

private:

    ObjectPoolItem<TObject>         _buffer[KObjectCount];
    ObjectStack<int, KObjectCount>  _stackIdx;
};


template <typename TObject, std::size_t KObjectCount>
template <typename... Args>
bool ObjectPool<TObject, KObjectCount>::Init(Args... args)
{
    int ret = true;
    for (std::size_t idx = 0; idx < KObjectCount; ++idx)
    {
        if (true != _buffer[idx]._obj.Init(args...))
        {
            //TODO: "Release" 'es should be called on the previously initialized objects.
            //LLOG_ERROR << "Unable to initialize PoolObject";
            ret = false;
            break;
        }
        else
        {
            _buffer[idx]._idx = idx;
            _stackIdx.Push(idx);
        }
    }

    if (false == ret)
    {
        Release();
    }

    return ret;
}


template <typename TObject, std::size_t KObjectCount>
void ObjectPool<TObject, KObjectCount>::Release()
{
    Reset();
    for (std::size_t idx = 0; idx < KObjectCount; ++idx)
    {
        _buffer[idx]._obj.Release();
    }
}


template <typename TObject, std::size_t KObjectCount>
ObjectPoolItem<TObject> *ObjectPool<TObject, KObjectCount>::ObjectAcquire()
{
    int idx = -1;
    if ((0 > _stackIdx.Pop(idx)) && (idx < 0))
    {
        return nullptr;
    }

    return _buffer + idx;
}


template <typename TObject, std::size_t KObjectCount>
bool ObjectPool<TObject, KObjectCount>::ObjectRelease(ObjectPoolItem<TObject> *p)
{
    //TODO: 1. Check if the ObjecyPoolItem is currently Acquired
    //TODO: 2. Check if the ObjecyPoolItem is not Released twice (present _stackIdx)
    //TODO: 3. A debug mode check could be added to make the validations above

    assert(p);
    return -1 < _stackIdx.Push(p->_idx);
}


template <typename TObject, std::size_t KObjectCount>
void ObjectPool<TObject, KObjectCount>::Reset()
{
    _stackIdx.Reset();
}


template <typename TObject, std::size_t KObjectCount>
constexpr std::size_t ObjectPool<TObject, KObjectCount>::Size() const
{
    return KObjectCount;
}



/**
 *
 *@brief ObjectPoolItem
 * An ObjectPool guard which Aquires/Releases on ctor/dtor.
 *
 *
 */




template <typename TObject, std::size_t KObjectCount>
class ObjectPoolLock
{
    public:
        virtual ~ObjectPoolLock();
        ObjectPoolLock(ObjectPool <TObject, KObjectCount> &pool);
        ObjectPoolLock(const ObjectPoolLock &)              = delete;
        ObjectPoolLock(ObjectPoolLock &&)                   = default;
        ObjectPoolLock &operator = (const ObjectPoolLock &) = delete;
        ObjectPoolLock &operator = (ObjectPoolLock&&)       = default;

        operator TObject*();
        operator TObject*() const;
        operator TObject&();
        operator TObject&() const;
        TObject*        operator ->();
        const TObject*  operator ->() const;
        TObject&        operator * ();
        const TObject&  operator * () const;

        TObject*        get();
        const TObject*  get() const;

    private:
        ObjectPool <TObject, KObjectCount>  & _pool;
        ObjectPoolItem <TObject>            * _object;
};



template <typename TObject, std::size_t KObjectCount>
ObjectPoolLock<TObject, KObjectCount>::ObjectPoolLock(ObjectPool <TObject, KObjectCount> &pool)
    : _pool(pool)
{
    _object = _pool.ObjectAcquire();
}


template <typename TObject, std::size_t KObjectCount>
ObjectPoolLock<TObject, KObjectCount>::~ObjectPoolLock()
{
    _pool.ObjectRelease(_object);
}


template <typename TObject, std::size_t KObjectCount>
TObject* ObjectPoolLock<TObject, KObjectCount>::get()
{
    return &_object->Obj();
}


template <typename TObject, std::size_t KObjectCount>
const TObject*  ObjectPoolLock<TObject, KObjectCount>::get() const
{
    return &_object->Obj();
}


template <typename TObject, std::size_t KObjectCount>
ObjectPoolLock<TObject, KObjectCount>::operator TObject*()
{
    return get();
}


template <typename TObject, std::size_t KObjectCount>
ObjectPoolLock<TObject, KObjectCount>::operator TObject*() const
{
    return get();
}


template <typename TObject, std::size_t KObjectCount>
ObjectPoolLock<TObject, KObjectCount>::operator TObject&()
{
    return get();
}


template <typename TObject, std::size_t KObjectCount>
ObjectPoolLock<TObject, KObjectCount>::operator TObject&() const
{
    return get();
}


template <typename TObject, std::size_t KObjectCount>
TObject* ObjectPoolLock<TObject, KObjectCount>::operator ->()
{
    return get();
}


template <typename TObject, std::size_t KObjectCount>
const TObject* ObjectPoolLock<TObject, KObjectCount>::operator ->() const
{
    return get();
}


template <typename TObject, std::size_t KObjectCount>
TObject& ObjectPoolLock<TObject, KObjectCount>::operator *()
{
    return get();
}


template <typename TObject, std::size_t KObjectCount>
const TObject& ObjectPoolLock<TObject, KObjectCount>::operator *() const
{
    return get();
}


#endif // __OBJECT_POOL_HPP__
