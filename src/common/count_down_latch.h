#pragma once

#include "common/thread.h"

namespace ib
{
namespace common
{

class count_down_latch : boost::noncopyable
{
public:
    explicit count_down_latch(int count);

    void wait();

    void count_down();

    int get_count() const;

private:
    mutable mutex mutex_;
    condition condition_;
    int count_;
};

}
}

namespace ib_common = ib::common;
