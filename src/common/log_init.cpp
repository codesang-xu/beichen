#include "common/log_init.h"

namespace ib
{
namespace common
{

muduo::AsyncLogging * async_log_init::asyncLog = NULL;
muduo::LogFile *sync_log_init::logFile = NULL;

}
}