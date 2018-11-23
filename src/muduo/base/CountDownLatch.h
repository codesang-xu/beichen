#pragma once

#include "Condition.h"
#include "Mutex.h"

namespace muduo
{

class CountDownLatch : boost::noncopyable
{
public:
    explicit CountDownLatch(int count);

    void wait();

    void countDown();

    int get_count() const;

private:
    mutable MutexLock mutex_;
    Condition condition_;
    int count_;
};

}