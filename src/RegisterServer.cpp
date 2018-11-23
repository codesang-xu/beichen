#include "RegisterServer.h"

#include <vector>
#include <algorithm> 
#include "common/ipccmsg_helper.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#ifndef NOPUBLIC

#include "public/Spliter.h"
#else

#include "Spliter.h"
#endif
using namespace ib_common;

const int RegisterServer::HEARTBEAT_INTERVAL = 3000;
const int RegisterServer::GET_HEARTBEAT_TIMEOUT = 6000;
const int RegisterServer::REGISTER_TIMEOUT = 10000;

RegisterServer::RegisterServer()
{
	m_registerServerA.reset();
	m_registerServerB.reset();
	m_isUsingHeartbeatA = false;
	m_isUsingHeartbeatB = false;
	m_lastSendHeartbeatTime = system_clock::now();
	m_registering = false;
	m_registerTimer = invalid_timer_id;
	m_getingHeartbeat = false;
	m_getHeartBeatTimer = invalid_timer_id;
	m_checkGetServiceTimer = invalid_timer_id;
	m_GetServiceInvokeID = 0;
	m_redirectServer = NULL;
}

RegisterServer::~RegisterServer()
{ 
	RegisterEnd();
	GetHeartBeatServerEnd();
	stopGetServiceTimeout();
}

void RegisterServer::Init(creditudp_transport_ptr sock, string serName, int serType, 
    unsigned int localIp, unsigned short localPort, makelogfunc logfunc)
{
	m_Sock = sock;
    m_logfunc = logfunc;

	m_myself.serviceName = serName;
	m_myself.serviceType = (ESeverType)serType;
	m_myself.ip = localIp;
	m_myself.port = localPort;

	startGetServiceTimeout();
}

void RegisterServer::StartRegister(RedirectServer* redirectServer)
{
	m_redirectServer = redirectServer;
	StartRegister();
}

void RegisterServer::StartRegister(
    unsigned int registerServerAIp, unsigned short registerServerAPort, 
    unsigned int registerServerBIp, unsigned short registerServerBPort)
{
	m_registerServerA.ip = registerServerAIp;
	m_registerServerA.port = registerServerAPort;

	m_registerServerB.ip = registerServerBIp;
	m_registerServerB.port = registerServerBPort;

	StartRegister();
}

bool RegisterServer::DisposeMsg(char *pmsg, int msgsize, 
    unsigned long srcip, unsigned short srcport)
{
	CIPCCMsgExchanger msgExchanger;
	__EIPCCMsgClass msgClass;
	__EIPCCMsgType msgType;
	msgExchanger.GetMsgHeader(pmsg, msgsize, msgClass, msgType);

    bool ret = false;
	switch (msgType)
	{
	case Type_OneKeySwitch_ServiceRegisterConf://注册中心回复
	{
		SIPCCMsg_OneKeySwitch_ServiceRegisterConf ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage);
        m_logfunc(to_str(ipccMessage), get_file_name(__FILE__), __LINE__, 0);
        ret = On_SIPCCMsg_OneKeySwitch_ServiceRegisterConf(m_Sock, msgClass, msgType, ipccMessage, srcip, srcport);
		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_OneKeySwitch_ServiceConfirmConf://服务问答回复确认
	{
		SIPCCMsg_OneKeySwitch_ServiceConfirmConf ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage);
        m_logfunc(to_str(ipccMessage), get_file_name(__FILE__), __LINE__, 0);
        ret = On_SIPCCMsg_OneKeySwitch_ServiceConfirmConf(m_Sock, msgClass, msgType, ipccMessage, srcip, srcport);
		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_OneKeySwitch_GetHeartBeatConf://取心跳服务回复
	{
		SIPCCMsg_OneKeySwitch_GetHeartBeatConf ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage);
        m_logfunc(to_str(ipccMessage), get_file_name(__FILE__), __LINE__, 0);
        ret = On_SIPCCMsg_OneKeySwitch_GetHeartBeatConf(m_Sock, msgClass, msgType, ipccMessage, srcip, srcport);
		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_OneKeySwitch_ServiceConfirm://其它服务的询问
	{
		SIPCCMsg_OneKeySwitch_ServiceConfirm ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage);
        m_logfunc(to_str(ipccMessage), get_file_name(__FILE__), __LINE__, 0);
        ret = On_SIPCCMsg_OneKeySwitch_ServiceConfirm(m_Sock, msgClass, msgType, ipccMessage, srcip, srcport);
		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_OneKeySwitch_ServerHeartBeatConf://心跳异常发回来的信令
	{
		//忽略该信息
		break;
		SIPCCMsg_OneKeySwitch_ServerHeartBeatConf ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage);
        m_logfunc(to_str(ipccMessage), get_file_name(__FILE__), __LINE__, 0);
        ret = On_SIPCCMsg_OneKeySwitch_ServerHeartBeatConf(m_Sock, msgClass, msgType, ipccMessage, srcip, srcport);
		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_OneKeySwitch_ServiceTimeout://注册服务发来的服务异常
	{

		SIPCCMsg_OneKeySwitch_ServiceTimeout ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage);
        m_logfunc(to_str(ipccMessage), get_file_name(__FILE__), __LINE__, 0);
        ret = On_SIPCCMsg_OneKeySwitch_ServiceTimeout(m_Sock, msgClass, msgType, ipccMessage, srcip, srcport);
		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_OneKeySwitch_GetServiceConf://向注册中心取服务回值
	{

		SIPCCMsg_OneKeySwitch_GetServiceConf ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage);
        m_logfunc(to_str(ipccMessage), get_file_name(__FILE__), __LINE__, 0);
        ret = On_SIPCCMsg_OneKeySwitch_GetServiceConf(m_Sock, msgClass, msgType, ipccMessage, srcip, srcport);
		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_OneKeySwitch_SwitchService_Ctrl://注册中心摧过来的消息
	{
		SIPCCMsg_OneKeySwitch_SwitchService_Ctrl ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage);
        m_logfunc(to_str(ipccMessage), get_file_name(__FILE__), __LINE__, 0);
        ret = On_SIPCCMsg_OneKeySwitch_SwitchService_Ctrl(m_Sock, msgClass, msgType, ipccMessage, srcip, srcport);
		ipccMessage.ReleaseMemory();
	}
	break;
	default:
		return false;
	}

    return ret;
}

void RegisterServer::On_SIPCCMsg_Redirect_GetServiceConf(string invokeid, 
	vector<ServiceInfo> & serviceVec, vector<RedisDBInfo> redisVec)
{
	if (m_registerServerA.isLive)
	{
        m_logfunc("redis A is live, ignore this message", get_file_name(__FILE__), __LINE__, 0);
		return ;
	}

	if (serviceVec.size() > 0)
	{
		if (serviceVec[0].serviceType == EST_RegisterServer)
		{
			m_registerServerA.ip = serviceVec[0].ip;
			m_registerServerA.port = serviceVec[0].port;

			Register();
		}
	}
}

bool RegisterServer::On_SIPCCMsg_OneKeySwitch_ServiceRegisterConf(const creditudp_transport_ptr & credit_udp_ptr,
	int  ipccmsg_class, int ipccmsg_type, SIPCCMessage & ipccmsg,
	unsigned long src_ip, unsigned short src_port)
{
	SIPCCMsg_OneKeySwitch_ServiceRegisterConf &msg = dynamic_cast<SIPCCMsg_OneKeySwitch_ServiceRegisterConf&>(ipccmsg);
	m_myself.serviceID = msg.ServiceID;

	if (src_ip == m_registerServerA.ip && src_port == m_registerServerA.port)
	{
		m_registerServerA.isLive = true;
        m_logfunc("register A live", get_file_name(__FILE__), __LINE__, 0);
	}
	else if (src_ip == m_registerServerB.ip && src_port == m_registerServerB.port)
	{
		m_registerServerB.isLive = true;
        m_logfunc("register B live", get_file_name(__FILE__), __LINE__, 0);
	}

	RegisterEnd();

	if (msg.HeartBeatServerIp != 0)
	{
		m_isUsingHeartbeatA = true;
		m_serverHeartbeatA.ip = msg.HeartBeatServerIp;
		m_serverHeartbeatA.port = msg.HeartBeatServerPort;
        m_logfunc("has heart beat A", get_file_name(__FILE__), __LINE__, 0);
	}
	
	if (msg.HeartBeatServerBackupIp != 0)
	{
		m_isUsingHeartbeatB = true;
		m_serverHeartbeatB.ip = msg.HeartBeatServerBackupIp;
		m_serverHeartbeatB.port = msg.HeartBeatServerBackupIPort;
        m_logfunc("has heart beat B", get_file_name(__FILE__), __LINE__, 0);
	}

	if (m_isUsingHeartbeatA || m_isUsingHeartbeatB)
	{
		HeartBeatConfirm();
	}

	return true;
}

bool RegisterServer::On_SIPCCMsg_OneKeySwitch_ServiceConfirmConf(const creditudp_transport_ptr & credit_udp_ptr,
	int  ipccmsg_class, int ipccmsg_type, SIPCCMessage & ipccmsg,
	unsigned long src_ip, unsigned short src_port)
{
	SIPCCMsg_OneKeySwitch_ServiceConfirmConf &msg = dynamic_cast<SIPCCMsg_OneKeySwitch_ServiceConfirmConf&>(ipccmsg);

	if (msg.InvokeId == "0")
	{
		m_serverHeartbeatA.isLive = true;
        m_logfunc("Heartbeat A live", get_file_name(__FILE__), __LINE__, 0);
	}
	else if (msg.InvokeId == "1")
	{
		m_serverHeartbeatB.isLive = true;
        m_logfunc("Heartbeat B live", get_file_name(__FILE__), __LINE__, 0);
	}

	SendHeartBeatToHeartBeatServer();

	GetHeartBeatServerEnd();

	return true;
}


bool RegisterServer::On_SIPCCMsg_OneKeySwitch_GetServiceConf(const creditudp_transport_ptr & credit_udp_ptr,
	int  ipccmsg_class, int ipccmsg_type, SIPCCMessage & ipccmsg,
	unsigned long src_ip, unsigned short src_port)
{
	SIPCCMsg_OneKeySwitch_GetServiceConf &msg = dynamic_cast<SIPCCMsg_OneKeySwitch_GetServiceConf&>(ipccmsg);
//	class finder
//	{
//	public:
//		finder(string invokid)
//		{
//			this->invokid = invokid;
//		}
//		
//		bool operator() (GetServiceCallInfo& callinfo)
//		{
//			return callinfo.invokeid == invokid;
//		}
//		string invokid;
//	};
        std::list<GetServiceCallInfo>::iterator it=m_GetServiceCallInfo.begin();
        for(; it!=m_GetServiceCallInfo.end(); it++ ){
            if(it->invokeid == msg.InvokeId)
                break;
        }
	//std::list<GetServiceCallInfo>::iterator it = std::find_if(m_GetServiceCallInfo.begin(), m_GetServiceCallInfo.end(), finder(msg.InvokeId));
	if (it != m_GetServiceCallInfo.end())
	{
		std::vector<ServiceInfo> result;
		if (msg.Service1ID != 0)
		{
			result.push_back(ServiceInfo(msg.Service1ID, (ESeverType)msg.Service1Type, msg.service1IP, msg.service1Port));
		}
		if (msg.Service2ID != 0)
		{
			result.push_back(ServiceInfo(msg.Service2ID, (ESeverType)msg.Service2Type, msg.service2IP, msg.service2Port));
		}
		if (msg.Service3ID != 0)
		{
			result.push_back(ServiceInfo(msg.Service3ID, (ESeverType)msg.Service3Type, msg.service3IP, msg.service3Port));
		}
		if (msg.Service4ID != 0)
		{
			result.push_back(ServiceInfo(msg.Service4ID, (ESeverType)msg.Service4Type, msg.service4IP, msg.service4Port));
		}
		it->cb(msg.InvokeId, result);
		m_GetServiceCallInfo.erase(it);
	}
	return true;
}

bool RegisterServer::On_SIPCCMsg_OneKeySwitch_GetHeartBeatConf(const creditudp_transport_ptr & credit_udp_ptr,
	int  ipccmsg_class, int ipccmsg_type, SIPCCMessage & ipccmsg,
	unsigned long src_ip, unsigned short src_port)
{
	SIPCCMsg_OneKeySwitch_GetHeartBeatConf &msg = dynamic_cast<SIPCCMsg_OneKeySwitch_GetHeartBeatConf&>(ipccmsg);

	switch (msg.operationMode)
	{
	case -1://需要重新注册,考虑时间差
	{
	}
	break;
	case 0:
	{
		m_isUsingHeartbeatA = false;
		m_isUsingHeartbeatB = false;
		//m_logfunc("无心跳模试");
	}
	break;
	case 2:
		////保证两个heartbeat时，可能发生有一个heartbeat失效，而返回两个heartbeat中有一个是在用的时
		////bool a_ok = (!(m_serverHeartbeatA.isLive && m_serverHeartbeatA.equal(msg.HeartBeatServerIP, msg.HeartBeatServerPort)) &&
		////	         !(m_serverHeartbeatB.isLive && m_serverHeartbeatB.equal(msg.HeartBeatServerIP, msg.HeartBeatServerPort)));

		////bool b_ok = (!(m_serverHeartbeatB.isLive && m_serverHeartbeatB.equal(msg.HeartBeatServerBackupIP, msg.HeartBeatServerBackupPort)) &&
		////	         !(m_serverHeartbeatB.isLive && m_serverHeartbeatB.equal(msg.HeartBeatServerBackupIP, msg.HeartBeatServerBackupPort)));

		////if (!m_serverHeartbeatA.isLive)
		////{
		////	if (a_ok)
		////	{
		////		m_serverHeartbeatA.serviceID = msg.HeartBeatServerID;
		////		m_serverHeartbeatA.ip = msg.HeartBeatServerIP;
		////		m_serverHeartbeatA.port = msg.HeartBeatServerPort;
		////		a_ok = false;
		////	}
		////	else if (b_ok)
		////	{
		////		m_serverHeartbeatA.serviceID = msg.HeartBeatServerBackupID;
		////		m_serverHeartbeatA.ip = msg.HeartBeatServerBackupIP;
		////		m_serverHeartbeatA.port = msg.HeartBeatServerBackupPort;
		////		b_ok = false;
		////	}
		////	HeartBeatConfirm("0");
		////}

		////if (!m_serverHeartbeatB.isLive)
		////{
		////	if (a_ok)
		////	{
		////		m_serverHeartbeatB.serviceID = msg.HeartBeatServerID;
		////		m_serverHeartbeatB.ip = msg.HeartBeatServerIP;
		////		m_serverHeartbeatB.port = msg.HeartBeatServerPort;
		////	}
		////	else if (b_ok)
		////	{
		////		m_serverHeartbeatB.serviceID = msg.HeartBeatServerBackupID;
		////		m_serverHeartbeatB.ip = msg.HeartBeatServerBackupIP;
		////		m_serverHeartbeatB.port = msg.HeartBeatServerBackupPort;
		////	}
		////	HeartBeatConfirm("1");
		////}

		if (msg.HeartBeatServerIP!=0 && msg.HeartBeatServerPort!=0)
		{
			m_serverHeartbeatA.serviceID = msg.HeartBeatServerID;
			m_serverHeartbeatA.ip = msg.HeartBeatServerIP;
			m_serverHeartbeatA.port = msg.HeartBeatServerPort;
			HeartBeatConfirm("0");
		}
		
		if (msg.HeartBeatServerBackupIP != 0 && msg.HeartBeatServerBackupPort != 0)
		{
			m_serverHeartbeatB.serviceID = msg.HeartBeatServerBackupID;
			m_serverHeartbeatB.ip = msg.HeartBeatServerBackupIP;
			m_serverHeartbeatB.port = msg.HeartBeatServerBackupPort;
			HeartBeatConfirm("1");
		}

		break;
	}

	return true;
}

bool RegisterServer::On_SIPCCMsg_OneKeySwitch_ServiceConfirm(const creditudp_transport_ptr & credit_udp_ptr,
	int  ipccmsg_class, int ipccmsg_type, SIPCCMessage & ipccmsg,
	unsigned long src_ip, unsigned short src_port)
{
	SIPCCMsg_OneKeySwitch_ServiceConfirm &msg = dynamic_cast<SIPCCMsg_OneKeySwitch_ServiceConfirm&>(ipccmsg);

	if (msg.ServiceID == m_myself.serviceID && msg.ServiceType == m_myself.serviceType)
	{
		send_SIPCCMsg_OneKeySwitch_ServiceConfirmConf(
			m_Sock, src_ip, src_port,
			msg.InvokeId,
			msg.ServiceID,
			msg.ServiceType
			);

		////m_logfunc("确认正常发送回值");
	}
	else
	{
		////m_logfunc("非不本服务认确，不回复");
        m_logfunc("非不本服务认确，不回复", get_file_name(__FILE__), __LINE__, 0);
	}
	return true;
}

bool RegisterServer::On_SIPCCMsg_OneKeySwitch_ServerHeartBeatConf(const creditudp_transport_ptr & credit_udp_ptr,
	int  ipccmsg_class, int ipccmsg_type, SIPCCMessage & ipccmsg,
	unsigned long src_ip, unsigned short src_port)
{
	SIPCCMsg_OneKeySwitch_ServerHeartBeatConf &msg = dynamic_cast<SIPCCMsg_OneKeySwitch_ServerHeartBeatConf&>(ipccmsg);

	if (m_serverHeartbeatA.isAddrAvailable())
	{
		m_serverHeartbeatA.isLive = false;
        m_logfunc("Heartbeat A dead", get_file_name(__FILE__), __LINE__, 0);
	}
	else if (m_serverHeartbeatB.isAddrAvailable())
	{
		m_serverHeartbeatB.isLive = false;
        m_logfunc("Heartbeat B dead", get_file_name(__FILE__), __LINE__, 0);
	}

	StartGetHeartBeatServer(__FILE__, __LINE__);

	return true;
}

bool RegisterServer::On_SIPCCMsg_OneKeySwitch_ServiceTimeout(const creditudp_transport_ptr & credit_udp_ptr,
	int  ipccmsg_class, int ipccmsg_type, SIPCCMessage & ipccmsg,
	unsigned long src_ip, unsigned short src_port)
{
	SIPCCMsg_OneKeySwitch_ServiceTimeout &msg = dynamic_cast<SIPCCMsg_OneKeySwitch_ServiceTimeout&>(ipccmsg);

	if (m_serverHeartbeatA.equal(msg.ServiceID))
	{
		m_serverHeartbeatA.isLive = false;
        m_logfunc("Heartbeat A dead", get_file_name(__FILE__), __LINE__, 0);
        StartGetHeartBeatServer(__FILE__, __LINE__);
    }
	else if (m_serverHeartbeatB.equal(msg.ServiceID))
	{
		m_serverHeartbeatB.isLive = false;
        m_logfunc("Heartbeat B dead", get_file_name(__FILE__), __LINE__, 0);
        StartGetHeartBeatServer(__FILE__, __LINE__);
    }
	else
	{
        return false;
	}

	return true;
}

bool RegisterServer::On_SIPCCMsg_OneKeySwitch_SwitchService_Ctrl(const creditudp_transport_ptr & credit_udp_ptr,
	int  ipccmsg_class, int ipccmsg_type, SIPCCMessage & ipccmsg,
	unsigned long src_ip, unsigned short src_port)
{
	return false;
}

void RegisterServer::Heartbeat()
{
	time_point<system_clock> now = system_clock::now();
	if ((now - m_lastSendHeartbeatTime) > boost::chrono::milliseconds(HEARTBEAT_INTERVAL))
	{
		SendHeartBeatToHeartBeatServer();
	}
	checkState();
}
string RegisterServer::GetService(GetServiceCallBack cb,
	int32_t  GetServiceCount,
	int32_t  Service1ID,
	int32_t  Service1Type,
	uint32_t service1IP,
	uint16_t service1Port,
	int32_t  Service2ID,
	int32_t  Service2Type,
	uint32_t service2IP,
	uint16_t service2Port,
	int32_t  Service3ID,
	int32_t  Service3Type,
	uint32_t service3IP,
	uint16_t service3Port,
	int32_t  Service4ID,
	int32_t  Service4Type,
	uint32_t service4IP,
	uint16_t service4Port)
{
	if (IsOK())
	{
		int32_t  serIp = m_registerServerA.isLive ? m_registerServerA.ip : m_registerServerB.ip;
		uint16_t serPort = m_registerServerA.isLive ? m_registerServerA.port : m_registerServerB.port;
		boost::mutex::scoped_lock lock(m_GetServiceMutex);
		string invokid = boost::lexical_cast<string>(m_GetServiceInvokeID++);
		m_GetServiceCallInfo.push_back(GetServiceCallInfo(invokid, cb, system_clock::now()));
		send_SIPCCMsg_OneKeySwitch_GetService(
			m_Sock, serIp, serPort,
			invokid,
			m_myself.serviceID,
			m_myself.serviceType,
			GetServiceCount,
			Service1ID,
			Service1Type,
			service1IP,
			service1Port,
			Service2ID,
			Service2Type,
			service2IP,
			service2Port,
			Service3ID,
			Service3Type,
			service3IP,
			service3Port,
			Service4ID,
			Service4Type,
			service4IP,
			service4Port
			);
		return invokid;
	}
	
	return "";
}

string RegisterServer::GetService(GetServiceCallBack cb,
	int32_t  Service1Type,
	int32_t  Service2Type,
	int32_t  Service3Type,
	int32_t  Service4Type)
{
	int count = 0;
	if (Service1Type != 0)
		count++;
	if (Service2Type != 0)
		count++;
	if (Service3Type != 0)
		count++;
	if (Service4Type != 0)
		count++;

	return GetService(cb,
		count,
		0, Service1Type, 0, 0,
		0, Service2Type, 0, 0,
		0, Service3Type, 0, 0,
		0, Service4Type, 0, 0
		);
}

string RegisterServer::GetService(string serviceInfoList, GetServiceCallBack cb)
{
	boost::mutex::scoped_lock lock(m_GetServiceMutex);
	string invokid = boost::lexical_cast<string>(m_GetServiceInvokeID++);
	m_GetServiceCallInfo.push_back(GetServiceCallInfo(invokid, cb, system_clock::now()));
	send_SIPCCMsg_OneKeySwitch_GetService(
		m_Sock, m_registerServerA.ip, m_registerServerA.port,
		invokid,
		m_myself.serviceID,
		m_myself.serviceType,
		serviceInfoList
		);
	return invokid;
}

void RegisterServer::CancelGetService(string invokid)
{
	boost::mutex::scoped_lock lock(m_GetServiceMutex);
	for (list<GetServiceCallInfo>::iterator it = m_GetServiceCallInfo.begin(); it != m_GetServiceCallInfo.end(); it++)
	{
		if (it->invokeid == invokid)
		{
			m_GetServiceCallInfo.erase(it);
			return;
		}
	}
}

//void RegisterServer::SendIPCCMsg(SIPCCMessage &ipccmessage, uint32_t ip, uint16_t port, string file, int line)
//{
//	if (ip == 0 || port == 0)
//	{
//		return;
//	}
//	CIPCCMsgExchanger msgExchanger;
//	char* theBuf = NULL;
//	int theBufContent = 0;
//	msgExchanger.SerializeMsg(ipccmessage, theBuf, theBufContent);
//	string sErrInfo("");
//
//	if (theBuf == NULL || theBufContent == 0)
//	{
//		////m_logfunc("SendIPCCMsg_UDP return theBuf == NULL || theBufContent == 0 ");
//		return;
//	}
//
//	using namespace boost::asio::ip;
//	m_Sock->SendData(
//		address_v4(ntohl(ip)).to_string(),
//		boost::lexical_cast<string>(ntohs(port)),
//		theBuf,
//		theBufContent,
//		sErrInfo);
//
//	if (ipccmessage.msgClass != Class_OCXHeart &&
//		ipccmessage.msgClass != Class_HeartBeat &&
//		ipccmessage.msgType != Type_ACD_CallingQueueInfo &&
//		ipccmessage.msgType != Type_OneKeySwitch_ServerHeartBeat &&
//		ipccmessage.msgType != Type_OneKeySwitch_ServerHeartBeatConf &&
//		ipccmessage.msgType != Type_Client_GetCallingQueueInfo &&
//		ipccmessage.msgType != Type_OneKeySwitch_ClientHeartBeat)
//	{
//		//ostringstream oss;
//		//oss << "sent, class=" << ipccmsg_class_to_str(ipccmessage.msgClass)
//		//	<< ", type=" << ipccmsg_type_to_str(ipccmessage.msgClass, ipccmessage.msgType)
//		//	<< ", ip=" << format_ip_addr(ip)
//		//	<< ", port=" << ntohs(port);
//		//string::size_type pos = file.find_last_of('/');
//		//{
//		//	pos = file.find_last_of('\\');
//		//}
//		//if (pos != string::npos)
//		//{
//		//	oss << ", " << file.substr(++pos) << "@" << line << endl;
//		//}
//		//else
//		//{
//		//	oss << endl;
//		//}
//		//oss << ipccmsg_to_str(ipccmessage);
//		//m_logfunc(oss.str());
//	}
//
//	if (theBuf != NULL)
//		delete theBuf;
//}

void RegisterServer::send_SIPCCMsg_OneKeySwitch_ServerHeartBeat(
	const creditudp_transport_ptr & credit_udp_ptr, uint32_t dest_ip, uint16_t dest_port,
	int32_t  ServiceID,
	int32_t  ServerType,
	uint32_t sendtime,
	string   GwName,
	string   ComputerSymbol,
	uint32_t GwManagerServerIp,
	int32_t  isProxyRegister,
	int32_t  Service1ID,
	int32_t  Service1Type,
	int32_t  Service2ID,
	int32_t  Service2Type,
	int32_t  Service3ID,
	int32_t  Service3Type,
	int32_t  Service4ID,
	int32_t  Service4Type
	)
{
	SIPCCMsg_OneKeySwitch_ServerHeartBeat msg;

    msg.GwName            = GwName           ;
    msg.ComputerSymbol    = ComputerSymbol   ;
    msg.ServerType        = ServerType       ;
    msg.GwManagerServerIp = GwManagerServerIp;
    msg.ServiceID         = ServiceID        ;
    msg.Service1ID        = Service1ID       ;
    msg.Service1Type      = Service1Type     ;
    msg.Service2ID        = Service2ID       ;
    msg.Service2Type      = Service2Type     ;
    msg.Service3ID        = Service3ID       ;
    msg.Service3Type      = Service3Type     ;
    msg.Service4ID        = Service4ID       ;
    msg.Service4Type      = Service4Type     ;
    msg.sendtime          = sendtime         ;
    msg.isProxyRegister   = isProxyRegister  ;

	m_Sock->send_ipccmsg(msg, dest_ip, dest_port);
}

void RegisterServer::SendHeartBeatToHeartBeatServer()
{
	if (m_serverHeartbeatA.isLive)
	{
		send_SIPCCMsg_OneKeySwitch_ServerHeartBeat(
			m_Sock, m_serverHeartbeatA.ip, m_serverHeartbeatA.port,
			m_myself.serviceID,
			m_myself.serviceType,
			0 //todo sendtime
			);
	}

	if (m_serverHeartbeatB.isLive)
	{
		send_SIPCCMsg_OneKeySwitch_ServerHeartBeat(
			m_Sock, m_serverHeartbeatB.ip, m_serverHeartbeatB.port,
			m_myself.serviceID,
			m_myself.serviceType,
			0 //todo sendtime
			);
	}

	m_lastSendHeartbeatTime = system_clock::now();
}

void RegisterServer::send_SIPCCMsg_OneKeySwitch_ServiceConfirmConf(
	const creditudp_transport_ptr & credit_udp_ptr, uint32_t dest_ip, uint16_t dest_port,
	string  InvokeId,
	int32_t ServiceID,
	int32_t ServiceType
	)
{
	SIPCCMsg_OneKeySwitch_ServiceConfirmConf msg;

	msg.InvokeId    = InvokeId   ;
	msg.ServiceID   = ServiceID  ;
	msg.ServiceType = ServiceType;

	m_Sock->send_ipccmsg(msg, dest_ip, dest_port);
}

void RegisterServer::send_SIPCCMsg_OneKeySwitch_GetHeartBeat(
	const creditudp_transport_ptr & credit_udp_ptr, uint32_t dest_ip, uint16_t dest_port,
	string file, int line,
	string   InvokeId,
	int32_t  ServiceID,
	int32_t  ServiceType,
	uint32_t HeartBeatServerIP,
	uint16_t HeartBeatServerPort,
	uint32_t HeartBeatServerBackupIP,
	uint16_t HeartBeatServerBackupPort
	)
{
	SIPCCMsg_OneKeySwitch_GetHeartBeat msg;
    msg.InvokeId                  = InvokeId                 ;
    msg.HeartBeatServerIP         = HeartBeatServerIP        ;
    msg.HeartBeatServerPort       = HeartBeatServerPort      ;
    msg.HeartBeatServerBackupIP   = HeartBeatServerBackupIP  ;
    msg.HeartBeatServerBackupPort = HeartBeatServerBackupPort;
    msg.ServiceID                 = ServiceID                ;
    msg.ServiceType               = ServiceType              ;

	m_Sock->send_ipccmsg(msg, dest_ip, dest_port, file, line);
}

void RegisterServer::send_SIPCCMsg_OneKeySwitch_ServiceConfirm(
	const creditudp_transport_ptr & credit_udp_ptr, uint32_t dest_ip, uint16_t dest_port,
	string   InvokeId,
	int32_t  ServiceID,
	int32_t  ServiceType,
	uint32_t serviceWanIP,
	uint32_t serviceLanIP,
	uint16_t servicePort
	)
{
	SIPCCMsg_OneKeySwitch_ServiceConfirm msg;
	msg.InvokeId = InvokeId;
	msg.ServiceID = ServiceID;
	msg.ServiceType = ServiceType;
	msg.serviceWanIP = serviceWanIP;
	msg.serviceLanIP = serviceLanIP;
	msg.servicePort = servicePort;

	m_Sock->send_ipccmsg(msg, dest_ip, dest_port);
}

void RegisterServer::send_SIPCCMsg_Redirect_GetService(
	const creditudp_transport_ptr & credit_udp_ptr, uint32_t dest_ip, uint16_t dest_port,
	int32_t serviceType,
	string  domainName,
	string  queryServiceList,
	string  netpath
	)
{
	SIPCCMsg_Redirect_GetService msg;
	msg.serviceType = serviceType;
	msg.domainName  = domainName;
	msg.queryServiceList = queryServiceList;
	msg.netPath = netpath;

	m_Sock->send_ipccmsg(msg, dest_ip, dest_port);
}

void RegisterServer::send_SIPCCMsg_OneKeySwitch_GetService(
	const creditudp_transport_ptr & credit_udp_ptr, uint32_t dest_ip, uint16_t dest_port,
	string   InvokeId,
	int32_t  ServiceID,
	int32_t  ServiceType,
	int32_t  GetServiceCount,
	int32_t  Service1ID,
	int32_t  Service1Type,
	uint32_t service1IP,
	uint16_t service1Port,
	int32_t  Service2ID,
	int32_t  Service2Type,
	uint32_t service2IP,
	uint16_t service2Port,
	int32_t  Service3ID,
	int32_t  Service3Type,
	uint32_t service3IP,
	uint16_t service3Port,
	int32_t  Service4ID,
	int32_t  Service4Type,
	uint32_t service4IP,
	uint16_t service4Port
	)
{
	SIPCCMsg_OneKeySwitch_GetService msg;
	msg.InvokeId = InvokeId;
	msg.ServiceID = ServiceID;
	msg.ServiceType = ServiceType;
	msg.GetServiceCount = GetServiceCount;
	msg.Service1ID = Service1ID;
	msg.Service1Type = Service1Type;
	msg.service1IP = service1IP;
	msg.service1Port = service1Port;
	msg.Service2ID = Service2ID;
	msg.Service2Type = Service2Type;
	msg.service2IP = service2IP;
	msg.service2Port = service2Port;
	msg.Service3ID = Service3ID;
	msg.Service3Type = Service3Type;
	msg.service3IP = service3IP;
	msg.service3Port = service3Port;
	msg.Service4ID = Service4ID;
	msg.Service4Type = Service4Type;
	msg.service4IP = service4IP;
	msg.service4Port = service4Port;
	msg.serviceInfoList = "";

	m_Sock->send_ipccmsg(msg, dest_ip, dest_port);
}

void RegisterServer::send_SIPCCMsg_OneKeySwitch_GetService(
	const creditudp_transport_ptr & credit_udp_ptr, uint32_t dest_ip, uint16_t dest_port,
	string   InvokeId,
	int32_t  ServiceID,
	int32_t  ServiceType,
	string   serviceInfoList
	)
{
	SIPCCMsg_OneKeySwitch_GetService msg;
	msg.InvokeId = InvokeId;
	msg.ServiceID = ServiceID;
	msg.ServiceType = ServiceType;
	msg.serviceInfoList = serviceInfoList;

	m_Sock->send_ipccmsg(msg, dest_ip, dest_port);
}

void RegisterServer::startRegisterTimer()
{
	if (m_registerTimer == invalid_timer_id)
	{
		m_registerTimer = timer_service_inst::get_service().run_after(REGISTER_TIMEOUT,
			boost::bind(&RegisterServer::OnRegisterTimer, this));
        m_logfunc("start register timer " + boost::lexical_cast<string>(m_registerTimer)
            , get_file_name(__FILE__), __LINE__, 0);
	}
}

void RegisterServer::stopRegisterTimer()
{
	m_logfunc("stop register timer " + boost::lexical_cast<string>(m_registerTimer)
        , get_file_name(__FILE__), __LINE__, 0);
	if (m_registerTimer != invalid_timer_id)
	{
		timer_service_inst::get_service().cancel(m_registerTimer);
		m_registerTimer = invalid_timer_id;
	}
}

void RegisterServer::OnRegisterTimer()
{
	m_logfunc("register time out, timerid:" + boost::lexical_cast<string>(m_registerTimer)
        , get_file_name(__FILE__), __LINE__, 0);
	RegisterEnd();
	GetHeartBeatServerEnd();
	checkState();
}

void RegisterServer::startGetHeartBeatTimer()
{
	if (m_getHeartBeatTimer == invalid_timer_id)
	{
		m_getHeartBeatTimer = timer_service_inst::get_service().run_after(GET_HEARTBEAT_TIMEOUT,
			boost::bind(&RegisterServer::OnGetHeartBeatTimer, this));
		m_logfunc("start GetHeartBeatTimer timer " + boost::lexical_cast<string>(m_getHeartBeatTimer)
            , get_file_name(__FILE__), __LINE__, 0);
	}
}

void RegisterServer::stopGetHeartBeatTimer()
{
	m_logfunc("stop GetHeartBeat timer " + boost::lexical_cast<string>(m_getHeartBeatTimer)
        , get_file_name(__FILE__), __LINE__, 0);
	if (m_getHeartBeatTimer != invalid_timer_id)
	{
		timer_service_inst::get_service().cancel(m_getHeartBeatTimer);
		m_getHeartBeatTimer = invalid_timer_id;
	}
}

void RegisterServer::OnGetHeartBeatTimer()
{
	m_logfunc("GetHeartBeat time out, timerid:" + boost::lexical_cast<string>(m_getHeartBeatTimer)
        , get_file_name(__FILE__), __LINE__, 0);
	if (m_registerServerA.isLive)
	{
		m_registerServerA.isLive = false;
        m_logfunc("register A dead", get_file_name(__FILE__), __LINE__, 0);
	}
	else if (m_registerServerB.isLive)
	{
		m_registerServerB.isLive = false;
        m_logfunc("register B dead", get_file_name(__FILE__), __LINE__, 0);
	}

	GetHeartBeatServerEnd();

	if (m_registerServerB.isLive)
	{
		StartGetHeartBeatServer(__FILE__, __LINE__);
	}
	else
	{
		StartRegister();
	}
}

void RegisterServer::checkState()
{
	if (!(m_registerServerA.isLive || m_registerServerB.isLive))
	{
		StartRegister();
	}
	else if ((m_isUsingHeartbeatA || m_isUsingHeartbeatB) && 
		     !(m_isUsingHeartbeatA && m_serverHeartbeatA.isLive) &&
			 !(m_isUsingHeartbeatB && m_serverHeartbeatB.isLive))
	{
		StartGetHeartBeatServer(__FILE__, __LINE__);
	}
}

void RegisterServer::StartRegister()
{
	boost::mutex::scoped_lock lock(m_registerMutex);
	if (!m_registering)
	{
		m_registering = true;
		if (m_redirectServer != NULL)
		{
			m_redirectServer->GetService("51", m_myself.serviceType, 
				boost::bind(&RegisterServer::On_SIPCCMsg_Redirect_GetServiceConf, this, _1, _2, _3), 
				__FILE__, __LINE__);
		}
		else
		{
			Register();
		}

		startRegisterTimer();
	}
}

void RegisterServer::RegisterEnd()
{
	boost::mutex::scoped_lock lock(m_registerMutex);
	m_registering = false;
	stopRegisterTimer();
}

void RegisterServer::Register()
{
	if (m_registerServerA.isAddrAvailable())
	{
		SIPCCMsg_OneKeySwitch_ServiceRegister regmsg;
		regmsg.ServerName = m_myself.serviceName;
		regmsg.ServerType = m_myself.serviceType;
		regmsg.wanIP = m_myself.ip;
		regmsg.wanPort = m_myself.port;
		regmsg.lanIP = m_myself.ip;
		regmsg.lanPort = m_myself.port;
		regmsg.ServerID = m_myself.serviceID;
		m_Sock->send_ipccmsg(regmsg, m_registerServerA.ip, m_registerServerA.port);
	}

	if (m_registerServerB.isAddrAvailable())
	{
		SIPCCMsg_OneKeySwitch_ServiceRegister regmsg;
		regmsg.ServerName = m_myself.serviceName;
		regmsg.ServerType = m_myself.serviceType;
		regmsg.wanIP = m_myself.ip;
		regmsg.wanPort = m_myself.port;
		regmsg.lanIP = m_myself.ip;
		regmsg.lanPort = m_myself.port;
		regmsg.ServerID = m_myself.serviceID;
		m_Sock->send_ipccmsg(regmsg, m_registerServerB.ip, m_registerServerB.port);
	}
}

void RegisterServer::SetGetHeartBeat(bool started)
{
	boost::mutex::scoped_lock lock(m_getHeartBeatMutex);
	m_getingHeartbeat = started;
}

void RegisterServer::StartGetHeartBeatServer(string file, int line)
{
	ServiceInfo registerServer;
	if (m_registerServerA.isLive)
	{
		registerServer = m_registerServerA;
	}
	else if (m_registerServerB.isLive)
	{
		registerServer = m_registerServerB;
	}
	else
	{
		return;
	}

	boost::mutex::scoped_lock lock(m_getHeartBeatMutex);
	if (!m_getingHeartbeat)
	{
		if ((m_isUsingHeartbeatA && !m_serverHeartbeatA.isLive) &&
			(m_isUsingHeartbeatB && !m_serverHeartbeatA.isLive))
		{
			send_SIPCCMsg_OneKeySwitch_GetHeartBeat(
				m_Sock, registerServer.ip, registerServer.port,
				file, line,
				"2",
				m_myself.serviceID,
				m_myself.serviceType,
				m_serverHeartbeatA.ip,
				m_serverHeartbeatA.port,
				m_serverHeartbeatB.ip,
				m_serverHeartbeatB.port
				);
			m_getingHeartbeat = true;
			startGetHeartBeatTimer();
		}
		else if (m_isUsingHeartbeatA && !m_serverHeartbeatA.isLive)
		{
			send_SIPCCMsg_OneKeySwitch_GetHeartBeat(
				m_Sock, registerServer.ip, registerServer.port,
				file, line,
				"0",
				m_myself.serviceID,
				m_myself.serviceType,
				m_serverHeartbeatA.ip,
				m_serverHeartbeatA.port
				);
			m_getingHeartbeat = true;
			startGetHeartBeatTimer();
		}
		else if (m_isUsingHeartbeatB && !m_serverHeartbeatA.isLive)
		{
			send_SIPCCMsg_OneKeySwitch_GetHeartBeat(
				m_Sock, registerServer.ip, registerServer.port,
				file, line,
				"1",
				m_myself.serviceID,
				m_myself.serviceType,
				0,
				0,
				m_serverHeartbeatB.ip,
				m_serverHeartbeatB.port
				);
			m_getingHeartbeat = true;
			startGetHeartBeatTimer();
		}
	}
}

void RegisterServer::GetHeartBeatServerEnd()
{
	boost::mutex::scoped_lock lock(m_getHeartBeatMutex);
	m_getingHeartbeat = false;
	stopGetHeartBeatTimer();
}

void RegisterServer::HeartBeatConfirm(string id)
{
	if (id.empty() || (id == "0"))
	{
		send_SIPCCMsg_OneKeySwitch_ServiceConfirm(
			m_Sock, m_serverHeartbeatA.ip, m_serverHeartbeatA.port,
			"0",             //InvokeId,
			m_myself.serviceID,   //ServiceID,
			m_myself.serviceType, //ServiceType,
			m_myself.ip,          //serviceWanIP
			m_myself.ip,          //serviceLanIP
			m_myself.port         //servicePort
			);
	}
	
	if (id.empty() || id == "1")
	{
		send_SIPCCMsg_OneKeySwitch_ServiceConfirm(
			m_Sock, m_serverHeartbeatB.ip, m_serverHeartbeatB.port,
			"1",             //InvokeId,
			m_myself.serviceID,   //ServiceID,
			m_myself.serviceType, //ServiceType,
			m_myself.ip,          //serviceWanIP
			m_myself.ip,          //serviceLanIP
			m_myself.port         //servicePort
			);
	}
}

void RegisterServer::startGetServiceTimeout()
{
	if (m_checkGetServiceTimer == invalid_timer_id)
	{
		m_checkGetServiceTimer = timer_service_inst::get_service().run_after(10000,
			boost::bind(&RegisterServer::checkGetServiceTimeout, this));
	}
}

void RegisterServer::stopGetServiceTimeout()
{
	if (m_checkGetServiceTimer != invalid_timer_id)
	{
		timer_service_inst::get_service().cancel(m_checkGetServiceTimer);
		m_checkGetServiceTimer = invalid_timer_id;
	}
}

void RegisterServer::checkGetServiceTimeout()
{
	m_logfunc("On GetService timer:" + boost::lexical_cast<string>(m_checkGetServiceTimer)
        , get_file_name(__FILE__), __LINE__, 0);
	class lessthan
	{
	public:
		lessthan(time_point<system_clock> tp)
			:lessthan_tp(tp)
		{}

		bool operator () (map<string, pair<GetServiceCallBack, time_point<system_clock> > >::value_type & value)
		{
			return (value.second.second < lessthan_tp);
		}

	private:
		time_point<system_clock> lessthan_tp;
	};

	time_point<system_clock> lasttime = system_clock::now() - boost::chrono::minutes(2);
	boost::mutex::scoped_lock lock(m_GetServiceMutex);
	for (list<GetServiceCallInfo>::iterator it = m_GetServiceCallInfo.begin(); it != m_GetServiceCallInfo.end(); it++)
	{
		if (it->starttime < lasttime)
		{
			it = m_GetServiceCallInfo.erase(it);
		}
		else
		{
			return;
		}
	}
}

bool RegisterServer::IsOK()
{
	return m_registerServerA.isLive || m_registerServerB.isLive;
}
