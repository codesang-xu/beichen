#ifndef IPCC_SERVICE_TIMER_SERVICE_H
#define IPCC_SERVICE_TIMER_SERVICE_H

#include <cassert>

#include <boost/thread/thread.hpp>

#include "common/callbacks.h"
#include "common/timer_id.h"
#include "common/timer_queue.h"

namespace ib
{
namespace timer
{

class timer_service
{
public:
    timer_service(bool create_thread = true)
        :service_(), 
         timer_queue_(new timer_queue(&service_)),
         running(false),
         create_thread_(create_thread)
    { }

	~timer_service()
	{
		stop();
	}

    timer_id run_after(int delay, const timer_call_back& cb);
    void cancel(timer_id timerId);

    io_service& get_service()
    {
        return service_;
    };

    void run()
    {
        if (create_thread_)
        {
            assert(!running);
            running = true;
            work_.reset(new io_service::work(service_));
            thread_ptr_.reset(new thread(bind(&io_service::run, &service_)));
        }
        else
        {
            service_.run();
        }
    };

    void stop()
    {
        if (running && thread_ptr_)
        {
            thread_ptr_->join();
            thread_ptr_.reset();
        }
        running = false;
        service_.stop();
    };

private:
    bool create_thread_;
    bool running;
    io_service service_;
    scoped_ptr<io_service::work> work_;
    scoped_ptr<timer_queue> timer_queue_;
    scoped_ptr<thread> thread_ptr_;
};

class timer_service_inst
{
private:
	timer_service_inst()
	{
		timer_service_.run();
	}
	~timer_service_inst()
	{
		timer_service_.stop();
	}

public:
	static timer_service& get_service()
	{
		static timer_service_inst inst_;
		return inst_.timer_service_;
	}

private:
	timer_service timer_service_;
};

}
}

namespace ib_common = ib::common;

#endif