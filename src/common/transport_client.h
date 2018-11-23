#ifndef IB_COMMON_TRANSPORT_CLIENT_H
#define IB_COMMON_TRANSPORT_CLIENT_H

#include "common/creditudp_bundle.h"
#include "common/creditudp_transport.h"
#include "common/ipcc_message.h"

//todo
//#include "common/proxy_support_transport.h"

namespace ib
{
namespace common
{
class proxy_support_tcp_transport;
class transport_client : public enable_shared_from_this<transport_client>
{
private:
    typedef transport_client            self;
	typedef shared_ptr<self>            self_ptr;
    typedef creditudp_bundle_ptr        udp_transport_ptr;
    //typedef proxy_support_tcp_transport        tcp_transport;
    //typedef shared_ptr<tcp_transport>          tcp_transport_ptr;
    //typedef tcp_transport::endpoint_type       endpoint_type;
    //typedef tcp_transport::message_callback    transport_message_callback;
    //typedef tcp_transport::closed_callback     closed_callback;

public:
    typedef function<void (const self_ptr &, int/*msg class*/, int/*msg type*/, 
              char*, int, uint32_t, uint16_t)> message_callback;
    //typedef tcp_transport::connection_callback connection_callback;
    //typedef tcp_transport::closed_callback     closed_callback;

public:
    transport_client(uint16_t local_port, string name, bool use_credit = true, bool create_thread = true);
	transport_client(uint16_t local_port, string name, make_log_function makelogfunc, bool use_credit = true, bool create_thread = true);
	~transport_client();

	void add_filter(const msglog_filter & filter)
	{
		log_filter_.add(filter);
		if (!creditudp_)
		{
			creditudp_->add_filter(filter);
		}
	}

    //not thread safe
    void set_message_callback(message_callback cb);

    //not thread safe
    //void set_closed_callback(closed_callback cb);

    void set_udp_proxy(uint32_t proxy_ip, uint16_t proxy_port);

//todo
//    void async_connect_tcp_proxy(uint32_t proxy_ip, uint16_t proxy_port,
//            const connection_callback & conn_callback);

	bool send_ipccmsg(shared_ptr<SIPCCMessage> ipccmsg,
          unsigned long  dest_ip,    
          unsigned short dest_port,  
		  string file = "", int line = 0,
          bool refuse_proxy = false);

	bool send(char *data, int size,
          unsigned long  dest_ip,   
          unsigned short dest_port, 
          bool refuse_proxy = false);

    void close()
    {
//todo
//        if (tcp_)
//        {
//            tcp_->close();
//        }
    }

    //unsigned short nbo_udp_local_port()
    //{
    //    return udp_.get_creditudp_ptr()->get_nbo_local_port();
    //}

    //unsigned short hbo_udp_local_port()
    //{
    //    return htons(nbo_udp_local_port());
    //}

    creditudp_transport_ptr get_udp_transport()
    {
        return creditudp_;
    }

private:
    void on_credit_udp_recved(creditudp_transport_ptr const & cudp,
            int ipccmsg_class,
            int ipccmsg_type,
            char msg_buf[],
            int buf_len,
            unsigned long src_ip,
            unsigned short src_port);
//todo
//    void on_proxy_connected(const system::error_code & error);
//
//    void on_tcp_proxy_closed(const system::error_code & error);
//
//    void on_tcp_proxy_recved(char * buf, size_t buf_size, int msg_class, int msg_type,
//            uint32_t src_ip, uint16_t src_port);
    
public:
    string name_;

private:
    uint16_t local_port_;

    creditudp_bundle_ptr  udp_;
    creditudp_transport_ptr creditudp_;

    //tcp_transport_ptr tcp_;

    mutex msg_cb_mutex_;
    message_callback message_callback_;

	msglog_filter log_filter_;

//todo
//    closed_callback closed_callback_;
//    connection_callback connection_callback_;

    //io_service io_service_;
    //io_service::work io_service_work_;
    //thread io_thread_;
};

typedef shared_ptr<transport_client> transport_client_ptr;

}
}

namespace ib_common = ib::common;

#endif
