#include <assert.h>
#include <stdio.h>

#include <iomanip>
#include <sstream>

#include <boost/thread/tss.hpp> 

#include <Windows.h>

#include "CurrentThread.h"

namespace muduo
{

namespace CurrentThread
{
	boost::thread_specific_ptr<unsigned long>  t_cachedTidPtr;
	boost::thread_specific_ptr<string> t_tidStringPtr;
	boost::thread_specific_ptr<string> t_threadNamePtr;
}

namespace detail
{

unsigned long gettid()
{
    return GetCurrentThreadId();
}

}//namespace detail

}//namespace muduo

void muduo::CurrentThread::cacheTid()
{
	if (tls_get<unsigned long>(t_cachedTidPtr) == 0)
	{
		unsigned long tid = detail::gettid();
		tls_set<unsigned long>(t_cachedTidPtr, tid);

		std::ostringstream oss;
		oss << std::setw(6) << tls_get<unsigned long>(t_cachedTidPtr);

		tls_set<string>(t_tidStringPtr, oss.str());
	}
}
