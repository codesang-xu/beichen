#include "common/ipccmsg_loop.h"

//#include <muduo/base/Logging.h>

//#include "common/fixed_str.h"
#include "common/ipccmsg_helper.h"
#include "common/transport_client.h"


#ifndef NOPUBLIC
#include "public/CreditUDP.h"
#else
#include "CreditUDP.h"
#endif

namespace ib
{
namespace common
{
ipccmsg_loop::ipccmsg_loop()
    : mutex_(),
      my_thread_id_(this_thread::get_id()),
      looping_(false)
{
	//DEBUG("ipccmsg_loop constructed");
}

ipccmsg_loop::~ipccmsg_loop()
{
    //DEBUG ("ipccmsg_loop destructed");
}

void ipccmsg_loop::receive_ipccmsg_callback(const transport_client_ptr & credit_udp_ptr_,
            int ipccmsg_class,
            int ipccmsg_type,
			char *msg_buf,
            int buf_len,
            unsigned long src_ip,
            unsigned short src_port)
{
    if (message_callback_)
    {
        funcs_.put(bind<void>(ipccmsg_callback_wrapper(msg_buf, buf_len),
            message_callback_,
            credit_udp_ptr_,
            ipccmsg_class,
            ipccmsg_type,
            src_ip,
            src_port));
    }
    else
    {
    	//WARN("discard message");
    }
}

void ipccmsg_loop::loop()
{
    assert(!looping_);
//TODO !important    assert_in_loop_thread();
    looping_ = true;
    functor func;
    while(func = funcs_.take())
    {
        if (func){
            func();
        }
        else
        {
            break;//quit
        }
    }
    looping_ = false;
}

void ipccmsg_loop::stop()
{
	funcs_.put(functor());
}

void ipccmsg_loop::run_in_loop(const functor& cb)
{
    if (is_in_loop_thread())
    {
        cb();
    }
    else
    {
        queue_in_loop(cb);
    }
}

void ipccmsg_loop::queue_in_loop(const functor& cb)
{
    funcs_.put(cb);
}

void ipccmsg_loop::abort_not_in_loop_thread()
{
    //
}

}
}
