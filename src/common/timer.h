#pragma once

#include <boost/atomic.hpp>

#include "common/callbacks.h"

namespace ib
{
namespace timer
{
using namespace ib::common;

	typedef boost::chrono::high_resolution_clock hr_clock;
	typedef hr_clock::time_point hr_time_point;
	typedef boost::chrono::milliseconds ms;

	class timer : boost::noncopyable
	{
	public:
		timer(const timer_call_back& cb, hr_time_point when)
			: callback_(cb),
			expiration_(when),
			sequence_(s_numCreated_.fetch_add(1))
		{}

		void run() const
		{
			callback_();
		}

		int64_t sequence() const { return sequence_; }
		void restart();
		hr_time_point expiration() const  { return expiration_; }
		static int64_t numCreated() { return s_numCreated_.load(); }

		~timer(void) { }

	private:

		const timer_call_back callback_;
		hr_time_point expiration_;
		const int64_t sequence_;

		static boost::atomic_ulong s_numCreated_;
};

}
}

namespace ib_common = ib::common;
