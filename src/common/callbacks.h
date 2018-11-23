#ifndef IPCC_SERVICE_CALLBACKS_H
#define IPCC_SERVICE_CALLBACKS_H

#include "common/functional.h"
#include "common/transport_client.h"

namespace ib
{
namespace common
{
typedef function<void()> timer_call_back;

class creditudp_transport;

typedef function<void (transport_client_ptr,
    int,//ipccmsg_class
    int,//ipccmsg_type
    char[],
    int,
    unsigned long,
    unsigned short)> message_callback;

}
}

namespace ib_common = ib::common;

#endif