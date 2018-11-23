// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_CURRENTTHREAD_H
#define MUDUO_BASE_CURRENTTHREAD_H

#include <muduo/base/Config.h>
#include <muduo/base/Types.h>


#ifdef _WIN32
#include <muduo/base/Tls.h>

namespace muduo
{
using namespace boost;
namespace CurrentThread
{
  // internal
  extern boost::thread_specific_ptr<unsigned long>  t_cachedTidPtr;
  extern boost::thread_specific_ptr<string> t_tidStringPtr;
  extern boost::thread_specific_ptr<string> t_threadNamePtr;

  void cacheTid();

  inline int tid()
  {
	  if (__builtin_expect(tls_get<unsigned long>(t_cachedTidPtr) == 0, 0))
	  {
		  cacheTid();
	  }
	  return tls_get<unsigned long>(t_cachedTidPtr);
  }

  inline const char* tidString() // for logging
  {
	  return tls_get<string>(t_tidStringPtr).c_str();
  }

  inline const char* name()
  {
	  return tls_get<string>(t_threadNamePtr).c_str();
  }

  bool isMainThread();

  void sleepUsec(int64_t usec);
}
}
#else

namespace muduo
{
using namespace boost;
namespace CurrentThread
{
  // internal
  extern __thread int t_cachedTid;
  extern __thread char t_tidString[32];
  extern __thread int t_tidStringLength;
  extern __thread const char* t_threadName;
  void cacheTid();

  inline int tid()
  {
    if (__builtin_expect(t_cachedTid == 0, 0))
    {
      cacheTid();
    }
    return t_cachedTid;
  }

  inline const char* tidString() // for logging
  {
    return t_tidString;
  }

  inline int tidStringLength() // for logging
  {
    return t_tidStringLength;
  }

  inline const char* name()
  {
    return t_threadName;
  }

  bool isMainThread();

  void sleepUsec(int64_t usec);
}
}
#endif
#endif
