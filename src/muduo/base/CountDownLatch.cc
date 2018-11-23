#include "CountDownLatch.h"

using namespace muduo;

CountDownLatch::CountDownLatch(int count)
    : mutex_(),
        condition_(),
        count_( count )
{
};

void CountDownLatch::wait()
{
    MutexLockGuard lock( mutex_ );
    while (count_ > 0) {
        condition_.wait( mutex_ );
    }
};

void CountDownLatch::countDown()
{
    MutexLockGuard lock( mutex_ );
    --count_;
    if (count_ == 0) {
        condition_.notify_all();
    }
};

int CountDownLatch::get_count() const
{
    MutexLockGuard lock( mutex_ );
    return count_;
};
