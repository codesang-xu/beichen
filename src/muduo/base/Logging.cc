#include <muduo/base/Config.h>
#include <muduo/base/Logging.h>

#include <muduo/base/CurrentThread.h>
#include <muduo/base/StringPiece.h>
#include <muduo/base/Timestamp.h>
#ifdef _WIN32
#include <muduo/base/Tls.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <iomanip>
#include <sstream>
#endif

using namespace boost;
namespace muduo
{
#ifdef _WIN32
	boost::thread_specific_ptr<string> t_time_ptr;
	boost::thread_specific_ptr<time_t> t_lastSecond_ptr;
#else
	__thread char t_errnobuf[512];
	__thread char t_time[32];
	__thread time_t t_lastSecond;
#endif

const char* strerror_tl(int savedErrno)
{
#ifdef _WIN32
  //strerror_s(t_errnobuf, sizeof t_errnobuf, savedErrno);
  //return t_errnobuf;
	return "";
#else
  return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
#endif
}

Logger::LogLevel initLogLevel()
{
  if (::getenv("MUDUO_M_LOG_TRACE"))
    return Logger::LL_TRACE;
  else if (::getenv("MUDUO_LOG_DEBUG"))
    return Logger::LL_DEBUG;
  else
    return Logger::LL_INFO;
}

Logger::LogLevel g_logLevel = initLogLevel();

const char* LogLevelName[Logger::LL_NUM_LOG_LEVELS] =
{
  "TRACE ",
  "DEBUG ",
  "INFO  ",
  "WARN  ",
  "ERROR ",
  "FATAL ",
};

// helper class for known string length at compile time
class T
{
 public:
  T(const char* str, unsigned len)
    :str_(str),
     len_(len)
  {
    assert(strlen(str) == len_);
  }

  const char* str_;
  const unsigned len_;
};

inline LogStream& operator<<(LogStream& s, T v)
{
  s.append(v.str_, v.len_);
  return s;
}

inline LogStream& operator<<(LogStream& s, const Logger::SourceFile& v)
{
  s.append(v.data_, v.size_);
  return s;
}

void defaultOutput(const char* msg, int len)
{
  size_t n = fwrite(msg, 1, len, stdout);
  //FIXME check n
  (void)n;
}

void defaultFlush()
{
  fflush(stdout);
}

Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;

}

using namespace muduo;

Logger::Impl::Impl(LogLevel level, int savedErrno, const SourceFile& file, int line)
  : time_(Timestamp::now()),
    stream_(),
    level_(level),
    line_(line),
    basename_(file)
{
  formatTime();
  CurrentThread::tid();

#ifdef _WIN32
  stream_ << T(CurrentThread::tidString(), 6) << " ";
#else
  stream_ << T(CurrentThread::tidString(), CurrentThread::tidStringLength()) << " ";
#endif // _WIN32

  stream_ << T(LogLevelName[level], 6);
  if (savedErrno != 0)
  {
    stream_ << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
  }
}
#ifdef _WIN32
void Logger::Impl::formatTime()
{
	int64_t microSecondsSinceEpoch = time_.microSecondsSinceEpoch();
	time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / 1000000);
	int microseconds = static_cast<int>(microSecondsSinceEpoch % 1000000);
	if (seconds != tls_get<time_t>(t_lastSecond_ptr))
	{
		tls_set<time_t>(t_lastSecond_ptr, seconds);
		struct tm tm_time;
		::localtime_r(&seconds, &tm_time); // FIXME TimeZone::fromUtcTime  5u54

		std::ostringstream oss_time_buf;
		oss_time_buf << std::setfill('0')
			<< std::setw(4) << (tm_time.tm_year + 1900)
			<< std::setw(2) << (tm_time.tm_mon + 1)
			<< std::setw(2) << tm_time.tm_mday
			<< " "
			<< std::setw(2) << tm_time.tm_hour << ":"
			<< std::setw(2) << tm_time.tm_min << ":"
			<< std::setw(2) << tm_time.tm_sec;

		tls_set<string>(t_time_ptr, oss_time_buf.str());
}
	Fmt us(".%04d", microseconds / 10000);
	stream_ << T(tls_get<string>(t_time_ptr).c_str(), 17) << T(us.data(), 5);
}
#else
void Logger::Impl::formatTime()
{
	int64_t microSecondsSinceEpoch = time_.microSecondsSinceEpoch();
	time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / 1000000);
	int microseconds = static_cast<int>(microSecondsSinceEpoch % 1000000);
	if (seconds != t_lastSecond)
	{
		t_lastSecond = seconds;
		struct tm tm_time;
		::localtime_r(&seconds, &tm_time); // FIXME TimeZone::fromUtcTime  5u54

		int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
			tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
			tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
	}
	Fmt us(".%04d", microseconds / 10000);
	stream_ << T(t_time, 17) << T(us.data(), 5);
}
#endif


void Logger::Impl::finish()
{
  stream_ << " - " << basename_ << ':' << line_ << '\n';
}

Logger::Logger(SourceFile file, int line)
  : impl_(LL_INFO, 0, file, line)
{
}

Logger::Logger(SourceFile file, int line, LogLevel level, const char* func)
  : impl_(level, 0, file, line)
{
  impl_.stream_ << func << ' ';
}

Logger::Logger(SourceFile file, int line, LogLevel level)
  : impl_(level, 0, file, line)
{
}

Logger::Logger(SourceFile file, int line, bool toAbort)
  : impl_(toAbort?LL_FATAL:LL_ERROR, errno, file, line)
{
}

Logger::~Logger()
{
  impl_.finish();
  const LogStream::Buffer& buf(stream().buffer());
  g_output(buf.data(), buf.length());
  if (impl_.level_ == LL_FATAL)
  {
    g_flush();
    _exit(0);
  }
}

void Logger::setLogLevel(Logger::LogLevel level)
{
  g_logLevel = level;
}

void Logger::setOutput(OutputFunc out)
{
  g_output = out;
}

void Logger::setFlush(FlushFunc flush)
{
  g_flush = flush;
}
