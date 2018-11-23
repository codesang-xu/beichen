#pragma once

#include <boost/thread/mutex.hpp>

namespace muduo
{
typedef boost::mutex MutexLock;
typedef boost::mutex::scoped_lock MutexLockGuard;
}