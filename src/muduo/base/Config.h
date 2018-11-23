#ifndef MUDUO_BASE_CONFIG_H
#define MUDUO_BASE_CONFIG_H

#define MUDUO_STD_STRING

#ifdef _WIN32
    #define __builtin_expect( EXP, C ) (EXP)
    #define __thread  __declspec(thread)
    #define fwrite_unlocked _fwrite_nolock
    #define setbuffer( stream, buf, bufsize ) setvbuf( (stream), (buf), (bufsize)?_IOFBF:_IONBF, (bufsize) )
    #define gmtime_r( arg_tm, arg_time_t )  gmtime_s( (arg_time_t), (arg_tm) )
    #define localtime_r( arg_tm, arg_time_t )  localtime_s( (arg_time_t), (arg_tm) )
    #define strerror_r( errno, errno_buf, errno_buflen )  strerror_s(  (errno_buf), (errno_buflen), (errno) )
#endif

#endif  // MUDUO_BASE_CONFIG_H
