#pragma once
#include <muduo/base/Mutex.h>

#include <boost/thread/condition.hpp>

namespace muduo
{

class Condition : public boost::condition
{
public:
  bool waitForSeconds(MutexLock &lock, int seconds);
};

}