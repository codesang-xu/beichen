#include "common/creditudp_poller.h"

#include <string>

#include "common/ipcc_message.h"

namespace ib
{
namespace common
{

creditudp_poller::creditudp_poller(const creditudp_transport_ptr &cudp_ptr, const message_callback &on_msg_cb)
    : quit_(false),
      credit_udp_ptr_(cudp_ptr),
      on_ipccmsg_cb_(on_msg_cb),
      poll_thread_(bind(&creditudp_poller::loop, this))
{
}

creditudp_poller::~creditudp_poller(void)
{
    quit();
}

void creditudp_poller::loop(void)
{
    assert(on_ipccmsg_cb_);

    const int MAX_MSG_SIZE = 1024*4;
    while(!quit_)
	{
		char *msg_ptr = NULL;
		int  msg_size = MAX_MSG_SIZE;
        int  msg_class;
        int  msg_type;
		unsigned long src_ip;
		unsigned short src_port;
	    string error;
		
		int received_size = credit_udp_ptr_->get_ipccmsg(msg_ptr, 
                msg_size, 
                msg_class, 
                msg_type, 
                src_ip, 
                src_port, 
                error);
		
        if (received_size > 0)
        {
        	//TODO important!!!!! may be rash
            on_ipccmsg_cb_(credit_udp_ptr_, 
                msg_class, 
                msg_type, 
                msg_ptr, 
                received_size, 
                src_ip, 
                src_port);
        }
	}
}

void creditudp_poller::quit(void)
{
    quit_ = true;
    poll_thread_.join();
}

}
}
