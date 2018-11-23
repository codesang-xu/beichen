#include "common/count_down_latch.h"

namespace ib
{
namespace common
{

count_down_latch::count_down_latch(int count)
    : mutex_(),
        condition_(),
        count_( count )
{
};

void count_down_latch::wait()
{
	mutex::scoped_lock lock(mutex_);
    while (count_ > 0) {
        condition_.wait( mutex_ );
    }
};

void count_down_latch::count_down()
{
	mutex::scoped_lock lock(mutex_);
    --count_;
    if (count_ == 0) {
        condition_.notify_all();
    }
};

int count_down_latch::get_count() const
{
	mutex::scoped_lock lock(mutex_);
    return count_;
};

}
}