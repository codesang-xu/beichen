#ifndef IB_COMMON_FIXED_STR_H
#define IB_COMMON_FIXED_STR_H

#include <assert.h>
#include <stdio.h>
#include <string>

#include <muduo/base/Logging.h>

namespace ib
{
namespace common
{
namespace detail
{

template<int SIZE=100>
class fixed_str_
{
public:
    fixed_str_(int size, char* str)
    {
        assert(SIZE>size);
        memset(buffer, 0, size);
        char fmt[10];
        snprintf(fmt, sizeof fmt, "%%-%ds", size);
        snprintf(buffer, size, fmt, str);
    }

    fixed_str_(int size, string str)
    {
        assert(SIZE>size);
        memset(buffer, 0, size);
        char fmt[10];
#ifdef _WIN32
        _snprintf_s(fmt, sizeof fmt, _TRUNCATE, "%%-%ds", size);
        _snprintf_s(buffer, size, _TRUNCATE, fmt, str.c_str());
#else
        snprintf(fmt, sizeof fmt, "%%-%ds", size);
        snprintf(buffer, size, fmt, str.c_str());
#endif
    }

    const char* data() const
    {
        return buffer;
    }

    int length() const
    {
        return strlen(buffer);
    }

private:
    char buffer[SIZE];
};
}

typedef detail::fixed_str_<> fixed_str;

inline muduo::LogStream& operator<<(muduo::LogStream& s, const fixed_str& str)
{
  s.append(str.data(), str.length());
  return s;
}

}
}

namespace ib_common = ib::common;

#endif
