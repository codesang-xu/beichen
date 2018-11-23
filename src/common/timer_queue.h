#ifndef IPCC_SERVICE_TIMER_QUEUE_H
#define IPCC_SERVICE_TIMER_QUEUE_H

#include <set>
#include <map>

#include <boost/asio.hpp>

#include "common/callbacks.h"
#include "common/thread.h"
#include "common/timer.h"
#include "common/timer_id.h"

using namespace boost::asio;

namespace ib
{
namespace timer
{

class timer;
class timer_queue : boost::noncopyable
{
public:
    timer_queue(io_service *arg_service)
        : timer_service_(*arg_service),
          mutex_(),
          timers_(),
          true_timer_(*arg_service)
    { }
    
    ~timer_queue()
    { }

    timer_id add_timer(const timer_call_back& cb,
                hr_time_point when,
                int interval)
    {
	    hr_time_point expire_time = when + ms(static_cast<ms::rep>(interval));
        timer* timer_ = new timer(cb, expire_time);
 
        timer_service_.post(bind(&timer_queue::add_timer_callback, 
            this, timer_));

        return static_cast<timer_id>(timer_->sequence());
    }

	void cancel(timer_id tid)
    {
        timer_service_.post(bind(&timer_queue::cancel_callback, this, tid));
    }

private:
    typedef std::pair<hr_time_point, timer*> entry;
    typedef std::set<entry> timer_list;
	typedef std::map<int64_t, timer*> active_timer_set;
	typedef active_timer_set::value_type active_timer;

private:
    void cancel_callback(timer_id tid);
    void add_timer_callback(timer* arg_timer);

    bool insert(timer* arg_timer);
    void reset_true_timer(hr_time_point when);
	void reset_true_timer_nolock(hr_time_point when);

    void timer_expired_handler(const boost::system::error_code& ec);

	std::vector<entry> get_expired(hr_time_point now);
	void reset(const std::vector<entry>& expired, hr_time_point now);

private:
    io_service &timer_service_;

    mutex mutex_;
    timer_list timers_;

	//for cancel()
	active_timer_set active_timers_;

	typedef basic_waitable_timer<hr_clock> hr_timer;
	hr_timer true_timer_;
};

}
}

namespace ib_common = ib::common;

#endif