#ifndef IPCC_SERVICE_TIMER_ID_H
#define IPCC_SERVICE_TIMER_ID_H

namespace ib
{
namespace timer
{

typedef boost::int64_t timer_id;
const static timer_id invalid_timer_id = -1;

}
}

namespace ib_common = ib::common;

#endif