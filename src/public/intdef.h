#ifndef __PUBLIC_INTDEF__H__

#ifndef PROJECT_INT_DEF
#ifdef CSTD_INT
#include <cstdint>
#else
#include <boost/cstdint.hpp>
using boost::int8_t;
using boost::int16_t;
using boost::int32_t;
using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::int64_t;
using boost::uint64_t;

#endif
#else
typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;

#if defined(_MSC_VER) 
#if _MSC_VER < 1300
typedef __int64            int64_t;
typedef unsigned __int64   uint64_t;
#else
typedef long long           int64_t;
typedef unsigned long long  uint64_t;
#endif
#else
typedef long int           int64_t;
typedef unsigned long int  uint64_t;
#endif
#endif

#endif