#ifndef REDIRECT_SERVER_H
#define REDIRECT_SERVER_H

#include <string>
using namespace std;

#include <boost/chrono.hpp>
using namespace boost::chrono;

#include "common/creditudp_transport.h"
#include "common/server_addr_pack.h"
#include "common/timer_service.h"
#ifndef NOPUBLIC
#include "public/CreditUDP.h"
#include "public/IPCCMessage.h"
#else
#include "CreditUDP.h"
#include "IPCCMessage.h"
#endif
#include "ServiceInfo.h"

using namespace ib_common;
using namespace ib::timer;

class RedirectServer
{
public:
	RedirectServer();

	~RedirectServer(){};

public:
	void Init(creditudp_transport_ptr sock, string domain);

	typedef boost::function<void(string, vector<ServiceInfo> &, vector<RedisDBInfo> &)> GetServiceCallBack;

	string GetService(
		string queryServiceList,
		int32_t serviceType,
		GetServiceCallBack ck,
		string file = "",
		int line = 0
		);

	bool On_SIPCCMsg_Redirect_GetServiceConf(const creditudp_transport_ptr & credit_udp_ptr,
		int ipccmsg_class, int ipccmsg_type, SIPCCMessage & ipccmsg,
		unsigned long src_ip, unsigned short src_port);

private:
	void send_SIPCCMsg_Redirect_GetService(
		const creditudp_transport_ptr & credit_udp_ptr, uint32_t dest_ip, uint16_t dest_port,
		int32_t serviceType,
		string domainName,
		string queryServiceList,
		string  netpath
		);

	timer_id setGetServiceTimer(int invokeid);
	void stopGetServiceTimer(int timer_id);
	void onGetServiceTimer(int invokeid);

private:
	class GetServiceInfo
	{
	public:
		GetServiceInfo()
		{}

		GetServiceInfo(const GetServiceCallBack &gscb,
			const endpoint &ep, timer_id timerid)
			:cb(gscb),
			redirect_ep(ep),
			timerID(timerid)
		{}

		GetServiceCallBack cb;
		endpoint redirect_ep;
		timer_id timerID;
	};

	string m_redirectDomain;
	uint16_t m_redirectPort;
	server_addr_pack m_redirectServerAddr;

	creditudp_transport_ptr m_Sock;

	//mutex m_timerMutex;
	//timer_id m_getServiceTimer;

	boost::mutex m_cbMutex_;
	map<int, GetServiceInfo> m_getServiceCallBack;

	int m_invokeID;
};

#endif
