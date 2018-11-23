#ifndef IB_COMMON_CREDITUDP_TRANSPORT_H
#define IB_COMMON_CREDITUDP_TRANSPORT_H

#include <string>
#include <set>

#include "common/functional.h"
#include "common/ipcc_message.h"
#include "common/memory.h"
#ifndef NOPUBLIC
#include "public/CreditUDP.h"
#else
#include "CreditUDP.h"
#endif

namespace ib
{
namespace common
{

class creditudp_transport;
typedef shared_ptr<creditudp_transport> creditudp_transport_ptr;

class msglog_filter
{
public:
#define ANY  -1
	class item
	{
	public:
		item()
			: msg_class_(-2), msg_type_(-2)
		{}

		item(int msg_class, int msg_type)
			: msg_class_(msg_class), msg_type_(msg_type)
		{}

		~item() {}

		bool operator < (const item & other) const
		{
			return (
				(msg_class_ < other.msg_class_) ||
				(msg_class_ == other.msg_class_ && msg_type_ < other.msg_type_)
				);
		}
	private:
		int msg_class_;
		int msg_type_;
	};

public:
	msglog_filter() 
	{};

	~msglog_filter() {};

	msglog_filter* disable(int32_t msg_class, int32_t msg_type)
	{
		disabled_.insert(item(msg_class, msg_type));
		return this;
	}

	msglog_filter* disable_class(int32_t msg_class)
	{
		return disable(msg_class, ANY);
	}

	void disable_all()
	{
		disabled_.insert(item(ANY, ANY));
	}

	bool filter(int32_t msg_class, int32_t msg_type)
	{
		return (
			(disabled_.find(item(ANY, ANY)) == disabled_.end()) &&
			(disabled_.find(item(msg_class, ANY)) == disabled_.end()) &&
			(disabled_.find(item(msg_class, msg_type)) == disabled_.end())
			);
		/*
		return !((disabled_.find(item(ANY, ANY)) != disabled_.end()) ||
			(disabled_.find(item(msg_class, ANY)) != disabled_.end()) ||
			(disabled_.find(item(msg_class, msg_type)) != disabled_.end()))
		*/
 
	}

	bool empty()
	{
		return disabled_.empty();
	}

	void add(msglog_filter filter)
	{
		for (std::set<item>::iterator it = filter.disabled_.begin(); it != filter.disabled_.end(); it++)
		{
			disabled_.insert(*it);
		}
	}
private:
	std::set<item> disabled_;
};

typedef function<void(string)> make_log_function;
class creditudp_transport
{
public:
	creditudp_transport(string local_port, string tname, bool use_credit = true);
	creditudp_transport(string local_port, string tname, make_log_function makelogfunc, bool use_credit = true);
	~creditudp_transport(void);
    CCreditUDP& get_socket();

	void add_filter(const msglog_filter & filter)
	{
		logfilter_.add(filter);
	}

	bool send_ipccmsg(SIPCCMessage& ipccmsg,
          unsigned long  dest_ip,    //ÍøÂçÐò
          unsigned short dest_port,  //ÍøÂçÐò
		  string file = "", int line = 0,
          bool refuse_proxy = false); 

	bool send_ipccmsg(shared_ptr<SIPCCMessage> ipccmsg,
		unsigned long  dest_ip,    //ÍøÂçÐò
		unsigned short dest_port,  //ÍøÂçÐò
		string file = "", int line = 0,
		bool refuse_proxy = false);

	bool send(char *data, int size,
          unsigned long  dest_ip,    //ÍøÂçÐò
          unsigned short dest_port,  //ÍøÂçÐò
		  bool refuse_proxy = false);

    // ip,portÎªÍøÂçÐòµÄÖµ
	int get_ipccmsg(char*& buffer, int buffer_size, int &msg_class, int &msg_type, unsigned long &ip, unsigned short &port, string &error, int defer_time=5000);

    ////@return ÍøÂçÐò
    //int get_nbo_local_port()
    //{
    //    return credit_udp_ptr_->GetLocalPort_NBO();
    //}

    //void set_proxy(uint32_t proxy_ip, uint16_t proxy_port)
    //{
    //	proxy_ip_ = proxy_ip;
    //	proxy_port_ = proxy_port;
    //}

    //bool is_using_proxy()
    //{
    //    return proxy_port_ > 0;
    //}

public:
	string name_;

private:
	void default_make_log(string logmsg);

private:
    scoped_ptr<CCreditUDP> credit_udp_ptr_;
	uint32_t proxy_ip_;
	uint16_t proxy_port_;

	msglog_filter logfilter_;
	make_log_function make_log_func_;

	const static int send_buf_size;
	const static int receive_buf_size;
	const static int CREDITUDP_BUFFER_SIZE;
};


}
}

namespace ib_common = ib::common;

#endif
