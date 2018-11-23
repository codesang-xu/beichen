#ifndef REGISTER_SERVER_H
#define REGISTER_SERVER_H

#include <string>
#include <list>
using namespace std;

#include <boost/chrono.hpp>
using namespace boost::chrono;

#include "common/creditudp_transport.h"
#include "common/server_addr_pack.h"
#include "common/timer_service.h"
#ifndef NOPUBLIC
#include "public/CreditUDP.h"
#include "public/MemLog.h"
#include "public/IPCCMessage.h"
#else
#include "CreditUDP.h"
#include "MemLog.h"
#include "IPCCMessage.h"
#endif
#include "ServiceInfo.h"
#include "RedirectServer.h"

using namespace ib_common;
using namespace ib::timer;

//算法简述：
//1、注册过程由一个总的定时器控制超时
//2、两个注册中心相继失效后，启动重新注册过程
//3、在使用双注册中心时，只需保证有一个是正常的即可
//4、在有两个heartbeat时，只需保证有一个是正常的即可
//5、去注册中心申请heartbeat的过程需要串行化，即在开始时要加锁，当两个heartbeat相继失效后
//   其实只在重新申请一个，这个请求回来后可检查另一个是否有效，亦可不做检查,由定时检查去发现
//6、heartbeat的失效可能由心跳超时发现，也可能是注册中心通知
//7、注册中心失效由heartbeat失效后，再申请heartbeat无响应发现，注册中心失效而heartbeat有效时无法发现
//8、Heartbeat()函数由应用主线程定时调用，不可以在内部自己用定时器调用
class RegisterServer
{
public:
    typedef boost::function<void(const string &, const char *, int, int)> makelogfunc;

public:
	RegisterServer();
	~RegisterServer();

public:
    void Init(creditudp_transport_ptr sock, string serName, int serType, unsigned int localIp, unsigned short localPort, makelogfunc logfunc);
	void StartRegister(unsigned int registerServerAIp, unsigned short registerServerAPort, unsigned int registerServerBIp, unsigned short registerServerBPort);
	void StartRegister(RedirectServer* redirectServer);
	bool DisposeMsg(char *pmsg, int msgsize, unsigned long srcip, unsigned short srcport);
	void Heartbeat();
	bool IsOK();
	const ServiceInfo & GetServiceInfo() { return m_myself; };
	const ServiceInfo & GetRegisterAInfo() { return m_registerServerA; };
	const ServiceInfo & GetRegisterBInfo() { return m_registerServerB; };

public:
	typedef boost::function<void(string, vector<ServiceInfo>&)> GetServiceCallBack;
	string GetService(GetServiceCallBack cb,
		int32_t  GetServiceCount,
		int32_t  Service1ID,
		int32_t  Service1Type,
		uint32_t service1IP = 0,
		uint16_t service1Port = 0,
		int32_t  Service2ID = 0,
		int32_t  Service2Type = 0,
		uint32_t service2IP = 0,
		uint16_t service2Port = 0,
		int32_t  Service3ID = 0,
		int32_t  Service3Type = 0,
		uint32_t service3IP = 0,
		uint16_t service3Port = 0,
		int32_t  Service4ID = 0,
		int32_t  Service4Type = 0,
		uint32_t service4IP = 0,
		uint16_t service4Port = 0);
	string GetService(GetServiceCallBack cb,
		int32_t  Service1Type,
		int32_t  Service2Type = 0,
		int32_t  Service3Type = 0,
		int32_t  Service4Type = 0);
	string GetService(string serviceInfoList, GetServiceCallBack cb);
	void CancelGetService(string invokid);

private:
	void On_SIPCCMsg_Redirect_GetServiceConf(string invokeid, vector<ServiceInfo> & serviceVec, vector<RedisDBInfo> redisVec);

	bool On_SIPCCMsg_OneKeySwitch_ServiceRegisterConf(const creditudp_transport_ptr & credit_udp_ptr,
		int  ipccmsg_class, int ipccmsg_type, SIPCCMessage & ipccmsg,
		unsigned long src_ip, unsigned short src_port);

	bool On_SIPCCMsg_OneKeySwitch_GetServiceConf(const creditudp_transport_ptr & credit_udp_ptr,
		int  ipccmsg_class, int ipccmsg_type, SIPCCMessage & ipccmsg,
		unsigned long src_ip, unsigned short src_port);

	bool On_SIPCCMsg_OneKeySwitch_GetHeartBeatConf(const creditudp_transport_ptr & credit_udp_ptr,
		int  ipccmsg_class, int ipccmsg_type, SIPCCMessage & ipccmsg,
		unsigned long src_ip, unsigned short src_port);

	bool On_SIPCCMsg_OneKeySwitch_ServiceConfirmConf(const creditudp_transport_ptr & credit_udp_ptr,
		int  ipccmsg_class, int ipccmsg_type, SIPCCMessage & ipccmsg,
		unsigned long src_ip, unsigned short src_port);

	bool On_SIPCCMsg_OneKeySwitch_ServiceConfirm(const creditudp_transport_ptr & credit_udp_ptr,
		int  ipccmsg_class, int ipccmsg_type, SIPCCMessage & ipccmsg,
		unsigned long src_ip, unsigned short src_port);

	bool On_SIPCCMsg_OneKeySwitch_ServerHeartBeatConf(const creditudp_transport_ptr & credit_udp_ptr,
		int  ipccmsg_class, int ipccmsg_type, SIPCCMessage & ipccmsg,
		unsigned long src_ip, unsigned short src_port);

	bool On_SIPCCMsg_OneKeySwitch_ServiceTimeout(const creditudp_transport_ptr & credit_udp_ptr,
		int  ipccmsg_class, int ipccmsg_type, SIPCCMessage & ipccmsg,
		unsigned long src_ip, unsigned short src_port);

	bool On_SIPCCMsg_OneKeySwitch_SwitchService_Ctrl(const creditudp_transport_ptr & credit_udp_ptr,
		int  ipccmsg_class, int ipccmsg_type, SIPCCMessage & ipccmsg,
		unsigned long src_ip, unsigned short src_port);

private:
	//void SendIPCCMsg(SIPCCMessage &ipccmessage, uint32_t ip, uint16_t port, string file = "", int line = 0);

	void send_SIPCCMsg_OneKeySwitch_ServerHeartBeat(
		const creditudp_transport_ptr & credit_udp_ptr, uint32_t dest_ip, uint16_t dest_port,
		int32_t  ServiceID,
		int32_t  ServerType,
		uint32_t sendtime     = 0,
		string   GwName       = "",
		string   ComputerSymbol = "",
		uint32_t GwManagerServerIp = 0,
		int32_t  isProxyRegister = 0,
		int32_t  Service1ID   = 0,
		int32_t  Service1Type = 0,
		int32_t  Service2ID   = 0,
		int32_t  Service2Type = 0,
		int32_t  Service3ID   = 0,
		int32_t  Service3Type = 0,
		int32_t  Service4ID   = 0,
		int32_t  Service4Type = 0
		);

	void send_SIPCCMsg_OneKeySwitch_ServiceConfirmConf(
		const creditudp_transport_ptr & credit_udp_ptr, uint32_t dest_ip, uint16_t dest_port,
		string  InvokeId,
		int32_t ServiceID,
		int32_t ServiceType
		);

	void send_SIPCCMsg_OneKeySwitch_GetHeartBeat(
		const creditudp_transport_ptr & credit_udp_ptr, uint32_t dest_ip, uint16_t dest_port,
		string file, int line,
		string   InvokeId,
		int32_t  ServiceID,
		int32_t  ServiceType,
		uint32_t HeartBeatServerIP = 0,
		uint16_t HeartBeatServerPort = 0,
		uint32_t HeartBeatServerBackupIP = 0,
		uint16_t HeartBeatServerBackupPort = 0
		);

	void send_SIPCCMsg_OneKeySwitch_ServiceConfirm(
		const creditudp_transport_ptr & credit_udp_ptr, uint32_t dest_ip, uint16_t dest_port,
		string   InvokeId,
		int32_t  ServiceID,
		int32_t  ServiceType,
		uint32_t serviceWanIP,
		uint32_t serviceLanIP,
		uint16_t servicePort
		);

	void send_SIPCCMsg_Redirect_GetService(
		const creditudp_transport_ptr & credit_udp_ptr, uint32_t dest_ip, uint16_t dest_port,
		int32_t serviceType,
		string domainName,
		string queryServiceList,
		string  netpath
		);

	void send_SIPCCMsg_OneKeySwitch_GetService(
		const creditudp_transport_ptr & credit_udp_ptr, uint32_t dest_ip, uint16_t dest_port,
		string   InvokeId,
		int32_t  ServiceID,
		int32_t  ServiceType,
		int32_t  GetServiceCount = 0,
		int32_t  Service1ID = 0,
		int32_t  Service1Type = 0,
		uint32_t service1IP = 0,
		uint16_t service1Port = 0,
		int32_t  Service2ID = 0,
		int32_t  Service2Type = 0,
		uint32_t service2IP = 0,
		uint16_t service2Port = 0,
		int32_t  Service3ID = 0,
		int32_t  Service3Type = 0,
		uint32_t service3IP = 0,
		uint16_t service3Port = 0,
		int32_t  Service4ID = 0,
		int32_t  Service4Type = 0,
		uint32_t service4IP = 0,
		uint16_t service4Port = 0
		);

	void send_SIPCCMsg_OneKeySwitch_GetService(
		const creditudp_transport_ptr & credit_udp_ptr, uint32_t dest_ip, uint16_t dest_port,
		string   InvokeId,
		int32_t  ServiceID,
		int32_t  ServiceType,
		string   serviceInfoList
		);

private:

	void checkState();

	void StartRegister();
	void RegisterEnd();

	void Register();
	void HeartBeatConfirm(string id = "");
	void startRegisterTimer();
	void stopRegisterTimer();
	void OnRegisterTimer();

	void SetGetHeartBeat(bool started);
	void StartGetHeartBeatServer(string file, int line);
	void GetHeartBeatServerEnd();
	void SendHeartBeatToHeartBeatServer();
	void startGetHeartBeatTimer();
	void stopGetHeartBeatTimer();
	void OnGetHeartBeatTimer();

	void startGetServiceTimeout();
	void stopGetServiceTimeout();
	void checkGetServiceTimeout();

private:
	RedirectServer* m_redirectServer;

	string m_myDomain;
	ServiceInfo m_myself;

	ServiceInfo m_registerServerA;
	ServiceInfo m_registerServerB;

	bool m_isUsingHeartbeatA;
	ServiceInfo m_serverHeartbeatA;

	bool m_isUsingHeartbeatB;
	ServiceInfo m_serverHeartbeatB;

	time_point<system_clock> m_lastSendHeartbeatTime;

	boost::mutex m_registerMutex;
	bool m_registering;
	timer_id m_registerTimer;

    boost::mutex m_getHeartBeatMutex;
	bool m_getingHeartbeat;
	timer_id m_getHeartBeatTimer;

	timer_id m_checkGetServiceTimer;

	creditudp_transport_ptr m_Sock;
    makelogfunc m_logfunc;

    boost::mutex m_GetServiceMutex;
	uint32_t m_GetServiceInvokeID;
	struct GetServiceCallInfo
	{
		GetServiceCallInfo(string invokeid, GetServiceCallBack cb, time_point<system_clock> starttime)
		{
			this->invokeid = invokeid;
			this->cb = cb;
			this->starttime = starttime;
		}
		string invokeid;
		GetServiceCallBack cb;
		time_point<system_clock> starttime;
	};
	std::list<GetServiceCallInfo> m_GetServiceCallInfo;

private:
	static const int HEARTBEAT_INTERVAL;
	static const int GET_HEARTBEAT_TIMEOUT;
	static const int REGISTER_TIMEOUT;
};

#endif
