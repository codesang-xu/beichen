#include <muduo/base/Config.h>
#include <muduo/base/Condition.h>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace muduo
{

bool Condition::waitForSeconds(MutexLock &lock, int seconds)
{
    return timed_wait(lock, boost::get_system_time() + boost::posix_time::seconds(seconds));
}

}
