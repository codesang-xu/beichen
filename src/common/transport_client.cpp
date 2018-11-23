#include "common/transport_client.h"

namespace ib
{
namespace common
{

	transport_client::transport_client(uint16_t local_port, string name, bool use_credit, bool create_thread)
		: name_(name),
		local_port_(local_port)
		//io_service_(),
		//io_service_work_(io_service_),
		//io_thread_(bind(&io_service::run, &io_service_)),
		//udp_(boost::lexical_cast<string>(local_port),
		//	bind(&self::on_credit_udp_recved,
		//		this, _1, _2, _3, _4, _5, _6, _7), name, use_credit)
	{
        if (create_thread){
		    udp_.reset(new creditudp_bundle(boost::lexical_cast<string>(local_port),
			    bind(&self::on_credit_udp_recved,
				    this, _1, _2, _3, _4, _5, _6, _7), name, use_credit));
            creditudp_ = udp_->get_creditudp_ptr();
        } else {
            creditudp_.reset(
                new creditudp_transport(boost::lexical_cast<string>(local_port), name, use_credit));
        }

		//LOG_DEBUG << "transport_client constructe, local_port_:" << local_port;
	}

	transport_client::transport_client(uint16_t local_port, string name, make_log_function makelogfunc, bool use_credit, bool create_thread)
		: name_(name),
		local_port_(local_port)
		//io_service_(),
		//io_service_work_(io_service_),
		//io_thread_(bind(&io_service::run, &io_service_)),
		//udp_(boost::lexical_cast<string>(local_port),
		//    bind(&self::on_credit_udp_recved,
		//        this, _1, _2, _3, _4, _5, _6, _7), makelogfunc, name, use_credit)
	{
        if (create_thread){
		    udp_.reset(new creditudp_bundle(boost::lexical_cast<string>(local_port),
			    bind(&self::on_credit_udp_recved,
				    this, _1, _2, _3, _4, _5, _6, _7), makelogfunc, name, use_credit));
            creditudp_ = udp_->get_creditudp_ptr();
        } else {
            creditudp_.reset(
                new creditudp_transport(boost::lexical_cast<string>(local_port), name, makelogfunc, use_credit));
        }

		//LOG_DEBUG << "transport_client constructe, local_port_:" << local_port;
	}

	transport_client::~transport_client()
	{
		close();
		//io_service_.stop();
		//io_thread_.join();
		//LOG_DEBUG << "transport_client destructe";
	}

	void transport_client::set_message_callback(message_callback cb)
	{
		boost::mutex::scoped_lock l(msg_cb_mutex_);
		message_callback_ = cb;
	}

	////not thread safe
	//void transport_client::set_closed_callback(closed_callback cb)
	//{
	//	scoped_lock l(msgcb_mutex_);
	//	closed_callback_ = cb;
	//}

	//void transport_client::set_udp_proxy(uint32_t proxy_ip, uint16_t proxy_port)
	//{
	//	udp_.get_creditudp_ptr()->set_proxy(proxy_ip, proxy_port);
	//}

	//void transport_client::async_connect_tcp_proxy(uint32_t proxy_ip, uint16_t proxy_port,
	//	const connection_callback & conn_callback)
	//{//not 100% safe
	//	connection_callback_ = conn_callback;
	//	tcp_->async_connect(endpoint_type(ip::address_v4(ntohl(proxy_ip)), ntohs(proxy_port)),
	//		bind(&self::on_proxy_connected, shared_from_this(), _1));
	//}

	bool transport_client::send_ipccmsg(boost::shared_ptr<SIPCCMessage> ipccmsg,
		unsigned long  dest_ip,    //网络序
		unsigned short dest_port,  //网络序
		string file, int line,
		bool refuse_proxy)
	{
		//if (!refuse_proxy && tcp_)
		//{
		//	tcp_->send_ipccmsg(ipccmsg, dest_ip, dest_port);
		//}
		//else
		{
			return creditudp_->send_ipccmsg(ipccmsg,
				dest_ip, dest_port, file, line, refuse_proxy);
		}
	}

	bool transport_client::send(char *data, int size,
		unsigned long  dest_ip,    //网络序
		unsigned short dest_port,  //网络序
		bool refuse_proxy)
	{
		return creditudp_->send(data, size, dest_ip, dest_port, refuse_proxy);
	}

	void transport_client::on_credit_udp_recved(creditudp_transport_ptr const & cudp,
		int ipccmsg_class,
		int ipccmsg_type,
		char msg_buf[],
		int buf_len,
		unsigned long src_ip,
		unsigned short src_port)
	{
		if (message_callback_)
		{
			message_callback_(shared_from_this(), ipccmsg_class, ipccmsg_type,
				msg_buf, buf_len, src_ip, src_port);
		}
	}

	//void transport_client::on_proxy_connected(const boost::system::error_code & error)
	//{
	//	connection_callback_(error);
	//	connection_callback_ = connection_callback();
	//	if (!error)
	//	{
	//		//start receive
	//		tcp_->set_closed_callback(bind(&self::on_tcp_proxy_closed,
	//			shared_from_this(), _1));
	//		tcp_->async_receive(bind(&self::on_tcp_proxy_recved,
	//			shared_from_this(), _1, _2, _3, _4, _5, _6));
	//	}
	//}

	//void transport_client::on_tcp_proxy_closed(const boost::system::error_code & error)
	//{
	//	tcp_.reset();
	//	closed_callback_(error);
	//	//警告：此处没有释放closed_callback_，注意可能引起的内存泄露
	//	//closed_callback_ = closed_callback();
	//}

	//void transport_client::on_tcp_proxy_recved(char * buf, size_t buf_size, int msg_class, int msg_type,
	//	uint32_t src_ip, uint16_t src_port)
	//{
	//	if (message_callback_)
	//	{
	//		message_callback_(shared_from_this(), msg_class, msg_type,
	//			buf, buf_size, src_ip, src_port);
	//	}
	//}


}
}

