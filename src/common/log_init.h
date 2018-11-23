#pragma once

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/LogFile.h>
#include <muduo/base/Logging.h>

#include "common/util.h"

namespace ib
{
namespace common
{

using namespace muduo;

class async_log_init
{
public:
    async_log_init(void)
        : running_(false)
    {};
    
    ~async_log_init(void)
    {
        asyncLog->stop();
    }

    void init(muduo::Logger::LogLevel ll, string name)
    {
        Logger::setLogLevel(ll);
	    asyncLog = new muduo::AsyncLogging(get_app_root()+"/log", name, kRollSize);
        Logger::setOutput(&async_log_init::output);
	    Logger::setFlush(&async_log_init::flush);
    }

	void set_log_level(muduo::Logger::LogLevel ll)
	{
		Logger::setLogLevel(ll);
	};

    void start()
    {
        running_ = true;
        asyncLog->start();
    }

    void stop()
    {
        if (running_)
            asyncLog->stop();
    }

private:
    static void output(const char* msg, int len)
    {
        asyncLog->append(msg, len);
		flush();
	}

    static void flush()
    {
	    fflush(NULL);
    }

private:
    static muduo::AsyncLogging * asyncLog;
    bool running_;
    const static int kRollSize = 50*1000*1000;
};

class sync_log_init
{
public:
    sync_log_init(void) {};

    ~sync_log_init(void) {};

	void init(muduo::Logger::LogLevel ll, string name)
    {
        Logger::setLogLevel(ll);
		logFile = new muduo::LogFile(get_app_root() + "/log", name, kRollSize);
        Logger::setOutput(&sync_log_init::output);
	    Logger::setFlush(&sync_log_init::flush);
    }

private:
    static void output(const char* msg, int len)
    {
	    logFile->append(msg, len);
	    logFile->flush();
    }

    static void flush()
    {
	    fflush(NULL);
    }

private:
    static muduo::LogFile *logFile;
    const static int kRollSize = 50*1000*1000;
};

}
}

namespace ib_common = ib::common;

