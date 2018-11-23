#ifndef IB_COMMON_BLOCKING_QUEUE_H
#define IB_COMMON_BLOCKING_QUEUE_H


#include <assert.h>
#include <deque>

#include "common/thread.h"

namespace ib
{
namespace common
{
template<typename T>
class blocking_queue /*: noncopyable*/
{
public:

    blocking_queue(void)
        : mutex_(),
          not_empty_(),
          queue_()
    {
    }

    void put(const T& x)
    {
		lock_guard<mutex> lock(mutex_);
		queue_.push_back(x);
        not_empty_.notify_all();
    }

    T take()
    {
        unique_lock<mutex> lock(mutex_);
        while (queue_.empty())
        {
            not_empty_.wait(lock);
        }

        assert(!queue_.empty());
        T ret(queue_.front());
        queue_.pop_front();

        return ret;
    }

    T timed_take(int millisecond)
    {
        typedef boost::chrono::system_clock clock;
        typedef clock::time_point time_point;
        typedef boost::chrono::milliseconds milliseconds;
		using boost::cv_status;

        time_point abs_time = clock::now() + milliseconds(millisecond);

        T ret;
        unique_lock<mutex> lock(mutex_);
        while (queue_.empty())
        {
            if (not_empty_.wait_until(lock, abs_time) == cv_status::timeout) {
                break;
            }
        }

        if (!queue_.empty())
        {
            ret = queue_.front();
            queue_.pop_front();
        }

        return ret;
    }

    void clear()
    {
        unique_lock<mutex> lock(mutex_);
        queue_.clear();
    }

    void swap(std::deque<T> & otherqueue)
    {
        unique_lock<mutex> lock(mutex_);
        queue_.swap(otherqueue);
    }

    size_t size()
    {
		lock_guard<mutex> lock(mutex_);
        return queue_.size();
    }

private:
    mutex         mutex_;
    condition     not_empty_;
    std::deque<T> queue_;
};

}
}

namespace ib_common = ib::common;

#endif
