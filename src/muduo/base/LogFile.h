#ifndef MUDUO_BASE_LOGFILE_H
#define MUDUO_BASE_LOGFILE_H

#include <muduo/base/Mutex.h>
#include <muduo/base/Types.h>

#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
using namespace boost::posix_time;

namespace muduo
{

class LogFile : boost::noncopyable
{
 public:
	 typedef boost::chrono::system_clock::time_point time_point;

 public:
	 LogFile(const string& basedir, 
		  const string& basename,
          size_t rollSize,
          bool threadSafe = true,
          int flushInterval = 3);
  ~LogFile();

  void append(const char* logline, int len);
  void flush();

 private:
  void append_unlocked(const char* logline, int len);

  static string getLogFileName(const string& basedir, const string& basename, ptime thetime);
  void   rollFile(string newfileID = "");

  string getFileID(string log);

  const string basedir_;
  const string basename_;
  const size_t rollSize_;
  const int flushInterval_;

  int count_;

  boost::scoped_ptr<MutexLock> mutex_;
  time_t startOfPeriod_;
  time_t lastRoll_;
  time_t lastFlush_;
  class File;
  boost::scoped_ptr<File> file_;

  const static int kCheckTimeRoll_ = 1024;
  const static int kRollPerSeconds_ = 60*60*24;

  string fileID_;
};

}
#endif  // MUDUO_BASE_LOGFILE_H
