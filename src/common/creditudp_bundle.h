#ifndef IB_COMMON_CREDITUDP_BUNDLE_H
#define IB_COMMON_CREDITUDP_BUNDLE_H

#include "common/creditudp_poller.h"
#include "common/creditudp_transport.h"


namespace ib
{
namespace common
{

class creditudp_bundle /*: noncopyable*/
{
public:
    typedef creditudp_poller::message_callback message_callback;

public:
    creditudp_bundle(string local_port, const message_callback &on_msg_cb, string tname, bool use_credit = true)
        : creditudp_(new creditudp_transport(local_port, tname, use_credit)),
          poller_(creditudp_, on_msg_cb)
    { }

	creditudp_bundle(string local_port, const message_callback &on_msg_cb, make_log_function makelogfunc, 
		string tname, bool use_credit = true)
		: creditudp_(new creditudp_transport(local_port, tname, makelogfunc, use_credit)),
          poller_(creditudp_, on_msg_cb)
    { }

    ~creditudp_bundle()
    { }

    creditudp_transport_ptr get_creditudp_ptr()
    {
        return creditudp_;
    }

private:
    creditudp_transport_ptr creditudp_;
    creditudp_poller poller_;
};

typedef shared_ptr<creditudp_bundle> creditudp_bundle_ptr;

}
}

namespace ib_common = ib::common;

#endif
