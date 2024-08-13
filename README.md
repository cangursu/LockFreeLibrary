# Lock-Free Object Queue, Stack, and Pool Library

This library provides high-performance, lock-free, wait-free, and thread-safe implementations of `Queue`, `Stack`, `Pool`, and `Work Queue Thread` data structures.

## Build/Install Instructions

Follow the traditional cmake commands below to build the entire library project.
```sh
mkdir -p build && cd build/
cmake ..
cmake --build
```
To install
```sh
cmake --install .
```



## Core Implementation

The core library is header-only and uses template static polymorphism. Header files are located in the `lib/inc` directory:

- `lib/inc/ObjectQueue.hpp`
- `lib/inc/ObjectPool.hpp`
- `lib/inc/ObjectStack.hpp`
- `lib/src/WorkQueue.cpp`

## Usage Examples

### Object Pool

The following code snippet demonstrates basic usage of `ObjectPool`:

- Refer to `prj/src/mainObjectPool.cpp` for more detailed basic usage.
- Refer to `lib/test/src/test_ObjectPool.cpp` for more specific use cases, such as multi-threaded MPMC (multi-producer multi-consumer) usage.

```cpp
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

### Queue

The following code snippet demonstrates basic usage of `ObjectQueue`:

- Refer to `prj/src/mainObjectQueue.cpp` for more detailed basic usage.
- Refer to `lib/test/src/test_ObjectQueue.cpp` for more specific use cases, such as multi-threaded MPMC (multi-producer multi-consumer) usage.

```cpp
ObjectQueue<int, 4> queue;
if (!queue.PushBack(-1))
{
    std::cout << "Unable to push into the Queue\n";
}

int val = 0;
if (!queue.Pop(val))
{
    std::cout << "Unable to pop from the Queue\n";
}
```

### Stack

The following code snippet demonstrates basic usage of `ObjectStack`:

- Refer to `prj/src/mainObjectStack.cpp` for more detailed basic usage.
- Refer to `lib/test/src/test_ObjectStack.cpp` for more specific use cases, such as multi-threaded MPMC (multi-producer multi-consumer) usage.

```cpp
ObjectStack<int, 4> stc;
if (0 > stc.Push(-1))
{
    std::cout << "Unable to push into the Stack\n";
}

int val;
if (0 > stc.Pop(val))
{
    std::cout << "Unable to pop from the Stack\n";
}
```

### Work Queue Thread

The following code snippet demonstrates basic usage of `WorkerQueue`, which creates a thread and waits for work. `WorkQueuePool` can be used for multiple threads of `WorkerQueue`:

- Refer to `prj/src/mainWorkQueue.cpp` for more detailed basic usage.
- Refer to `lib/test/src/test_WorkQueue.cpp` for more specific use cases.

```cpp
class MyQue : public WorkQueue<int, 4, Thread>
{
public:
    int Pop(int* pData)
    {
        std::cout << "Pop - data:" << *pData << std::endl;
        return 0;
    }
};

MyQue que;
que.Init(WQ_QUEUE_STATE::WORKING, "MainPushBack");
que.PushBack(1234);
```

## Additional Examples

More usage examples can be found in the `prj` submodule:

- `prj/src/mainObjectPool.cpp`
- `prj/src/mainObjectQueue.cpp`
- `prj/src/mainObjectStack.cpp`
- `prj/src/mainWorkQueue.cpp`
```

