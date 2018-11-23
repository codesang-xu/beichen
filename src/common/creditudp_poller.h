#ifndef IB_COMMON_CREDITUDP_LOOP_H
#define IB_COMMON_CREDITUDP_LOOP_H

#include "common/creditudp_transport.h"
#include "common/functional.h"
#include "common/thread.h"

namespace ib
{
namespace common
{

class creditudp_poller /*: noncopyable*/
{
public:
typedef function<void (const creditudp_transport_ptr &,
                              int ,//ipccmsg_class
                              int ,//ipccmsg_type
                              char[],
                              int,
                              unsigned long,
                              unsigned short)> message_callback;

public:
    creditudp_poller(const creditudp_transport_ptr &cudp_ptr, const message_callback &on_msg_cb);
public:
    ~creditudp_poller(void);

    void loop();
    void quit();
private:
    bool quit_;
    creditudp_transport_ptr credit_udp_ptr_;
    thread poll_thread_;
    message_callback on_ipccmsg_cb_;
};

}
}

namespace ib_common = ib::common;

#endif
