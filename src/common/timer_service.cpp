#include "timer_service.h"

#include "common/callbacks.h"
#include "common/timer.h"
#include "common/timer_id.h"
#include "common/timer_queue.h"

namespace ib
{
namespace timer
{

/**
@delay ms
**/
timer_id timer_service::run_after(int delay, const timer_call_back& cb)
{
  return timer_queue_->add_timer(cb, hr_time_point::clock::now(), delay);
}

void timer_service::cancel(timer_id timerId)
{
	timer_queue_->cancel(timerId);
}

}
}
