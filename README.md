## Lock-Free Object Queue, Stack, and Pool Library

This library provides high-performance, lock-free, wait-free, and thread-safe implementations of `Queue`, `Stack`, and `Pool` data structures.

### Core Implementation
The core library is header-only and uses template static polymorphism. Header files are located in the `lib/inc` directory:

* `lib/inc/ObjectQueue.hpp`
* `lib/inc/ObjectPool.hpp`
* `lib/inc/ObjectStack.hpp`

### Usage Example
- **Object Pool** :
The code snipped below demontrate the basic usage of ObjectPool.
   - Refer `prj/src/mainObjectPool.cpp` for more detailed basic usage.
   - Refer `lib/test/src/test_ObjectPool.cpp` for more specific usages such as muti-threaded MPMC (multi-producer multi-consumer) usage.

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

- **Queue** :
The code snipped below demontrate the basic usage of ObjectPool.
   - Refer `prj/src/mainObjectQueue.cpp` for more detailed basic usage.
   - Refer `lib/test/src/test_ObjectQueue.cpp` for more specific usages such as muti-threaded MPMC (multi-producer multi-consumer) usage.

```C++

    ObjectQueue<int, 4> queue;
    if (false == queue.PushBack(-1))
    {
        std::cout << "Enable to push into the Queue\n";
    }

    int val = 0;
    if (false == queue.Pop(val))
    {
        std::cout << "Enable to pop from the Queue\n";
    }

```

- **Stack** :
The code snipped below demontrate the basic usage of ObjectPool.
   - Refer `prj/src/mainObjectStack.cpp` for more detailed basic usage.
   - Refer `lib/test/src/test_ObjectStack.cpp` for more specific usages such as muti-threaded MPMC (multi-producer multi-consumer) usage.

```C++
    ObjectStack<int, 4> stc;
    if (0 > stc.Push(-1))
    {
        std::cout << "Enable to push into the Stack\n";
    }

    int val
    if (0 > stc.Pop(val))
    {
        std::cout << "Enable to push into the Stack\n";
    }
```

### Additional Examples
More usage examples can be found in the `prj` submodule:

* `prj/src/mainObjectPool.cpp`
* `prj/src/mainObjectQueue.cpp`
* `prj/src/mainObjectStack.cpp`

