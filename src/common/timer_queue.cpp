#include "timer_queue.h"

namespace ib
{
	namespace timer
	{

		void timer_queue::add_timer_callback(timer* arg_timer)
		{
			if (insert(arg_timer))
			{
				reset_true_timer(arg_timer->expiration());
			}
		}

		bool timer_queue::insert(timer* arg_timer)
		{
			bool earliestChanged = false;
			hr_time_point when = arg_timer->expiration();

			mutex::scoped_lock lock(mutex_);
			timer_list::iterator it = timers_.begin();
			if (it == timers_.end() || when < it->first)
			{
				earliestChanged = true;
			}
	{
		std::pair<timer_list::iterator, bool> result
			= timers_.insert(entry(when, arg_timer));
		assert(result.second); (void)result;
	}
	{
		std::pair<active_timer_set::iterator, bool> result
			= active_timers_.insert(active_timer(arg_timer->sequence(), arg_timer));
		assert(result.second); (void)result;
	}

	//M_LOG_TRACE << "insert timer to timer queue, id=" << arg_timer->sequence() << ", earliestChanged=" << earliestChanged;

	return earliestChanged;
		}

		void timer_queue::reset_true_timer(hr_time_point when)
		{
			//M_LOG_TRACE << "reset the true timer";

			mutex::scoped_lock lock(mutex_);
			reset_true_timer_nolock(when);
		}

		void timer_queue::reset_true_timer_nolock(hr_time_point when)
		{
			//M_LOG_TRACE << "reset the true timer";

			true_timer_.expires_at(when);
			true_timer_.async_wait(bind(&timer_queue::timer_expired_handler, this, boost::asio::placeholders::error));
		}

		void timer_queue::timer_expired_handler(
			const boost::system::error_code& ec)
		{
			if (ec == error::operation_aborted)
			{
				//M_LOG_TRACE << "the true timer cancled";
				return;
			}

			//M_LOG_TRACE << "the true timer fired";

			hr_time_point now = hr_time_point::clock::now();

			std::vector<entry> expired = get_expired(now);

			// safe to callback outside critical section
			for (std::vector<entry>::iterator it = expired.begin();
				it != expired.end(); ++it)
			{
				it->second->run();
			}

			reset(expired, now);
		}

		std::vector<timer_queue::entry> timer_queue::get_expired(hr_time_point now)
		{
			mutex::scoped_lock lock(mutex_);
			assert(timers_.size() == active_timers_.size());
			std::vector<entry> expired;
			entry sentry(now, reinterpret_cast<timer*>(0));
			timer_list::iterator end = timers_.lower_bound(sentry);
			assert(end == timers_.end() || now < end->first);
			std::copy(timers_.begin(), end, back_inserter(expired));
			timers_.erase(timers_.begin(), end);

			for (std::vector<entry>::iterator it = expired.begin();
				it != expired.end(); ++it)
			{
				size_t n = active_timers_.erase(it->second->sequence());
				assert(n == 1); (void)n;
			}

			assert(timers_.size() == active_timers_.size());

			return expired;
		}

		void timer_queue::reset(const std::vector<entry>& expired, hr_time_point now)
		{
			for (std::vector<entry>::const_iterator it = expired.begin();
				it != expired.end(); ++it)
			{
				delete it->second; // FIXME: no delete please
			}

	{
		mutex::scoped_lock lock(mutex_);

		if (!timers_.empty())
		{
			reset_true_timer_nolock(timers_.begin()->second->expiration());
		}
		//else
		//{
		//    M_LOG_TRACE << "timer queue empty";
		//}
	}
		}

		void timer_queue::cancel_callback(timer_id tid)
		{
			mutex::scoped_lock lock(mutex_);
			assert(timers_.size() == active_timers_.size());
			active_timer_set::iterator it = active_timers_.find(tid);
			if (it != active_timers_.end())
			{
				size_t n = timers_.erase(entry(it->second->expiration(), it->second));
				assert(n == 1); (void)n;
				delete it->second; // FIXME: no delete please
				active_timers_.erase(it);
			}
			assert(timers_.size() == active_timers_.size());
}

}
}
