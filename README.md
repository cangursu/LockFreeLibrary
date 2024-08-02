## Lock-Free Object Queue, Stack, and Pool Library

This library provides high-performance, lock-free, wait-free, and thread-safe implementations of `Queue`, `Stack`, and `Pool` data structures.

### Core Implementation
The core library is header-only and uses template static polymorphism. Header files are located in the `lib/inc` directory:

* `lib/inc/ObjectQueue.hpp`
* `lib/inc/ObjectPool.hpp`
* `lib/inc/ObjectStack.hpp`

### Usage Example

```c++
class Object
{
public:
    Object() {}
    bool Init(int val) { _val = val; return true; }
    void Release() {}
    int _val = -1;
};

void main()
{
    ObjectPool<Object, 8> pool;
    if (!pool.Init(5))
    {
        // Handle initialization error
    }

    ObjectPoolItem<Object>* p = pool.ObjectAcquire();
    if (nullptr == p)
    {
        // Handle acquisition failure
    }
    pool.ObjectRelease(p);

    pool.Release();
}
```

### Additional Examples
More usage examples can be found in the `prj` submodule:

* `prj/src/mainObjectPool.cpp`
* `prj/src/mainObjectQueue.cpp`
* `prj/src/mainObjectStack.cpp`

