#ifndef IPCC_SERVICE_ON_OFF_SWITCH_H
#define IPCC_SERVICE_ON_OFF_SWITCH_H

#include "common/functional.h"
#include "common/thread.h"

namespace ib
{
namespace common
{

typedef function<void()> switch_function;

class on_off_switch /*: public noncopyable*/
{
public:
	on_off_switch()
        : on_(false)
    { }

    bool on(switch_function on_func)
    {
        lock_guard<mutex> lock(mutex_);
        if (!on_)
        {
            on_ = true;
            on_func();
            return true;
        }
        else
        {
            return false;
        }
    }

    bool on()
    {
        lock_guard<mutex> lock(mutex_);
        return on_ = true;
    }

    bool off(switch_function off_func)
    {
        lock_guard<mutex> lock(mutex_);
        if (on_)
        {
            on_ = false;
            off_func();
            return true;
        }
        else
        {
            return false;
        }
    }

    bool off()
    {
        lock_guard<mutex> lock(mutex_);
        return on_ = false;
	}

	void exec(switch_function func)
	{
		lock_guard<mutex> lock(mutex_);
		if (on_)
		{
			func();
		}
	}

    bool is_on()
    {
        return on_;
    }

    bool is_off()
    {
        return !on_;
    }

private:
    mutex mutex_;
    bool on_;
};

class no_lock_on_off_switch
{
public:
	no_lock_on_off_switch()
		: on_(false)
	{ }

	bool on(switch_function on_func)
	{
		if (!on_)
		{
			on_ = true;
			on_func();
			return true;
		}
		else
		{
			return false;
		}
	}

	bool on()
	{
		return on_ = true;
	}

	bool off(switch_function off_func)
	{
		if (on_)
		{
			on_ = false;
			off_func();
			return true;
		}
		else
		{
			return false;
		}
	}

	bool off()
	{
		return on_ = false;
	}

	bool is_on()
	{
		return on_;
	}

	bool is_off()
	{
		return !on_;
	}

private:
	bool on_;
};
}
}

namespace ib_common = ib::common;


#endif
