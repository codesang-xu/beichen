#ifndef _COMMON_THREAD_H_
#define _COMMON_THREAD_H_

#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

namespace ib
{
    using boost::mutex;
    using boost::lock_guard;
    using boost::thread;
    using boost::unique_lock;
    using boost::condition;
    typedef unique_lock<mutex> scoped_lock;

    namespace this_thread = boost::this_thread;
}

#endif