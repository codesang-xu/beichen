#include "RedirectServer.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#ifndef NOPUBLIC
#include "public/ServerInfoDef.h"
#include "public/Spliter.h"
#else
#include "ServerInfoDef.h"
#include "Spliter.h"
#endif

RedirectServer::RedirectServer()
	: m_redirectPort(0)
	, m_invokeID(0)
{
}

void RedirectServer::Init(creditudp_transport_ptr sock, string domain)
{
	m_Sock = sock;
	m_redirectDomain = domain;
	m_redirectPort = htons(ServerInfoDef::Port_RedirectServer);
	m_redirectServerAddr.resolve(domain);
}

string RedirectServer::GetService(
	string queryServiceList,
	int32_t serviceType,
	GetServiceCallBack ck,
	string file,
	int line
	)
{
	if (m_redirectServerAddr.size() > 0)
	{
		SIPCCMsg_Redirect_GetService msg;
		msg.serviceType = serviceType;
		msg.domainName = m_redirectDomain;
		msg.queryServiceList = queryServiceList;
		msg.netPath = boost::lexical_cast<string>(++m_invokeID);

		m_Sock->send_ipccmsg(msg, m_redirectServerAddr.ip(), m_redirectPort, file, line);

		timer_id tid = setGetServiceTimer(m_invokeID);//粗略的定时机制
		{
			boost::mutex::scoped_lock lock(m_cbMutex_);
			m_getServiceCallBack[m_invokeID] = 
				GetServiceInfo(ck, m_redirectServerAddr.get_addr(), tid);
		}

		return msg.netPath;
	}
	return "";
}

bool RedirectServer::On_SIPCCMsg_Redirect_GetServiceConf(const creditudp_transport_ptr & credit_udp_ptr,
	int ipccmsg_class, int ipccmsg_type, SIPCCMessage & ipccmsg,
	unsigned long src_ip, unsigned short src_port)
{
	SIPCCMsg_Redirect_GetServiceConf & msg = 
		dynamic_cast<SIPCCMsg_Redirect_GetServiceConf&>(ipccmsg);

	int invokid = boost::lexical_cast<int>(msg.netPath);
	
	bool found = false;
	GetServiceInfo g;
	{
		boost::mutex::scoped_lock lock(m_cbMutex_);

		map<int, GetServiceInfo>::iterator it =
			m_getServiceCallBack.find(invokid);

		found = (it != m_getServiceCallBack.end());
		if (found)
		{
			g = it->second;
			m_getServiceCallBack.erase(it);
		}
	}

	if (found)
	{
		vector<ServiceInfo> serviceVec;
		CSpliter sp_service(msg.serviceList, ";", ",");
		for (int i = 0; i < sp_service.GetSize(); i++)
		{
			if (sp_service[i].GetSize() > 3)
			{
				serviceVec.push_back(
					ServiceInfo(
					boost::lexical_cast<uint32_t>(sp_service[i][1].c_str()),        //id
					(ESeverType)boost::lexical_cast<int>(sp_service[i][0].c_str()), //server type
					inet_addr(sp_service[i][2].c_str()),                            //ip
					htons(boost::lexical_cast<int>(sp_service[i][3].c_str())        //port
					)));
			}
		}

		vector<RedisDBInfo> redisVec;
		CSpliter sp_redis(msg.redisList, ";", ",");
		for (int i = 0; i < sp_redis.GetSize(); i++)
		{
			if (sp_redis[i].GetSize() > 4)
			{
				redisVec.push_back(
					RedisDBInfo(
					boost::lexical_cast<int>(sp_redis[i][0].c_str()),
					boost::lexical_cast<int>(sp_redis[i][1].c_str()),
					inet_addr(sp_redis[i][2].c_str()),                          //ip
					htons(boost::lexical_cast<int>(sp_redis[i][3].c_str())),    //port
					sp_redis[i][4].c_str()                                      //password
					));         //dbindex
			}
		}

		g.cb(msg.netPath, serviceVec, redisVec);
		stopGetServiceTimer(g.timerID);
	}

	return true;
}

timer_id RedirectServer::setGetServiceTimer(int invokeid)
{
	return timer_service_inst::get_service().run_after(5000,
		boost::bind(&RedirectServer::onGetServiceTimer, this, invokeid));
}
void RedirectServer::stopGetServiceTimer(int timer_id)
{
	if (timer_id != invalid_timer_id)
	{
		timer_service_inst::get_service().cancel(timer_id);
	}
}
void RedirectServer::onGetServiceTimer(int invokeid)
{
	bool found = false;
	GetServiceInfo g;
	{
		boost::mutex::scoped_lock lock(m_cbMutex_);

		map<int, GetServiceInfo>::iterator it =
			m_getServiceCallBack.find(invokeid);

		found = (it != m_getServiceCallBack.end());
		if (found)
		{
			g = it->second;
			m_getServiceCallBack.erase(it);
		}
	}

	if (found)
	{
		if (m_redirectServerAddr.get_addr() == g.redirect_ep)
		{
			m_redirectServerAddr.erase_current();
			if (m_redirectServerAddr.empty())
			{
				m_redirectServerAddr.resolve(m_redirectDomain);
			}
		}
		m_getServiceCallBack.erase(invokeid);
	}
}
