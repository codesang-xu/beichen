#include <muduo/base/Config.h>
#include <muduo/base/Timestamp.h>

#include <time.h>
#include <stdio.h>

#include <boost/static_assert.hpp>

#include <Windows.h>

using namespace muduo;

//BOOST_STATIC_ASSERT(sizeof(Timestamp) == sizeof(int64_t));

Timestamp::Timestamp(int64_t microseconds)
  : microSecondsSinceEpoch_(microseconds)
{
}

string Timestamp::toString() const
{
  char buf[32] = {0};
  int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
  int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
  _snprintf_s(buf, sizeof(buf)-1, _TRUNCATE, "%I64d.%06I64D", seconds, microseconds);
  return buf;
}

string Timestamp::toFormattedString() const
{
  char buf[32] = {0};
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
  int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
  struct tm tm_time;
  gmtime_r(&seconds, &tm_time);

  _snprintf_s(buf, sizeof(buf), _TRUNCATE, "%4d%02d%02d %02d:%02d:%02d.%06d",
      tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
      tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
      microseconds);
  return buf;
}

Timestamp Timestamp::now()
{
  SYSTEMTIME st;
  GetSystemTime(&st);

  FILETIME ft;
  SystemTimeToFileTime( &st, &ft );

  ULARGE_INTEGER ui;
  ui.LowPart = ft.dwLowDateTime;
  ui.HighPart = ft.dwHighDateTime;

  //116444736000000000 January 1, 1601 (UTC) µ½ 1970-01-01 00:00:00 UTC µÄ 100ÄÉÃëÊý
  return Timestamp((ui.QuadPart-116444736000000000)/10);
}

Timestamp Timestamp::invalid()
{
  return Timestamp();
}

