// Utility.cpp: implementation of the CUtility class.
//
//////////////////////////////////////////////////////////////////////

#include "public/StdAfx.h"

#ifdef _WIN32
#include "windows/mdump.h"
MiniDumper d("ACDServer");
#else
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#include <time.h>

#include "Utility.h"
#include "public/GlobalOperation.h"
#include "CallMgr.h"
#include "CorporationInfo.h"
#include "CorporationMgr.h"
#include "UserMgr.h"
#include "AgentMgr.h"
#include "CClientMgr.h"
#include "getid.h"
#include "SplitString.h"
#include "public/ini.h"
#include "public/Spliter.h"
#include "public/ServerInfoDef.h"
#include "public/xmlParser.h"
#include "DepartmentMgr.h"
#include "IpccMsgLogTrans.h"//add code by hyj 2013-8-27
#include "Log.h"

#include "common/count_down_latch.h"
#include "common/ipccmsg_dump.h"
#include "common/ipccmsg_helper.h"
#include "common/util.h"
using namespace ib_common;

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include "boost/algorithm/string.hpp"  
#include "boost/filesystem/path.hpp"  
#include "boost/filesystem/operations.hpp" 
#include "public/MemLog.h"

#include "msgpush/MessagePushMgr.h"

CMemLog   g_Logger;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
BOOL Check(long SerialNo, int AgentNum);
#else

#endif
//MiniDumper dd("ACDSERVER");

CUtility* CUtility::__Utility = NULL;

CUtility::CUtility()
{
	this->m_iPreNotifyMinute = -1;
	this->m_bNotifyDataCollect = true;

	this->m_sOutdialIp = 0;
	this->m_sOutdialPort = 0;
	m_sLogTransSign0="LogTrans0:";
	m_sLogTransSign1="LogTrans1:";
	m_sLogTransSign2="LogTrans2:";
	m_sLogTransSign3="LogTrans3:";
	m_ServerHeartoperationMode=0;	
	m_Abnormatime=0;
	m_UstateServerTime=0;
	m_SysManageServerTime=0;
	m_DataCenterServerTime=0;
	m_bAllowedPush = 1;

}

CUtility::~CUtility()
{
#ifndef _WIN32
	Release();
#endif  

}

void CUtility::Init()
{	
	g_Logger.Init(CMemLog::TIME_LOG,(GetAppPath() / boost::filesystem::path("IPCC_Run_Log")).string(), "ACD_", ".LOG");
	g_Logger.Start();

	{
		string slog;
		ostringstream os;

#ifdef _WIN32
		os << std::endl
			<<"---------------------------------------" << std::endl
			<< "Welcome using ACDServer component." << std::endl
			<< "Building Time   : " << __DATE__ << " " << __TIME__ << std::endl
			<<"---------------------------------------" << std::endl;
#else
		extern const char* VERSION;
		os << std::endl
			<< "---------------------------------------" << std::endl
			<< "Welcome using ACDServer component." << std::endl
			<< "System Version  : " << VERSION << std::endl
			<< "Building Time   : " << __DATE__ << " " << __TIME__ << std::endl
			<< "---------------------------------------" << std::endl;
#endif

		slog = os.str();
		g_Logger.MakeLog(slog);
	}	

	boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
#ifdef _WIN32
	WSADATA wsaData;
	
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return;
	}
#endif

	string sLog;
	ostringstream ostream;

	if ( !LoadConfig() )
	{
		ostream << "Configuration error, the service does not start";
		sLog = ostream.str();
		g_Logger.MakeLog(sLog);

		return;
	}

	//Initialization  m_mySock
	m_SockHolder.reset(new creditudp_transport(
		boost::lexical_cast<string>(ServerInfoDef::Port_ACDServer), "main",
		boost::bind(&CUtility::MakeLog, this, _1)));
	m_mySock = &m_SockHolder->get_socket();

    msglog_filter disable_filter;
    disable_filter.disable(Class_ACD, Type_ACD_DepartNAgentInfo);
    m_SockHolder->add_filter(disable_filter);

	if (IsAllowedPush())
	{
		CMessagePushMgr::GetInstance()->Start();
	}
	//m_AsyncHttpClient_ptr.reset(new CAsyncHttpClient());
	//m_AsyncHttpClient_ptr->init(30);

	//m_JWTEncode_ptr.reset(new JWTEncode(m_IPCCConfig.AuthKey));

	//APNs_Push_Test();

	//check licence
	this->m_bShouldCNT = false;
	this->m_MsgCNT = 0;
	this->m_IPCCConfig.ServiceID = 0;
	this->m_IPCCConfig.SysManageServer_ID = 0;
	this->m_IPCCConfig.UStateServer_ID = 0;

#ifndef NO_DOG
	if ( this->m_IPCCConfig.bIsLocalDog )  
	{
		//m_bShouldCNT = false;
#ifdef _WIN32
		if(!Check(this->m_iLicenceCode, this->m_iAgentsLicence))
		{
			ostream.str("");
			ostream << "没有足够许可: linsense=" << m_iLicenceCode
				<< ",agentsNum:" << m_iAgentsLicence;
			sLog = ostream.str();
			g_Logger.MakeLog(sLog);

			this->m_bShouldCNT = true;
			this->m_MsgCNT = 300000;
		}
		else if ( this->m_iAgentsLicence == 499 )
		{
			this->m_iAgentsLicence = 0x7FFFFFFF;

			ostream.str("");
			ostream << "对于Licence为499的情况，运营平台需求，自动将Licence设为无穷！";
			sLog = ostream.str();
			g_Logger.MakeLog(sLog);
		}

#else
		if ( this->m_iAgentsLicence == 499 )
		{
			this->m_iAgentsLicence = 0x7FFFFFFF;

			ostream.str("");
			ostream << "对于Licence为499的情况，运营平台需求，自动将Licence设为无穷！";
			sLog = ostream.str();
			g_Logger.MakeLog(sLog);
		}

#endif
	}
	else
	{
		SIPCCMsg_Licence_Query licenceQuery;
		licenceQuery.LicenceType = LicenceType_Agent;
		licenceQuery.ServerName = this->m_IPCCConfig.ACDServerName;
		licenceQuery.queryTag = NewGlobalInvokeID();
		licenceQuery.sysTime = time(NULL);
		licenceQuery.countValue = this->m_iAgentsLicence;
		licenceQuery.bStartQuery = true;

		this->SendIPCCMsg_UDP(
			licenceQuery,
			this->m_IPCCConfig.LicenceConfirmServer1IP,
			this->m_IPCCConfig.LicenceConfirmServer1Port);

		this->SendIPCCMsg_UDP(
			licenceQuery,
			this->m_IPCCConfig.LicenceConfirmServer2IP,
			this->m_IPCCConfig.LicenceConfirmServer2Port);

		m_iLicenceConfirmMode = 2;
		m_tLicenceConfirmTime = gettickcount();
		m_iLicenceConfirmCount = 1;
		m_sLicenceConfirmTag = licenceQuery.queryTag;
	}

	m_iLicenceConfirmMode = 1;
	m_tLicenceConfirmTime = 0;
#endif

    if (m_redirectServer || (!m_IPCCConfig.ACDRedisIP.empty() && m_IPCCConfig.ACDRedisPort != 0)) {
        m_redisServer_ACD = new RedisServer_ACD();
        if (m_redirectServer) {
            m_redisServer_ACD->Init(&g_Logger, m_redirectServer, EST_ACDServer);
        }
        else {
            m_redisServer_ACD->Init(&g_Logger, m_IPCCConfig.ACDRedisIP, m_IPCCConfig.ACDRedisPort
                , m_IPCCConfig.ACDRedisDB, m_IPCCConfig.ACDRedisPassword);
        }
    }

	m_bEgnRun = true;
	boost::thread thrd(ThreadSockEngine, this);

    if (!m_sRedirectServerDomain.empty() ||
        (m_IPCCConfig.RegisterServerA_IP != 0 && m_IPCCConfig.RegisterServerA_Port != 0) || 
        (m_IPCCConfig.RegisterServerB_IP != 0 && m_IPCCConfig.RegisterServerB_Port != 0))
	{
		m_registerServer = new RegisterServer();
		m_registerServer->Init(m_SockHolder, "WIPCC_ACDServer", EST_ACDServer,
			this->m_IPCCConfig.localIP, htons(ServerInfoDef::Port_ACDServer), boost::bind(&CUtility::MakeLog, this, _1));

        if (!m_sRedirectServerDomain.empty()) {
			m_redirectServer = new RedirectServer();
			m_redirectServer->Init(m_SockHolder, m_sRedirectServerDomain);

			m_registerServer->StartRegister(m_redirectServer);
		}
		else {
			m_registerServer->StartRegister(
				m_IPCCConfig.RegisterServerA_IP, m_IPCCConfig.RegisterServerA_Port,
				m_IPCCConfig.RegisterServerB_IP, m_IPCCConfig.RegisterServerB_Port);
		}

		int counter = 50;
		while (counter > 0) {
			if (m_registerServer->IsOK())
				break;

			boost::this_thread::sleep(boost::posix_time::milliseconds(500));
			counter--;
		}

		if (!m_registerServer->IsOK()) {
			MakeLog("注册失败");
		}
		else {
			ostringstream oss;
			while (m_IPCCConfig.UStateServer_IP == 0 ||
				m_IPCCConfig.DataCollectServer_IP == 0 ||
				m_IPCCConfig.SysManageServer_IP == 0 ||
				m_IPCCConfig.MonitorServer_IP == 0
				|| m_IPCCConfig.AreaCodeServer_IP == 0)
			{
				oss.str("");
				if (m_IPCCConfig.SysManageServer_IP == 0) {
					oss << EST_ConfigurationServer << ";";
				}
				if (m_IPCCConfig.UStateServer_IP == 0) {
					oss << EST_UStateServer << ";";
				}
				if (m_IPCCConfig.DataCollectServer_IP == 0) {
					oss << EST_DataCollectServe << ";";
				}
				if (m_IPCCConfig.MonitorServer_IP == 0) {
					oss << EST_MonitiorServer << ";";
				}
				if (m_IPCCConfig.AreaCodeServer_IP == 0) {
					oss << EST_AreaCodeServer << ";";
				}

				GetService(oss.str());

				counter = 50;
				while (counter > 0) {
					if (m_IPCCConfig.SysManageServer_IP != 0 &&
						m_IPCCConfig.UStateServer_IP != 0 &&
						m_IPCCConfig.DataCollectServer_IP != 0 &&
						m_IPCCConfig.AreaCodeServer_IP != 0) {
						break;
					}
					boost::this_thread::sleep(boost::posix_time::milliseconds(500));
					counter--;
				}

				if (m_IPCCConfig.UStateServer_IP == 0) {
					MakeLog("未获取到UState服务");
				}

				if (m_IPCCConfig.DataCollectServer_IP == 0) {
					MakeLog("未获取到DataCollectServer服务");
				}

				if (m_IPCCConfig.AreaCodeServer_IP == 0) {
					MakeLog("未获取到AreaCodeServer服务");
				}

				if (m_IPCCConfig.SysManageServer_IP == 0) {
					MakeLog("未获取到ConfigurationServer服务");
					MakeLog("启动失败退出服务");
					exit(0);
				}
			}
		}
	}
	

	boost::thread th_NotifyDataCollect(CUtility::Thread_NotifyDataCollect, this);
	time_t ltime;
	ltime = time(NULL);
	char str[32];
	strftime(str, sizeof(str), "%Y-%m-%d %H:%M:00", localtime(&ltime));

	if (m_IPCCConfig.DataCollectServer_IP != 0 &&
		m_IPCCConfig.DataCollectServer_Port != 0)
	{
		SIPCCMsg_ACD_ResetDataCollect resetDataCollect;
		resetDataCollect.curTime = str;
		CUtility::GetInstance()->SendIPCCMsg_UDP(
			resetDataCollect,
			CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_IP,
			CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_Port);
	}

	if (m_IPCCConfig.MonitorServer_IP != 0 &&
		m_IPCCConfig.MonitorServer_Port != 0)
	{
		SIPCCMsg_ACD_ResetMonitor resetMonitor;
		resetMonitor.curTime = str;
		CUtility::GetInstance()->SendToMonitorServer(resetMonitor);
	
	}

	if (m_IPCCConfig.SysManageServer_IP != 0)
	{
		//发送 LoadDepartments 的消息给维护程序
		SIPCCMsg_ACD_LoadDepartments loadDepartments;
		loadDepartments.serverIP = this->m_IPCCConfig.localserverIP;
		this->SendIPCCMsg_UDP(
			loadDepartments,
			this->m_IPCCConfig.SysManageServer_IP,
			ConvertPort(ServerInfoDef::Port_SysManageServer)
			);
	}

    ////string str_user("0|N5B726E2802AF016B00000012|6006|97558154|15110186295|广发银行|广发银行%^#test1|bj201806211743:广发银行:test1|test1||0|0||6006%^#15110186295%^#%^#2E1测试网关|3|2|1|010|9755|[p=udp,p=udp/rtp,c=G729]|3361999370|3389591724|60443|60443|0|0");
    ////CUserInfo user;
    ////user.FromString(str_user);

	//test
	//CAgentMgr::GetInstance()->AddAgent("wxyy1@zj.com", "nick:wxyy1@zj.com", "corpname", "departmentname",
	//	0, 100, 100, 100, 100, 1, "1001", 0, "bj33712407", "1:", "1001", "deptnickname", 1, 1, 5, "1000", false);
}

void CUtility::Release()
{
	//发送结束的消息给 ThreadEngineSocket
	SetRcvSockThreadEnd();

	m_mySock->Release();

	CCorporationMgr::destroy();
	CAgentMgr::destroy();

	CMessagePushMgr::GetInstance()->Stop();
	g_Logger.Stop();
}

UINT  CUtility::ThreadSockEngine(void * lpThis)
{
	CUtility* pUtility = (CUtility*)lpThis;
	
	pUtility->MainSockEngine();

	return 0;
}

void CUtility::MainSockEngine()
{
	g_Logger.MakeLog("开始启动");

	this->m_bEgnRun = true;

	string sErrInfo("");

	m_iPreTimeMark = gettickcount();
	m_iSendOutDialStateTime =  gettickcount();
	m_iCheckOverflowTime = gettickcount();

	vector<char> msg(2048);
	while(this->m_bEgnRun)
	{
		string strIp;
		string strPort;
		int iRet = -1;
		iRet = m_mySock->ReceiveData(msg, msg.size(), strIp, strPort, sErrInfo);

#ifndef NO_DOG
		if ( this->m_iLicenceConfirmMode > 1 )
		{
			unsigned long nowTime = gettickcount();
			if ( nowTime - m_tLicenceConfirmTime > 60000 )
			{
				if ( m_iLicenceConfirmCount >= 30 )
				{
					string sLog("licence服务器连接超时，ACD服务停止工作");
					g_Logger.MakeLog(sLog);
					return;
				}

				SIPCCMsg_Licence_Query licenceQuery;
				licenceQuery.LicenceType = LicenceType_Agent;
				licenceQuery.ServerName = this->m_IPCCConfig.ACDServerName;
				licenceQuery.queryTag = this->m_sLicenceConfirmTag;
				licenceQuery.sysTime = time(NULL);
				licenceQuery.countValue = this->m_iAgentsLicence;
				licenceQuery.bStartQuery = true;

				this->SendIPCCMsg_UDP(
					licenceQuery,
					this->m_IPCCConfig.LicenceConfirmServer1IP,
					this->m_IPCCConfig.LicenceConfirmServer1Port);

				this->SendIPCCMsg_UDP(
					licenceQuery,
					this->m_IPCCConfig.LicenceConfirmServer2IP,
					this->m_IPCCConfig.LicenceConfirmServer2Port);

				m_tLicenceConfirmTime = nowTime;
				m_iLicenceConfirmCount++;
			}
		}
		else if ( this->m_iLicenceConfirmMode == 1 )
		{
			if ( m_tLicenceConfirmTime > 0 )
			{
				unsigned long nowTime = gettickcount();
				if ( nowTime - m_tLicenceConfirmTime > 3600000 )
				{
					SIPCCMsg_Licence_Query licenceQuery;
					licenceQuery.LicenceType = LicenceType_Agent;
					licenceQuery.ServerName = this->m_IPCCConfig.ACDServerName;
					licenceQuery.queryTag = NewGlobalInvokeID();
					licenceQuery.sysTime = time(NULL);
					licenceQuery.countValue = this->m_iAgentsLicence;

					this->SendIPCCMsg_UDP(
						licenceQuery,
						this->m_IPCCConfig.LicenceConfirmServer1IP,
						this->m_IPCCConfig.LicenceConfirmServer1Port);

					this->SendIPCCMsg_UDP(
						licenceQuery,
						this->m_IPCCConfig.LicenceConfirmServer2IP,
						this->m_IPCCConfig.LicenceConfirmServer2Port);

					m_iLicenceConfirmMode = 3;
					m_tLicenceConfirmTime = gettickcount();
					m_iLicenceConfirmCount = 1;
					m_sLicenceConfirmTag = licenceQuery.queryTag;
				}
			}

		}
#endif

		if (m_registerServer && m_registerServer->IsOK())
		{
			m_registerServer->Heartbeat();
		}

		ScanAgentMap();
		ScanTXTSessionTimeout();
		CheckRingAgentExtTime();		
		CheckSendOutDialState();		

		if(!m_bEgnRun)
		{
			break;
		}

		if(-1 == iRet)
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(1));
			continue;	
		}

		if (iRet < 1)
			continue;

		char creditUDPHead[9];
		if(iRet>9)
			memcpy(creditUDPHead, &msg[0], 9);
		if(0 == memcmp(creditUDPHead, "CreditUDP", 9))
		{
			g_Logger.MakeLog("非法消息");
			continue;
		}

		if(m_bShouldCNT)
		{
			m_MsgCNT--;
			if(m_MsgCNT <= 0)
			{
				string sLog("licence is not enough, service stopped");
				g_Logger.MakeLog(sLog);
				return;
			}
		}

		this->DisposeSockMsg(&msg[0], iRet, strIp, strPort);
	}
}
bool CUtility::DisposeSockMsg(char *pmsg, int msgsize, const string &strIp, const string &strPort)
{
	using namespace boost::asio::ip;
	unsigned int ip = inet_addr(strIp.c_str());
	unsigned short port= htons(boost::lexical_cast<unsigned short>(strPort));
	//处理 Socket 消息
	CIPCCMsgExchanger msgExchanger;

	__EIPCCMsgClass msgClass;
	__EIPCCMsgType msgType;
	msgExchanger.GetMsgHeader(pmsg, msgsize, msgClass, msgType);

	if(msgClass == Class_EndEgn)
	{
		if(ip == m_IPCCConfig.localIP)
			return false;
	}
	

	if(msgClass == Class_HeartBeat)
	{
		if ( msgType == Type_HeartBeat )
		{
			//更新 ip/port
			SIPCCMsg_HeartBeat ipccMessage;
			msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

			this->OnHeartBeat((SIPCCMsg_HeartBeat*)(&ipccMessage), ip, port);

			ipccMessage.ReleaseMemory();
		}
		
		return true;
	}

	{

		if (msgClass != Class_OCXHeart &&
			msgClass != Class_HeartBeat &&
			msgType != Type_OneKeySwitch_GetHeartBeatConf &&
			msgType != Type_Client_GetCallingQueueInfo &&
			msgType != Type_OneKeySwitch_ServerHeartBeatConf &&
			msgType != Type_OneKeySwitch_ClientHeartBeatConf)
		{
			ostringstream ostream;
            ostream << "received"
                << ", type=" << ipccmsg_type_to_str(msgClass, msgType)
                << ", from=" << format_ip_addr(ip) << "@" << ntohs(port);
				//<< ", class=" << ipccmsg_class_to_str(msgClass);
			g_Logger.MakeLog(ostream.str());
		}
	}

	string sLog;
	ostringstream ostream;

	switch (msgType)
	{

	///////////////////// [register //////////////////////
	case Type_OneKeySwitch_ServiceRegisterConf:
	case Type_OneKeySwitch_ServiceConfirmConf:
	case Type_OneKeySwitch_GetHeartBeatConf:
	case Type_OneKeySwitch_ServiceConfirm:
	case Type_OneKeySwitch_ServerHeartBeatConf:
	case Type_OneKeySwitch_ServiceTimeout:
	case Type_OneKeySwitch_GetServiceConf:
	case Type_OneKeySwitch_SwitchService_Ctrl:
	{
		if (m_registerServer)
		{
			return m_registerServer->DisposeMsg(pmsg, msgsize, ip, port);
		}
		return true;
	}
	///////////////////// [register //////////////////////
	case Type_Redirect_GetServiceConf:
	{
		SIPCCMsg_Redirect_GetServiceConf ipccMessage;
		ipccMessage.AnalyzeBuf(pmsg, msgsize, ip, port);
		{
			ostringstream ostream;
			ostream << to_str(ipccMessage);
			g_Logger.MakeLog(ostream.str());
		}
		if (m_redirectServer)
		{
			m_redirectServer->On_SIPCCMsg_Redirect_GetServiceConf(m_SockHolder,
				msgClass, msgType, ipccMessage, ip, port);
		}
		return true;

	}
	break;
	case Type_Client_SetOutdialCase:		//增加部门(发送给ACD服务器)
	{
		SIPCCMsg_Client_SetOutdialCase ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		g_Logger.MakeLog(ipccmsg_to_str(ipccMessage));
		this->OnSetOutdialCase((SIPCCMsg_Client_SetOutdialCase*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_SysManage_AddDepartment:		//增加部门(发送给ACD服务器)
	{
		SIPCCMsg_SysManage_AddDepartment ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		g_Logger.MakeLog(ipccmsg_to_str(ipccMessage));
		this->OnAddDepartment((SIPCCMsg_SysManage_AddDepartment*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_SysManage_DelDepartment:		//删除部门(发送给ACD服务器)
	{
		SIPCCMsg_SysManage_DelDepartment ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		g_Logger.MakeLog(ipccmsg_to_str(ipccMessage));
		this->OnDelDepartment((SIPCCMsg_SysManage_DelDepartment*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_UserMakeCall:			//用户发起呼叫 (->ACD服务器)
	{
		SIPCCMsg_Client_UserMakeCall ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		g_Logger.MakeLog(ipccmsg_to_str(ipccMessage));
		this->OnUserMakeCall((SIPCCMsg_Client_UserMakeCall*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_UserHangupCall:		//用户挂断呼叫 (->ACD服务器)
	{
		SIPCCMsg_Client_UserHangupCall ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		g_Logger.MakeLog(ipccmsg_to_str(ipccMessage));
		this->OnUserHangupCall((SIPCCMsg_Client_UserHangupCall*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_AgentLogin:			//座席登录 (->ACD服务器)
	{
		SIPCCMsg_Client_AgentLogin ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		g_Logger.MakeLog(to_str(ipccMessage));

		this->OnAgentLogin((SIPCCMsg_Client_AgentLogin*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_AgentLogout:			//座席注销 (->ACD服务器)
	{
		SIPCCMsg_Client_AgentLogout ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "Client_AgentLogout:" << "agentID="
			<< ((SIPCCMsg_Client_AgentLogout*)&ipccMessage)->GetAgentID()
			<< ";" << "natip=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();

		g_Logger.MakeLog(sLog);
		this->OnAgentLogout((SIPCCMsg_Client_AgentLogout*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	// [scq/start/2010-04-23]
	case Type_Client_AgentLogoutDeparts:			//座席登录多个技能组后注销部分技能组 (->ACD服务器)
	{
		SIPCCMsg_Client_AgentLogoutDeparts ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "Client_AgentLogoutDeparts:" << "agentID="
			<< ((SIPCCMsg_Client_AgentLogoutDeparts*)&ipccMessage)->GetAgentID() << ";"
			<< "theDepartment=" << ((SIPCCMsg_Client_AgentLogoutDeparts*)&ipccMessage)->GetTheDepartment()
			<< ";" << "natip=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();

		g_Logger.MakeLog(sLog);
		this->OnAgentLogoutDeparts((SIPCCMsg_Client_AgentLogoutDeparts*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	// [scq/end/2010-04-23]
	case Type_Client_AgentPark:				//座席小休 (->ACD服务器)
	{
		SIPCCMsg_Client_AgentPark ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "Client_AgentPark:" << "agentID="
			<< ((SIPCCMsg_Client_AgentPark*)&ipccMessage)->GetAgentID()
			<< ";" << "department=" << ((SIPCCMsg_Client_AgentPark*)&ipccMessage)->GetTheDepartment()
			<< ";" << "natip=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();

		g_Logger.MakeLog(sLog);
		this->OnAgentPark((SIPCCMsg_Client_AgentPark*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_AgentUnPark:			//座席取消小休 (->ACD服务器)
	{
		SIPCCMsg_Client_AgentUnPark ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "Client_AgentUnPark:" << "agentID="
			<< ((SIPCCMsg_Client_AgentUnPark*)&ipccMessage)->GetAgentID()
			<< ";" << "department=" << ((SIPCCMsg_Client_AgentUnPark*)&ipccMessage)->GetTheDepartment()
			<< ";" << "natip=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();

		g_Logger.MakeLog(sLog);
		this->OnAgentUnPark((SIPCCMsg_Client_AgentUnPark*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_AgentAnswerCall:		//座席应答呼叫(->ACD服务器)
	{
		SIPCCMsg_Client_AgentAnswerCall ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);
		g_Logger.MakeLog(to_str(ipccMessage));

		this->OnAgentAnswerCall((SIPCCMsg_Client_AgentAnswerCall*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_AgentHangupCall:		//座席挂机(->ACD服务器)
	{
		SIPCCMsg_Client_AgentHangupCall ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		g_Logger.MakeLog(to_str(ipccMessage));

		this->OnAgentHangupCall((SIPCCMsg_Client_AgentHangupCall*)(&ipccMessage), ip, port);
	}
	break;
	case Type_Client_MediaRequest:	//用户发起媒体请求
	{
		SIPCCMsg_Client_MediaRequest ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);
		g_Logger.MakeLog(to_str(ipccMessage));
		this->OnMediaRequest((SIPCCMsg_Client_MediaRequest*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_GetCallingQueueInfo:
	{
		SIPCCMsg_Client_GetCallingQueueInfo ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);
		this->OnGetCallingQueueInfo((SIPCCMsg_Client_GetCallingQueueInfo*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_AgentCallReceived:
	{
		SIPCCMsg_Client_AgentCallReceived ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);
		g_Logger.MakeLog(to_str(ipccMessage));
		this->OnAgentCallReceived((SIPCCMsg_Client_AgentCallReceived*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_AgentReset:
	{
		SIPCCMsg_Client_AgentReset ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "Client_AgentReset:" << "agentID="
			<< ((SIPCCMsg_Client_AgentReset*)&ipccMessage)->GetAgentID()
			<< ";" << "natip=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();
		g_Logger.MakeLog(sLog);

		this->OnAgentReset((SIPCCMsg_Client_AgentReset*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_AgentQueryState:
	{
		SIPCCMsg_Client_AgentQueryState ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);
		g_Logger.MakeLog(to_str(ipccMessage));

		this->OnAgentQueryState((SIPCCMsg_Client_AgentQueryState*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_CallUserConf:
	{
		SIPCCMsg_Client_CallUserConf ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);
		g_Logger.MakeLog(to_str(ipccMessage));
		this->OnClient_CallUserConf((SIPCCMsg_Client_CallUserConf*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_CallUserFailed:
	{
		SIPCCMsg_Client_CallUserFailed ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "Client_CallUserFailed:" << "agentID="
			<< ((SIPCCMsg_Client_CallUserFailed*)&ipccMessage)->GetCallerParty()
			<< ";" << "failedCause=" << ((SIPCCMsg_Client_CallUserFailed*)&ipccMessage)->failedCause
			<< ";" << "natip=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();
		g_Logger.MakeLog(sLog);

		this->OnClient_CallUserFailed((SIPCCMsg_Client_CallUserFailed*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_UserAnswerCall:		//用户应答呼叫(->ACD服务器)
	{
		SIPCCMsg_Client_UserAnswerCall ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);
		g_Logger.MakeLog(to_str(ipccMessage));
		this->OnClient_UserAnswerCall((SIPCCMsg_Client_UserAnswerCall*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_UserAnswerCall_Real:		//用户应答呼叫(->ACD服务器)
	{
		SIPCCMsg_Client_UserAnswerCall_Real ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);
		g_Logger.MakeLog(to_str(ipccMessage));
		this->OnClient_UserAnswerCall_Real((SIPCCMsg_Client_UserAnswerCall_Real*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_AgentMakeCall:
	{
		SIPCCMsg_Client_AgentMakeCall ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		g_Logger.MakeLog(to_str(ipccMessage));

		this->On_SIPCCMsg_Client_AgentMakeCall((SIPCCMsg_Client_AgentMakeCall*)(&ipccMessage), ip, port);
	}
	break;
	case Type_Client_PrepareTransfer:
	{
		SIPCCMsg_Client_PrepareTransfer ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "Client_PrepareTransfer:"
			<< ";" << "callID=" << ((SIPCCMsg_Client_PrepareTransfer*)&ipccMessage)->GetCallID()
			<< ";" << "theParty=" << ((SIPCCMsg_Client_PrepareTransfer*)&ipccMessage)->GetTheParty()
			<< ";" << "otherParty=" << ((SIPCCMsg_Client_PrepareTransfer*)&ipccMessage)->GetOtherParty()
			<< ";" << "audioIP=" << ((SIPCCMsg_Client_PrepareTransfer*)&ipccMessage)->AudioIp
			<< ";" << "audioPort=" << ((SIPCCMsg_Client_PrepareTransfer*)&ipccMessage)->AudioPort
			<< ";" << "natIP=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();

		g_Logger.MakeLog(sLog);
		this->OnClient_PrepareTransfer((SIPCCMsg_Client_PrepareTransfer*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_TransReady:
	{
		SIPCCMsg_Client_TransReady ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "Client_TransReady:"
			<< ";" << "presideAgent=" << ((SIPCCMsg_Client_TransReady*)&ipccMessage)->GetPresideAgent()
			<< ";" << "readyParty=" << ((SIPCCMsg_Client_TransReady*)&ipccMessage)->GetReadyParty()
			<< ";" << "natIP=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();

		g_Logger.MakeLog(sLog);
		this->OnClient_TransReady((SIPCCMsg_Client_TransReady*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_TransferCall:
	{
		SIPCCMsg_Client_TransferCall ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "Client_TransferCall:"
			<< ";" << "theParty=" << ((SIPCCMsg_Client_TransferCall*)&ipccMessage)->GetTheParty()
			<< ";" << "presideAgent=" << ((SIPCCMsg_Client_TransferCall*)&ipccMessage)->GetPresideAgent()
			<< ";" << "otherParty=" << ((SIPCCMsg_Client_TransferCall*)&ipccMessage)->GetOtherParty()
			<< ";" << "userLineNo=" << ((SIPCCMsg_Client_TransferCall*)&ipccMessage)->userLineNo
			<< ";" << "userLineIp=" << ((SIPCCMsg_Client_TransferCall*)&ipccMessage)->userLineIp
			<< ";" << "userLinePort=" << ((SIPCCMsg_Client_TransferCall*)&ipccMessage)->userLinePort
			<< ";" << "natIP=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();

		g_Logger.MakeLog(sLog);
		this->OnClient_TransferCall((SIPCCMsg_Client_TransferCall*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_ClearPreConnection:
	{
		SIPCCMsg_Client_ClearPreConnection ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "Client_ClearPreConnection:"
			<< ";" << "notifiedParty=" << ((SIPCCMsg_Client_ClearPreConnection*)&ipccMessage)->GetNotifiedParty()
			<< ";" << "natIP=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();

		g_Logger.MakeLog(sLog);
		this->OnClient_ClearPreConnection((SIPCCMsg_Client_ClearPreConnection*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_GetDepartNAgentInfo:
	{
		SIPCCMsg_Client_GetDepartNAgentInfo ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "Client_GetDepartNAgentInfo:"
			<< ";" << "invokeID=" << ((SIPCCMsg_Client_GetDepartNAgentInfo*)&ipccMessage)->invokeID
			<< ";" << "corp=" << ((SIPCCMsg_Client_GetDepartNAgentInfo*)&ipccMessage)->GetCorporationName()
			<< ";" << "depart=" << ((SIPCCMsg_Client_GetDepartNAgentInfo*)&ipccMessage)->GetTheDepartment()
			<< ";" << "mode=" << ((SIPCCMsg_Client_GetDepartNAgentInfo*)&ipccMessage)->getMode
			<< ";" << "natIP=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();

		//g_Logger.MakeLog(sLog);
		this->OnClient_GetDepartNAgentInfo((SIPCCMsg_Client_GetDepartNAgentInfo*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_GetGroupRequest:
	{
		SIPCCMsg_Client_GetGroupRequest ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "Client_GetGroupRequest:"
			<< ";" << "invokeID=" << ipccMessage.invokeID
			<< ";" << "natIP=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();

		g_Logger.MakeLog(sLog);
		this->OnClient_GetGroupRequest((SIPCCMsg_Client_GetGroupRequest*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_MediaReady:
	{
		SIPCCMsg_Client_MediaReady ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "Client_MediaReady:"
			<< ";" << "targetName=" << ((SIPCCMsg_Client_MediaReady*)&ipccMessage)->GetTargetName()
			<< ";" << "natIP=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();

		g_Logger.MakeLog(sLog);
		this->OnClient_MediaReady((SIPCCMsg_Client_MediaReady*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_SysManage_DelACDCorp:
	{
		SIPCCMsg_SysManage_DelACDCorp ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "SysManage_DelACDCorp:" << "corpName="
			<< ((SIPCCMsg_SysManage_DelACDCorp*)&ipccMessage)->corpName
			<< ";" << "natip=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();

		g_Logger.MakeLog(sLog);
		this->OnSysManage_DelACDCorp((SIPCCMsg_SysManage_DelACDCorp*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Gateway_QueryGWConf:
	{
		SIPCCMsg_Gateway_QueryGWConf ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);
		this->OnGateway_QueryGWConf((SIPCCMsg_Gateway_QueryGWConf*)(&ipccMessage), ip, port);
	}
	break;
	case Type_Gateway_RingAgentExtConf:
	{
		SIPCCMsg_Gateway_RingAgentExtConf ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);
		this->OnGateway_RingAgentExtConf((SIPCCMsg_Gateway_RingAgentExtConf*)(&ipccMessage), ip, port);
	}
	break;
	case Type_Gateway_RingAgentExtFailed:
	{
		SIPCCMsg_Gateway_RingAgentExtFailed ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);
		this->OnGateway_RingAgentExtFailed((SIPCCMsg_Gateway_RingAgentExtFailed*)(&ipccMessage), ip, port);
	}
	break;
	case Type_Gateway_AgentAnswerCall:
	{
		SIPCCMsg_Gateway_AgentAnswerCall ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);
		this->OnGateway_AgentAnswerCall((SIPCCMsg_Gateway_AgentAnswerCall*)(&ipccMessage), ip, port);
	}
	break;
	case Type_Gateway_AgentHangupCall:
	{
		SIPCCMsg_Gateway_AgentHangupCall ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);
		this->OnGateway_AgentHangupCall((SIPCCMsg_Gateway_AgentHangupCall*)(&ipccMessage), ip, port);
	}
	break;
	case Type_Gateway_UserHangupCall:
	{
		SIPCCMsg_Gateway_UserHangupCall ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);
		this->OnGateway_UserHangupCall((SIPCCMsg_Gateway_UserHangupCall*)(&ipccMessage), ip, port);
	}
	break;
	case Type_Gateway_StopRingAgentExtConf:
	{
		SIPCCMsg_Gateway_StopRingAgentExtConf ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);
		this->OnGateway_StopRingAgentExtConf((SIPCCMsg_Gateway_StopRingAgentExtConf*)(&ipccMessage), ip, port);
	}
	break;
	case Type_Gateway_CancelRingAgentExt:
	{
		SIPCCMsg_Gateway_CancelRingAgentExt ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);
		this->OnGateway_CancelRingAgentExt((SIPCCMsg_Gateway_CancelRingAgentExt*)(&ipccMessage), ip, port);
	}
	break;
	case Type_Gateway_IsAgentLogin:
	{
		SIPCCMsg_Gateway_IsAgentLogin ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);
		this->OnGateway_IsAgentLogin((SIPCCMsg_Gateway_IsAgentLogin*)(&ipccMessage), ip, port);
	}
	break;
	case Type_Gateway_CallAgentExtFailed:
	{
		SIPCCMsg_Gateway_CallAgentExtFailed ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);
		this->OnGateway_CallAgentExtFailed((SIPCCMsg_Gateway_CallAgentExtFailed*)(&ipccMessage), ip, port);
	}
	break;
	case Type_Gateway_CallAgentFailed:
	{
		SIPCCMsg_Gateway_CallAgentFailed ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);
		this->OnGateway_CallAgentFailed((SIPCCMsg_Gateway_CallAgentFailed*)(&ipccMessage), ip, port);
	}
	break;
	case Type_Gateway_AgentExtCallSucc:
	{
		SIPCCMsg_Gateway_AgentExtCallSucc ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);
		this->OnGateway_AgentExtCallSucc((SIPCCMsg_Gateway_AgentExtCallSucc*)(&ipccMessage), ip, port);
	}
	break;
	case Type_Gateway_CallAgentConf:
	{
		SIPCCMsg_Gateway_CallAgentConf ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);
		this->OnGateway_CallAgentConf((SIPCCMsg_Gateway_CallAgentConf*)(&ipccMessage), ip, port);
	}
	break;
	case Type_Client_BeforeTransfer:
	{
		SIPCCMsg_Client_BeforeTransfer ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		this->OnClient_BeforeTransfer((SIPCCMsg_Client_BeforeTransfer*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Gateway_CallUserFailed_WpAgent:
	{
		SIPCCMsg_Gateway_CallUserFailed_WpAgent ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		this->OnGateway_CallUserFailed_WpAgent((SIPCCMsg_Gateway_CallUserFailed_WpAgent*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Gateway_CallCleared:
	{
		SIPCCMsg_Gateway_CallCleared ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		this->OnGateway_CallCleared((SIPCCMsg_Gateway_CallCleared*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Record_RecInfo2ACD:
	{
		SIPCCMsg_Record_RecInfo2ACD ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		this->OnRecord_RecInfo2ACD((SIPCCMsg_Record_RecInfo2ACD*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_GetTalkingAgent:
	{
		SIPCCMsg_Client_GetTalkingAgent ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		this->OnClient_GetTalkingAgent((SIPCCMsg_Client_GetTalkingAgent*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_SuperDisconnect:
	{
		SIPCCMsg_Client_SuperDisconnect ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		this->OnClient_SuperDisconnect((SIPCCMsg_Client_SuperDisconnect*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_MonitRequest:
	{
		SIPCCMsg_Client_MonitRequest ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		this->OnClient_MonitRequest((SIPCCMsg_Client_MonitRequest*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Gateway_GetDeptQueueNum:
	{
		SIPCCMsg_Gateway_GetDeptQueueNum ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		this->OnGateway_GetDeptQueueNum((SIPCCMsg_Gateway_GetDeptQueueNum*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Gateway_GetAgentState:
	{
		SIPCCMsg_Gateway_GetAgentState ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		this->OnGateway_GetAgentState((SIPCCMsg_Gateway_GetAgentState*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;

	case Type_Gateway_GetDeptWaitingAgentNum:
	{
		SIPCCMsg_Gateway_GetDeptWaitingAgentNum ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		this->OnGateway_GetDeptWaitingAgentNum((SIPCCMsg_Gateway_GetDeptWaitingAgentNum*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;

	case Type_Gateway_GetDeptLoginAgentNum:
	{
		SIPCCMsg_Gateway_GetDeptLoginAgentNum ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		this->OnGateway_GetDeptLoginAgentNum((SIPCCMsg_Gateway_GetDeptLoginAgentNum*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_SysManage_NotifyACDLicence:
	{
		SIPCCMsg_SysManage_NotifyACDLicence ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		this->OnGetACDLicence((SIPCCMsg_SysManage_NotifyACDLicence*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Gateway_GetDeptOverflow:
	{
		SIPCCMsg_Gateway_GetDeptOverflow ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		this->OnGetDeptOverflow((SIPCCMsg_Gateway_GetDeptOverflow*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_SendStrMsg:	//  [3/2/2009 lixg]
	{
		SIPCCMsg_Client_SendStrMsg ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		this->OnClientSendStrMsg((SIPCCMsg_Client_SendStrMsg*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;

	case Type_Client_ReturnToAgent:			//IVR转回 (->ACD服务器)
	{
		SIPCCMsg_Client_ReturnToAgent ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "Client_ReturnToAgent:" << "agentID="
			<< ((SIPCCMsg_Client_ReturnToAgent*)&ipccMessage)->agentID
			<< ";" << "IVRUUIData=" << ((SIPCCMsg_Client_ReturnToAgent*)&ipccMessage)->IVRUUIData
			<< ";" << "bIsSuccess=" << ((SIPCCMsg_Client_ReturnToAgent*)&ipccMessage)->bIsSuccess
			<< ";" << "natip=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();

		g_Logger.MakeLog(sLog);
		this->OnClient_ReturnToAgent((SIPCCMsg_Client_ReturnToAgent*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Operator2ACD_SendStrMsg:	//  [11/2/2009 lixg]
	{
		SIPCCMsg_Operator2ACD_SendStrMsg ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		this->OnOperatorSendStrMsg((SIPCCMsg_Operator2ACD_SendStrMsg*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Gateway_WpAgent_LineNo:	//  [11/2/2009 lixg]
	{
		SIPCCMsg_Gateway_WpAgent_LineNo ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		this->OnWpAgent_LineNo((SIPCCMsg_Gateway_WpAgent_LineNo*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Gateway_WpAgentRelease:	//  [11/2/2009 lixg]
	{
		SIPCCMsg_Gateway_WpAgentRelease ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		this->OnWpAgentRelease((SIPCCMsg_Gateway_WpAgentRelease*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Gateway_WpAgentCallAnswerred:
	{
		SIPCCMsg_Gateway_WpAgentCallAnswerred ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "Gateway_WpAgentCallAnswerred:" << "agentID="
			<< ((SIPCCMsg_Gateway_WpAgentCallAnswerred*)&ipccMessage)->agentID
			<< ";" << "gwAgentLineNo=" << ((SIPCCMsg_Gateway_WpAgentCallAnswerred*)&ipccMessage)->gwAgentLineNo
			<< ";" << "gwUserLineNo=" << ((SIPCCMsg_Gateway_WpAgentCallAnswerred*)&ipccMessage)->gwUserLineNo
			<< ";" << "gwAgentLineIp=" << ((SIPCCMsg_Gateway_WpAgentCallAnswerred*)&ipccMessage)->gwAgentLineIp
			<< ";" << "gwAgentLinePort=" << ((SIPCCMsg_Gateway_WpAgentCallAnswerred*)&ipccMessage)->gwAgentLinePort
			<< ";" << "gwUserLineIp=" << ((SIPCCMsg_Gateway_WpAgentCallAnswerred*)&ipccMessage)->gwUserLineIp
			<< ";" << "gwUserLinePort=" << ((SIPCCMsg_Gateway_WpAgentCallAnswerred*)&ipccMessage)->gwUserLinePort
			<< ";" << "natip=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();
		g_Logger.MakeLog(sLog);

		this->OnGateway_WpAgentCallAnswerred((SIPCCMsg_Gateway_WpAgentCallAnswerred*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_OutDial_SetOutdialGroup:
	{
		SIPCCMSG_OutDial_SetOutdialGroup ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "OutDial_SetOutdialGroup:" << "GroupName=" << ipccMessage.GroupName
			<< ";" << "natip=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();
		g_Logger.MakeLog(sLog);

		this->OnOutDial_SetOutdialGroup((SIPCCMSG_OutDial_SetOutdialGroup*)(&ipccMessage), ip, port);
	}
	break;
	case Type_OutDial_ExitOutDial:
	{
		SIPCCMSG_OutDial_ExitOutDial ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "SIPCCMSG_OutDial_ExitOutDial:" << "AgentId=" << ipccMessage.AgentId
			<< ";" << "natip=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();
		g_Logger.MakeLog(sLog);

		this->OnOutDial_ExitOutDial((SIPCCMSG_OutDial_ExitOutDial*)(&ipccMessage), ip, port);
	}
	break;
	case Type_OutDial_JoinOutDial:
	{
		SIPCCMSG_OutDial_JoinOutDial ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "SIPCCMSG_OutDial_JoinOutDial:" << "AgentId=" << ipccMessage.AgentId
			<< ";" << "natip=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();
		g_Logger.MakeLog(sLog);

		this->OnOutDial_JoinOutDial((SIPCCMSG_OutDial_JoinOutDial*)(&ipccMessage), ip, port);
	}
	break;
	case Type_SysManage_MoveAgent:
	{
		SIPCCMSG_SysManage_MoveAgent ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "SIPCCMSG_SysManage_MoveAgent:" << "DeptmentId=" << ipccMessage.DeptmentId
			<< ";AgentList=" << ipccMessage.AgentList
			<< ";IsOutdial=" << ipccMessage.IsOutdial
			<< ";" << "natip=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();
		g_Logger.MakeLog(sLog);

		this->OnSysManage_MoveAgent((SIPCCMSG_SysManage_MoveAgent*)(&ipccMessage), ip, port);
	}
	break;
	case Type_Licence_QueryConf:
	{
		SIPCCMsg_Licence_QueryConf ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "SIPCCMsg_Licence_QueryConf:"
			<< ";" << "queryTag=" << ipccMessage.queryTag
			<< ";" << "FailedCause=" << ipccMessage.FailedCause
			<< ";" << "LicenceConfirmPassed=" << (bool)(ipccMessage.LicenceConfirmPassed)
			<< ";" << "natIP=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();

		g_Logger.MakeLog(sLog);
		this->OnLicence_QueryConf((SIPCCMsg_Licence_QueryConf*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Gateway_ConferenceStartConf: //add code by hyj 2012-12-12
	{

		SIPCCMsg_Gateway_ConferenceStartConf ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage);

		this->OnGateway_ConferenceStartConf(ipccMessage, ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_Client_ConferenceEnd:
	{

		SIPCCMsg_Client_ConferenceEnd       ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage);

		OnClient_ConferenceEnd(ipccMessage, ip, port);
	}
	break;
	case   Type_Gateway_UserLeaveConference:
	{
		SIPCCMsg_Gateway_UserLeaveConference      ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage);
		OnGateway_UserLeaveConference(ipccMessage, ip, port);
	}
	break;
	//todo
	////////////case Type_OneKeySwitch_ServerHeartBeatConf:
	////////////	{
	////////////		SIPCCMsg_OneKeySwitch_ServerHeartBeatConf msg;
	////////////		msgExchanger.DeSerializeMsg(pmsg , msgsize, msg);
	////////////		OnServerHeartBeatConf(msg, ip, port);
	////////////	}
	////////////	break;
	case Type_SysManage_AddSkillGroup:		//增加部门(发送给ACD服务器)
	{
		SIPCCMsg_SysManage_AddSkillGroup ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "SysManage_AddSkillGroup:"
			<< "managerid=" << ipccMessage.managerid
			<< ";SGId=" << ipccMessage.SGId
			<< ";SGName=" << ipccMessage.SGName
			<< ";queuingAlgorithm="<<ipccMessage.queuingAlgorithm
			<< ";" << "natip=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();

		g_Logger.MakeLog(sLog);
		this->OnAddSkillGroup((SIPCCMsg_SysManage_AddSkillGroup*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_SysManage_DelSkillGroup:		//删除部门(发送给ACD服务器)
	{
		SIPCCMsg_SysManage_DelSkillGroup ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "SysManage_DelSkillGroup:"
			<< ";SGId=" << ipccMessage.SGId
			<< ";SGName=" << ipccMessage.SGName
			<< ";" << "natip=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();

		g_Logger.MakeLog(sLog);
		this->OnDelSkillGroup((SIPCCMsg_SysManage_DelSkillGroup*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_PhoneSearch_SMSInfoConf:
	{
		SIPCCMsg_PhoneSearch_SMSInfoConf ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "PhoneSearch_SMSInfoConf:" << "sHandleId="
			<< ((SIPCCMsg_PhoneSearch_SMSInfoConf*)&ipccMessage)->sHandleId
			<< ";" << "natip=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();

		g_Logger.MakeLog(sLog);
		this->OnPhoneSearch_SMSInfoConf((SIPCCMsg_PhoneSearch_SMSInfoConf*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_SysManage_AgentInfoChange:
	{
		SIPCCMsg_SysManage_AgentInfoChange ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "SysManage_AgentInfoChange:" << "agentName = "
			<< ((SIPCCMsg_SysManage_AgentInfoChange*)&ipccMessage)->agentName
			<< ";" << "cause=" << ((SIPCCMsg_SysManage_AgentInfoChange*)&ipccMessage)->cause
			<< ";" << "natip=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();

		g_Logger.MakeLog(sLog);
		this->OnSysManage_AgentInfoChange((SIPCCMsg_SysManage_AgentInfoChange*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	/*case Type_Redirect_GetServiceConf:
	{
	SIPCCMsg_Redirect_GetServiceConf ipccMessage;
	msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

	this->OnRedirect_GetServiceConf(ipccMessage, ip, port);

	ipccMessage.ReleaseMemory();
	}
	break;*/
	case Type_FileTransfer_UploadRequest:
	{
		SIPCCMsg_FileTransfer_UploadRequest ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		sLog = "";
		ostream.str("");
		ostream << "FileTransfer_UploadRequest:" << "agentID = "
			<< ((SIPCCMsg_FileTransfer_UploadRequest*)&ipccMessage)->agentID
			<< ";" << "description = " << ((SIPCCMsg_FileTransfer_UploadRequest*)&ipccMessage)->description
			<< ";" << "natip=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();

		g_Logger.MakeLog(sLog);
		this->OnFileTransfer_UploadRequest((SIPCCMsg_FileTransfer_UploadRequest*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_CProxy_CMakeCall:
	{
		SIPCCMsg_CProxy_CMakeCall ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);
		g_Logger.MakeLog(to_str(ipccMessage));

		this->OnCProxy_CMakeCall((SIPCCMsg_CProxy_CMakeCall*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_CProxy_HangupCall:
	{
		SIPCCMsg_CProxy_HangupCall ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

        g_Logger.MakeLog(to_str(ipccMessage));
		this->OnCProxy_HangupCall((SIPCCMsg_CProxy_HangupCall*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
	case Type_OneKeySwitch_NotifyMsg:
	{
		SIPCCMsg_OneKeySwitch_NotifyMsg ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		g_Logger.MakeLog(to_str(ipccMessage));

		this->OnNotifyMsg((SIPCCMsg_OneKeySwitch_NotifyMsg*)(&ipccMessage), ip, port);

		ipccMessage.ReleaseMemory();
	}
	break;
    case Type_OneKeySwitch_ServiceUnavailable:
    {
        SIPCCMsg_OneKeySwitch_ServiceUnavailable ipccMessage;
        msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

        g_Logger.MakeLog(to_str(ipccMessage));

        this->On_SIPCCMsg_OneKeySwitch_ServiceUnavailable(&ipccMessage, ip, port);

    }
    break;
    case Type_Client_QueryAgentsState:
    {
        SIPCCMsg_Client_QueryAgentsState ipccMessage;
        msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

        this->On_SIPCCMsg_Client_QueryAgentsState(&ipccMessage, ip, port);

    }
	break;
	case Type_Client_AgentSwitchToFrontDesk:
	{
		SIPCCMsg_Client_AgentSwitchToFrontDesk ipccMessage;
		msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

		this->OnClient_AgentSwitchToFrontDesk(&ipccMessage, ip, port);

	}
    break;
    case Type_MakeCallViaClient:
    {
        SIPCCMsg_MakeCallViaClient ipccMessage;
        msgExchanger.DeSerializeMsg(pmsg, msgsize, ipccMessage, ip, port);

        g_Logger.MakeLog(to_str(ipccMessage));
        this->OnMakeCallViaClient(&ipccMessage, ip, port);
    }
    break;
    default:
    {
        ostream.str("");
        ostream << "Invalid Message ,  msgClass= " << msgClass
            << " msgType= " << msgType;

#ifdef __GNUC_LINUX__
        extern const char* VERSION;    
        ostream << " Current ACDServer Version " << VERSION;
#endif // __GNUC_LINUX__
        
        g_Logger.MakeLog(ostream.str());
    }
		break;
	}

	return true;
}

void CUtility::OnSetOutdialCase(SIPCCMsg_Client_SetOutdialCase* pMsg, unsigned long ip, unsigned short port)
{
	CDepartmentInfo* pDept = CCorporationMgr::GetInstance()->GetDepartmentInfo(pMsg->corp, pMsg->dept);
	SIPCCMsg_ACD_SetTalkSpan msg;
	msg.dept = pDept->m_sDeptid;
	msg.caseId = pMsg->caseId;
	msg.span = pMsg->talkNProcessSpan;
	SendIPCCMsg_UDP(msg, pDept->m_OutDialSys_Info.OutdialEngIp, pDept->m_OutDialSys_Info.OutdialEngPort);

	ostringstream os;
	os << "SendTalkSpan; dept="
		<< msg.dept
		<< "; span=" << msg.span
		<< "; caseId=" << msg.caseId
		<< "; ip=" << pDept->m_OutDialSys_Info.OutdialEngIp
		<< "; port=" << pDept->m_OutDialSys_Info.OutdialEngPort;
	g_Logger.MakeLog(os.str());
}

void CUtility::OnAddDepartment(SIPCCMsg_SysManage_AddDepartment *addDepartment, unsigned long ip, unsigned short port)
{
	m_SysManageServerTime=0;
	string sTempStr = addDepartment->GetTheDepartment();
	string sCorp = "";
	string sDepart = "";

	if(!SplitCorpAndDepart(sTempStr, sCorp, sDepart))
		return;

	if(sCorp.length() <= 0 || sDepart.length() <= 0)
		return;

	CCorporationInfo* corp;
	corp = CCorporationMgr::GetInstance()->GetCorporation(sCorp);
	if(corp == NULL)
	{
		CCorporationMgr::GetInstance()->AddCorporationInfo(sCorp, addDepartment->managerid);
		corp = CCorporationMgr::GetInstance()->GetCorporation(sCorp);
		if (corp == NULL)
		{
			return;
		}
	}

	bool bIsReLoad = false;
	CDepartmentInfo* pDept = corp->AddDepartment(sDepart, addDepartment->deptid, &bIsReLoad);
	if (pDept)
	{
		if ( addDepartment->timeoutValue > 0 )
		{
			pDept->m_iOverflowTime = addDepartment->timeoutValue * 1000;
		}
		else
		{
			pDept->m_iOverflowTime = 0;
		}
		
		pDept->m_sOverflowTarget = addDepartment->overFlowDest;
		pDept->m_iOverFlowDestType = addDepartment->overFlowDestType;

		if ( addDepartment->maxRingTime > 0 )
		{
			pDept->m_iMaxRingTime = addDepartment->maxRingTime * 1000;
		}
		else
		{
			pDept->m_iMaxRingTime = 0;
		}

		pDept->m_iDeptType = addDepartment->deptType;

		pDept->m_OutDialSys_Info.bIsOutDialGroup = (addDepartment->isOutDial==0)?false:true;
		pDept->m_OutDialSys_Info.GroupId = addDepartment->outdialGroupID;
		pDept->m_OutDialSys_Info.OutdialEngIp = (addDepartment->dialEngineIP == "") ? m_sOutdialIp : ConvertIP(addDepartment->dialEngineIP);
		pDept->m_OutDialSys_Info.OutdialEngPort = (addDepartment->dialEnginePort == 0) ? m_sOutdialPort : htons(addDepartment->dialEnginePort);

		pDept->m_sManagerid = addDepartment->managerid;
		pDept->m_sDeptid = addDepartment->deptid;

		pDept->m_sNickname = addDepartment->nickname;
        pDept->SetQueuingAlgorithm(addDepartment->queuingAlgorithm);
	}

	if ( !bIsReLoad )
	{
		SIPCCMsg_ACD_AddCorpNDeptInfo  AddCorpNDeptInfo;
		AddCorpNDeptInfo.corpName = sCorp;
		AddCorpNDeptInfo.deptName = sDepart;
		
		CUtility::GetInstance()->SendToMonitorServer(AddCorpNDeptInfo);
	}	
		
	SIPCCMsg_ACD_AddDepartmentConf addDepartmentConf;
	addDepartmentConf.invokeID = addDepartment->invokeID;
	addDepartmentConf.SetTheDepartment(addDepartment->GetTheDepartment());
	this->SendIPCCMsg_UDP(addDepartmentConf, ip, port);
}

void CUtility::OnUserMakeCall(SIPCCMsg_Client_UserMakeCall *userMakeCall, unsigned long ip, unsigned short port, bool bIsOverflow)
{
	string sDepartMent = userMakeCall->GetTheDepartment();

	if(sDepartMent.find("Agent^") == -1)
		func_UserCallDepart(userMakeCall, ip, port, bIsOverflow);
	else
		func_UserCallAgent(userMakeCall, ip, port, bIsOverflow);
}

void CUtility::OnUserHangupCall(SIPCCMsg_Client_UserHangupCall *userHangupCall, unsigned long ip, unsigned short port)
{
	CSkillGroupInfo* pSkillGroupInfo = NULL;
	CDepartmentInfo* pDepartmentInfo = NULL; 
    CUserInfo* pUserInfo = NULL;
    CAgentInfo* pAgentInfo = NULL;

	if(userHangupCall->bAgentAssigned == 0) {	//未分配座席
		pSkillGroupInfo = CCorporationMgr::GetInstance()->GetSkillGroupInfo(userHangupCall->GetTheOtherParty());
		if ( pSkillGroupInfo == NULL ) {
			pDepartmentInfo = CCorporationMgr::GetInstance()->GetDepartmentInfo(userHangupCall->GetTheOtherParty());
		}

		pUserInfo = CUserMgr::GetInstance()->GetUserInfo(userHangupCall->GetUserName());

        if (pUserInfo == NULL) {
            string sAgent = CAgentMgr::GetInstance()->m_mapUserCallAgent[userHangupCall->GetUserName()];
            if (!sAgent.empty()) {
                pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(sAgent);
                if (pAgentInfo != NULL && pAgentInfo->m_userInfo.m_sUserName != userHangupCall->GetUserName()) {
                    pUserInfo = &pAgentInfo->m_userInfo;
                }
            }
        }
	}
    else {
        pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(userHangupCall->GetTheOtherParty());
        pUserInfo = &pAgentInfo->m_userInfo;

        g_Logger.MakeLog("UserInfo:" + pUserInfo->ToString());

        if (pUserInfo->m_iQueueType == 2){
            pSkillGroupInfo = CCorporationMgr::GetInstance()->GetSkillGroupInfo(pUserInfo->m_sCorpName, pUserInfo->m_sQueueID);
        }
        else if(pUserInfo->m_iQueueType == 1){
            pDepartmentInfo = CCorporationMgr::GetInstance()->GetDepartmentInfo(pUserInfo->m_sCorpName, pUserInfo->m_sQueueName);
        }
        else {
            pDepartmentInfo = CCorporationMgr::GetInstance()->GetDepartmentInfo(pAgentInfo->m_sCorporation, pAgentInfo->m_sDepartment);
        }
    }

    if ((pDepartmentInfo != NULL || pSkillGroupInfo != NULL) &&
        (pAgentInfo == NULL || pAgentInfo->m_bCallFromVDN)
        ){
        SIPCCMsg_ACD_CallLeaveVDN CallLeaveVDN;
        CallLeaveVDN.vdnNo = pUserInfo->m_sQueueName;
        CallLeaveVDN.corpName = pUserInfo->m_sCorpName;
        CallLeaveVDN.callID = pUserInfo->m_CallID_sCallIn;
        CallLeaveVDN.UserLevel = pUserInfo->m_iUserLevel;
        CallLeaveVDN.leaveTime = CLog::GetLocalTime();
        CallLeaveVDN.LeaveQueueTime = CLog::GetLocalTime();

        if (pAgentInfo) {
            if (!pAgentInfo->m_sQueueArriveTime.empty()) {
                CallLeaveVDN.LeaveQueueTime = CLog::GetLocalTime();
            }
            CallLeaveVDN.agentID = pAgentInfo == NULL ? "" : pAgentInfo->m_sAgentID;
        }
        
        if (pSkillGroupInfo != NULL) {
            CallLeaveVDN.SkillGroupID = pUserInfo->m_sQueueID;
            CallLeaveVDN.SkillGroupName = pUserInfo->m_sQueueName;
            CallLeaveVDN.managerid = pSkillGroupInfo->m_sManagerid;
            CallLeaveVDN.CurQueueNum = pSkillGroupInfo->GetCQueueSize();
        }
        else {//pDepartmentInfo 必不为空
            CallLeaveVDN.deptid = pUserInfo->m_sQueueID;
            CallLeaveVDN.managerid = pDepartmentInfo->m_sManagerid;
            CallLeaveVDN.CurQueueNum = pDepartmentInfo->GetCQueueSize();
        }

        if (userHangupCall->bIvrHangupCall == 0) {
            CallLeaveVDN.leaveCause = VDNEndCause_CallAbandon;
        }
        else {
            CallLeaveVDN.leaveCause = VDNEndCause_Overflow;
        }

        CUtility::GetInstance()->SendIPCCMsg_UDP(
            CallLeaveVDN,
            CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_IP,
            CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_Port);

        CUtility::GetInstance()->SendToMonitorServer(CallLeaveVDN);
    }

    if (pAgentInfo) {
        if (!pAgentInfo->IsPC()){
            pAgentInfo->delCallDelivered(pAgentInfo->m_sCallID);
        }

        if (pAgentInfo->m_iCurState == Agent_Ringing) {
            if (pAgentInfo->m_iLoginType != LoginType_OnlyWeb) {
                //StopRing
                SIPCCMsg_ACD_StopRingAgentExt StopRingAgentExt;
                StopRingAgentExt.agentID = pAgentInfo->m_sAgentID;
                StopRingAgentExt.ipNo_AgentExt = pAgentInfo->m_sIPNo_TheAgent;
                StopRingAgentExt.handleId = userHangupCall->handleId;		//by heyj2011-08-15
                StopRingAgentExt.userName = pAgentInfo->m_userInfo.m_sUserName;
                CUtility::GetInstance()->SendIPCCMsg_UDP(StopRingAgentExt, ip, port);
            }
        }

        pAgentInfo->m_bPSTNHungup = true;

        //发送 CallReleased 消息给座席
        SIPCCMsg_ACD_CallReleased callReleased;
        callReleased.SetReleasedParty(userHangupCall->GetUserName());
        callReleased.callID = pAgentInfo->m_sCallID;
        callReleased.toAgentID = pAgentInfo->m_sAgentID;
        this->SendIPCCMsg_UDP(callReleased, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);
    }

    SIPCCMsg_ACD_CallHanguped callHanguped;
    callHanguped.SetUserName(userHangupCall->GetUserName());
    callHanguped.SetReleasedParty(userHangupCall->GetTheOtherParty());
    callHanguped.handleId = userHangupCall->handleId;//by heyj 2011-08-15
    this->SendIPCCMsg_UDP(callHanguped, ip, port);

    //从 callingList 中删除用户
    if (pDepartmentInfo){
        pDepartmentInfo->RemoveFromCQueue(userHangupCall->GetUserName());
    }
    else if (pSkillGroupInfo) {
        pSkillGroupInfo->RemoveFromCQueue(userHangupCall->GetUserName());
    }

    CAgentMgr::GetInstance()->m_mapUserCallAgent[userHangupCall->GetUserName()] = "";
}

void CUtility::OnAgentLogin(SIPCCMsg_Client_AgentLogin *agentLogin, unsigned long ip, unsigned short port)
{
	//log
	string sLog;
	ostringstream ostream;
	ostream.str("");

	string sTempStr = agentLogin->GetTheDepartment();
	string sCorp = "";
	string sDepart = "";
	string sSGId = agentLogin->SkillGroupID;
	int iLoginType = agentLogin->loginType;
	string sAgentPhoneNo = agentLogin->agentPhoneNo;

	if( agentLogin->GetAgentID() == "" )
	{
		SIPCCMsg_ACD_AgentLoginFailed agentLoginFailed;
		agentLoginFailed.failedCause = "AgentID为空";
		agentLoginFailed.handleId=agentLogin->handleId ;//by heyj 2011-08-15
		this->SendIPCCMsg_UDP(agentLoginFailed, ip, port);
		return;
	}

	if(!SplitCorpAndDepart(sTempStr, sCorp, sDepart))
	{
		//返回登录失败的消息给座席
		SIPCCMsg_ACD_AgentLoginFailed agentLoginFailed;
		agentLoginFailed.handleId=agentLogin->handleId ;//by heyj 2011-08-15
		this->SendIPCCMsg_UDP(agentLoginFailed, ip, port);
		return;
	}

	//察看 corp
	CCorporationInfo* corp = CCorporationMgr::GetInstance()->GetCorporation(sCorp);
	if(corp == NULL)
	{
		//返回登录失败的消息给座席
		SIPCCMsg_ACD_AgentLoginFailed agentLoginFailed;
		agentLoginFailed.agentId = agentLogin->GetAgentID();
		agentLoginFailed.handleId=agentLogin->handleId ;//by heyj 2011-08-15
		this->SendIPCCMsg_UDP(agentLoginFailed, ip, port);
		return;
	}

	//察看 department 是否存在
	CDepartmentInfo* pDepartmentInfo = 
		CCorporationMgr::GetInstance()->GetDepartmentInfo(sCorp, sDepart);    
	if(NULL == pDepartmentInfo)
	{
		//返回登录失败的消息给座席
		SIPCCMsg_ACD_AgentLoginFailed agentLoginFailed;
		agentLoginFailed.agentId = agentLogin->GetAgentID();
		agentLoginFailed.handleId=agentLogin->handleId;
		this->SendIPCCMsg_UDP(agentLoginFailed, ip, port);
		return;
	}
	
    //1、重登处理
    //  1.1、保留状态，条件：非强制重置
    //       如果状态不同以客户端登录状态为准同时根据新状态更新队列
    //       登录完成返回成功
    //
    //  1.2、不保留状态，条件：强制重新登录
    //       踢掉原客户端
    //       转3处理
    //
    //2、登录处理
    //  创建数据结构，进入排队队列或者分配呼叫

    CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(agentLogin->GetAgentID());
    bool exist = (pAgentInfo != NULL);
    if (exist
        && CUtility::GetInstance()->m_IPCCConfig.bIsEnterpriseVersion
        && !CUtility::GetInstance()->m_IPCCConfig.bAllowForceLogout
        && pAgentInfo->GetCurState() != Agent_Waiting
        && pAgentInfo->GetCurState() != Agent_Parking
        && pAgentInfo->GetCurState() != Agent_ForeProcess
        && pAgentInfo->GetCurState() != Agent_BackProcess)
    {
        //返回登录失败的消息给座席--已经有座席登陆且状态不允许被剔出
        SIPCCMsg_ACD_AgentLoginFailed agentLoginFailed;
        agentLoginFailed.agentId = agentLogin->GetAgentID();
        agentLoginFailed.failedCause = "此座席已登录，且处于工作状态";
        agentLoginFailed.handleId = agentLogin->handleId;
        this->SendIPCCMsg_UDP(agentLoginFailed, ip, port);
        return;
    }

//加密狗相关，使用加密狗的项目几乎没有了 ，此段代码没有验证过 
#ifndef NO_DOG
	if(CAgentMgr::GetInstance()->GetAgentCount() >= m_iAgentsLicence)
	{
		//没有足够许可
		SIPCCMsg_ACD_AgentLoginFailed agentLoginFailed;
		agentLoginFailed.agentId = agentLogin->GetAgentID();
		agentLoginFailed.failedCause = "没有足够许可";
		agentLoginFailed.handleId=agentLogin->handleId ;//by heyj 2011-08-15

		this->SendIPCCMsg_UDP(agentLoginFailed, ip, port);
		ostream << "SIPCCMsg_ACD_AgentLoginFailed(step5-没有足够许可):" << "agentID=" << agentLogin->GetAgentID()
			<< ";" << "corpNdepart=" << sTempStr
			<< ";" << "corp=" << sCorp
			<< ";" << "depart=" << sDepart
			<< ";" << "loginType=" << iLoginType
			<< ";" << "agentPhoneNo=" << sAgentPhoneNo
			<< ";" << "natip=" << format_ip_addr(ip)
			<< ";" << "natport=" << ntohs(port);
		sLog = ostream.str();
		g_Logger.MakeLog( sLog);
		return;
	}

	bool bOutLicence = false;
	if ( !m_IPCCConfig.bIsEnterpriseVersion )
	{
		if (iLoginType == LoginType_OnlyPSTN)
		{
			if (corp->GetMobileAgentCount() >= corp->m_dynamicMobileAgentLicence)
			{
				bOutLicence = true;
			}
		}
		else
		{
			if (corp->GetWebAgentCount() >= corp->m_dynamicWebAgentLicence)
			{
				bOutLicence = true;
			}
		}
	}

	if(bOutLicence)
	{
		//没有足够许可
		SIPCCMsg_ACD_AgentLoginFailed agentLoginFailed;
		agentLoginFailed.agentId = agentLogin->GetAgentID();
		agentLoginFailed.failedCause = "没有足够许可";
		agentLoginFailed.handleId=agentLogin->handleId ;//by heyj 2011-08-15

		this->SendIPCCMsg_UDP(agentLoginFailed, ip, port);
		ostream << "SIPCCMsg_ACD_AgentLoginFailed(step55-没有足够许可):" << "agentID=" << agentLogin->GetAgentID()
			<< ";" << "corpNdepart=" << sTempStr
			<< ";" << "corp=" << sCorp
			<< ";" << "depart=" << sDepart
			<< ";" << "loginType=" << iLoginType
			<< ";" << "agentPhoneNo=" << sAgentPhoneNo
			<< ";" << (iLoginType == LoginType_OnlyPSTN ? "m_dynamicMobileAgentLicence=" : "m_dynamicWebAgentLicence=")
			<< (iLoginType == LoginType_OnlyPSTN ? corp->m_dynamicMobileAgentLicence : corp->m_dynamicWebAgentLicence)
			<< ";" << (iLoginType == LoginType_OnlyPSTN ? "MobileAgentCount=" : "WebAgentCount=")
			<< (iLoginType == LoginType_OnlyPSTN ? corp->GetMobileAgentCount() : corp->GetWebAgentCount())
			<< ";" << "natip=" << format_ip_addr(ip)
			<< ";" << "natport=" << ntohs(port);
		sLog = ostream.str();
		g_Logger.MakeLog( sLog);
		return;
	}
#endif

    bool isStateInvalid = (pAgentInfo && (agentLogin->iLoginSuccMode != pAgentInfo->GetCurState()));

    //1、重登处理
    //  1.1、保留状态，条件：非强制重置
    //       如果状态不同以客户端登录状态为准同时根据新状态更新队列
    //       登录完成返回成功
    if (pAgentInfo && !agentLogin->resetServerState) {
        SIPCCMsg_ACD_AgentLoginSucc agentLoginSucc;
        agentLoginSucc.iLoginSuccMode = agentLogin->iLoginSuccMode;
        agentLoginSucc.agentId = agentLogin->GetAgentID();
        agentLoginSucc.bAgentExist = 1;
        agentLoginSucc.handleId = agentLogin->handleId;
        agentLoginSucc.deptNickname = pDepartmentInfo->m_sNickname;
        this->SendIPCCMsg_UDP(agentLoginSucc, ip, port);

        if (isStateInvalid) {
            pAgentInfo->SetState(agentLogin->iLoginSuccMode);
        }

        return; 
    }

    //  1.2、不保留状态，条件：强制重新登录
    //       踢掉原客户端
    //       转3处理
    if (exist){
        //强制迁出处理
        if (pAgentInfo->m_ulAgentIP != ip
            || pAgentInfo->m_usAgentPort != port) {
            SIPCCMsg_ACD_ForceLogout forceLogout;
            forceLogout.cause = ForceLogoutCause_ReLogin;
            forceLogout.agentName = agentLogin->GetAgentID();

            SendIPCCMsg_UDP(forceLogout, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);
        }

        pAgentInfo->SetState(Agent_NoLogin);

        CAgentMgr::GetInstance()->GetInstance()->DelAgent(agentLogin->GetAgentID());
        pAgentInfo = NULL;
    }

    //2、登录处理
    //  创建数据结构，进入排队队列或者分配呼叫
    pAgentInfo = CAgentMgr::GetInstance()->AddAgent(agentLogin->GetAgentID(), agentLogin->nickName, sCorp,
        sDepart, agentLogin->natType, ip, port, agentLogin->lanIP, agentLogin->lanMainSockPort, iLoginType,
        sAgentPhoneNo, agentLogin->iNeedDisposal, pDepartmentInfo->m_sManagerid, pDepartmentInfo->m_sDeptid,
        agentLogin->extNo, pDepartmentInfo->m_sNickname, agentLogin->allowAudioCall, agentLogin->allowTXTCall,
        agentLogin->maxTXTDialogCount, agentLogin->loginPlatform, sSGId, agentLogin->HaveHBtoACD);

    if (pAgentInfo == NULL)
    {
        SIPCCMsg_ACD_AgentLoginFailed agentLoginFailed;
        agentLoginFailed.agentId = agentLogin->GetAgentID();
        agentLoginFailed.failedCause = "登陆异常，请稍后重试";
        agentLoginFailed.handleId = agentLogin->handleId;

        this->SendIPCCMsg_UDP(agentLoginFailed, ip, port);
        return;
    }

    pAgentInfo->SetTXTSessionTimeout(agentLogin->txtSessionTimeout);
    pAgentInfo->SetDeviceToken(agentLogin->deviceToken);

    int agentstate = agentLogin->iLoginSuccMode == 0 ? Agent_Waiting : agentLogin->iLoginSuccMode;
    SIPCCMsg_ACD_AgentLoginSucc agentLoginSucc;
    agentLoginSucc.iLoginSuccMode = agentstate;
    agentLoginSucc.agentId = agentLogin->GetAgentID();
    agentLoginSucc.bAgentExist = 0;
    agentLoginSucc.handleId = agentLogin->handleId;
    agentLoginSucc.deptNickname = pDepartmentInfo->m_sNickname;
    this->SendIPCCMsg_UDP(agentLoginSucc, ip, port);

    pAgentInfo->SetState(agentstate, true);
}

void CUtility::OnAgentLogout(SIPCCMsg_Client_AgentLogout *agentLogout, unsigned long ip, unsigned short port)
{
	//获取座席
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(agentLogout->GetAgentID());
	if(pAgentInfo == NULL)
	{
		//返回 AgentLogoutConf消息
		SIPCCMsg_ACD_AgentLogoutConf agentLogoutConf;
		agentLogoutConf.agentId = agentLogout->GetAgentID();
		agentLogoutConf.handleId=agentLogout->handleId; //by heyj 2011-08-15
		this->SendIPCCMsg_UDP(agentLogoutConf, ip, port);
		return;
	}
	///////begin  conference ///////////////////////////////////////////////////////////////////////////////////////////////	
	int  nconferenceresult= ConferenceAgentHuageup(pAgentInfo,ip,  port,"[SIPCCMsg_Client_AgentLogout]",true,false);	
	///////end  conference ///////////////////////////////////////////////////////////////////////////////////////////////

	if ( pAgentInfo->m_iCurState == Agent_Talking && pAgentInfo->bPSTNCall )
	{
		SIPCCMsg_Client_AgentHangupCall AgentHangupCall;
		AgentHangupCall.SetAgentID(agentLogout->GetAgentID());

		//if( pLogedAgent->m_iLoginType == LoginType_OnlyPSTN )
		if ( pAgentInfo->bCallin )
		{
			AgentHangupCall.SetTheOtherParty(pAgentInfo->m_userInfo.m_sUserName);
		}
		else
		{
			AgentHangupCall.SetTheOtherParty(pAgentInfo->m_sCalledParty);
		}

		this->SendIPCCMsg_UDP(AgentHangupCall, m_IPCCConfig.UStateServer_IP, m_IPCCConfig.UStateServer_Port);

		ostringstream ostream;
		ostream << "=+= When agent is talking, agent logout! =+= :" << "agentID=" << agentLogout->GetAgentID();
		string sLog = ostream.str();
		g_Logger.MakeLog( sLog);
		ostream.str("");
	}

	if(agentLogout->loginType == LoginType_OnlyPSTN)
	{
		if(pAgentInfo->m_iLoginType == LoginType_OnlyWeb || pAgentInfo->m_iLoginType == LoginType_WebPSTN)
		{
			SIPCCMsg_ACD_ForceLogout forceLogout;
			forceLogout.cause = ForceLogoutCause_ReLogin;
			forceLogout.agentName = agentLogout->GetAgentID();
			this->SendIPCCMsg_UDP(forceLogout, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

			//log
			string sLog;
			ostringstream ostream;
			ostream.str("");
			ostream << "OnAgentLogout, Agent ForceLogout:" << "name=" << agentLogout->GetAgentID()
				<< ";" << "ip=" << pAgentInfo->m_ulAgentIP
				<< ";" << "port=" << pAgentInfo->m_usAgentPort;
			sLog = ostream.str();
			g_Logger.MakeLog( sLog);
		}
	}

	//返回 AgentLogoutConf消息
	SIPCCMsg_ACD_AgentLogoutConf agentLogoutConf;
	agentLogoutConf.agentId = agentLogout->GetAgentID();
	agentLogoutConf.handleId=agentLogout->handleId; //by heyj 2011-08-15
	this->SendIPCCMsg_UDP(agentLogoutConf, ip, port);

    //置座席状态为 Agent_NoLogin
    CAgentMgr::GetInstance()->SetAgentState(agentLogout->GetAgentID(), Agent_NoLogin);

	//从 agentMap 中删除座席
	CAgentMgr::GetInstance()->DelAgent(agentLogout->GetAgentID());
}

void CUtility::OnAgentLogoutDeparts(SIPCCMsg_Client_AgentLogoutDeparts *agentLogout, unsigned long ip, unsigned short port)
{
	// log
	string sLog;
	ostringstream ostream;
	ostream.str("");

	// 获取座席
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(agentLogout->GetAgentID());
	if ( NULL==pAgentInfo )
	{
		// 返回注销失败消息给座席
		SIPCCMsg_ACD_AgentLogoutDepartsConf agentLogoutConf;
		agentLogoutConf.agentId = agentLogout->GetAgentID();
		agentLogoutConf.theDepartmentFailed = agentLogout->GetTheDepartment();
		agentLogoutConf.theDepartmentSuccess = "";
		agentLogoutConf.bLogoutSuccess = false;
		this->SendIPCCMsg_UDP(agentLogoutConf, ip, port);
		ostream << "SIPCCMsg_ACD_AgentLogoutDepartsFailed(获取座席失败):" << "agentID=" << agentLogout->GetAgentID()
			<< ";" << "natip=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();
		g_Logger.MakeLog( sLog);
		return;
	}

	// 解析公司和多个技能组ID(公司名称%^#技能组ID|…|技能组ID)
	string sTempStr = agentLogout->GetTheDepartment();
	string sCorp = "";
	string sSkillGroup = "";
	if ( !SplitCorpAndDepart(sTempStr, sCorp, sSkillGroup) )
	{
		// 返回注销失败消息给座席
		SIPCCMsg_ACD_AgentLogoutDepartsConf agentLogoutConf;
		agentLogoutConf.agentId = agentLogout->GetAgentID();
		agentLogoutConf.theDepartmentFailed = agentLogout->GetTheDepartment();
		agentLogoutConf.theDepartmentSuccess = "";
		agentLogoutConf.bLogoutSuccess = false;
		this->SendIPCCMsg_UDP(agentLogoutConf, ip, port);
		ostream << "SIPCCMsg_ACD_AgentLogoutDepartsFailed(step1-企业,分组字串解析失败):" << "agentID=" << agentLogout->GetAgentID()
			<< ";" << "corpNdepart=" << sTempStr
			<< ";" << "natip=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();
		g_Logger.MakeLog( sLog);
		return;
	}

	// 察看corp
	CCorporationInfo* corp = CCorporationMgr::GetInstance()->GetCorporation(sCorp);
	if ( corp==NULL )
	{
		// 返回注销失败消息给座席
		SIPCCMsg_ACD_AgentLogoutDepartsConf agentLogoutConf;
		agentLogoutConf.agentId = agentLogout->GetAgentID();
		agentLogoutConf.theDepartmentFailed = agentLogout->GetTheDepartment();
		agentLogoutConf.theDepartmentSuccess = "";
		agentLogoutConf.bLogoutSuccess = false;
		this->SendIPCCMsg_UDP(agentLogoutConf, ip, port);
		ostream << "SIPCCMsg_ACD_AgentLogoutDepartsFailed(step2-corp == NULL):" << "agentID=" << agentLogout->GetAgentID()
			<< ";" << "corpNdepart=" << sTempStr
			<< ";" << "corp=" << sCorp
			<< ";" << "skillGroup=" << sSkillGroup
			<< ";" << "natip=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();
		g_Logger.MakeLog( sLog);
		return;
	}

	// 察看departs
	CSpliter spSkillGroup(sSkillGroup, "|");
	string strSGIdTemp = "";
	string strCorpNDepartsSuccess = sCorp + "%^#";
	string strCorpNDepartsFailed = sCorp + "%^#";
	bool bFlag = true;
	for ( int i=0; i<spSkillGroup.GetSize(); i++ )
	{
		// 只要有一个技能组注销失败，则返回失败；全部注销成功，则返回成功。 
		strSGIdTemp = spSkillGroup[i];
		string strSGName = "";
		if ( !strSGIdTemp.empty() )
		{
			CSkillGroupInfo* pSkillGroupInfo = 
				CCorporationMgr::GetInstance()->GetSkillGroupInfo(sCorp, strSGIdTemp);
			if( NULL!=pSkillGroupInfo )
			{
				strCorpNDepartsSuccess += strSGIdTemp;
				strCorpNDepartsSuccess += "|";

                pAgentInfo->LogoutSkillGroup(strSGIdTemp);
                pAgentInfo->LogoutTXTSkillGroup(strSGIdTemp);
            }
			else
			{
				strCorpNDepartsFailed += strSGIdTemp;
				strCorpNDepartsFailed += "|";
				bFlag = false;
			}
		}
	}
	// 注销失败返回部分注销成功和注销失败的技能组。
	if ( !bFlag )
	{
		// 返回注销失败消息给座席
		SIPCCMsg_ACD_AgentLogoutDepartsConf agentLogoutConf;
		agentLogoutConf.agentId = agentLogout->GetAgentID();
		agentLogoutConf.theDepartmentFailed = strCorpNDepartsFailed;
		agentLogoutConf.theDepartmentSuccess = strCorpNDepartsSuccess;
		agentLogoutConf.bLogoutSuccess = false;
		this->SendIPCCMsg_UDP(agentLogoutConf, ip, port);
		ostream << "SIPCCMsg_ACD_AgentLogoutDepartsFailed(step3-pDepartmentInfo is NULL):" << "agentID=" << agentLogout->GetAgentID()
			<< ";" << "corpNdepart=" << sTempStr
			<< ";" << "corp=" << sCorp
			<< ";" << "skillGroup=" << sSkillGroup
			<< ";" << "natip=" << ip
			<< ";" << "natport=" << port;
		sLog = ostream.str();
		g_Logger.MakeLog( sLog);
		return;
	}

	// 返回AgentLogoutDepartsConf消息
	SIPCCMsg_ACD_AgentLogoutDepartsConf agentLogoutConf;
	agentLogoutConf.agentId = agentLogout->GetAgentID();
	agentLogoutConf.theDepartmentFailed = "";
	agentLogoutConf.theDepartmentSuccess = agentLogout->GetTheDepartment();
	agentLogoutConf.bLogoutSuccess = true;
	this->SendIPCCMsg_UDP(agentLogoutConf, ip, port);

}

void CUtility::OnAgentPark(SIPCCMsg_Client_AgentPark *agentPark, unsigned long ip, unsigned short port)
{
	//获取座席
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(agentPark->GetAgentID());
	if(pAgentInfo == NULL)
	{
		//返回 AgentLogoutConf消息
		SIPCCMsg_ACD_AgentLogoutConf agentLogoutConf;
		agentLogoutConf.agentId = agentPark->GetAgentID();
		this->SendIPCCMsg_UDP(agentLogoutConf, ip, port);
		return;
	}

	CDepartmentInfo* pDepartmentInfo = 
		CCorporationMgr::GetInstance()->GetDepartmentInfo(pAgentInfo->m_sCorporation, pAgentInfo->m_sDepartment);
	if(NULL == pDepartmentInfo)	//可能部门已删除
	{
		//置座席状态为 Agent_NoLogin
		CAgentMgr::GetInstance()->SetAgentState(agentPark->GetAgentID(), Agent_NoLogin);
		//返回 AgentLogoutConf消息
		SIPCCMsg_ACD_AgentLogoutConf agentLogoutConf;
		agentLogoutConf.agentId = agentPark->GetAgentID();
		this->SendIPCCMsg_UDP(agentLogoutConf, ip, port);
		//从 agentMap 中删除座席
		CAgentMgr::GetInstance()->DelAgent(agentPark->GetAgentID());

		return;
	}

	if(pAgentInfo->m_iCurState != Agent_Ringing)
		return;

	//置座席状态为小休
	pAgentInfo->SetState(Agent_Parking);

	//返回 AgentParkConf
	SIPCCMsg_ACD_AgentParkConf agentParkConf;
	this->SendIPCCMsg_UDP(agentParkConf, ip, port);
}

void CUtility::OnAgentUnPark(SIPCCMsg_Client_AgentUnPark *agentUnPark, unsigned long ip, unsigned short port)
{
	//获取座席
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(agentUnPark->GetAgentID());
	if(pAgentInfo == NULL)
	{
		//返回 AgentLogoutConf消息
		SIPCCMsg_ACD_AgentLogoutConf agentLogoutConf;
		agentLogoutConf.agentId = agentUnPark->GetAgentID();
		this->SendIPCCMsg_UDP(agentLogoutConf, ip, port);
		return;
	}

	//返回 AgentUnParkConf
	SIPCCMsg_ACD_AgentUnParkConf agentUnParkConf;
	this->SendIPCCMsg_UDP(agentUnParkConf, ip, port);

    //置座席状态为 waitinging
    pAgentInfo->SetState(Agent_Waiting);
}

void CUtility::OnAgentAnswerCall(SIPCCMsg_Client_AgentAnswerCall *agentAnswerCall, unsigned long ip, unsigned short port)
{
	switch (agentAnswerCall->callType)
	{
	case E_MESSAGE_CALL:case E_WEBCHAT_CALL:
		return OnAnswerTXTCall(*agentAnswerCall, ip, port);
	case E_EMAIL_CALL:
		return OnAnswerEmailCall(*agentAnswerCall, ip, port);
	case E_NORMAL_CALL:case E_VIDEO_CALL:
		break;
	default:
	{
		ostringstream os;
		string sTemp("");
		os << "OnAgentAnswerCall: unknown calltype =  " << agentAnswerCall->callType;
		sTemp = os.str();
		g_Logger.MakeLog(sTemp);
		return;
	}
	
	}

	//此时可能用户已经挂机,需要判断座席是否为震铃状态,如果不是震铃则认为用户已挂机，不处理这个应答消息
	CAgentInfo* pAgentInfo_Main = CAgentMgr::GetInstance()->GetAgentInfo(agentAnswerCall->GetAgentID());
	if(pAgentInfo_Main == NULL)
		return;

	if (!pAgentInfo_Main->IsPC()){
		pAgentInfo_Main->delCallDelivered(agentAnswerCall->callId);
	}

	if (pAgentInfo_Main->m_iCurState != Agent_Ringing)
		return;

	//呼叫方为座席,置本座席状态为 Agent_OpTalking
//	CAgentMgr::GetInstance()->SetAgentState(agentAnswerCall->GetAgentID(), Agent_OpTalking);
	
	string sAccount = agentAnswerCall->GetTheOTherParty();
	string sTemp = sAccount.substr(0, 8);

	if ( sTemp == "CClient_" )
	{
		sAccount = sAccount.substr(8, sAccount.length()-8);

		CCClientInfo* pCClientInfo_other = CCClientMgr::GetInstance()->GetCClientInfo(sAccount);

		if (pCClientInfo_other == NULL)
		{
			//发送 CallReleased 消息给座席
			SIPCCMsg_ACD_CallReleased callReleased;
			callReleased.SetReleasedParty(agentAnswerCall->GetTheOTherParty());
			this->SendIPCCMsg_UDP(callReleased, ip, port);
			return;
		}

		//发送 CallEstablished 消息给本座席
		SIPCCMsg_ACD_CallEstablished callEstablished_toMain;
		callEstablished_toMain.SetTheOtherParty(agentAnswerCall->GetTheOTherParty());
		callEstablished_toMain.remoteNatType = -1;
		callEstablished_toMain.nickName = sAccount;
		callEstablished_toMain.callType = agentAnswerCall->callType;

		this->SendIPCCMsg_UDP(callEstablished_toMain, ip, port);

		//发送 CallEstablished 消息给 other 座席
		SIPCCMsg_ACD_CallEstablished callEstablished_toOther;
		callEstablished_toOther.SetTheOtherParty(agentAnswerCall->GetAgentID());
		callEstablished_toOther.remoteNatType = pAgentInfo_Main->m_iAgentNatType;
		callEstablished_toOther.nickName = pAgentInfo_Main->m_sNickName;
		callEstablished_toOther.callingAgentID = pCClientInfo_other->m_sAccount;
		callEstablished_toOther.callID = pCClientInfo_other->m_CallID_sCallIn;
		callEstablished_toOther.callType = agentAnswerCall->callType;
		callEstablished_toOther.managerId = pAgentInfo_Main->m_sManagerid;

		this->SendIPCCMsg_UDP(callEstablished_toOther, pCClientInfo_other->m_ulUserIP, pCClientInfo_other->m_usUserPort);

		return;

	}

	CAgentInfo* pAgentInfo_Other = CAgentMgr::GetInstance()->GetAgentInfo(agentAnswerCall->GetTheOTherParty());
	if(pAgentInfo_Other == NULL)
	{
		//置座席状态为 "处理"
		//???
		//发送 CallReleased 消息给座席
		SIPCCMsg_ACD_CallReleased callReleased;
		callReleased.SetReleasedParty(agentAnswerCall->GetTheOTherParty());
		callReleased.callType = agentAnswerCall->callType;
		this->SendIPCCMsg_UDP(callReleased, ip, port);
		return;
	}

	if(pAgentInfo_Other->m_iLoginType == LoginType_WebPSTN)
	{
		if(pAgentInfo_Other->m_iCurState != Agent_Dialing && pAgentInfo_Other->m_iCurState != Agent_DialUnderHold)
			return;
		//发送 callAnswered 网关
		SIPCCMsg_ACD_CallAnswered_WebPstnAgent CallAnswered_WebPstnAgent; //绑电话坐席呼叫 voip 坐席, voip坐席应答后
		CallAnswered_WebPstnAgent.gwAgentLine = pAgentInfo_Other->m_WebPstnAgentCurGwLine;
		CallAnswered_WebPstnAgent.otherAgentID = agentAnswerCall->GetAgentID();
		this->SendIPCCMsg_UDP(CallAnswered_WebPstnAgent, pAgentInfo_Other->m_gwCtrlIP, pAgentInfo_Other->m_gwCtrlPort);
	}

	//发送 CallEstablished 消息给本座席
	SIPCCMsg_ACD_CallEstablished callEstablished_toMain;
	callEstablished_toMain.SetTheOtherParty(agentAnswerCall->GetTheOTherParty());
	callEstablished_toMain.remoteNatType = pAgentInfo_Other->m_iAgentNatType;
	callEstablished_toMain.nickName = pAgentInfo_Other->m_sNickName;
	callEstablished_toMain.callType = agentAnswerCall->callType;

	this->SendIPCCMsg_UDP(callEstablished_toMain, ip, port);

	/*pAgentInfo_Main->SetClientAgentState(Agent_OpTalking);*/

	//发送 CallEstablished 消息给 other 座席
	SIPCCMsg_ACD_CallEstablished callEstablished_toOther;
	callEstablished_toOther.SetTheOtherParty(agentAnswerCall->GetAgentID());
	callEstablished_toOther.remoteNatType = pAgentInfo_Main->m_iAgentNatType;
	callEstablished_toOther.nickName = pAgentInfo_Main->m_sNickName;
	callEstablished_toOther.callType = agentAnswerCall->callType;

	this->SendIPCCMsg_UDP(callEstablished_toOther, pAgentInfo_Other->m_ulAgentIP, pAgentInfo_Other->m_usAgentPort);

	/*pAgentInfo_Other->SetClientAgentState(Agent_Talking);*/
}

void CUtility::OnAnswerTXTCall(SIPCCMsg_Client_AgentAnswerCall & answerCall, unsigned long ip, unsigned short port)
{
	
    CAgentInfo* pAgentInfo_Main = CAgentMgr::GetInstance()->GetAgentInfo(answerCall.GetAgentID());
    if (pAgentInfo_Main == NULL)
        return;

	if (!pAgentInfo_Main->IsPC()){
		pAgentInfo_Main->PushFlag = 0;
		pAgentInfo_Main->delCallDelivered(answerCall.callId);
	}

    pAgentInfo_Main->OnAnswerTXTCall(answerCall, ip, port);
	
}

void CUtility::OnAnswerEmailCall(SIPCCMsg_Client_AgentAnswerCall & answerCall, unsigned long ip, unsigned short port)
{
	CAgentInfo* pAgentInfo_Main = CAgentMgr::GetInstance()->GetAgentInfo(answerCall.GetAgentID());
	if (pAgentInfo_Main == NULL)
		return;

	if (!pAgentInfo_Main->IsPC()){

		pAgentInfo_Main->delCallDelivered(answerCall.callId);
	}

	pAgentInfo_Main->OnAnswerEmailCall(answerCall, ip, port);
}

void CUtility::OnAgentHangupCall(SIPCCMsg_Client_AgentHangupCall *agentHangupCall, unsigned long ip, unsigned short port)
{
	switch (agentHangupCall->callType)
	{
	case E_MESSAGE_CALL:case E_WEBCHAT_CALL:
		return OnHangupTXTCall(*agentHangupCall, ip, port);
	case E_EMAIL_CALL:
		return OnHangupEMailCall(*agentHangupCall, ip, port);
	case E_NORMAL_CALL:case E_VIDEO_CALL:
		break;
	default:
		return;
	}

    //需要处理一下几种情况
    //1、会议中挂断
    //2、通话中挂断（与外线通话时挂断此消息会被发送到ustate不会到acdserver）
    //2.1、席间通话，主叫或被叫挂机
    //2.2、座席与C端通话，座席挂断
    //3、拨号中挂断（座席呼座席时,主叫座席挂断）
    //3.1 座席呼座席
    //3.2 座席呼组
    //
    //agentHangupCall->GetTheOtherParty() 可能为3种内容：1、组名（部门或技能组），2、座席账号，3、c端账号
    //
    //

    CSkillGroupInfo* pSkillGroupInfo = NULL;
    CDepartmentInfo* pDepartmentInfo = NULL;
    CAgentInfo* pAgentInfo_main = NULL;
    CAgentInfo* pAgentInfo_other = NULL;
    CCClientInfo* pCClientInfo_other = NULL;
    bool bProccessVDN = false;
    int32_t leaveCause = VDNEndCause_Normal;

	pAgentInfo_main = CAgentMgr::GetInstance()->GetAgentInfo(agentHangupCall->GetAgentID());
	if(pAgentInfo_main == NULL)
		return;

    pAgentInfo_main->delCallDelivered(agentHangupCall->GetCallID());

    //1、会议中挂断
    int nconferenceresult = ConferenceAgentHuageup(pAgentInfo_main, ip, port, "[SIPCCMsg_Client_AgentHangupCall]", false, false);
    if (nconferenceresult == 1)
    {
        return;
    }

    if (pAgentInfo_main->m_iCurState == Agent_Talking ||
        pAgentInfo_main->m_iCurState == Agent_OpTalking) {
        pAgentInfo_other = CAgentMgr::GetInstance()->GetAgentInfo(agentHangupCall->GetTheOtherParty());

        //2、通话中挂断（与外线通话时挂断此消息会被发送到ustate不会到acdserver）
        //2.1、席间通话，主叫或被叫挂机
        //2.2、座席与C端通话，座席挂断

        if (pAgentInfo_other == NULL) {
            string sAccount = agentHangupCall->GetTheOtherParty().substr(0, 8);
            if (!sAccount.empty()) {
                pCClientInfo_other = CCClientMgr::GetInstance()->GetCClientInfo(sAccount);
            }
        }
    }
    else {
        //3、拨号中挂断（座席呼座席时,主叫座席挂断）
        //3.1 座席呼座席
        //3.2 座席呼组
        bProccessVDN = true;
        leaveCause = VDNEndCause_CallAbandon;
        if (!agentHangupCall->bAgentAssigned) {
            pSkillGroupInfo = CCorporationMgr::GetInstance()->GetSkillGroupInfo(agentHangupCall->GetTheOtherParty());
            if (pSkillGroupInfo == NULL) {
                pDepartmentInfo = CCorporationMgr::GetInstance()->GetDepartmentInfo(agentHangupCall->GetTheOtherParty());
            }
        }

        if (pSkillGroupInfo == NULL && pDepartmentInfo == NULL) {
            pAgentInfo_other = CAgentMgr::GetInstance()->GetAgentInfo(agentHangupCall->GetTheOtherParty());
        }
        
        if (pAgentInfo_other) {
            if (!pAgentInfo_other->m_sCurSkillInfo.id.empty()) {
                pSkillGroupInfo = CCorporationMgr::GetInstance()->GetSkillGroupInfo(pAgentInfo_main->m_sCorporation, pAgentInfo_other->m_sCurSkillInfo.id);
            }
            else {
                pDepartmentInfo = CCorporationMgr::GetInstance()->GetDepartmentInfo(pAgentInfo_main->m_sCalledCorp, pAgentInfo_main->m_sCalledDepart);
            }
        }

        if (pDepartmentInfo != NULL) {
            pDepartmentInfo->RemoveFromCQueue(agentHangupCall->GetAgentID());
        }

        if (pSkillGroupInfo != NULL) {
            pDepartmentInfo->RemoveFromCQueue(agentHangupCall->GetAgentID());
        }

        if (pAgentInfo_other) {
            pAgentInfo_other->delCallDelivered(agentHangupCall->GetCallID());
        }
    }

    //处理发送给原座席消息
    SIPCCMsg_ACD_CallReleased callReleased_main;
    callReleased_main.SetReleasedParty(agentHangupCall->GetTheOtherParty());
    this->SendIPCCMsg_UDP(callReleased_main, ip, port);

    //处理发送给目的座席的消息
    if (pAgentInfo_other != NULL) {
        //发送 CallReleased 消息给目的座席
        SIPCCMsg_ACD_CallReleased callReleased_other;
        callReleased_other.SetReleasedParty(agentHangupCall->GetAgentID());
        this->SendIPCCMsg_UDP(callReleased_other, pAgentInfo_other->m_ulAgentIP, pAgentInfo_other->m_usAgentPort);
    }

    //处理c端消息
    if (pCClientInfo_other != NULL) {
        SIPCCMsg_ACD_CallReleased callReleased_other;
        callReleased_other.SetReleasedParty(agentHangupCall->GetAgentID());
        callReleased_other.toAgentID = pCClientInfo_other->m_sAccount;
        callReleased_other.callID = pCClientInfo_other->m_CallID_sCallIn;
        this->SendIPCCMsg_UDP(callReleased_other, pCClientInfo_other->m_ulUserIP, pCClientInfo_other->m_usUserPort);

        CCClientMgr::GetInstance()->DelCClient(pCClientInfo_other->m_sAccount);
    }

    //处理vdn消息
    if (bProccessVDN) {
        if (pSkillGroupInfo == NULL && pDepartmentInfo == NULL) {
            g_Logger.MakeLog("error OnAgentHangupCall depart和skill均为空");
            return;
        }

        if (pAgentInfo_other == NULL || pAgentInfo_other->m_bCallFromVDN) {
            SIPCCMsg_ACD_CallLeaveVDN CallLeaveVDN;
            CallLeaveVDN.managerid = pAgentInfo_main->m_sManagerid;
            CallLeaveVDN.corpName = pAgentInfo_main->m_sCorporation;
            CallLeaveVDN.callID = pAgentInfo_main->m_sCallID;
            CallLeaveVDN.agentID = pAgentInfo_other == NULL ? "" : pAgentInfo_other->m_sAgentID;
            CallLeaveVDN.leaveCause = leaveCause;
            CallLeaveVDN.LeaveQueueTime = CLog::GetLocalTime();
            CallLeaveVDN.leaveTime = (pAgentInfo_other != NULL) ? pAgentInfo_other->m_sQueueArriveTime : CLog::GetLocalTime();
            CallLeaveVDN.bIntoQueue = (pAgentInfo_other == NULL || pAgentInfo_other->m_bCallFromQueue);

            //if (bCallSG) search hook
            bool bCallSG = (pSkillGroupInfo != NULL);
            if (bCallSG) {
                CallLeaveVDN.vdnNo = pSkillGroupInfo->m_sSkillGroupName;
                CallLeaveVDN.SkillGroupID = pSkillGroupInfo->m_sSkillGroupid;
                CallLeaveVDN.SkillGroupName = pSkillGroupInfo->m_sSkillGroupName;
                CallLeaveVDN.CurQueueNum = pSkillGroupInfo->GetCQueueSize();
            }
            else {
                CallLeaveVDN.vdnNo = pDepartmentInfo->m_sDepartmentName;
                CallLeaveVDN.deptid = pDepartmentInfo->m_sDeptid;
                CallLeaveVDN.CurQueueNum = pDepartmentInfo->GetCQueueSize();
            }

            CUtility::GetInstance()->SendIPCCMsg_UDP(
                CallLeaveVDN,
                CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_IP,
                CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_Port);

            CUtility::GetInstance()->SendToMonitorServer(CallLeaveVDN);
        }
    }
}

CUtility* CUtility::GetInstance()
{
	if(0 == __Utility)
		__Utility = new CUtility();

	return __Utility;
}

void CUtility::destroy()
{
	if(0 != __Utility)
		delete __Utility;

	__Utility = 0;
}

void CUtility::SendIPCCMsg_UDP(SIPCCMessage &ipccmessage, unsigned long ip, unsigned short port, string file, int line, int nReSendCount, bool makelog)
{
	if (ip == 0)
		return;

	CIPCCMsgExchanger msgExchanger;

	char* theBuf;
	int theBufContent = 0;
	msgExchanger.SerializeMsg(ipccmessage, theBuf, theBufContent);

	if (theBufContent < 0)
	{
		ostringstream oss;
		oss << "消息序列化失败： " << ipccmsg_to_str(ipccmessage)
			<< " ;" << get_file_name(file)
			<< ":" << line;
		g_Logger.MakeLog(oss.str());
		return;
	}

	string sErrInfo("");
	using namespace boost::asio::ip;
	m_mySock->SendData(address_v4( ntohl(ip)).to_string(), boost::lexical_cast<string>(ntohs(port)),theBuf,theBufContent,sErrInfo);

    if (makelog &&
        ipccmessage.msgClass != Class_OCXHeart &&
		ipccmessage.msgClass != Class_HeartBeat &&
		ipccmessage.msgType != Type_ACD_CallingQueueInfo &&
		ipccmessage.msgType != Type_ACD_NotifyDataCollect &&
		ipccmessage.msgType != Type_ACD_AgentStateInfo &&
        ipccmessage.msgType != Type_ACD_DepartNAgentInfo &&
		ipccmessage.msgType != Type_OneKeySwitch_ServerHeartBeat &&
		ipccmessage.msgType != Type_OneKeySwitch_ServerHeartBeatConf &&
		ipccmessage.msgType != Type_Client_GetCallingQueueInfo &&
		ipccmessage.msgType != Type_OutDial_SetAgentCount &&
		ipccmessage.msgType != Type_OneKeySwitch_ClientHeartBeat)
	{
		ostringstream oss;
		if (!file.empty())
		{
			oss << "sent"
				<< ", type=" << ipccmsg_type_to_str(ipccmessage.msgClass, ipccmessage.msgType)
				<< ", to=" << format_ip_addr(ip) << "@" << ntohs(port)
				<< ", " << get_file_name(file) << ":" << line << endl
			    //<< ", class=" << ipccmsg_class_to_str(ipccmessage.msgClass) << endl
				<< "                    " << ipccmsg_to_str(ipccmessage);
		}
		else
		{
			oss << "sent"
				<< ", type=" << ipccmsg_type_to_str(ipccmessage.msgClass, ipccmessage.msgType)
				<< ", to=" << format_ip_addr(ip) << "@" << ntohs(port) << endl
			    //<< ", class=" << ipccmsg_class_to_str(ipccmessage.msgClass) << endl
				<< "                    " << ipccmsg_to_str(ipccmessage);
		}

		g_Logger.MakeLog(oss.str());
	}

	if(theBuf != NULL)
		delete[] theBuf;
}

void CUtility::SendToMonitorServer(SIPCCMessage &ipccmessage)
{
	if (CUtility::GetInstance()->m_IPCCConfig.MonitorServer_IP != 0)
	{
		//Monitor
		CUtility::GetInstance()->SendIPCCMsg_UDP(
			ipccmessage,
			CUtility::GetInstance()->m_IPCCConfig.MonitorServer_IP,
			CUtility::GetInstance()->m_IPCCConfig.MonitorServer_Port, 
            __FILE__, __LINE__, 10, false);
	}
	if (CUtility::GetInstance()->m_IPCCConfig.MonitorServer_Backup_IP != 0)
	{
		CUtility::GetInstance()->SendIPCCMsg_UDP(
			ipccmessage,
			CUtility::GetInstance()->m_IPCCConfig.MonitorServer_Backup_IP,
            CUtility::GetInstance()->m_IPCCConfig.MonitorServer_Backup_Port,
            __FILE__, __LINE__, 10, false);
	}
}

void CUtility::SetRcvSockThreadEnd()
{
	SIPCCMsg_EndEgn EndEgn;

	this->SendIPCCMsg_UDP(
		EndEgn, 
		this->m_IPCCConfig.localIP, 
		::ConvertPort(ServerInfoDef::Port_ACDServer)
		);
}

void CUtility::OnDelDepartment(SIPCCMsg_SysManage_DelDepartment *delDepartment, unsigned long ip, unsigned short port)
{
	string sTempStr = delDepartment->GetTheDepartment();
	string sCorp = "";
	string sDepart = "";

	if(!SplitCorpAndDepart(sTempStr, sCorp, sDepart))
		return;

	CCorporationInfo* corp;
	corp = CCorporationMgr::GetInstance()->GetCorporation(sCorp);

	if(corp != NULL)
		corp->DelDepartment(sDepart);

	//返回 delDepartmentConf
	SIPCCMsg_ACD_DelDepartmentConf delDepartmentConf;
	delDepartmentConf.SetTheDepartment(delDepartment->GetTheDepartment());
	this->SendIPCCMsg_UDP(delDepartmentConf, ip, port);	

	//通知监控服务，删除分组的信息
	SIPCCMsg_ACD_DelDepartment ACD_DelDepartment;
	ACD_DelDepartment.corpName = sCorp;
	ACD_DelDepartment.deptName = sDepart;

	CUtility::GetInstance()->SendToMonitorServer(ACD_DelDepartment);
}

void CUtility::OnMediaRequest(SIPCCMsg_Client_MediaRequest *mediaRequest, unsigned long ip, unsigned short port)
{
	//用户媒体请求->给座席发送媒体连接请求
	//获取座席信息

	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(mediaRequest->GetTargetName());
	if(pAgentInfo == NULL)
		return;

	//给座席发送媒体连接请求
	SIPCCMsg_ACD_MediaRequest mediaRequestToAgent;
	mediaRequestToAgent.SetCallID(mediaRequest->GetCallID());
	mediaRequestToAgent.mediaType = mediaRequest->mediaType;
	mediaRequestToAgent.seq = mediaRequest->seq;
	mediaRequestToAgent.transmitServerIP = mediaRequest->transmitServerIP;
	mediaRequestToAgent.transmitServerPort = mediaRequest->transmitServerPort;
	this->SendIPCCMsg_UDP(mediaRequestToAgent, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);
}

bool CUtility::LoadConfig()
{
	string ipccConfigFile = GetAppPath().string() + "/IPCC_Config.ini";

	CIni configIni;
	if (!configIni.Open(ipccConfigFile.c_str()))
		return false;
	m_iAgentsLicence = 0;
	m_iLicenceCode = 0;
	m_IPCCConfig.ACDServerName = "";
	m_IPCCConfig.LicenceConfirmServer1IP = 0;
	m_IPCCConfig.LicenceConfirmServer1Port = 0;
	m_IPCCConfig.LicenceConfirmServer2IP = 0;
	m_IPCCConfig.LicenceConfirmServer2Port = 0;
	m_IPCCConfig.bIsLocalDog = (ReadText(configIni, "config", "bIsLocalDog", "1") == "1");
	if (m_IPCCConfig.bIsLocalDog)
	{
		string sAcdLicence = GetAppPath().string() + "/ACDLicence.ini";

		CIni licenceIni;
		if (licenceIni.Open(sAcdLicence.c_str()))
		{
			m_iAgentsLicence = atoi(ReadText(licenceIni, "Config", "AllAgentNum").c_str());
			m_iLicenceCode = atoi(ReadText(licenceIni, "Config", "SN").c_str());
		}
		licenceIni.Close();
	}
	else
	{
		this->m_IPCCConfig.ACDServerName = ReadText(configIni, "config", "ACDServerName");
		if (m_IPCCConfig.ACDServerName.empty())
		{
			g_Logger.MakeLog("使用网络狗时，服务名为空，启动失败");
			return false;
		}

		m_IPCCConfig.LicenceConfirmServer1IP = ConvertIP(ReadText(configIni, "config", "LicenceConfirmServer1IP", "0.0.0.0"));
		m_IPCCConfig.LicenceConfirmServer1Port = htons(atoi(ReadText(configIni, "config", "LicenceConfirmServer1Port").c_str()));

		m_IPCCConfig.LicenceConfirmServer2IP = ConvertIP(ReadText(configIni, "config", "LicenceConfirmServer2IP", "0.0.0.0"));
		m_IPCCConfig.LicenceConfirmServer2Port = htons(atoi(ReadText(configIni, "config", "LicenceConfirmServer2Port").c_str()));

		if ((m_IPCCConfig.LicenceConfirmServer1IP == 0) && (m_IPCCConfig.LicenceConfirmServer2IP == 0))
		{
			g_Logger.MakeLog("Licence server配置错误，启动失败");
			return false;
		}
	}

	m_IPCCConfig.localIP = 0;
	m_IPCCConfig.localserverIP = ReadText(configIni, "config", "LocalServerIP");
	if (!m_IPCCConfig.localserverIP.empty())
	{
		m_IPCCConfig.localIP = ConvertIP(m_IPCCConfig.localserverIP);
		GetNewVersionInvokeID(this->m_IPCCConfig.localIP);
	}

	//PortConfigMode
	this->m_IPCCConfig.PortConfigMode = atoi(ReadText(configIni, "config", "PortConfigMode").c_str());

	if (this->m_IPCCConfig.PortConfigMode == 1)
	{
		ServerInfoDef::Load();

		g_Logger.MakeLog("--使用配置文件ServerInfoDef.xml中的构件端口方案--");
	}

	m_sRedirectServerDomain = ReadText(configIni, "config", "RedirectServer_Domain");

	m_IPCCConfig.RegisterServerA_IP = ConvertIP(ReadText(configIni, "config", "RegisterServerA_IP", "0.0.0.0"));
	m_IPCCConfig.RegisterServerA_Port = htons(atoi(ReadText(configIni, "config", "RegisterServerA_Port").c_str()));

	m_IPCCConfig.RegisterServerB_IP = ConvertIP(ReadText(configIni, "config", "RegisterServerB_IP", "0.0.0.0"));
	m_IPCCConfig.RegisterServerB_Port = htons(atoi(ReadText(configIni, "config", "RegisterServerB_Port").c_str()));

	//DataCollectServer
	m_IPCCConfig.DataCollectServer_ID = 0;
	m_IPCCConfig.DataCollectServer_IP = ConvertIP(ReadText(configIni, "config", "Local_IP_DataCollectServer", "0.0.0.0"));
	m_IPCCConfig.DataCollectServer_Port = htons(atoi(ReadText(configIni, "config", "Local_Port_DataCollectServer").c_str()));
	if (m_IPCCConfig.DataCollectServer_Port == 0)
	{
		m_IPCCConfig.DataCollectServer_Port = ConvertPort(ServerInfoDef::Port_DataCollectServer);
	}

	//SysManageServer_IP
	this->m_IPCCConfig.SysManageServer_ID = 0;
	m_IPCCConfig.SysManageServer_IP = ConvertIP(ReadText(configIni, "config", "Local_IP_SysManageServer", "0.0.0.0"));
	m_IPCCConfig.SysManageServer_Port = htons(atoi(ReadText(configIni, "config", "Local_Port_SysManageServer").c_str()));
	if (m_IPCCConfig.SysManageServer_Port == 0)
	{
		m_IPCCConfig.SysManageServer_Port = ConvertPort(ServerInfoDef::Port_SysManageServer);
	}

	//UStateServer_IP
	this->m_IPCCConfig.UStateServer_ID = 0;
	m_IPCCConfig.UStateServer_IP = ConvertIP(ReadText(configIni, "config", "Local_IP_UStateServer", "0.0.0.0"));
	m_IPCCConfig.UStateServer_Port = htons(atoi(ReadText(configIni, "config", "Local_Port_UStateServer").c_str()));
	if (m_IPCCConfig.UStateServer_Port == 0)
	{
		m_IPCCConfig.UStateServer_Port = ConvertPort(ServerInfoDef::Port_UStateServer);
	}

	//MonitorServer_IP
	m_IPCCConfig.MonitorServer_ID = 0;
	m_IPCCConfig.MonitorServer_IP = ConvertIP(ReadText(configIni, "config", "Local_IP_MonitorServer", "0.0.0.0"));
	m_IPCCConfig.MonitorServer_Port = htons(atoi(ReadText(configIni, "config", "Local_Port_MonitorServer").c_str()));

	m_IPCCConfig.MonitorServer_Backup_ID = 0;
	m_IPCCConfig.MonitorServer_Backup_IP = ConvertIP(ReadText(configIni, "config", "Local_IP_MonitorServer_Backup", "0.0.0.0"));
	m_IPCCConfig.MonitorServer_Backup_Port = htons(atoi(ReadText(configIni, "config", "Local_Port_MonitorServer_Backup").c_str()));
 
	//AreaCodeServer_IP
	m_IPCCConfig.AreaCodeServer_ID = 0;
	m_IPCCConfig.AreaCodeServer_IP = ConvertIP(ReadText(configIni, "config", "Local_IP_AreaCodeServer", "0.0.0.0"));
	m_IPCCConfig.AreaCodeServer_Port = htons(atoi(ReadText(configIni, "config", "Local_Port_AreaCodeServer").c_str()));
	if (m_IPCCConfig.AreaCodeServer_Port == 0)
	{
		m_IPCCConfig.AreaCodeServer_Port = ConvertPort(ServerInfoDef::Port_AreaCodeServer);
	}

	//CallerNoForSend_PiccMode
	m_IPCCConfig.iCallerNoForSend_PiccMode = atoi(ReadText(configIni, "config", "CallerNoForSend_PiccMode").c_str());

	m_sOutdialIp = ConvertIP(ReadText(configIni, "OutDial", "ip", "0.0.0.0"));
	m_sOutdialPort = htons(atoi(ReadText(configIni, "OutDial", "port").c_str()));

	m_IPCCConfig.bIsEnterpriseVersion = (ReadText(configIni, "versionInfo", "IsEnterpriseVersion", "0") == "1");
	m_IPCCConfig.bAllowForceLogout = (ReadText(configIni, "versionInfo", "AllowForceLogout", "0") == "1");


	m_bAllowedPush = (ReadText(configIni, "versionInfo", "AllowPushToApp", "1") == "1");

	if (m_bAllowedPush)
	{
		SPushServerConfig config;

		config.sXG_IOS_AccessID = ReadText(configIni, "MobilePush", "XG_IOS_AccessID", "");
		config.sXG_IOS_SecretKey = ReadText(configIni, "MobilePush", "XG_IOS_SecretKey", "");
		//IOS 生产环境1，开发环境2
		config.iXG_IOS_Environment = atoi(ReadText(configIni, "MobilePush", "XG_IOS_Environment", "1").c_str());

		config.sXG_Android_AccessID = ReadText(configIni, "MobilePush", "XG_Android_AccessID", "");
		config.sXG_Android_SecretKey = ReadText(configIni, "MobilePush", "XG_Android_SecretKey", "");

		config.BundleId = ReadText(configIni, "Provider", "bundleId", "");
		config.CertFile = ReadText(configIni, "Provider", "cert", "");
		config.AuthKey = ReadText(configIni, "Provider", "authKey", "");
		config.AuthKeyID = ReadText(configIni, "Provider", "authKeyId", "");
		config.TeamID = ReadText(configIni, "Provider", "teamId", "");

		CMessagePushMgr::GetInstance()->Init(config, boost::bind(&CUtility::MakeLog, this, _1));
	}

    m_IPCCConfig.RegisterServerA_IP = ConvertIP(ReadText(configIni, "config", "RegisterServerA_IP", "0.0.0.0"));
    m_IPCCConfig.RegisterServerA_Port = htons(atoi(ReadText(configIni, "config", "RegisterServerA_Port").c_str()));

    m_IPCCConfig.RegisterServerB_IP = ConvertIP(ReadText(configIni, "config", "RegisterServerB_IP", "0.0.0.0"));
    m_IPCCConfig.RegisterServerB_Port = htons(atoi(ReadText(configIni, "config", "RegisterServerB_Port").c_str()));

    //ACDRedis
    try {
        m_IPCCConfig.ACDRedisIP = ReadText(configIni, "ACDRedis", "IP");
        m_IPCCConfig.ACDRedisPort = boost::lexical_cast<uint16_t>(ReadText(configIni, "ACDRedis", "Port"));
        m_IPCCConfig.ACDRedisDB = boost::lexical_cast<uint16_t>(ReadText(configIni, "ACDRedis", "DB"));
        m_IPCCConfig.ACDRedisPassword = ReadText(configIni, "ACDRedis", "Password");
    }
    catch (...) {
    }

	configIni.Close();

	if ((this->m_IPCCConfig.MonitorServer_IP == this->m_IPCCConfig.DataCollectServer_IP)
		&& (this->m_IPCCConfig.MonitorServer_Port == this->m_IPCCConfig.DataCollectServer_Port))
	{
		this->m_IPCCConfig.MonitorServer_IP = 0;
		this->m_IPCCConfig.MonitorServer_Port = 0;
		g_Logger.MakeLog("配置文件中采集与监控相同");
	}

	stringstream ss;
	ss << "local config finished, "
		<< ",LocalDog=" << (m_IPCCConfig.bIsLocalDog ? "1" : "0")
		<< ",LicenceCode=" << m_iLicenceCode
		<< ",ACDServerName=" << m_IPCCConfig.ACDServerName
		<< ",LicenceConfirmServer1IP=" << format_ip_addr(m_IPCCConfig.LicenceConfirmServer1IP)
		<< ",LicenceConfirmServer1Port=" << ntohs(m_IPCCConfig.LicenceConfirmServer1Port)
		<< ",LicenceConfirmServer2IP=" << format_ip_addr(m_IPCCConfig.LicenceConfirmServer2IP)
		<< ",LicenceConfirmServer2Port=" << ntohs(m_IPCCConfig.LicenceConfirmServer2Port)
		<< ",AgentsLicence=" << m_iAgentsLicence
		<< ",localIP=" << format_ip_addr(m_IPCCConfig.localIP)
		<< ",PortConfigMode=" << m_IPCCConfig.PortConfigMode
		<< ",RedirectDomain=" << m_sRedirectServerDomain
		<< ",RegisterServerA_IP=" << format_ip_addr(m_IPCCConfig.RegisterServerA_IP)
		<< ",RegisterServerA_Port=" << ntohs(m_IPCCConfig.RegisterServerA_Port)
		<< ",RegisterServerB_IP=" << format_ip_addr(m_IPCCConfig.RegisterServerB_IP)
		<< ",RegisterServerB_Port=" << ntohs(m_IPCCConfig.RegisterServerB_Port)
		<< ",DataCollectServer_IP=" << format_ip_addr(m_IPCCConfig.DataCollectServer_IP)
		<< ",DataCollectServer_Port=" << ntohs(m_IPCCConfig.DataCollectServer_Port)
		<< ",SysManageServer_IP=" << format_ip_addr(m_IPCCConfig.SysManageServer_IP)
		<< ",SysManageServer_Port=" << ntohs(m_IPCCConfig.SysManageServer_Port)
		<< ",UStateServer_IP=" << format_ip_addr(m_IPCCConfig.UStateServer_IP)
		<< ",UStateServer_Port=" << ntohs(m_IPCCConfig.UStateServer_Port)
		<< ",MonitorServer_IP=" << format_ip_addr(m_IPCCConfig.MonitorServer_IP)
		<< ",MonitorServer_Port=" << ntohs(m_IPCCConfig.MonitorServer_Port)
		<< ",MonitorServer_Backup_IP=" << format_ip_addr(m_IPCCConfig.MonitorServer_Backup_IP)
		<< ",MonitorServer_Backup_Port=" << ntohs(m_IPCCConfig.MonitorServer_Backup_Port)
		<< ",iCallerNoForSend_PiccMode=" << m_IPCCConfig.iCallerNoForSend_PiccMode
		<< ",m_sOutdialIp=" << format_ip_addr(m_sOutdialIp)
		<< ",m_sOutdialPort=" << ntohs(m_sOutdialPort)
		<< ",bIsEnterpriseVersion=" << (m_IPCCConfig.bIsEnterpriseVersion ? "1" : "0")
		<< ",bAllowForceLogout=" << (m_IPCCConfig.bAllowForceLogout ? "1" : "0")
		<< ",bAllowPushToApp=" << (m_bAllowedPush ? "1" : "0");

	g_Logger.MakeLog(ss.str());

	return true;	
}

void CUtility::OnHeartBeat(SIPCCMsg_HeartBeat *heartBeat, unsigned long ip, unsigned short port)
{
	//更新座席信息
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAnyAgent(heartBeat->GetTheSender());
	if(pAgentInfo == NULL)
	{
		SIPCCMsg_ACD_ForceLogout forceLogout;
		forceLogout.agentName = heartBeat->GetTheSender();
		forceLogout.cause = ForceLogoutCause_NoExist;
		this->SendIPCCMsg_UDP(forceLogout, ip, port);

		//log
		string sLog;
		ostringstream ostream;
		ostream.str("");
		ostream << "HeartBeat,Agent ForceLogout:" << "name=" << heartBeat->GetTheSender()
			<< "," << "ip=" << format_ip_addr(ip)
			<< "," << "port=" << ntohs(port);
		sLog = ostream.str();
		g_Logger.MakeLog( sLog);

		return;
	}

	//回送 response
	SIPCCMsg_HeartBeatResponse HeartBeatResponse;
	this->SendIPCCMsg_UDP(HeartBeatResponse, ip, port, __FILE__, __LINE__, 1);

	//更新最后文本通话时间
	std::vector<CTxtSessionInfo>::iterator it = heartBeat->txtSessionInfo.begin();
	while (it != heartBeat->txtSessionInfo.end())
	{
		string sLog;
		ostringstream ostream;
		ostream.str("");
		ostream << "HeartBeat,CTxtSessionInfo:" << "name=" << pAgentInfo->m_sAgentID
			<< ",callid=" << (*it).callID
			<< ",noMsgTime=" << (*it).noMsgTime
			<< ",txtSessionTimeout=" << pAgentInfo->txtSessionTimeout;
		sLog = ostream.str();
		g_Logger.MakeLog(sLog);

		pAgentInfo->UpdateTXTSessionTime((*it).callID, (*it).noMsgTime);
		it++;
	}

	//if ( heartBeat->GetTheSender() == "lihr@infobird.cn" || heartBeat->GetTheSender() == "liuyj@infobird.cn" )
	//{
	//	//log
	//	string sLog;
	//	ostringstream ostream;
	//	ostream.str("");
	//	ostream << "OnHeartBeat:" << "agent=" << heartBeat->GetTheSender()
	//		<< ";" << "ip=" << ip
	//		<< ";" << "port=" << port;
	//	sLog = ostream.str();
	//}

	if(pAgentInfo->bAgentDormancy)
	{
		CDepartmentInfo* pDepartmentInfo = 
			CCorporationMgr::GetInstance()->GetDepartmentInfo(pAgentInfo->m_sCorporation, pAgentInfo->m_sDepartment);
		if(pDepartmentInfo == NULL)
			return;

		pAgentInfo->bAgentDormancy = false;

		//log
		string sLog;
		ostringstream ostream;
		ostream.str("");
		ostream << "HeartBeat,Agent relive:" << "name=" << pAgentInfo->m_sAgentID
			<< "," << "depart=" << pAgentInfo->m_sDepartment
			<< "," << "oldaddr=" << format_ip_addr(pAgentInfo->m_ulAgentIP) << "@" << ntohs(pAgentInfo->m_usAgentPort)
			<< "," << "newaddr=" << format_ip_addr(ip) << "@" << ntohs(port)
			<< "," << "PreHeartBeatTime" << pAgentInfo->m_iPreHeartBeatMark;
		sLog = ostream.str();
		g_Logger.MakeLog( sLog);

		//增加座席到 mapAgentInDepartment,此时agent并为删除，不需添加
		//pDepartmentInfo->AddAgentToMap(pAgentInfo->m_sAgentID);
		//判断座席状态是否 Agent_Waiting
		pAgentInfo->AgentRelive();
	}

	if(pAgentInfo->m_ulAgentIP!=ip || pAgentInfo->m_usAgentPort!=port)
	{
		//log
		string sLog;
		ostringstream ostream;
		ostream.str("");
		ostream << "HeartBeat,UpdateIPPort:" << "name=" << pAgentInfo->m_sAgentID
			<< ";" << "depart=" << pAgentInfo->m_sDepartment
			<< "," << "oldaddr=" << format_ip_addr(pAgentInfo->m_ulAgentIP) << "@" << ntohs(pAgentInfo->m_usAgentPort)
			<< "," << "newaddr=" << format_ip_addr(ip) << "@" << ntohs(port)
			<< "," << "PreHeartBeatTime" << pAgentInfo->preHeartTime;
		sLog = ostream.str();
		g_Logger.MakeLog( sLog);
	}

	pAgentInfo->m_ulAgentIP = ip;
	pAgentInfo->m_usAgentPort = port;
	pAgentInfo->m_iPreHeartBeatMark = gettickcount();
	pAgentInfo->preHeartTime = GetLocalTimeForMessageCall();
	pAgentInfo->PushFlag = 0;

	if ( !CAgentMgr::GetInstance()->m_bCheckBak )
	{
		if ( pAgentInfo->m_bNeedCheck )
		{
			CAgentMgr::GetInstance()->m_mapCheckOnlineAgents.erase(pAgentInfo->m_sAgentID);

			CAgentMgr::GetInstance()->m_mapCheckOnlineAgents_bak[pAgentInfo->m_sAgentID] = pAgentInfo;	

			pAgentInfo->m_bNeedCheck = false;
		}

	}
	else
	{
		if ( !pAgentInfo->m_bNeedCheck )
		{
			CAgentMgr::GetInstance()->m_mapCheckOnlineAgents_bak.erase(pAgentInfo->m_sAgentID);

			CAgentMgr::GetInstance()->m_mapCheckOnlineAgents[pAgentInfo->m_sAgentID] = pAgentInfo;

			pAgentInfo->m_bNeedCheck = true;
		}

	}


}

void CUtility::CheckSendOutDialState()
{
	//5分钟扫描一次
	unsigned long iCurTimeMark = gettickcount();
	if(iCurTimeMark - m_iSendOutDialStateTime > 300*1000)
	{
		OutDial_SetAgentCount();
		m_iSendOutDialStateTime = iCurTimeMark;
	}
}

void CUtility::CheckOverflow()
{
	unsigned long iCurTimeMark = gettickcount();
	if(iCurTimeMark - m_iCheckOverflowTime > 1000)
	{
		CCorporationMgr::GetInstance()->CheckOverflow();
		m_iCheckOverflowTime = iCurTimeMark;
	}
}

void CUtility::ScanAgentMap()
{
	//每 60s 扫描 1 次
	unsigned long iCurTimeMark = gettickcount();
	if(iCurTimeMark - m_iPreTimeMark > 60*1000)
	{
		CAgentMgr::GetInstance()->CleanUpAgentMap();
		m_iPreTimeMark = iCurTimeMark;
	}
}

void CUtility::ScanTXTSessionTimeout()
{
	//每 20s 扫描 1 次
	unsigned long iCurTimeMark = gettickcount();
	if (iCurTimeMark - m_iPreSessionTimeMark > 20 * 1000)
	{
		CAgentMgr::GetInstance()->ClearTXTSessionTimeout();
		m_iPreSessionTimeMark = iCurTimeMark;
	}
}

void CUtility::OnAgentCallReceived(SIPCCMsg_Client_AgentCallReceived *agentCallReceived, unsigned long ip, unsigned short port)
{	
	string sAccount = agentCallReceived->GetTheOtherParty();
	string sTempSub = sAccount.substr(0, 8);

	if ( sTempSub == "CClient_" )
	{
		sAccount = sAccount.substr(8, sAccount.length()-8);

		CCClientInfo* pCClientInfo_caller = CCClientMgr::GetInstance()->GetCClientInfo(sAccount);
		if ( pCClientInfo_caller == NULL )
		{
			return;
		}

		CAgentInfo* pAgentInfo_Called = CAgentMgr::GetInstance()->GetAgentInfo(agentCallReceived->GetAgentID());
		if (pAgentInfo_Called == NULL)
		{
			return;
		}

		SIPCCMsg_ACD_CallArriveToAgent callArriveToAgent;
		callArriveToAgent.SetAgentID(agentCallReceived->GetAgentID());
		callArriveToAgent.lanIP = pAgentInfo_Called->m_ulLanIP;
		callArriveToAgent.lanMainSockPort = pAgentInfo_Called->m_usLanMainSockPort;
		callArriveToAgent.agentExtNo = agentCallReceived->agentExtNo;
		callArriveToAgent.audioStreamDescription = agentCallReceived->audioStreamDescription;
		callArriveToAgent.callID = pCClientInfo_caller->m_CallID_sCallIn;
		callArriveToAgent.agentNickName = agentCallReceived->nickName;
		callArriveToAgent.callingAgentID = pCClientInfo_caller->m_sAccount;
		callArriveToAgent.managerID = pAgentInfo_Called->m_sManagerid;
		callArriveToAgent.callType = agentCallReceived->callType;

		this->SendIPCCMsg_UDP(callArriveToAgent, pCClientInfo_caller->m_ulUserIP, pCClientInfo_caller->m_usUserPort);
		
		return;
	}

	//通知呼叫座席已经分配到被叫座席
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(agentCallReceived->GetTheOtherParty());
	if(pAgentInfo == NULL)
		return;

	//冗余保护
	if(pAgentInfo->m_iLoginType == LoginType_WebPSTN)
		return;

	CAgentInfo* pAgentInfo_Called = CAgentMgr::GetInstance()->GetAgentInfo(agentCallReceived->GetAgentID());
	if(pAgentInfo_Called == NULL)
		return;

	pAgentInfo_Called->sipAgentPhoneIP1 = agentCallReceived->sipAgentPhoneIP1;
	pAgentInfo_Called->sipAgentPhoneRtpPort1 = agentCallReceived->sipAgentPhoneRtpPort1;
	pAgentInfo_Called->sipAgentPhoneRtcpPort1 = agentCallReceived->sipAgentPhoneRtcpPort1;

	SIPCCMsg_ACD_CallArriveToAgent callArriveToAgent;
	callArriveToAgent.SetAgentID(agentCallReceived->GetAgentID());
	callArriveToAgent.lanIP = pAgentInfo_Called->m_ulLanIP;
	callArriveToAgent.lanMainSockPort = pAgentInfo_Called->m_usLanMainSockPort;
	//ehang sip话机ip 端口
	callArriveToAgent.sipAgentPhoneIP1 = agentCallReceived->sipAgentPhoneIP1;
	callArriveToAgent.sipAgentPhoneRtpPort1 = agentCallReceived->sipAgentPhoneRtpPort1;
	callArriveToAgent.sipAgentPhoneRtcpPort1 = agentCallReceived->sipAgentPhoneRtcpPort1;

	callArriveToAgent.agentExtNo = agentCallReceived->agentExtNo;
    callArriveToAgent.audioStreamDescription = agentCallReceived->audioStreamDescription;
	callArriveToAgent.callType = agentCallReceived->callType;

	////////////ostringstream os;
	////////////string sTemp("");
	////////////os << "OnAgentCallReceived;" << "sender's sip:" << callArriveToAgent.sipAgentPhoneIP1;
	////////////os << "," << agentCallReceived->sipAgentPhoneRtpPort1 << "," << agentCallReceived->sipAgentPhoneRtcpPort1;
	////////////sTemp = os.str();
	////////////g_Logger.MakeLog(sTemp);
	this->SendIPCCMsg_UDP(callArriveToAgent, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);
}

void CUtility::OnGetCallingQueueInfo(SIPCCMsg_Client_GetCallingQueueInfo *getCallingQueueInfo, unsigned long ip, unsigned short port)
{

    //////ostringstream os;
    //////os.str();
    //////os << "SIPCCMsg_Client_GetCallingQueueInfo:" << "username=" << getCallingQueueInfo->GetUserName()
    //////    << "department=" << getCallingQueueInfo->GetTheDepartment();
    //////MakeLog(os.str());

	// [scq/start/2010-05-11]
	string sTempStr = getCallingQueueInfo->GetTheDepartment();
	string sCorp = "";
	string sDepart = "";

	// 拆分公司和技能组
	if ( !SplitCorpAndDepart(sTempStr, sCorp, sDepart) ){
		return;
	}

	//察看 corp
	CCorporationInfo* corp = CCorporationMgr::GetInstance()->GetCorporation(sCorp);
	if ( NULL==corp ){
		return;
	}

	SIPCCMsg_ACD_CallingQueueInfo callingQueueInfo;
	callingQueueInfo.invokeID = getCallingQueueInfo->invokeID;
	callingQueueInfo.userNumInQueue = 0;
	callingQueueInfo.agentNumInTheDepartment = 0;
	string userName = getCallingQueueInfo->GetUserName();

    CAgentInfo *pAgent = CAgentMgr::GetInstance()->GetAgentInfo(userName);
    if (pAgent)
    {
        CDepartmentInfo* pDepartmentInfo = NULL;
        pDepartmentInfo = CCorporationMgr::GetInstance()->GetDepartmentInfo(sCorp, sDepart);
        if (NULL == pDepartmentInfo){
            return;
        }
        callingQueueInfo.userNumInQueue = pDepartmentInfo->GetCQueueSize();
        callingQueueInfo.agentNumInTheDepartment = pDepartmentInfo->GetAgentNum();

        CPriorityQueue<AgentSkillInfo>::Iterator it = pAgent->m_SkillList.Begin();
        for (; it != pAgent->m_SkillList.End(); it++) {
            CSkillGroupInfo* pSkillGroupInfo =
                CCorporationMgr::GetInstance()->GetSkillGroupInfo(sCorp, it->m_Value.id);
            if (pSkillGroupInfo != NULL) {
                callingQueueInfo.userNumInQueue += pSkillGroupInfo->GetCQueueSize();
            }
        }

        ////////os.str();
        ////////os << "SIPCCMsg_ACD_CallingQueueInfo:" << "userNumInQueue=" << callingQueueInfo.userNumInQueue
        ////////    << "agentNumInTheDepartment=" << callingQueueInfo.agentNumInTheDepartment
        ////////    << "userNumInQueues" << callingQueueInfo.userNumInQueues;
        ////////MakeLog(os.str());

        this->SendIPCCMsg_UDP(callingQueueInfo, ip, port);
        return;
    }


	// sDepart格式	userName为空时, DeptName|SGId1|SGId2|……或userName不为空时, DeptName或SGId
	if ( userName != "" )
	{
		CSkillGroupInfo* pSkillGroupInfo = NULL;
		CDepartmentInfo* pDepartmentInfo = NULL;

		pSkillGroupInfo = CCorporationMgr::GetInstance()->GetSkillGroupInfo(sCorp, sDepart);

		if ( pSkillGroupInfo == NULL )
		{
			pDepartmentInfo = CCorporationMgr::GetInstance()->GetDepartmentInfo(sCorp, sDepart);
			if ( NULL!=pDepartmentInfo )
			{
				callingQueueInfo.userNumInQueue = pDepartmentInfo->GetCQueueSize();
				callingQueueInfo.agentNumInTheDepartment = pDepartmentInfo->GetAgentNum();
			}
			else
			{
				return;
			}
		}
		else
		{
            callingQueueInfo.userNumInQueue = pSkillGroupInfo->GetCQueueSize();
			callingQueueInfo.agentNumInTheDepartment = pSkillGroupInfo->GetAgentNum();
		}
	}
	else
	{
		CSpliter strDepart(sDepart, "|");
		string strDepartTemp = "";
		string strResult = "";
		ostringstream os;
		os.str("");

		for ( int i=0; i<strDepart.GetSize(); i++ )
		{
			strDepartTemp = strDepart[i];
			if ( !strDepartTemp.empty() )
			{
				if ( i == 0 )
				{
					//察看 department 是否存在
					CDepartmentInfo* pDepartmentInfo = 
						CCorporationMgr::GetInstance()->GetDepartmentInfo(sCorp, strDepartTemp);
					if ( NULL!=pDepartmentInfo )
					{
                        callingQueueInfo.userNumInQueue = pDepartmentInfo->GetCQueueSize();
                        os << pDepartmentInfo->GetCQueueSize() << "|";
                        //todo
					}
					else
					{
						return;
					}
				}
				else
				{
					CSkillGroupInfo* pSkillGroupInfo = CCorporationMgr::GetInstance()->GetSkillGroupInfo(sCorp, strDepartTemp);
					if ( NULL!=pSkillGroupInfo )
					{
                        callingQueueInfo.userNumInQueue += pSkillGroupInfo->GetCQueueSize();
                        os << pSkillGroupInfo->GetCQueueSize() << "|";

					}
					else
					{
						os << "|";
					}
				}
			}
		}

		strResult = os.str();
		if ( !strResult.empty() )
		{
			callingQueueInfo.userNumInQueues = strResult.substr( 0, strResult.length()-1 );
		}
	}

    ////////os.str();
    ////////os << "SIPCCMsg_ACD_CallingQueueInfo:" << "userNumInQueue=" << callingQueueInfo.userNumInQueue
    ////////    << "agentNumInTheDepartment=" << callingQueueInfo.agentNumInTheDepartment
    ////////    << "userNumInQueues" << callingQueueInfo.userNumInQueues;
	
    ////////MakeLog(os.str());
	this->SendIPCCMsg_UDP(callingQueueInfo, ip, port);
}

void CUtility::OnAgentReset(SIPCCMsg_Client_AgentReset *agentReset, unsigned long ip, unsigned short port)
{
	//座席复位，删除相关 call , 从waitingList中删除座席， 删除座席
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(agentReset->GetAgentID());
	if(pAgentInfo == NULL)
		return;

	CDepartmentInfo* pDepartmentInfo = CCorporationMgr::GetInstance()->GetDepartmentInfo(pAgentInfo->m_sCorporation, pAgentInfo->m_sDepartment);
	if(pDepartmentInfo != NULL)
		pDepartmentInfo->RemoveFromWAQueue(agentReset->GetAgentID());
	
	//发送座席 logout 的消息给 DataCollectServer
	SIPCCMsg_ACD_AgentStateInfo agentStateInfo;
	agentStateInfo.SetAgentID(agentReset->GetAgentID());
	agentStateInfo.SetCorporationName(pAgentInfo->m_sCorporation);
	agentStateInfo.SetTheDepartment(pAgentInfo->m_sDepartment);
	agentStateInfo.managerid = pAgentInfo->m_sManagerid;
	agentStateInfo.deptid = pAgentInfo->m_sDeptid;
	agentStateInfo.agentState = Agent_NoLogin;
	//agentStateInfo.SetCurTime(pAgentInfo->m_sCurStateBeginTime);
	agentStateInfo.SetCurTime(CLog::GetLocalTime());
	agentStateInfo.SetCallID("");
	agentStateInfo.SetKeyWord("");
	agentStateInfo.SetUserName("");
	agentStateInfo.nickName = pAgentInfo->m_sNickName;
	

	//begin  add code by hyj 2013-8-27
	//string slogtrans=CUtility::GetInstance()->GetMsgLogTransContext(&agentStateInfo);
	agentStateInfo.sData="";
	//string strlog=CUtility::GetInstance()->m_sLogTransSign2+slogtrans;
	//g_Logger.MakeLog(strlog);
	//end --hyj

	SendIPCCMsg_UDP(
		agentStateInfo, 
		CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_IP, 
		CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_Port);
	
	CUtility::GetInstance()->SendToMonitorServer(agentStateInfo);

	CAgentMgr::GetInstance()->DelAgent(agentReset->GetAgentID());
}

string CUtility::CreateCallID()
{
	string sCallID="";
#ifdef _WIN32
	 sCallID = (LPCTSTR)BSTRToSessionId((BSTR)GetGId());
#else
     sCallID = BSTRToSessionId((char*)GetGId().bstr.c_str());
#endif
	return sCallID;
}

string CUtility::GetNewInvokeID()
{
	static int iCount = 0;
	static long iTime = time(NULL);

	char c[20] = {0};
	int iLen = sprintf(c,"%08X%08X", iTime, iCount );

	iCount++;

	return c;
}

void CUtility::OnAgentQueryState(SIPCCMsg_Client_AgentQueryState *agentQueryState, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");
	string sTemp("");

	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(agentQueryState->GetAgentID());
	if(pAgentInfo == NULL)
	{
		os << "OnAgentQueryState; pAgentInfo==NULL; AgentName="
			<< agentQueryState->GetAgentID()
			<< "; state=" << agentQueryState->agentState;
		sTemp = os.str();
		g_Logger.MakeLog(sTemp);

		SIPCCMsg_ACD_ForceLogout forceLogout;
		forceLogout.cause = ForceLogoutCause_NoExist;
		forceLogout.agentName = agentQueryState->GetAgentID();
		this->SendIPCCMsg_UDP(forceLogout, ip, port);

		return;
	}
	pAgentInfo->bAgentDormancy = false;
	pAgentInfo->PushFlag = 0;

	int state = agentQueryState->agentState;

	SIPCCMsg_ACD_AgentQueryStateConf agentQueryStateConf;
	agentQueryStateConf.handleId = agentQueryState->handleId;

	if ( state < Agent_NoLogin || state > Agent_IVRnWait )
	{
		agentQueryStateConf.bQuerySucess = 0;
		SendIPCCMsg_UDP(agentQueryStateConf, ip, port);
		g_Logger.MakeLog("--座席申请无效状态--");
		return;
	}

	if ( pAgentInfo->m_iCurState == state )
	{
		os << "OnAgentQueryState; 坐席状态设置重复; AgentName="
			<< agentQueryState->GetAgentID()
			<< "; curstate=" << agentQueryState->agentState
			<< "; state=" << state;
		sTemp = os.str();
		g_Logger.MakeLog(sTemp);

		agentQueryStateConf.SetAdditionalInfo("");
		agentQueryStateConf.agentState = state;
		agentQueryStateConf.bQuerySucess = 1;
		SendIPCCMsg_UDP(agentQueryStateConf, ip, port);

		return;
	}
	////begin add code by heyj conference/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if( ( (pAgentInfo->m_iCurState ==Agent_ConferencePreside) && (state == Agent_BackProcess)) ||((pAgentInfo->m_iCurState==Agent_ConferenceJoin) && (state == Agent_BackProcess)))
	{
	    pAgentInfo->ClearConferenceParam();//清除会议信息
	}

	if(state == Agent_ConferencePreside)//主控
	{
	  if(pAgentInfo->m_iCurState ==Agent_TalkUnderHold || pAgentInfo->m_iCurState ==Agent_ConferenceJoin)
	  {
	   // 
	   // g_Logger.MakeLog("允许坐席主控开会");
	  }
	  else
	  {
		  agentQueryStateConf.bQuerySucess = 0;
		  SendIPCCMsg_UDP(agentQueryStateConf, ip, port);
		  g_Logger.MakeLog("会议主持坐席申请失败，坐席当前状态不是 【保持下通话】或【参会状态】");
		  return;
	  }
	}
	
	if( (pAgentInfo->m_iCurState!=Agent_OpTalking) && (state == Agent_ConferenceJoin))//参与
	{
		agentQueryStateConf.bQuerySucess = 0;
		SendIPCCMsg_UDP(agentQueryStateConf, ip, port);
		g_Logger.MakeLog("会议参与坐席申请失败，坐席当前状态不是 【被动席间通话】");
		return ;
	}
    ///end add code  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if ( pAgentInfo->m_iCurState != Agent_Talking && state == Agent_IVRnWait )
	{
		//请求状态失败
		agentQueryStateConf.agentState = state;
		agentQueryStateConf.bQuerySucess = 0;
		SendIPCCMsg_UDP(agentQueryStateConf, ip, port);

		os.str("");
		os << "OnAgentQueryState; m_iCurState="
			<< pAgentInfo->m_iCurState
			<< "; AgentName="
			<< agentQueryState->GetAgentID()
			<< "; state=" << agentQueryState->agentState;
		sTemp = os.str();
		g_Logger.MakeLog(sTemp);

		return;
	}

    g_Logger.MakeLog("agent:" + pAgentInfo->to_str());
    
    if (pAgentInfo->m_iCurState == Agent_Ringing)	//座席震铃
	{
        unsigned long tip = gettickcount() - pAgentInfo->m_iStateBeginTime;
		if(!(state==Agent_Talking || 
            state==Agent_OpTalking ||
            state == Agent_BackProcess || 
            state == Agent_Waiting || 
            (state == Agent_Parking && tip > 5000)))
		{
			//请求状态失败
			agentQueryStateConf.agentState = state;
			agentQueryStateConf.bQuerySucess = 0;
			SendIPCCMsg_UDP(agentQueryStateConf, ip, port);

			os.str("");
			os << "OnAgentQueryState; m_iCurState==Agent_Ringing; AgentName="
				<< agentQueryState->GetAgentID()
				<< "; state=" << agentQueryState->agentState;
			sTemp = os.str();
			g_Logger.MakeLog(sTemp);

			if (state == Agent_ForeProcess || state == Agent_Parking)
			{
				g_Logger.MakeLog("当坐席为振铃状态时，坐席外拔请求不再在处理");
			}
			else
			{
				CAgentMgr::GetInstance()->SetAgentState(pAgentInfo->m_sAgentID, Agent_NoLogin);
				SIPCCMsg_ACD_AgentLogoutConf agentLogoutConf;
				agentLogoutConf.agentId = agentQueryState->GetAgentID();
				this->SendIPCCMsg_UDP(agentLogoutConf, ip, port);
				CAgentMgr::GetInstance()->DelAgent(agentQueryState->GetAgentID());
			}

			return;		
		}
		//VDN ===========
        else if (((state == Agent_Talking) || (state == Agent_OpTalking)) && pAgentInfo->m_bCallFromVDN)
		{
            pAgentInfo->m_bCallFromVDN = false;

			CSkillGroupInfo* pSkillGroupInfo = NULL;
			CDepartmentInfo* pDepartmentInfo = NULL;

			SIPCCMsg_ACD_CallLeaveVDN CallLeaveVDN;
			if ( pAgentInfo->m_sCurSkillInfo.id == "" )
			{
				CallLeaveVDN.vdnNo = pAgentInfo->m_sDepartment;
				CallLeaveVDN.deptid = pAgentInfo->m_sDeptid;

				pDepartmentInfo = CCorporationMgr::GetInstance()->GetDepartmentInfo( pAgentInfo->m_sCorporation, pAgentInfo->m_sDepartment );
				if ( pDepartmentInfo != NULL )
                    CallLeaveVDN.CurQueueNum = pDepartmentInfo->GetCQueueSize();
			}
			else
			{
                CallLeaveVDN.vdnNo = pAgentInfo->m_sCurSkillInfo.name;
                CallLeaveVDN.SkillGroupID = pAgentInfo->m_sCurSkillInfo.id;
				CallLeaveVDN.SkillGroupName = pAgentInfo->m_sCurSkillInfo.name;

				pSkillGroupInfo = CCorporationMgr::GetInstance()->GetSkillGroupInfo( pAgentInfo->m_sCorporation, pAgentInfo->m_sCurSkillInfo.id );
				if ( pSkillGroupInfo != NULL )
                    CallLeaveVDN.CurQueueNum = pSkillGroupInfo->GetCQueueSize();
			}
			CallLeaveVDN.UserLevel = pAgentInfo->m_userInfo.m_iUserLevel;//如果非电话呼入m_iUserLevel为0
			CallLeaveVDN.corpName = pAgentInfo->m_sCorporation;
			CallLeaveVDN.callID = pAgentInfo->m_sCallID;
			CallLeaveVDN.leaveTime = CLog::GetLocalTime();
			CallLeaveVDN.leaveCause = VDNEndCause_Normal;
			CallLeaveVDN.managerid = pAgentInfo->m_sManagerid;
			CallLeaveVDN.LeaveQueueTime = pAgentInfo->m_sQueueArriveTime;
			CallLeaveVDN.agentID = pAgentInfo->m_sAgentID;
			CallLeaveVDN.bIntoQueue = pAgentInfo->m_bCallFromQueue;

			CUtility::GetInstance()->SendIPCCMsg_UDP(
				CallLeaveVDN, 
				CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_IP, 
				CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_Port);

			CUtility::GetInstance()->SendToMonitorServer(CallLeaveVDN);
		}
		//VDN ===========
        if (state == Agent_Waiting || state == Agent_Talking){
			if (!pAgentInfo->IsPC()){
				pAgentInfo->delCallDelivered(pAgentInfo->m_sCallID);
			}
		}
	}

	string sCallID("");
	string sCallTransID("");
	
	switch(state)
	{
	case Agent_Dialing:
		{
			if(pAgentInfo->m_iCurState == Agent_TalkUnderHold)
			{
				state = Agent_Talking;
			}

			pAgentInfo->bCallin = false;
			pAgentInfo->bInnerCall = (agentQueryState->bInnerCall==0)?false:true;
			pAgentInfo->bPSTNCall = (agentQueryState->bPSTNCall==0)?false:true;

			//sCallID = this->GetNewInvokeID();
			sCallID = GetNewVersionInvokeID();
			pAgentInfo->SetCallID(sCallID);
			sCallTransID = sCallID;
			break;
		}
	case Agent_DialUnderHold:
		{
			pAgentInfo->bCallin_Hold = false;
			pAgentInfo->bInnerCall_Hold = (agentQueryState->bInnerCall==0)?false:true;
			pAgentInfo->bPSTNCall_Hold = (agentQueryState->bPSTNCall==0)?false:true;

//			sCallID = this->CreateCallID();
			sCallID = pAgentInfo->m_sCallID;
			//pAgentInfo->m_sCallID_Hold = this->GetNewInvokeID();
			sCallTransID = pAgentInfo->m_sCallID;
			break;
		}
    //add by hyyj 2012-12-17  
	case Agent_ConferencePreside:
	case Agent_ConferenceJoin:
		{
		
		   pAgentInfo->bApplyforConference = true;
		
		}
		break;

	default:
		break;
	}
 
	//暂时返回成功消息
	agentQueryStateConf.SetAdditionalInfo("");
	agentQueryStateConf.agentState = state;
	agentQueryStateConf.bQuerySucess = 1;
	agentQueryStateConf.SetAdditionalInfo(sCallID);
	agentQueryStateConf.callTransID = sCallTransID;
	SendIPCCMsg_UDP(agentQueryStateConf, ip, port);

	if(state == Agent_Parking)
	{
		pAgentInfo->m_iParkReasonID = agentQueryState->parkReasonID;
		pAgentInfo->m_sParkReason = agentQueryState->GetParkReason();
	}

	if ( pAgentInfo->m_iCurState == Agent_Talking && (state == Agent_Waiting || state == Agent_BackProcess) )
	{
		CDepartmentInfo* pDepartmentInfo = 
			CCorporationMgr::GetInstance()->GetDepartmentInfo(pAgentInfo->m_sCorporation, pAgentInfo->m_sDepartment);
		if(pDepartmentInfo == NULL)
		{
			//置座席状态为 Agent_NoLogin
			CAgentMgr::GetInstance()->SetAgentState(pAgentInfo->m_sAgentID, Agent_NoLogin);
			//返回 AgentLogoutConf消息
			SIPCCMsg_ACD_AgentLogoutConf agentLogoutConf;
			agentLogoutConf.agentId = agentQueryState->GetAgentID();
			this->SendIPCCMsg_UDP(agentLogoutConf, ip, port);

			//从 agentMap 中删除座席
			CAgentMgr::GetInstance()->DelAgent(agentQueryState->GetAgentID());

			return;
		}
	}

	pAgentInfo->SetState(state);
}

void CUtility::OnWpAgent_LineNo(SIPCCMsg_Gateway_WpAgent_LineNo* pMsg, unsigned long ip, unsigned short port)
{
	ostringstream os;
	string sLog("");
	os << "WpAgent_LineNo:"
		<< "agentID=" << pMsg->agentID
		<< ";gwAgentLine=" << pMsg->gwAgentLine
		<< ";gwUserLine=" << pMsg->gwUserLine
		<< ";ip=" << ip
		<< ";port=" << port;

	sLog = os.str();
	g_Logger.MakeLog(sLog);

	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(pMsg->agentID);
	if(pAgentInfo == NULL)
	{
		g_Logger.MakeLog("没有坐席信息");
		return;
	}

	pAgentInfo->m_WebPstnAgentCurGwLine = pMsg->gwAgentLine;
	pAgentInfo->m_CurGwUserLineNo = pMsg->gwUserLine;

	SIPCCMsg_ACD_WpAgent_LineNo msg2;
	msg2.agentID = pMsg->agentID;
	msg2.gwAgentLine = pMsg->gwAgentLine;
	msg2.gwUserLine = pMsg->gwUserLine;

	SendIPCCMsg_UDP(msg2, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);
}

void CUtility::OnWpAgentRelease(SIPCCMsg_Gateway_WpAgentRelease* pMsg, unsigned long ip, unsigned short port)
{
	ostringstream os;
	string sLog("");
	os << "OnWpAgentRelease:"
		<< "agentID=" << pMsg->agentID
		<< ";ip=" << ip
		<< ";port=" << port;

	sLog = os.str();
	g_Logger.MakeLog(sLog);

	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(pMsg->agentID);
	if(pAgentInfo == NULL)
	{
		g_Logger.MakeLog("没有坐席信息");
		return;
	}

	if(pAgentInfo->m_iLoginType != LoginType_WebPSTN)
	{
		g_Logger.MakeLog("坐席不是WebPSTN");
		return;
	}

	pAgentInfo->SetState(Agent_BackProcess);

	pAgentInfo->SetClientAgentState(Agent_BackProcess);
}

void CUtility::OnClient_CallUserConf(SIPCCMsg_Client_CallUserConf *callUserConf, unsigned long ip, unsigned short port)
{
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(callUserConf->GetCallerParty());
    if (pAgentInfo == NULL) {
        char* theBuf = NULL;
        int theBufContent = 0;
        CIPCCMsgExchanger msgExchanger;
        msgExchanger.SerializeMsg(*callUserConf, theBuf, theBufContent);

        SIPCCMsg_ACD_UniversalFailure universalFailure;
        universalFailure.errorCode = SIPCCMsg_ACD_UniversalFailure::AGENT_NOT_FOUND;
        universalFailure.setOrgMessage(theBuf, theBufContent);

        if (theBuf != NULL)
            delete[] theBuf;

        SendIPCCMsg_UDP(universalFailure, ip, port);

		return;
    }


	if(pAgentInfo->m_iCurState == Agent_DialUnderHold && pAgentInfo->m_sCalledParty != "")
	{
		pAgentInfo->m_sCalledParty = callUserConf->GetUserName();
	}
	else if ( pAgentInfo->m_iCurState == Agent_Dialing )
	{
		pAgentInfo->m_sCalledNo = callUserConf->calleeNo;
		pAgentInfo->m_sCalledParty = callUserConf->GetUserName();
	}
	
	SIPCCMsg_ACD_CallUserConf acd_CallUserConf;
	acd_CallUserConf.bCallFailed = callUserConf->bCallFailed;
	acd_CallUserConf.invokeID = callUserConf->invokeID;
	acd_CallUserConf.SetUserName(callUserConf->GetUserName());
	acd_CallUserConf.audioIP = callUserConf->audioIP;
	acd_CallUserConf.audioPort = callUserConf->audioPort;
	acd_CallUserConf.lanIP = callUserConf->lanIP;

	acd_CallUserConf.sipBoardIP  =			callUserConf->sipBoardIP ;
	acd_CallUserConf.sipBoardRtpPort  =     callUserConf->sipBoardRtpPort ;
	acd_CallUserConf.sipBoardRtcpPort   =	callUserConf->sipBoardRtcpPort;
	acd_CallUserConf.sipTransIP  =			callUserConf->sipTransIP      ;
	acd_CallUserConf.sipTransRtpPort  =		callUserConf->sipTransRtpPort ;
	acd_CallUserConf.sipTransRtcpPort   =	callUserConf->sipTransRtcpPort;

	acd_CallUserConf.trans = callUserConf->trans;
	acd_CallUserConf.codec = callUserConf->codec;
	acd_CallUserConf.unit = callUserConf->unit;

	SendIPCCMsg_UDP(acd_CallUserConf, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

	////ostringstream os;
	////string sLog("");
	////os << "acd_CallUserConf:"
	////	<< "bCallFailed=" << (int)callUserConf->bCallFailed
	////	<< ";username=" << callUserConf->GetUserName()
	////	<< ";agnetip=" << pAgentInfo->m_ulAgentIP
	////	<< ";agnetport=" << pAgentInfo->m_usAgentPort
	////	<< ";username=" << callUserConf->GetUserName()
	////	<< ";sipBoardIP ="      << ConvertNatIP2HostIP(callUserConf->sipBoardIP).c_str()
	////	<< ";sipBoardRtpPort =" << ConvertNatPort2HostPort(callUserConf->sipBoardRtpPort)
	////	<< ";sipBoardRtcpPort=" << ConvertNatPort2HostPort(callUserConf->sipBoardRtcpPort)
	////	<< ";sipTransIP      =" << ConvertNatIP2HostIP(callUserConf->sipTransIP).c_str()
	////	<< ";sipTransRtpPort =" << ConvertNatPort2HostPort(callUserConf->sipTransRtpPort)
	////	<< ";sipTransRtcpPort=" <<ConvertNatPort2HostPort(callUserConf->sipTransRtcpPort);

	////sLog = os.str();
	////g_Logger.MakeLog(sLog);
	
	/*if(acd_CallUserConf.bCallFailed != 0)
	{
		SIPCCMsg_ACD_NotifyAgentStateEvent notifyAgentStateEvent;
		notifyAgentStateEvent.SetCorporationName(pAgentInfo->m_sCorporation);
		notifyAgentStateEvent.SetTheDepartment(pAgentInfo->m_sDepartment);
		notifyAgentStateEvent.SetAgentID(pAgentInfo->m_sAgentID);
		notifyAgentStateEvent.stateEvent = Event_DialFailed;
		notifyAgentStateEvent.SetCallTarger(callUserConf->GetCalledNo());
		notifyAgentStateEvent.SetCurrentTime(CLog::GetLocalTime());
	}*/
}

void CUtility::OnClient_UserAnswerCall(SIPCCMsg_Client_UserAnswerCall *userAnswerCall, unsigned long ip, unsigned short port)
{
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(userAnswerCall->GetTheOTherParty());
	if(pAgentInfo == NULL)
	{
		//给用户发送 callAnswered
		SIPCCMsg_ACD_CallAnswered callAnswered;
		callAnswered.SetUserName(userAnswerCall->GetUserName());
		callAnswered.handleId=userAnswerCall->handleId;//by heyj 2011-08-15
		//add callID
		SendIPCCMsg_UDP(callAnswered, ip, port);

		//接着给用户发送 callReleased
		SIPCCMsg_UState_CallHanguped callHanguped;
		callHanguped.SetUserName(userAnswerCall->GetUserName());
		callHanguped.SetReleasedParty(userAnswerCall->GetTheOTherParty());
		callHanguped.handleId=userAnswerCall->handleId;//by heyj 2011-08-15
		this->SendIPCCMsg_UDP(callHanguped, ip, port);

		return;
	}

	if(pAgentInfo->m_iLoginType == LoginType_OnlyPSTN)
	{
		if ( userAnswerCall->outDialType == OutDialType_WebPSTNDial )
		{
			if ( userAnswerCall->lineType == 1 )
			{
				pAgentInfo->m_WebPstnAgentCurGwLine = userAnswerCall->userLineNo;
				pAgentInfo->m_CurGwLineIP = userAnswerCall->userLineIp;
				pAgentInfo->m_CurGwLinePort = userAnswerCall->userLinePort;

				return;
			}

			CSpliter sp(userAnswerCall->privateData, ";");

			if ( sp[0] == "WebPSTNDial" )
			{
				pAgentInfo->m_sPrivateData = userAnswerCall->privateData;
			}
			else
			{
				return;
			}

			string sCallerNo = sp[3];
			if ( sCallerNo.length() > 1 )
			{
				sCallerNo = sCallerNo.substr(1, sCallerNo.length());
			}
			string sCalledNo = sp[4];
			if ( sCalledNo.length() > 1 )
			{
				sCalledNo = sCalledNo.substr(1, sCalledNo.length());
			}	
			string sInvokeid = sp[7];
			string sUserData = "";
			if ( sp[10] == "WebService" )
			{
				sUserData = sp[11];
			}
			
			SIPCCMsg_WebService_DialingCall webService_DialingCall;
			webService_DialingCall.ani = sCallerNo;
			webService_DialingCall.dnis = sCalledNo;
			webService_DialingCall.ucid = sInvokeid;
			webService_DialingCall.director = 1;
			webService_DialingCall.agentId = pAgentInfo->m_sAgentID;
			webService_DialingCall.station = pAgentInfo->m_sAgentPhoneNo;
			//CTime ct = CTime::GetCurrentTime();
			//webService_DialingCall.operationTime = ct.Format("%Y-%m-%d %H:%M:%S");
			boost::posix_time::ptime pt = boost::posix_time::second_clock::local_time();
         	string strTime = boost::gregorian::to_iso_extended_string(pt.date())+" "+boost::posix_time::to_simple_string(pt.time_of_day());
		    webService_DialingCall.operationTime =strTime;
			webService_DialingCall.event = "Dial";
			webService_DialingCall.userData = sUserData;

			SendIPCCMsg_UDP(webService_DialingCall, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

			string sLog("");
			ostringstream os;
			os.str("");
			os <<  "WebService_DialingCall: "
				<< "ani=" << sCallerNo
				<< ";dnis=" << sCalledNo
				<< ";ucid=" << sInvokeid
				<< ";director=1"
				<< ";agentId=" << pAgentInfo->m_sAgentID
				<< ";station=" << pAgentInfo->m_sAgentPhoneNo
				<< ";userData=" << sUserData
				<< ";ip=" << pAgentInfo->m_ulAgentIP
				<< ";port=" << pAgentInfo->m_usAgentPort;

			sLog = os.str();
			g_Logger.MakeLog(sLog);
		}
		else if ( userAnswerCall->outDialType == OutDialType_RingAgentExt && userAnswerCall->lineType == 1 )
		{
			CSpliter strSp(pAgentInfo->m_userInfo.m_sUUIData, "%^#");
			string sIVRTrack = strSp[2];

			SIPCCMsg_WebService_RingingCall webService_RingingCall;
			webService_RingingCall.ani = pAgentInfo->m_userInfo.m_sCallerNo;
			webService_RingingCall.dnis = pAgentInfo->m_sAgentPhoneNo;
			webService_RingingCall.ucid = pAgentInfo->m_sCallID;
			webService_RingingCall.director = 0;
			webService_RingingCall.ivrTrack = sIVRTrack;
			webService_RingingCall.agentId = pAgentInfo->m_sAgentID;
			webService_RingingCall.station = pAgentInfo->m_sAgentPhoneNo;
			//CTime ct = CTime::GetCurrentTime();
			//webService_RingingCall.operationTime = ct.Format("%Y-%m-%d %H:%M:%S");
			boost::posix_time::ptime pt = boost::posix_time::second_clock::local_time();
			string strTime = boost::gregorian::to_iso_extended_string(pt.date())+" "+boost::posix_time::to_simple_string(pt.time_of_day());
            webService_RingingCall.operationTime = strTime;
			webService_RingingCall.event = "Ringing";

			SendIPCCMsg_UDP(webService_RingingCall, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

			string sLog("");
			ostringstream os;
			os.str("");
			os <<  "WebService_RingingCall: "
				<< "ani=" << pAgentInfo->m_userInfo.m_sCallerNo
				<< ";dnis=" << pAgentInfo->m_sAgentPhoneNo
				<< ";ucid=" << pAgentInfo->m_sCallID
				<< ";director=0"
				<< ";ivrTrack=" << sIVRTrack
				<< ";agentId=" << pAgentInfo->m_sAgentID
				<< ";station=" << pAgentInfo->m_sAgentPhoneNo
				<< ";ip=" << pAgentInfo->m_ulAgentIP
				<< ";port=" << pAgentInfo->m_usAgentPort;

			sLog = os.str();
			g_Logger.MakeLog(sLog);
		}

		return;
	}

	pAgentInfo->m_CurGwUserLineNo = userAnswerCall->userLineNo;
	pAgentInfo->m_CurUserLineIP = userAnswerCall->userLineIp;
	pAgentInfo->m_CurUserLinePort = userAnswerCall->userLinePort;

	//给座席发送 callEstablished
	SIPCCMsg_ACD_CallEstablished callEstablished;
	callEstablished.SetTheOtherParty(userAnswerCall->GetUserName());
	callEstablished.handleId=userAnswerCall->handleId;//by heyj 2011-08-15
    callEstablished.audioStreamDescription = userAnswerCall->audioStreamDescription;
	callEstablished.userLineNo = userAnswerCall->userLineNo;
	callEstablished.userLineIp = userAnswerCall->userLineIp;
	callEstablished.userLinePort = userAnswerCall->userLinePort;
	SendIPCCMsg_UDP(callEstablished, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

	//给用户发送 callAnswered
	SIPCCMsg_ACD_CallAnswered callAnswered;
	callAnswered.SetUserName(userAnswerCall->GetUserName());
	callAnswered.handleId=userAnswerCall->handleId;//by heyj 2011-08-15
	//add callID
	callAnswered.callId = pAgentInfo->m_sCallID;
	SendIPCCMsg_UDP(callAnswered, ip, port);
}

void CUtility::OnClient_UserAnswerCall_Real(SIPCCMsg_Client_UserAnswerCall_Real* userAnswerCall, unsigned long ip, unsigned short port)
{
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(userAnswerCall->GetTheOTherParty());
	if(pAgentInfo == NULL)
	{
		//给用户发送 callAnswered
		SIPCCMsg_ACD_CallAnswered callAnswered;
		callAnswered.SetUserName(userAnswerCall->GetUserName());
		callAnswered.handleId=userAnswerCall->handleId;//by heyj 2011-08-15
		//add callID
		SendIPCCMsg_UDP(callAnswered, ip, port);

		//接着给用户发送 callReleased
		SIPCCMsg_UState_CallHanguped callHanguped;
		callHanguped.SetUserName(userAnswerCall->GetUserName());
		callHanguped.SetReleasedParty(userAnswerCall->GetTheOTherParty());	
		callHanguped.handleId=userAnswerCall->handleId;//by heyj 2011-08-15
		this->SendIPCCMsg_UDP(callHanguped, ip, port);

		return;
	}

	//给座席发送 callEstablished_Real
	SIPCCMsg_ACD_CallEstablished_Real callEstablished;
	callEstablished.SetTheOtherParty(userAnswerCall->GetUserName());
	callEstablished.userLineNo = userAnswerCall->userLineNo;
	callEstablished.userLineIp = userAnswerCall->userLineIp;
	callEstablished.userLinePort = userAnswerCall->userLinePort;
	
	SendIPCCMsg_UDP(callEstablished, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);
}

void CUtility::On_SIPCCMsg_Client_AgentMakeCall(SIPCCMsg_Client_AgentMakeCall *agentMakeCall, unsigned long ip, unsigned short port)
{

	switch (agentMakeCall->callType)
	{
	case E_MESSAGE_CALL:case E_WEBCHAT_CALL:case E_EMAIL_CALL:
	{
		return OnMakeMessageCall(*agentMakeCall, ip, port);
	}
	case E_NORMAL_CALL:case E_VIDEO_CALL:
	{
		string sAgentID_caller = agentMakeCall->GetAgentID();
		CAgentInfo* pAgentInfo_caller = CAgentMgr::GetInstance()->GetAgentInfo(sAgentID_caller);
		if (pAgentInfo_caller == NULL || pAgentInfo_caller->m_iLoginType == LoginType_WebPSTN)
			return;

		if (agentMakeCall->targetType == CallParty_Agent)
			func_AgentCallAgent(agentMakeCall, ip, port);
		else if (agentMakeCall->targetType == CallParty_Department)
			func_AgentCallDepartment(agentMakeCall, ip, port);
		return;
	}

	default:
		break;

	}

	ostringstream os;
	string sTemp("");
	os << "On_SIPCCMsg_Client_AgentMakeCall: unknown calltype =  " << agentMakeCall->callType;
	sTemp = os.str();
	g_Logger.MakeLog(sTemp);

	/*
    if (agentMakeCall->callType != E_NORMAL_CALL)
	{
        return OnMakeMessageCall(*agentMakeCall, ip, port);
	}

	string sAgentID_caller = agentMakeCall->GetAgentID();
	CAgentInfo* pAgentInfo_caller = CAgentMgr::GetInstance()->GetAgentInfo(sAgentID_caller);
	if (pAgentInfo_caller == NULL || pAgentInfo_caller->m_iLoginType == LoginType_WebPSTN)
		return;

	if(agentMakeCall->targetType == CallParty_Agent)
		func_AgentCallAgent(agentMakeCall, ip, port);
	else if(agentMakeCall->targetType == CallParty_Department)
		func_AgentCallDepartment(agentMakeCall, ip, port);
	*/
}

void CUtility::OnMakeCallViaClient(SIPCCMsg_MakeCallViaClient* makeCallViaClient, unsigned long ip, unsigned short port)
{
    CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(makeCallViaClient->calllingDevice);
    if (pAgentInfo == NULL) {
        return;
    }
    this->SendIPCCMsg_UDP(*makeCallViaClient, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

}

void CUtility::func_AgentCallAgent(SIPCCMsg_Client_AgentMakeCall *agentMakeCall, unsigned long ip, unsigned short port)
{
	string sAgentID_caller = agentMakeCall->GetAgentID();
	CAgentInfo* pAgentInfo_caller = CAgentMgr::GetInstance()->GetAgentInfo(sAgentID_caller);

	if(pAgentInfo_caller==NULL)	
		return;

	CAgentInfo* pAgentInfo_called = CAgentMgr::GetInstance()->GetAgentInfo(agentMakeCall->GetTargetPosition());

	if(pAgentInfo_called == NULL)
	{
		//座席未登录
		SIPCCMsg_ACD_AgentMakeCallConf agentMakeCallConf;
		agentMakeCallConf.callingState = AgentCalling_AgentNoLogin;
		agentMakeCallConf.callType = agentMakeCall->callType;
		this->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);
		return;
	}

	int error = 0;
	if (!CheckStateBeforeMakeCall(pAgentInfo_caller, pAgentInfo_called, error))
	{
		//查看被叫座席状态
		SIPCCMsg_ACD_AgentMakeCallConf agentMakeCallConf;
		agentMakeCallConf.callingState = error;
		agentMakeCallConf.callType = agentMakeCall->callType;
		this->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);
		return;
	}

	CDepartmentInfo* pDepartmentInfo =
		CCorporationMgr::GetInstance()->GetDepartmentInfo(pAgentInfo_called->m_sCorporation, pAgentInfo_called->m_sDepartment);
	if (pDepartmentInfo == NULL)
	{
		//座席未登录
		SIPCCMsg_ACD_AgentMakeCallConf agentMakeCallConf;
		agentMakeCallConf.callingState = AgentCalling_AgentNoLogin;
		agentMakeCallConf.callType = agentMakeCall->callType;
		this->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);
		return;
	}

	//1
	pAgentInfo_caller->m_sUUIData = agentMakeCall->GetUUIData();
	pAgentInfo_caller->sipAgentPhoneIP1 = agentMakeCall->sipAgentPhoneIP1;
	pAgentInfo_caller->sipAgentPhoneRtpPort1 = agentMakeCall->sipAgentPhoneRtpPort1;
	pAgentInfo_caller->sipAgentPhoneRtcpPort1 = agentMakeCall->sipAgentPhoneRtcpPort1;
	pAgentInfo_caller->agentUUIData = agentMakeCall->AgentUUIData(); //  [2/23/2009 lixg]
	pAgentInfo_caller->originalData = agentMakeCall->originalData;
	pAgentInfo_caller->audioStreamDescription = agentMakeCall->audioStreamDescription;
	pAgentInfo_caller->m_sCalledParty = agentMakeCall->GetTargetPosition();
	pAgentInfo_caller->m_sTheOtherAgentID = agentMakeCall->GetTargetPosition();

	//2
	if(pAgentInfo_called->m_iLoginType == LoginType_OnlyWeb)
		pAgentInfo_called->m_iCalledPartyType = CalledPartyType_WebAgent;
	else if(pAgentInfo_called->m_iLoginType == LoginType_WebPSTN)
		pAgentInfo_called->m_iCalledPartyType = CalledPartyType_WpAgent;

    pAgentInfo_called->m_bCallFromVDN = false;
    pAgentInfo_called->m_bCallFromQueue = false;
    pAgentInfo_called->m_sTheOtherAgentID = agentMakeCall->GetAgentID();
	pAgentInfo_called->SetCallID(agentMakeCall->GetCallID());
	pAgentInfo_called->bCallin = true;
	pAgentInfo_called->bInnerCall = (pAgentInfo_called->m_sCorporation == pAgentInfo_caller->m_sCorporation);
	pAgentInfo_called->bPSTNCall = false;

	//3
	//置座席 Ringing
	pAgentInfo_called->SetState(Agent_Ringing);

	//4
	//被叫座席空闲
	SIPCCMsg_ACD_AgentMakeCallConf agentMakeCallConf;
	agentMakeCallConf.callingState = AgentCalling_AgentRinging;
	agentMakeCallConf.SetCalledParty(pAgentInfo_called->m_sAgentID);
	agentMakeCallConf.lanIP = pAgentInfo_called->m_ulLanIP,
	agentMakeCallConf.lanMainSockPort = pAgentInfo_called->m_usLanMainSockPort;
	if(pAgentInfo_called->m_iLoginType == LoginType_WebPSTN)
		agentMakeCallConf.agentExtNo = pAgentInfo_called->m_sAgentPhoneNo;
	agentMakeCallConf.callType = agentMakeCall->callType;
	this->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);

	//5
	//发送 Delivered
	boost::shared_ptr<SIPCCMsg_ACD_CallDelivered> callDelivered(new SIPCCMsg_ACD_CallDelivered());
	callDelivered->SetCallerName(sAgentID_caller);
	if(pAgentInfo_caller->m_iLoginType == LoginType_WebPSTN)
	{
		callDelivered->callerType = CallParty_WebPstnAgent;
		callDelivered->audioIP = pAgentInfo_caller->m_CurGwLineIP;
		callDelivered->audioPort = pAgentInfo_caller->m_CurGwLinePort;
		callDelivered->wPstnAgentLineNo = pAgentInfo_caller->m_WebPstnAgentCurGwLine;
		callDelivered->userLineNo = pAgentInfo_caller->m_CurGwUserLineNo;
		callDelivered->wPstnAgentLineNo = pAgentInfo_caller->m_CurGwUserLineNo;
	}
	else
	{	
		callDelivered->callerType = CallParty_Agent;
		callDelivered->userLineNo = agentMakeCall->userLineNo;
		callDelivered->userLineIP = agentMakeCall->userLineIp;
		callDelivered->userLinePort = agentMakeCall->userLinePort;
	}
	callDelivered->AgentUUIData(pAgentInfo_caller->agentUUIData); //  [2/23/2009 lixg]
	callDelivered->SetCallID(agentMakeCall->GetCallID());
	callDelivered->SetUUIData(agentMakeCall->GetUUIData());
    callDelivered->uuiData2 = agentMakeCall->departmentID;
	callDelivered->gwPartNo = pAgentInfo_caller->m_userInfo.m_sPartNo;
	callDelivered->lanIP = pAgentInfo_caller->m_ulLanIP;
	callDelivered->lanMainSockPort = pAgentInfo_caller->m_usLanMainSockPort;
	callDelivered->sipAgentPhoneIP1 = agentMakeCall->sipAgentPhoneIP1;
	callDelivered->sipAgentPhoneRtcpPort1 = agentMakeCall->sipAgentPhoneRtcpPort1;
	callDelivered->sipAgentPhoneRtpPort1 = agentMakeCall->sipAgentPhoneRtpPort1;
	callDelivered->originalData = agentMakeCall->originalData;
	callDelivered->comSymbol = agentMakeCall->comSymbol;
	callDelivered->audioStreamDescription = agentMakeCall->audioStreamDescription;
	callDelivered->originalAgentID = agentMakeCall->orginalAgentID;
	callDelivered->originalCallID = agentMakeCall->orginalCallID;
	callDelivered->callType = agentMakeCall->callType;

	if ( pAgentInfo_called->IsAllowedPush())
	{
		pAgentInfo_called->addListCallDelivered(callDelivered);
		//若座席进入后台，推送来电信息
		Push_Message(pAgentInfo_called->m_sAgentID,agentMakeCall->callType,callDelivered->GetCallID());
	}

	this->SendIPCCMsg_UDP(*callDelivered.get(), pAgentInfo_called->m_ulAgentIP, pAgentInfo_called->m_usAgentPort);
 

}

void CUtility::func_AgentCallDepartment(SIPCCMsg_Client_AgentMakeCall *agentMakeCall, unsigned long ip, unsigned short port)
{
    //1、conf消息通用属性赋值
    SIPCCMsg_ACD_AgentMakeCallConf agentMakeCallConf;
    agentMakeCallConf.callID = agentMakeCall->GetCallID();
    agentMakeCallConf.callingAgentID = agentMakeCall->GetAgentID();
    agentMakeCallConf.SetCalledParty(agentMakeCall->GetTargetPosition());
    agentMakeCallConf.callType = agentMakeCall->callType;

    //2、查找并设置caller属性
	string sAgentID_caller = agentMakeCall->GetAgentID();
	CAgentInfo* pAgentInfo_caller = CAgentMgr::GetInstance()->GetAgentInfo(sAgentID_caller);

    if (pAgentInfo_caller == NULL) {
        agentMakeCallConf.callingState = AgentCalling_AgentNoLogin;
        this->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);
        return;
    }

	int error = 0;
	if (!CheckCallerStateBeforeMakeCall(pAgentInfo_caller, error))
	{
		agentMakeCallConf.callingState = error;
		this->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);
		return;
	}

	pAgentInfo_caller->m_sUUIData = agentMakeCall->GetUUIData();
	pAgentInfo_caller->sipAgentPhoneIP1 = agentMakeCall->sipAgentPhoneIP1;
	pAgentInfo_caller->sipAgentPhoneRtpPort1 = agentMakeCall->sipAgentPhoneRtpPort1;
	pAgentInfo_caller->sipAgentPhoneRtcpPort1 = agentMakeCall->sipAgentPhoneRtcpPort1;
	pAgentInfo_caller->agentUUIData = agentMakeCall->AgentUUIData(); //  [2/23/2009 lixg]
	pAgentInfo_caller->originalData = agentMakeCall->originalData;
	pAgentInfo_caller->audioStreamDescription = agentMakeCall->audioStreamDescription;
	pAgentInfo_caller->m_sCallID = agentMakeCall->GetCallID();
	pAgentInfo_caller->m_iAgentEnqueueTime = gettickcount();
	pAgentInfo_caller->m_iCalledPartyType = CalledPartyType_Department;
	pAgentInfo_caller->comSymbol = agentMakeCall->comSymbol;
	pAgentInfo_caller->originalAgentID = agentMakeCall->orginalAgentID;
	pAgentInfo_caller->originalCallID = agentMakeCall->orginalCallID;
	pAgentInfo_caller->m_iCallType = agentMakeCall->callType;

    //3、查找并设置target
    bool bCallSG = true;
    CSkillGroupInfo* pSkillGroupInfo = NULL;
    CDepartmentInfo* pDepartmentInfo = NULL;

    pSkillGroupInfo = CCorporationMgr::GetInstance()->GetSkillGroupInfo(agentMakeCall->GetTargetPosition());
    if (pSkillGroupInfo == NULL) {
        bCallSG = false;

        string sCorp;
        string sDepartment;
        CUtility::GetInstance()->SplitCorpAndDepart(agentMakeCall->GetTargetPosition(), sCorp, sDepartment);
        pAgentInfo_caller->m_sCalledCorp = sCorp;
        pAgentInfo_caller->m_sCalledDepart = sDepartment;

        pDepartmentInfo = CCorporationMgr::GetInstance()->GetDepartmentInfo(agentMakeCall->GetTargetPosition());
        if (pDepartmentInfo == NULL) {
            agentMakeCallConf.callingState = AgentCalling_DepartmentInexistence;
            this->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);
            return;
        }
    }

    //4、分配呼叫（结果：分配到座席，或电话排队）
    //0错误, 1呼叫成功分配， 2排队
    pair<int, boost::any> deliverState =
        (pDepartmentInfo == NULL) ? pSkillGroupInfo->CallIn(pAgentInfo_caller->m_sAgentID, CallParty_Agent, pAgentInfo_caller)
                                  : pDepartmentInfo->CallIn(pAgentInfo_caller->m_sAgentID, CallParty_Agent, pAgentInfo_caller);

    //5、设置本地变量
    CAgentInfo* pAgentInfo = NULL;
    if (deliverState.first == 1) {//分配到座席
        pAgentInfo = any_cast<CAgentInfo*>(deliverState.second);
        pAgentInfo->m_bCallFromVDN = true; //呼组的都进vdn
        pAgentInfo->m_bCallFromQueue = false; //呼组的都进vdn

        pAgentInfo_caller->m_sCalledParty = pAgentInfo->m_sAgentID;
        pAgentInfo_caller->m_sTheOtherAgentID = pAgentInfo->m_sAgentID;
    }

    //6、发送conf消息
    switch (deliverState.first) {
    case 1: {//分配到座席
        agentMakeCallConf.callingState = AgentCalling_AgentRinging;
        agentMakeCallConf.SetCalledParty(pAgentInfo->m_sAgentID);
        agentMakeCallConf.lanIP = pAgentInfo->m_ulLanIP;
        agentMakeCallConf.lanMainSockPort = pAgentInfo->m_usLanMainSockPort;
        break;
    }
    case 2: {//排队
        agentMakeCallConf.callingState = AgentCalling_AgentQueuing;
        agentMakeCallConf.userNumInQueue = any_cast<int>(deliverState.second);
        break;
    }
    }

    this->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);

    //7、发送vdn消息
    SIPCCMsg_ACD_CallEnterVDN CallEnterVDN;
    if (bCallSG) {
        CallEnterVDN.vdnName = pSkillGroupInfo->m_sSkillGroupid;
        CallEnterVDN.vdnNo = pSkillGroupInfo->m_sSkillGroupName;
        CallEnterVDN.managerid = pSkillGroupInfo->m_sManagerid;
        CallEnterVDN.SkillGroupID = pSkillGroupInfo->m_sSkillGroupid;
        CallEnterVDN.SkillGroupName = pSkillGroupInfo->m_sSkillGroupName;
    }
    else {
        CallEnterVDN.vdnName = pDepartmentInfo->m_sDepartmentName;
        CallEnterVDN.vdnNo = pDepartmentInfo->m_sDepartmentName;
        CallEnterVDN.managerid = pDepartmentInfo->m_sManagerid;
        CallEnterVDN.deptid = pDepartmentInfo->m_sDeptid;
    }
    CallEnterVDN.callerID = sAgentID_caller;
    CallEnterVDN.calleeID = pAgentInfo_caller->m_sCalledDepart;
    CallEnterVDN.callType = pAgentInfo_caller->m_iCalledPartyType;
    CallEnterVDN.enterTime = CLog::GetLocalTime();
    CallEnterVDN.callID = pAgentInfo_caller->m_sCallID;
    CallEnterVDN.corpName = pAgentInfo_caller->m_sCalledCorp;
    CallEnterVDN.callSrc = "";
    CallEnterVDN.extDevID = "";
    CallEnterVDN.status = 0;
    CallEnterVDN.bInnerCall = 1;
    CallEnterVDN.bIntoQueue = 0;
    CallEnterVDN.CurQueueNum = 0;

    if (deliverState.first == 2) {
        CallEnterVDN.bIntoQueue = 1;
        CallEnterVDN.CurQueueNum = any_cast<int>(deliverState.second);
    }

    CUtility::GetInstance()->SendIPCCMsg_UDP(
        CallEnterVDN,
        CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_IP,
        CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_Port);

    CUtility::GetInstance()->SendToMonitorServer(CallEnterVDN);
}

UINT CUtility::Thread_NotifyDataCollect(void* lpThis)
{
	CUtility* pUtility = (CUtility*)lpThis;
  
	SIPCCMsg_ACD_NotifyDataCollect notifyDataCollect;

	while(pUtility->m_bNotifyDataCollect)
	{

		string logtransconst="";
		string strlog="";
		//
		int iHour;				
		int iMinute;
		int iSecond;
		GetLocalTimeInfo(iHour, iMinute, iSecond);
		switch(iMinute)
		{
		case 0:case 15:case 30:case 45:
			{
				if(iMinute != pUtility->m_iPreNotifyMinute)
				{
					time_t ltime;
					ltime = time(NULL);
					char str[32];
					strftime(str, sizeof(str), "%Y-%m-%d %H:%M:00", localtime(&ltime));

					//发送消息出去给 DataCollectServer
					notifyDataCollect.CurrentHour = iHour;
					notifyDataCollect.theMinute = iMinute;
					notifyDataCollect.collectDate = str;
					//begin add code by hyj 2013-8-28
					notifyDataCollect.sData="";
					logtransconst=pUtility->GetMsgLogTransContext(&notifyDataCollect);					
					strlog=pUtility->m_sLogTransSign3+logtransconst;
					g_Logger.MakeLog(strlog);
					//end--hyj


					pUtility->SendIPCCMsg_UDP(notifyDataCollect, 
						pUtility->m_IPCCConfig.DataCollectServer_IP,
						pUtility->m_IPCCConfig.DataCollectServer_Port);

					CUtility::GetInstance()->SendToMonitorServer(notifyDataCollect);

					pUtility->m_iPreNotifyMinute = iMinute;

					//log
					string sLog;
					ostringstream ostream;
					ostream.str("");
					//ostream << "NotifyDataCollect,minute:" << iMinute;
					ostream << "NotifyDataCollect, collectDateTime:" << str;
					sLog = ostream.str();
					g_Logger.MakeLog(sLog);
				}
				break;
			}
		default:
			break;
		}
		//Sleep(1000);
		  boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
	}

	return 1;	
}

void CUtility::OnOutDial_SetOutdialGroup(SIPCCMSG_OutDial_SetOutdialGroup* ipccMessage, unsigned long ip, unsigned short port)
{
	CDepartmentInfo* pDept = CDepartmentMgr::GetInstance()->GetDept(ipccMessage->GroupName);
	if (pDept)
	{
		pDept->m_OutDialSys_Info.bIsOutDialGroup = true;
		pDept->m_OutDialSys_Info.OutdialEngIp = ip;
		pDept->m_OutDialSys_Info.OutdialEngPort = port;
		pDept->IsmapChanged = true;
		pDept->SendOutDialSys_DeptAgentsInfo();
	}
}

void CUtility::OnOutDial_ExitOutDial(SIPCCMSG_OutDial_ExitOutDial* ipccMessage, unsigned long ip, unsigned short port)
{
	CAgentInfo* pAgent = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->AgentId);
	if (pAgent)
	{
		pAgent->bExitOutdial = true;

		if(pAgent->m_iCurState == Agent_BackProcess)
		{
			CDepartmentInfo* pDept = pAgent->IsInOutDialDepartment(pAgent->m_sDepartment);
			if(pDept)
			{
				//结束处理时发送 OutDialSys 消息
				SIPCCMsg_OutDial_EndProcess	OutDial_EndProcess;
				OutDial_EndProcess.TaskId = pAgent->m_OutDialSys_Info.TaskId;
				OutDial_EndProcess.PhoneId = pAgent->m_OutDialSys_Info.PhoneId;
				OutDial_EndProcess.DialSeq = pAgent->m_OutDialSys_Info.DialSeq;
				OutDial_EndProcess.AgentName = pAgent->m_sAgentID;

				CUtility::GetInstance()->SendIPCCMsg_UDP(
					OutDial_EndProcess, pDept->m_OutDialSys_Info.OutdialEngIp, pDept->m_OutDialSys_Info.OutdialEngPort);
			}

		}
	}
}

void CUtility::OnOutDial_JoinOutDial(SIPCCMSG_OutDial_JoinOutDial* ipccMessage, unsigned long ip, unsigned short port)
{
	CAgentInfo* pAgent = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->AgentId);
	if (pAgent)
	{
		pAgent->bExitOutdial = false;
	}
}

void CUtility::OnSysManage_MoveAgent(SIPCCMSG_SysManage_MoveAgent* ipccMessage, unsigned long ip, unsigned short port)
{
	CDepartmentInfo* pDept = CDepartmentMgr::GetInstance()->GetDept(ipccMessage->DeptmentId);
	if (!pDept)
	{
		g_Logger.MakeLog("没有该组");
		return;
	}
	string sCorp = pDept->m_sCorpName;
	string sDept = pDept->m_sDepartmentName;
	int iIsOutdial = 2;
	if (ipccMessage->IsOutdial == "0")
	{
		iIsOutdial = 0;
	}
	else if (ipccMessage->IsOutdial == "1")
	{
		iIsOutdial = 1;
	}

	CSpliter cs(ipccMessage->AgentList, ";");
	for (int i = 0; i < cs.GetSize(); i++)
	{
		string sAgent = cs[i];
		CAgentInfo* pAgent = CAgentMgr::GetInstance()->GetAgentInfo(sAgent);
		if (pAgent)
		{
			SIPCCMSG_ACD_SwitchDept msg;
			msg.Corp = sCorp;
			msg.Dept = sDept;
			msg.IsOutdial = iIsOutdial;
			SendIPCCMsg_UDP(msg, pAgent->m_ulAgentIP, pAgent->m_usAgentPort);
		}
		else
		{
			ostringstream ostream;
			ostream << "没找到坐席:" << sAgent;
			g_Logger.MakeLog(ostream.str());
		}
	}
}

void CUtility::OnClient_CallUserFailed(SIPCCMsg_Client_CallUserFailed *callUserFailed, unsigned long ip, unsigned short port)
{
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(callUserFailed->GetCallerParty());
	if(pAgentInfo == NULL)
		return;

	SIPCCMsg_ACD_CallUserFailed acd_CallUserFailed;
	acd_CallUserFailed.SetUserName(callUserFailed->GetUserName());
	acd_CallUserFailed.failedCause = callUserFailed->failedCause;
	SendIPCCMsg_UDP(acd_CallUserFailed, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

	/*SIPCCMsg_ACD_NotifyAgentStateEvent notifyAgentStateEvent;
	notifyAgentStateEvent.SetCorporationName(pAgentInfo->m_sCorporation);
	notifyAgentStateEvent.SetTheDepartment(pAgentInfo->m_sDepartment);
	notifyAgentStateEvent.SetAgentID(pAgentInfo->m_sAgentID);
	notifyAgentStateEvent.stateEvent = Event_DialFailed;
	notifyAgentStateEvent.SetCallTarger(callUserFailed->GetCalledNo());
	notifyAgentStateEvent.SetCurrentTime(CLog::GetLocalTime());*/
	//this->SendIPCCMsg_UDP(notifyAgentStateEvent, this->m_IPCCConfig.DataCollectServer_IP, ConvertPort(ServerInfoDef::Port_DataCollectServer));
}

void CUtility::OnClient_AgentSwitchToFrontDesk(SIPCCMsg_Client_AgentSwitchToFrontDesk* ipccMessage, unsigned long ip, unsigned short port)
{
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->agentID);
	if (pAgentInfo == NULL)
		return;
	pAgentInfo->OnAgentSwitchToFrontDesk(*ipccMessage, ip, port);

}

void CUtility::OnClient_PrepareTransfer(SIPCCMsg_Client_PrepareTransfer *prepareTransfer, unsigned long ip, unsigned short port)
{
	string destAgent = prepareTransfer->GetTheParty();
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(destAgent);
	if(pAgentInfo == NULL)
		return;
/*
	if(prepareTransfer->otherPartyType == CallParty_WebPstnAgent)
	{
		CAgentInfo* pPresideAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(prepareTransfer->GetOtherParty());
		if(pPresideAgentInfo->m_iLoginType == LoginType_WebPSTN)
		{
			SIPCCMsg_ACD_PrepareTransfer ACD_PrepareTransfer;
			ACD_PrepareTransfer.SetCallID(prepareTransfer->GetCallID());
			ACD_PrepareTransfer.SetOtherParty(prepareTransfer->GetOtherParty());
			ACD_PrepareTransfer.otherPartyType = prepareTransfer->otherPartyType;
			ACD_PrepareTransfer.AudioIp = pPresideAgentInfo->m_CurUserLineIP;
			ACD_PrepareTransfer.AudioPort = pPresideAgentInfo->m_CurUserLinePort;
			SendIPCCMsg_UDP(ACD_PrepareTransfer, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);
		}
		return;
	}
*/
	SIPCCMsg_ACD_PrepareTransfer ACD_PrepareTransfer;
	ACD_PrepareTransfer.SetCallID(prepareTransfer->GetCallID());
	ACD_PrepareTransfer.SetOtherParty(prepareTransfer->GetOtherParty());	//gwUser
	ACD_PrepareTransfer.otherPartyType = prepareTransfer->otherPartyType;	//
	ACD_PrepareTransfer.AudioIp = prepareTransfer->AudioIp;
	ACD_PrepareTransfer.AudioPort = prepareTransfer->AudioPort;
	ACD_PrepareTransfer.lanIP = prepareTransfer->lanIP;
	SendIPCCMsg_UDP(ACD_PrepareTransfer, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);
}

void CUtility::OnClient_TransReady(SIPCCMsg_Client_TransReady *transReady, unsigned long ip, unsigned short port)
{
	string destAgent = transReady->GetPresideAgent();
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(destAgent);
	if(pAgentInfo == NULL)
		return;

	string readyAgent = transReady->GetReadyParty();
	CAgentInfo* pReadyAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(readyAgent);
	if(pReadyAgentInfo == NULL)
		return;

	pReadyAgentInfo->m_sOriSkillInfo.id = pAgentInfo->m_sCurSkillInfo.id;
	pReadyAgentInfo->m_sOriSkillInfo.name = pAgentInfo->m_sCurSkillInfo.name;

	SIPCCMsg_ACD_TransReady ACD_TransReady;

	ACD_TransReady.SetReadyParty(transReady->GetReadyParty());
	SendIPCCMsg_UDP(ACD_TransReady, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);
}

void CUtility::OnClient_TransferCall(SIPCCMsg_Client_TransferCall *transferCall, unsigned long ip, unsigned short port)
{
	string destAgent = transferCall->GetTheParty();
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(destAgent);
	if(pAgentInfo == NULL)
		return;
	
	if(pAgentInfo->m_iLoginType == LoginType_WebPSTN)
	{
		if(transferCall->GetPresideAgent() != destAgent)
		{
			pAgentInfo->SetState(Agent_Talking);
			
			pAgentInfo->SetClientAgentState(Agent_Talking, SetAgentStateOperType_TransPSTNFromVoipAgent);

			SIPCCMsg_ACD_TransferCall_WpAgent ACD_TransferCall_WpAgent;
			ACD_TransferCall_WpAgent.SetPresideAgent(transferCall->GetPresideAgent());
			ACD_TransferCall_WpAgent.SetOtherParty(transferCall->GetOtherParty());
			ACD_TransferCall_WpAgent.otherPartyType = transferCall->otherPartyType;
			ACD_TransferCall_WpAgent.wpAgentID = destAgent;
			ACD_TransferCall_WpAgent.wpAgentLineNo = pAgentInfo->m_WebPstnAgentCurGwLine;
			SendIPCCMsg_UDP(ACD_TransferCall_WpAgent, pAgentInfo->m_gwCtrlIP, pAgentInfo->m_gwCtrlPort);
			//return;
		}
	}

	SIPCCMsg_ACD_TransferCall ACD_TransferCall;
	ACD_TransferCall.SetPresideAgent(transferCall->GetPresideAgent());
	ACD_TransferCall.SetOtherParty(transferCall->GetOtherParty());
	ACD_TransferCall.SetTheParty(destAgent);
	ACD_TransferCall.userLineNo = transferCall->userLineNo;
	ACD_TransferCall.userLinePort = transferCall->userLinePort;
	ACD_TransferCall.userLineIp = transferCall->userLineIp;
	ACD_TransferCall.trans = transferCall->trans;
	ACD_TransferCall.codec = transferCall->codec;
	ACD_TransferCall.unit = transferCall->unit;
	ACD_TransferCall.audioStreamDescription = transferCall->audioStreamDescription;
	SendIPCCMsg_UDP(ACD_TransferCall, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

	CAgentInfo* pOldAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(transferCall->GetPresideAgent());
	if(pOldAgentInfo == NULL)
		return;

	pAgentInfo->bCallin = pOldAgentInfo->bCallin;
	pAgentInfo->bInnerCall = pOldAgentInfo->bInnerCall;
	pAgentInfo->bPSTNCall = pOldAgentInfo->bPSTNCall;
}

void CUtility::OnClient_ClearPreConnection(SIPCCMsg_Client_ClearPreConnection *clearPreConnection, unsigned long ip, unsigned short port)
{
	string destAgent = clearPreConnection->GetNotifiedParty();
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(destAgent);
	if(pAgentInfo == NULL)
		return;

	SIPCCMsg_ACD_ClearPreConnection ACD_ClearPreConnection;
	ACD_ClearPreConnection.SetNotifier(clearPreConnection->GetNotifier());
	SendIPCCMsg_UDP(ACD_ClearPreConnection, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);	
}

void CUtility::OnClient_AgentHoldCall(SIPCCMsg_Client_AgentHoldCall *agentHoldCall, unsigned long ip, unsigned short port)
{
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(agentHoldCall->GetHeldParty());
		
	SIPCCMsg_ACD_AgentHoldCallConf acd_AgentHoldCallConf;
	if(pAgentInfo==NULL || (pAgentInfo->m_iCurState!=Agent_Talking && pAgentInfo->m_iCurState!=Agent_OpTalking))
	{
		acd_AgentHoldCallConf.bHoldSucc = 0;
		this->SendIPCCMsg_UDP(acd_AgentHoldCallConf, ip, port);
		return;
	}

	acd_AgentHoldCallConf.bHoldSucc = 1;
	this->SendIPCCMsg_UDP(acd_AgentHoldCallConf, ip, port);

	SIPCCMsg_ACD_AgentHoldCall acd_AgentHoldCall;
	acd_AgentHoldCall.SetHeldParty(agentHoldCall->GetHeldParty());
	this->SendIPCCMsg_UDP(acd_AgentHoldCall, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);			
}

void CUtility::OnClient_AgentUnHoldCall(SIPCCMsg_Client_AgentUnHoldCall *agentUnHoldCall, unsigned long ip, unsigned short port)
{
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(agentUnHoldCall->GetHeldParty());
	if(pAgentInfo == NULL)
		return;
	
	SIPCCMsg_ACD_AgentUnHoldCall acd_AgentUnHoldCall;
	acd_AgentUnHoldCall.SetHeldParty(agentUnHoldCall->GetHeldParty());
	this->SendIPCCMsg_UDP(acd_AgentUnHoldCall, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);
}

bool CUtility::SplitCorpAndDepart(string sTempStr, string& sCorp, string& sDepart)
{
	CSpliter strSp(sTempStr, "%^#");
    if (strSp.GetSize() < 2){
        return false;
    }
	sCorp = strSp[0];
	sDepart = strSp[1];
	return true;
}

void CUtility::OnClient_GetDepartNAgentInfo(SIPCCMsg_Client_GetDepartNAgentInfo *getDepartNAgentInfo, unsigned long ip, unsigned short port)
{
	//获取组,座席信息
	string sCorp = getDepartNAgentInfo->GetCorporationName();
	string sDepart = getDepartNAgentInfo->GetTheDepartment();
	int iMode = getDepartNAgentInfo->getMode;

	if(sCorp == "")
		return;

	CCorporationInfo* corp = CCorporationMgr::GetInstance()->GetCorporation(sCorp);
	if(corp == NULL)
		return;

	corp->NotifyGetDepartNAgentInfo(getDepartNAgentInfo->invokeID, iMode, sDepart, ip, port);
}

void CUtility::OnClient_GetGroupRequest(SIPCCMsg_Client_GetGroupRequest *getGroupRequest, unsigned long ip, unsigned short port)
{
	if ((m_IPCCConfig.DataCollectServer_IP != ip) && (m_IPCCConfig.MonitorServer_IP == 0))
	{
		m_IPCCConfig.MonitorServer_IP = ip;
		m_IPCCConfig.MonitorServer_Port = port;
	}

	//获取组信息
	CCorporationMgr::GetInstance()->SendCorpGroupInfo(getGroupRequest->invokeID, ip, port);
}

void CUtility::MakeSendMsgLog(SIPCCMessage* ipccmessage, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");
	string sLog("");

	if(ipccmessage->msgType == Type_ACD_VDNStatusInfo)
	{
		SIPCCMsg_ACD_VDNStatusInfo* vDNStatusInfo = (SIPCCMsg_ACD_VDNStatusInfo*)ipccmessage;
		os << "VDNStatusInfo: corp=" << vDNStatusInfo->GetCorporationName()
			<< ";depart="<< vDNStatusInfo->GetTheDepartment()
			<< ";endTime=" << vDNStatusInfo->GetEndTime()
			<< ";callType=" << vDNStatusInfo->callType
			<< ";callerNo=" << vDNStatusInfo->GetCallerNo()
			<< ";calledNo=" << vDNStatusInfo->GetCalledNo()
			<< ";agentID=" << vDNStatusInfo->GetAgentID()
			<< ";responseSpan=" << vDNStatusInfo->responseTimeSpan
			<< ";endCause=" << vDNStatusInfo->endCause
			<< ";queueSpan=" << vDNStatusInfo->queueTimeSpan;

		sLog = os.str();
		g_Logger.MakeLog(sLog);
	}
}

void CUtility::OnClient_MediaReady(SIPCCMsg_Client_MediaReady *mediaReady, unsigned long ip, unsigned short port)
{
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(mediaReady->GetTargetName());
	if(pAgentInfo == NULL)
		return;
	
	SIPCCMsg_ACD_MediaReady acd_MediaReady;
	this->SendIPCCMsg_UDP(acd_MediaReady, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);
}

void CUtility::OnSysManage_DelACDCorp(SIPCCMsg_SysManage_DelACDCorp *delACDCorp, unsigned long ip, unsigned short port)
{
	//获取组,座席信息
	string sCorp = delACDCorp->corpName;

	if(sCorp == "")
		return;

	CCorporationMgr::GetInstance()->DelCorporationInfo(sCorp);
}

void CUtility::func_UserCallDepart(SIPCCMsg_Client_UserMakeCall *userMakeCall, unsigned long ip, unsigned short port, bool bIsOverflow)
{
    //1、conf消息设置通用属性
    SIPCCMsg_ACD_UserMakeCallConf userMakeCallConf;
    userMakeCallConf.SetUserName(userMakeCall->GetUserName());
    userMakeCallConf.handleId = userMakeCall->handleId;//by heyj 2011-08-15

    //2、查找呼叫目标
    bool bCallSG = true;
    CSkillGroupInfo* pSkillGroupInfo = NULL;
    CDepartmentInfo* pDepartmentInfo = NULL;

    pSkillGroupInfo = CCorporationMgr::GetInstance()->GetSkillGroupInfo(userMakeCall->GetTheDepartment());
    if (pSkillGroupInfo == NULL) {
        bCallSG = false;

        pDepartmentInfo = CCorporationMgr::GetInstance()->GetDepartmentInfo(userMakeCall->GetTheDepartment());
        if (pDepartmentInfo == NULL) {
            userMakeCallConf.callingState = UserCalling_Failed;
            this->SendIPCCMsg_UDP(userMakeCallConf, ip, port);
            return;
        }
    }

    //3、保存主叫信息
    CUserInfo* pUser = CUserMgr::GetInstance()->AddUser(
        userMakeCall->GetUserName(),
        userMakeCall->userType,
        99,
        userMakeCall->GetKeyWord(),
        userMakeCall->GetTheDepartment(),
        userMakeCall->GetUUIData(),
        userMakeCall->audioIP,
        userMakeCall->audioPort,
        userMakeCall->lanIP,
        userMakeCall->gwPartNo,
        userMakeCall->comSymbol,
        userMakeCall->callTransID,
        userMakeCall->sipBoardIP,
        userMakeCall->sipBoardRtpPort,
        userMakeCall->sipBoardRtcpPort,
        userMakeCall->sipTransIP,
        userMakeCall->sipTransRtpPort,
        userMakeCall->sipTransRtcpPort,
        userMakeCall->trans,
        userMakeCall->codec,
        userMakeCall->unit,
        userMakeCall->audioStreamDescription,
        userMakeCall->audioRtpPort,
        bCallSG,
        userMakeCall->UserLevel,
        bCallSG ? pSkillGroupInfo->m_sSkillGroupid : pDepartmentInfo->m_sDeptid
        );
    
    pUser->m_ulUserIP = ip;
    pUser->m_usUserPort = port;

    //4、发送vdn消息
    SIPCCMsg_ACD_CallEnterVDN CallEnterVDN;
    if (bCallSG) {
        CallEnterVDN.vdnNo = pSkillGroupInfo->m_sSkillGroupName;
        CallEnterVDN.vdnName = pSkillGroupInfo->m_sSkillGroupName;
        CallEnterVDN.managerid = pSkillGroupInfo->m_sManagerid;
        CallEnterVDN.SkillGroupID = pSkillGroupInfo->m_sSkillGroupid;
        CallEnterVDN.SkillGroupName = pSkillGroupInfo->m_sSkillGroupName;
    }
    else {
        CallEnterVDN.vdnNo = pDepartmentInfo->m_sDepartmentName;
        CallEnterVDN.vdnName = pDepartmentInfo->m_sDepartmentName;
        CallEnterVDN.managerid = pDepartmentInfo->m_sManagerid;
        CallEnterVDN.deptid = pDepartmentInfo->m_sDeptid;
    }

    CallEnterVDN.callerID = pUser->m_sCallerNo;
    CallEnterVDN.calleeID = pUser->m_sCalledNo;
    CallEnterVDN.callType = pUser->m_iCallType;
    CallEnterVDN.enterTime = CLog::GetLocalTime();
    CallEnterVDN.callID = pUser->m_CallID_sCallIn;
    CallEnterVDN.callSrc = "";
    CallEnterVDN.corpName = pUser->m_sCorpName;
    CallEnterVDN.extDevID = "";
    CallEnterVDN.status = 0;
    CallEnterVDN.bIsOverflow = bIsOverflow;
    CallEnterVDN.UserLevel = userMakeCall->UserLevel;
    CallEnterVDN.bInnerCall = 0;
    CallEnterVDN.bIntoQueue = 0;
    CallEnterVDN.CurQueueNum = 0;

    //5、分配呼叫（结果：分配给座席，或者呼叫排队）
    //0错误, 1呼叫成功分配， 2排队
    pair<int, boost::any> deliverState = 
        (pDepartmentInfo == NULL) ? pSkillGroupInfo->CallIn(pUser->m_sUserName, CallParty_User, pUser, userMakeCall->UserLevel)
                                  : pDepartmentInfo->CallIn(pUser->m_sUserName, CallParty_User, pUser, userMakeCall->UserLevel);

    //如果分配到了座席pUser会被删除
    pUser = NULL;

    //6、设置一些本地变量
    CAgentInfo* pAgentInfo = NULL;
    if (deliverState.first == 1) {//分配到座席
        pAgentInfo = any_cast<CAgentInfo*>(deliverState.second);
        pAgentInfo->m_bCallFromVDN = true; //呼组的都进vdn
        pAgentInfo->m_bCallFromQueue = false; //呼组的都进vdn
    }

    //7、发送conf消息
    switch (deliverState.first) {
    case 1: {//分配到座席
        userMakeCallConf.callingState = AgentCalling_AgentRinging;
        userMakeCallConf.SetAgentID(pAgentInfo->m_sAgentID);
        break;
    }
    case 2: default:{//排队
        userMakeCallConf.callingState = UserCalling_Queuing;
        userMakeCallConf.userNumInQueue = any_cast<int>(deliverState.second);
        break;
    }
    }
    this->SendIPCCMsg_UDP(userMakeCallConf, ip, port);

    //8、发送vdn消息
    if (deliverState.first == 2) { //排队
        CallEnterVDN.bIntoQueue = true;
        CallEnterVDN.CurQueueNum = any_cast<int>(deliverState.second);
    }

    CUtility::GetInstance()->SendIPCCMsg_UDP(
        CallEnterVDN,
        CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_IP,
        CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_Port);

    CUtility::GetInstance()->SendToMonitorServer(CallEnterVDN);
}

void CUtility::func_UserCallAgent(SIPCCMsg_Client_UserMakeCall *userMakeCall, unsigned long ip, unsigned short port, bool bIsOverflow)
{
	string sTempStr = userMakeCall->GetTheDepartment();
	string sAgentID = sTempStr.substr(6, sTempStr.length()-6);
	string sUserName = userMakeCall->GetUserName();

	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(sAgentID);

	SIPCCMsg_ACD_UserMakeCallConf userMakeCallConf;
	userMakeCallConf.SetUserName(userMakeCall->GetUserName());
	userMakeCallConf.handleId=userMakeCall->handleId;//by heyj 2011-08-15

	if(pAgentInfo == NULL)
	{
		userMakeCallConf.callingState = UserCalling_AgentNoLogon;
		this->SendIPCCMsg_UDP(userMakeCallConf, ip, port);
		return;
	}

	if(pAgentInfo->m_iCurState != Agent_Waiting)
	{
		userMakeCallConf.callingState = UserCalling_AgentBusy;
		this->SendIPCCMsg_UDP(userMakeCallConf, ip, port);
		return;
	}

	userMakeCallConf.callingState = UserCalling_Ringing;
	userMakeCallConf.SetAgentID(sAgentID);
	this->SendIPCCMsg_UDP(userMakeCallConf, ip, port);
	
	CUserInfo userInfo;
	userInfo.m_sUserName = sUserName;
	userInfo.m_iUserType = userMakeCall->userType; 
	userInfo.m_sOriginCalledTarget = userMakeCall->GetTheDepartment();
	userInfo.m_sCalledKeyWord = userMakeCall->GetKeyWord();
	userInfo.m_sUUIData = userMakeCall->GetUUIData();
	userInfo.m_audioIP = userMakeCall->audioIP;
	userInfo.m_audioPort = userMakeCall->audioPort;
	userInfo.m_audioRtpPort = userMakeCall->audioRtpPort;
	userInfo.m_lanIP = userMakeCall->lanIP;
    userInfo.m_asd = userMakeCall->audioStreamDescription;
    userInfo.isQueueing = false;
    userInfo.m_iQueueType = 0;
    userInfo.m_sQueueID = "";

	string sCorp("");
	string sDepartment("");
	CUtility::GetInstance()->SplitCorpAndDepart(userMakeCall->GetTheDepartment(), sCorp, sDepartment);
	userInfo.m_sCorpName = sCorp;
    userInfo.m_sQueueName = sDepartment;

	if(userInfo.m_iUserType == CallParty_VoiceGateWay)
	{
		CSpliter strSp(userInfo.m_sUUIData, "%^#");
		userInfo.m_sCallerNo = strSp[0];
		userInfo.m_sCalledNo = strSp[1];

		userInfo.m_iCallType = IPCCCallType_PSTN;

		pAgentInfo->m_CurGwUserLineNo = userMakeCall->GetUserName();
		pAgentInfo->m_CurUserLineIP = userMakeCall->audioIP;
		pAgentInfo->m_CurUserLinePort = userMakeCall->audioPort;
	}
	else
	{
		userInfo.m_sCallerNo = sUserName;
		userInfo.m_sCalledNo = userInfo.m_sCalledKeyWord;
		userInfo.m_iCallType = IPCCCallType_Internet;
	}

	pAgentInfo->UpdateUserInfo(&userInfo);

	string sCallID = userMakeCall->callTransID;
	pAgentInfo->SetCallID(sCallID);
    pAgentInfo->m_bCallFromVDN = false;
    pAgentInfo->m_bCallFromQueue = false;
    pAgentInfo->bCallin = true;
	pAgentInfo->bInnerCall = false;
	if(userInfo.m_iUserType == CallParty_VoiceGateWay)
		pAgentInfo->bPSTNCall = true;
	else
		pAgentInfo->bPSTNCall = false;

	//置座席 Ringing
	pAgentInfo->SetState(Agent_Ringing);

	//发送 Delivered
	//增加 comSymbol 设置
	boost::shared_ptr<SIPCCMsg_ACD_CallDelivered> callDelivered(new SIPCCMsg_ACD_CallDelivered);
	callDelivered->SetCallerName(sUserName);
	callDelivered->SetCallID(sCallID);
	callDelivered->SetKeyWord(userMakeCall->GetKeyWord());
	callDelivered->SetUUIData(userMakeCall->GetUUIData());
	callDelivered->callerType = userMakeCall->userType;
	callDelivered->audioIP = userMakeCall->audioIP;
	callDelivered->audioPort = userMakeCall->audioPort;
	callDelivered->audioRtpPort = userMakeCall->audioRtpPort;
	callDelivered->sipBoardIP = userMakeCall->sipBoardIP;
	callDelivered->sipBoardRtpPort = userMakeCall->sipBoardRtpPort;
	callDelivered->sipBoardRtcpPort = userMakeCall->sipBoardRtcpPort;
	callDelivered->sipTransIP = userMakeCall->sipTransIP;
	callDelivered->sipTransRtpPort = userMakeCall->sipTransRtpPort;
	callDelivered->sipTransRtcpPort = userMakeCall->sipTransRtcpPort;
	callDelivered->lanIP = userMakeCall->lanIP;
	callDelivered->gwPartNo = userMakeCall->gwPartNo;
	callDelivered->userLineNo = userMakeCall->GetUserName();
	callDelivered->userLineIP = userMakeCall->audioIP;
	callDelivered->userLinePort = userMakeCall->audioPort;
	callDelivered->comSymbol = userMakeCall->comSymbol;
	callDelivered->trans = userMakeCall->trans;
	callDelivered->codec = userMakeCall->codec;
	callDelivered->unit = userMakeCall->unit;
    callDelivered->audioStreamDescription = userMakeCall->audioStreamDescription;
	
	if(pAgentInfo->m_iLoginType == LoginType_WebPSTN)
	{
		callDelivered->audioIP = pAgentInfo->m_CurGwLineIP;
		callDelivered->audioPort = pAgentInfo->m_CurGwLinePort;
		callDelivered->wPstnAgentLineNo = pAgentInfo->m_WebPstnAgentCurGwLine;
		callDelivered->userLineNo = userMakeCall->GetUserName();
		callDelivered->userLineIP = userMakeCall->audioIP;
		callDelivered->userLinePort = userMakeCall->audioPort;
	}
	
	if (pAgentInfo->IsAllowedPush())
	{
		pAgentInfo->addListCallDelivered(callDelivered);
		Push_Message(pAgentInfo->m_sAgentID,E_NORMAL_CALL,callDelivered->GetCallID());
	}

	this->SendIPCCMsg_UDP(*callDelivered.get(), pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

	//log
	string sLog;
	ostringstream ostream;
	ostream.str("");
	ostream << "CallDistribute(userCallAgent):" << "userName=" << sUserName
		<< ";" << "agent=" << pAgentInfo->m_sAgentID
		<< ";" << "depart=" << pAgentInfo->m_sDepartment
		<< ";" << "uuidata=" << userMakeCall->GetUUIData()
		<< ";" << "callid=" << sCallID
		<< ";" << "partNo=" << userMakeCall->gwPartNo
		<< ";" << "audioIP=" << userMakeCall->audioIP
		<< ";" << "audioPort=" << userMakeCall->audioPort
		<< ";" << "audioRtpPort=" << userMakeCall->audioRtpPort
		<< ";" << "sipBoardIP=" << userMakeCall->sipBoardIP
		<< ";" << "sipBoardRtpPort=" << userMakeCall->sipBoardRtpPort
		<< ";" << "sipBoardRtcpPort=" << userMakeCall->sipBoardRtcpPort
		<< ";" << "natip=" << pAgentInfo->m_ulAgentIP
		<< ";" << "natport=" << pAgentInfo->m_usAgentPort
        << ";" << "mediaInfo=" << callDelivered->audioStreamDescription.to_str();
	sLog = ostream.str();
	g_Logger.MakeLog( sLog);

	//判坐席 loginType
	if(pAgentInfo->m_iLoginType == LoginType_OnlyWeb)
		return;

	SIPCCMsg_ACD_RingAgentExt RingAgentExt;
	RingAgentExt.SetAgentID(pAgentInfo->m_sAgentID);
	RingAgentExt.curVersion = "9.9.9";
	RingAgentExt.SetTargetPosition(userMakeCall->comSymbol);
	RingAgentExt.SetCorporationName(pAgentInfo->m_sCorporation);
	RingAgentExt.SetUUIData(pAgentInfo->m_sAgentPhoneNo);
	RingAgentExt.ipPhoneNo_User = pAgentInfo->m_userInfo.m_sUserName;
	RingAgentExt.managerId = pAgentInfo->m_sManagerid;
	CUtility::GetInstance()->SendIPCCMsg_UDP(RingAgentExt, m_IPCCConfig.UStateServer_IP, m_IPCCConfig.UStateServer_Port);
}

UINT CUtility::ThreadDelayHangup(void* lp)
{
	
	return 1;
}

void CUtility::OnGateway_QueryGWConf(SIPCCMsg_Gateway_QueryGWConf *ipccMessage, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");

	string sLog = "";
	os << "Gateway_QueryGWConf:" 
		<< "agentID=" << ipccMessage->agentID
		<< ";comSymbol=" << ipccMessage->comSymbol
		<< ";" << "gwName=" << ipccMessage->gwName
		<< ";" << "gwPartNo=" << ipccMessage->gwPartNo
		<< ";" << "newPhoneNo=" << ipccMessage->newPhoneNo
		<< ";" << "natip=" << ip
		<< ";" << "natport=" << port;
	sLog = os.str();

	g_Logger.MakeLog( sLog);

	CAgentInfo* pAgentInfo = NULL;
	pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->agentID);
	if(pAgentInfo == NULL)
		return;

	//发送 RingAgengExt 到 UStateServer
	SIPCCMsg_ACD_RingAgentExt RingAgentExt;
	RingAgentExt.SetAgentID(ipccMessage->agentID);
	RingAgentExt.curVersion = "9.9.9";
	RingAgentExt.SetTargetPosition(ipccMessage->comSymbol);
	RingAgentExt.SetCorporationName(pAgentInfo->m_sCorporation);
	RingAgentExt.SetUUIData(ipccMessage->newPhoneNo);
	RingAgentExt.ipPhoneNo_User = pAgentInfo->m_userInfo.m_sUserName;
	RingAgentExt.managerId = pAgentInfo->m_sManagerid;
	CUtility::GetInstance()->SendIPCCMsg_UDP(RingAgentExt, m_IPCCConfig.UStateServer_IP, m_IPCCConfig.UStateServer_Port);
}

void CUtility::OnGateway_RingAgentExtConf(SIPCCMsg_Gateway_RingAgentExtConf *ipccMessage, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");

	string sLog = "";
	os << "Gateway_RingAgentExtConf:" 
		<< "bCallFailed=" << (int)(ipccMessage->bCallFailed)
		<< ";invokeID=" << ipccMessage->invokeID
		<< ";ipNo_Agent=" << ipccMessage->ipNo_AgentExt
		<< ";" << "failedCause=" << ipccMessage->failedCause
		<< ";" << "agentID=" << ipccMessage->agentID
		<< ";" << "calledNo=" << ipccMessage->calledNo
		<< ";" << "calledAgentID=" << ipccMessage->CalledAgentID
		<< ";" << "calledAgentLineNo=" << ipccMessage->calledAgentLineNo
		<< ";" << "calledAgentLineIP=" << ipccMessage->calledAgentLineIp
		<< ";" << "calledAgentLinePort=" << ipccMessage->calledAgentLinePort
		<< ";" << "natip=" << ip
		<< ";" << "natport=" << port;
	sLog = os.str();

	g_Logger.MakeLog( sLog);

	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->agentID);
	if(pAgentInfo == NULL)
		return;

	pAgentInfo->m_WebPstnAgentCurGwLine = ipccMessage->calledAgentLineNo;
	pAgentInfo->m_CurGwLineIP = ipccMessage->calledAgentLineIp;
	pAgentInfo->m_CurGwLinePort = ipccMessage->calledAgentLinePort;
	pAgentInfo->m_gwCtrlIP = ip;
	pAgentInfo->m_gwCtrlPort = port;

	if(ipccMessage->bCallFailed != 0)
	{
        pAgentInfo->SetState(Agent_Waiting);
		return;
	}

	pAgentInfo->m_sIPNo_TheAgent = ipccMessage->ipNo_AgentExt;

	CUserInfo& user = pAgentInfo->m_userInfo;
	user.m_ulUserIP = ipccMessage->primarySourceIp;
	user.m_usUserPort = ipccMessage->primarySourcePort;

	//发送 RingAgengExt2 到 UStateServer
	SIPCCMsg_ACD_RingAgentExt2 RingAgentExt2;
	RingAgentExt2.SetAgentID(ipccMessage->agentID);
	RingAgentExt2.invokeID = ipccMessage->invokeID;
	RingAgentExt2.curVersion = "9.9.9";
	RingAgentExt2.SetTargetPosition(ipccMessage->ipNo_AgentExt);
	RingAgentExt2.SetCorporationName(pAgentInfo->m_sCorporation);
	RingAgentExt2.SetUUIData(ipccMessage->calledNo);
	RingAgentExt2.CalledAgentID = ipccMessage->CalledAgentID;
	RingAgentExt2.calledAgentLineNo = ipccMessage->calledAgentLineNo;
	RingAgentExt2.calledAgentLineIp = ipccMessage->calledAgentLineIp;
	RingAgentExt2.calledAgentLinePort = ipccMessage->calledAgentLinePort;
	RingAgentExt2.callTransID = pAgentInfo->m_sCallID;
	RingAgentExt2.managerId = pAgentInfo->m_sManagerid;

	if ( CUtility::m_IPCCConfig.iCallerNoForSend_PiccMode == 1 )
	{
		RingAgentExt2.callerNoForSend = user.m_sCallerNo;
	}
	else if ( CUtility::m_IPCCConfig.iCallerNoForSend_PiccMode == 2 )
	{
		RingAgentExt2.callerNoForSend = user.m_sCalledNo;
	}
 
	//通知坐席端
	CUtility::GetInstance()->SendIPCCMsg_UDP(RingAgentExt2, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);
	//通知  UState
	CUtility::GetInstance()->SendIPCCMsg_UDP(RingAgentExt2, m_IPCCConfig.UStateServer_IP, m_IPCCConfig.UStateServer_Port);

	if (pAgentInfo->m_iLoginType == LoginType_OnlyPSTN)
	{
		CDepartmentInfo* pDepartmentInfo = 
			CCorporationMgr::GetInstance()->GetDepartmentInfo(pAgentInfo->m_sCorporation, pAgentInfo->m_sDepartment);
		if(pDepartmentInfo == NULL)
		{
			return;
		}

		if ( pDepartmentInfo->m_iMaxRingTime == 0 )
		{
			return;
		}

		m_RingAgentExtMap[ipccMessage->agentID] = gettickcount();
	}
}

void CUtility::OnGateway_RingAgentExtFailed(SIPCCMsg_Gateway_RingAgentExtFailed *ipccMessage, unsigned long ip, unsigned short port)
{
    //
//////////	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->agentID);
//////////	if(pAgentInfo == NULL)
//////////		return;
//////////
//////////    bool bFailed = true;
//////////	string sNextAgentId = "";
//////////
//////////	CUserInfo& user = pAgentInfo->m_userInfo;
//////////	if (CallParty_VoiceGateWay == user.m_iUserType
//////////		&& user.m_CalledIsDept
//////////		&& pAgentInfo->m_iLoginType == LoginType_OnlyPSTN)
//////////	{
//////////		user.m_CallFailedAgents.insert(ipccMessage->agentID);
//////////
//////////		CDepartmentInfo* pDepartmentInfo = NULL;
//////////		CSkillGroupInfo* pSkillGroupInfo = NULL;
//////////
//////////		if ( user.m_bCallSkillGroup )
//////////		{
//////////			pSkillGroupInfo = CCorporationMgr::GetInstance()->GetSkillGroupInfo(user.m_sOriginCalledTarget);
//////////
//////////			if(pSkillGroupInfo == NULL)
//////////			{
//////////				goto conf;
//////////			}
//////////
//////////			int iAgentNumOfTheDepart = pSkillGroupInfo->GetAgentNum();
//////////
//////////			if(0 == iAgentNumOfTheDepart)	//无座席登录
//////////			{
//////////				goto conf;
//////////			}
//////////
//////////			sNextAgentId = pSkillGroupInfo->FrontOfWAQueue();//参数确定用户类型
//////////			if(sNextAgentId == "" || user.m_CallFailedAgents.find(sNextAgentId) != user.m_CallFailedAgents.end())
//////////			{
//////////				goto conf;
//////////			}
//////////
//////////			sNextAgentId = pDepartmentInfo->PopFromWAQueue();
//////////			CUserMgr::GetInstance()->AddUser(user);
//////////			pSkillGroupInfo->func_UserCallIn(user.m_sUserName, sNextAgentId, true);
//////////		}
//////////		else
//////////		{
//////////			pDepartmentInfo = CCorporationMgr::GetInstance()->GetDepartmentInfo(user.m_sOriginCalledTarget);
//////////
//////////			if(pDepartmentInfo == NULL)
//////////			{
//////////				goto conf;
//////////			}
//////////
//////////			int iAgentNumOfTheDepart = pDepartmentInfo->GetAgentNum();
//////////
//////////			if(0 == iAgentNumOfTheDepart)	//无座席登录
//////////			{
//////////				goto conf;
//////////			}
//////////
//////////			sNextAgentId = pDepartmentInfo->FrontOfWAQueue();//参数确定用户类型
//////////			if(sNextAgentId == "" || user.m_CallFailedAgents.find(sNextAgentId) != user.m_CallFailedAgents.end())
//////////			{
//////////				goto conf;
//////////			}
//////////
//////////            //todo ?什么逻辑
//////////			pDepartmentInfo->QueueToCQueue(true, user);
//////////			sNextAgentId = pDepartmentInfo->PopFromWAQueue();
//////////			pDepartmentInfo->func_AgentWaiting(sNextAgentId, true);
//////////		}
//////////		
//////////		bFailed = false;
//////////	}
//////////
//////////conf:
//////////	if (bFailed)
//////////	{
//////////		SIPCCMsg_ACD_UserMakeCallConf userMakeCallConf;
//////////		userMakeCallConf.SetUserName(pAgentInfo->m_userInfo.m_sUserName);
//////////		userMakeCallConf.callingState = UserCalling_Failed;
//////////		userMakeCallConf.handleId=ipccMessage->handleId;//by heyj 2011-08-15
//////////		this->SendIPCCMsg_UDP(userMakeCallConf, ip, port);
//////////	}
//////////
//////////	if (pAgentInfo->m_iLoginType == LoginType_OnlyPSTN)
//////////	{
//////////		m_RingAgentExtMap.erase(ipccMessage->agentID);
//////////
//////////		CDepartmentInfo* pDepartmentInfo = NULL;
//////////		CSkillGroupInfo* pSkillGroupInfo = NULL;
//////////
//////////		//VDN ===========
//////////		SIPCCMsg_ACD_CallLeaveVDN CallLeaveVDN;
//////////		if ( pAgentInfo->m_sCurSkillInfo.id == "" )
//////////		{
//////////			CallLeaveVDN.vdnNo = pAgentInfo->m_sDepartment;
//////////			CallLeaveVDN.deptid = pAgentInfo->m_sDeptid;
//////////
//////////			pDepartmentInfo = CCorporationMgr::GetInstance()->GetDepartmentInfo( pAgentInfo->m_sCorporation, pAgentInfo->m_sDepartment );
//////////			if ( pDepartmentInfo != NULL )
//////////				CallLeaveVDN.CurQueueNum = pDepartmentInfo->GetCallerNumInQueue();
//////////		}
//////////		else
//////////		{
//////////			CallLeaveVDN.SkillGroupID = pAgentInfo->m_sCurSkillInfo.id;
//////////			CallLeaveVDN.SkillGroupName = pAgentInfo->m_sCurSkillInfo.name;
//////////
//////////			pSkillGroupInfo = CCorporationMgr::GetInstance()->GetSkillGroupInfo( pAgentInfo->m_sCorporation, pAgentInfo->m_sCurSkillInfo.id );
//////////			if ( pSkillGroupInfo != NULL )
//////////				CallLeaveVDN.CurQueueNum = pSkillGroupInfo->GetCallerNumInQueue();
//////////		}
//////////		CallLeaveVDN.UserLevel = pAgentInfo->m_userInfo.m_iUserLevel;
//////////		CallLeaveVDN.corpName = pAgentInfo->m_sCorporation;
//////////		CallLeaveVDN.callID = pAgentInfo->m_sCallID;
//////////		CallLeaveVDN.leaveTime = CLog::GetLocalTime();
//////////		CallLeaveVDN.leaveCause = VDNEndCause_CallAbandon;
//////////		CallLeaveVDN.managerid = pAgentInfo->m_sManagerid;
//////////		CallLeaveVDN.LeaveQueueTime = pAgentInfo->m_sQueueArriveTime;
//////////		CallLeaveVDN.agentID = pAgentInfo->m_sAgentID;
//////////		CallLeaveVDN.bIntoQueue = pAgentInfo->m_bCallFromQueue;
//////////
//////////		pAgentInfo->m_bCallFromQueue = true;
//////////
//////////		ostringstream os;
//////////		string sTemp("");
//////////		os << "CUtility::OnGateway_RingAgentExtFailed--CallLeaveVDN";
//////////		if ( pSkillGroupInfo )
//////////		{
//////////			os << "; SkillGroupName=" << pAgentInfo->m_sCurSkillInfo.name
//////////				<< "; SkillGroupID=" << pAgentInfo->m_sCurSkillInfo.id;
//////////		}
//////////		else
//////////		{
//////////			os << "; VDNNo=" << pAgentInfo->m_sDepartment
//////////				<< "; deptid=" << pAgentInfo->m_sDeptid;
//////////		}
//////////		os	<< "; UserLevel=" << CallLeaveVDN.UserLevel
//////////			<< "; CallID=" << pAgentInfo->m_sCallID
//////////			<< "; CorpName=" << pAgentInfo->m_sCorporation
//////////			<< "; managerid=" << pAgentInfo->m_sManagerid
//////////			<< "; deptid=" << CallLeaveVDN.deptid
//////////			<< "; LeaveTime=" << CLog::GetLocalTime()
//////////			<< "; LeaveQueueTime=" << CallLeaveVDN.LeaveQueueTime
//////////			<< "; AgentID=" << CallLeaveVDN.agentID
//////////			<< "; LeaveCause=" << VDNEndCause_CallAbandon
//////////			<< "; CurQueueNum=" << CallLeaveVDN.CurQueueNum
//////////			<< "; bIntoQueue=" << ((bool)(CallLeaveVDN.bIntoQueue) ? "True" : "False");
//////////		sTemp = os.str();
//////////		g_Logger.MakeLog(sTemp);
//////////
//////////		//begin  add code by hyj 2013-8-27
//////////		string slogtrans=GetMsgLogTransContext(&CallLeaveVDN);
//////////		CallLeaveVDN.sData="";
//////////		string strlog=m_sLogTransSign1+slogtrans;
//////////		g_Logger.MakeLog(strlog);
//////////		//end --hyj
//////////
//////////		//DataCollect
//////////		CUtility::GetInstance()->SendIPCCMsg_UDP(
//////////			CallLeaveVDN, 
//////////			CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_IP, 
//////////			CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_Port);
//////////
//////////		CUtility::GetInstance()->SendToMonitorServer(CallLeaveVDN);
//////////		//VDN ===========
//////////
//////////        pAgentInfo->SetState(Agent_Waiting);
//////////    }
}

void CUtility::OnGateway_AgentAnswerCall(SIPCCMsg_Gateway_AgentAnswerCall *ipccMessage, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");

	string sLog = "";
	os << "Gateway_AgentAnswerCall:" 
		<< ";" << "agentID=" << ipccMessage->agentID
		<< ";" << "wpAgentID=" << ipccMessage->wpAgentID
		<< ";" << "ipNo_AgentExt=" << ipccMessage->ipNo_AgentExt
		<< ";" << "outDialType=" << ipccMessage->outDialType
		<< ";" << "calledOperType=" << ipccMessage->calledOperType
		<< ";" << "agentLineNo=" << ipccMessage->agentLineNo
		<< ";" << "agentLineIP=" << ipccMessage->agentLineIp
		<< ";" << "agentLinePort=" << ipccMessage->agentLinePort
		<< ";" << "userLineNo=" << ipccMessage->userLineNo
		<< ";" << "userLineIP=" << ipccMessage->userLineIp
		<< ";" << "userLinePort=" << ipccMessage->userLinePort
		<< ";" << "natip=" << ip
		<< ";" << "natport=" << port;
	sLog = os.str();

	g_Logger.MakeLog( sLog);

	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->agentID);
	if(pAgentInfo == NULL)
	{
		//SIPCCMsg_ACD_CallReleased ACD_CallReleased;
		return;
	}

	m_RingAgentExtMap.erase(ipccMessage->agentID);

    if (pAgentInfo->m_bCallFromVDN)
	{
		CDepartmentInfo* pDepartmentInfo = NULL;
		CSkillGroupInfo* pSkillGroupInfo = NULL;

		SIPCCMsg_ACD_CallLeaveVDN CallLeaveVDN;
		if ( pAgentInfo->m_sCurSkillInfo.id == "" )
		{
			CallLeaveVDN.vdnNo = pAgentInfo->m_sDepartment;
			CallLeaveVDN.deptid = pAgentInfo->m_sDeptid;

			pDepartmentInfo = CCorporationMgr::GetInstance()->GetDepartmentInfo( pAgentInfo->m_sCorporation, pAgentInfo->m_sDepartment );
			if ( pDepartmentInfo != NULL )
                CallLeaveVDN.CurQueueNum = pDepartmentInfo->GetCQueueSize();
		}
		else
		{
			CallLeaveVDN.SkillGroupID = pAgentInfo->m_sCurSkillInfo.id;
			CallLeaveVDN.SkillGroupName = pAgentInfo->m_sCurSkillInfo.name;

			pSkillGroupInfo = CCorporationMgr::GetInstance()->GetSkillGroupInfo( pAgentInfo->m_sCorporation, pAgentInfo->m_sCurSkillInfo.id );
			if ( pSkillGroupInfo != NULL )
                CallLeaveVDN.CurQueueNum = pSkillGroupInfo->GetCQueueSize();
		}
		CallLeaveVDN.UserLevel = pAgentInfo->m_userInfo.m_iUserLevel;
		CallLeaveVDN.corpName = pAgentInfo->m_sCorporation;
		CallLeaveVDN.callID = pAgentInfo->m_sCallID;
		CallLeaveVDN.leaveTime = CLog::GetLocalTime();
		CallLeaveVDN.leaveCause = VDNEndCause_Normal;
		CallLeaveVDN.managerid = pAgentInfo->m_sManagerid;
		CallLeaveVDN.LeaveQueueTime = pAgentInfo->m_sQueueArriveTime;
		CallLeaveVDN.agentID = pAgentInfo->m_sAgentID;
		CallLeaveVDN.bIntoQueue = pAgentInfo->m_bCallFromQueue;

		CUtility::GetInstance()->SendIPCCMsg_UDP(
			CallLeaveVDN, 
			CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_IP, 
			CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_Port);

		CUtility::GetInstance()->SendToMonitorServer(CallLeaveVDN);
	}

	if(ipccMessage->calledOperType == WpAgentLineOperType_AgentCallIn)
	{
		CAgentInfo* pwpAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->wpAgentID);
		if(pwpAgentInfo != NULL)
		{
			pwpAgentInfo->m_WebPstnAgentCurGwLine = ipccMessage->agentLineNo;
			pwpAgentInfo->m_CurGwLineIP = ipccMessage->agentLineIp;
			pwpAgentInfo->m_CurGwLinePort = ipccMessage->agentLinePort;
			pwpAgentInfo->m_gwCtrlIP = ip;
			pwpAgentInfo->m_gwCtrlPort = port;

			pwpAgentInfo->SetState(Agent_OpTalking);

			pwpAgentInfo->SetClientAgentState(Agent_OpTalking, WpAgentLineOperType_AgentCallIn, 
				ipccMessage->agentLineNo, "", "" , "", ipccMessage->agentLineIp, ipccMessage->agentLinePort);
		}
		SIPCCMsg_ACD_CallEstablished ACD_CallEstablished;
		ACD_CallEstablished.SetTheOtherParty(ipccMessage->ipNo_AgentExt);
		ACD_CallEstablished.ipPhoneNo_AgentExt = ipccMessage->ipNo_AgentExt;
		ACD_CallEstablished.outDialType = ipccMessage->outDialType;
		ACD_CallEstablished.ipPhoneNo_User = pAgentInfo->m_userInfo.m_sUserName;
		ACD_CallEstablished.handleId=ipccMessage->handleId;
		ACD_CallEstablished.managerId = pAgentInfo->m_sManagerid;
		CUtility::GetInstance()->SendIPCCMsg_UDP(ACD_CallEstablished, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);
		
		SIPCCMsg_ACD_CallAnswered callAnswered;
		callAnswered.SetUserName(ipccMessage->ipNo_AgentExt);
		callAnswered.handleId=ipccMessage->handleId;//by heyj 2011-08-15
		//add callID
		callAnswered.callId = pAgentInfo->m_sCallID;
		SendIPCCMsg_UDP(callAnswered, ip, port);

		return;
	}

	//设置 AgentTalking
	pAgentInfo->SetState(Agent_Talking);

	pAgentInfo->SetClientAgentState(Agent_Talking);
	
	SIPCCMsg_ACD_CallEstablished ACD_CallEstablished;
	ACD_CallEstablished.ipPhoneNo_AgentExt = ipccMessage->ipNo_AgentExt;
	ACD_CallEstablished.outDialType = ipccMessage->outDialType;
	ACD_CallEstablished.ipPhoneNo_User = pAgentInfo->m_userInfo.m_sUserName;
	ACD_CallEstablished.handleId=ipccMessage->handleId;//by heyj 2011-08-15
	CUtility::GetInstance()->SendIPCCMsg_UDP(ACD_CallEstablished, ip, port);	

	if (pAgentInfo->m_iLoginType == LoginType_OnlyPSTN)
	{
		if ( ipccMessage->outDialType == OutDialType_RingAgentExt )
		{
			SIPCCMsg_WebService_TalkingCall webService_TalkingCall;
			webService_TalkingCall.ani = pAgentInfo->m_userInfo.m_sCallerNo;
			webService_TalkingCall.dnis = pAgentInfo->m_sAgentPhoneNo;
			webService_TalkingCall.ucid = pAgentInfo->m_sCallID;
			webService_TalkingCall.agentId = pAgentInfo->m_sAgentID;
			webService_TalkingCall.station = pAgentInfo->m_sAgentPhoneNo;
			//CTime ct = CTime::GetCurrentTime();
			//webService_TalkingCall.operationTime = ct.Format("%Y-%m-%d %H:%M:%S");
			boost::posix_time::ptime pt = boost::posix_time::second_clock::local_time();
			string strTime = boost::gregorian::to_iso_extended_string(pt.date())+" "+boost::posix_time::to_simple_string(pt.time_of_day());
            webService_TalkingCall.operationTime = strTime;
			webService_TalkingCall.event = "Establish";

			SendIPCCMsg_UDP(webService_TalkingCall, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

			string sLog("");
			ostringstream os;
			os.str("");
			os <<  "WebService_TalkingCall: "
				<< "ani=" << pAgentInfo->m_userInfo.m_sCallerNo
				<< ";dnis=" << pAgentInfo->m_sAgentPhoneNo
				<< ";ucid=" << pAgentInfo->m_sCallID
				<< ";agentId=" << pAgentInfo->m_sAgentID
				<< ";station=" << pAgentInfo->m_sAgentPhoneNo
				<< ";ip=" << pAgentInfo->m_ulAgentIP
				<< ";port=" << pAgentInfo->m_usAgentPort;

			sLog = os.str();
			g_Logger.MakeLog(sLog);
		}
	}
	
}

void CUtility::OnGateway_AgentHangupCall(SIPCCMsg_Gateway_AgentHangupCall *ipccMessage, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");
	
	string sLog = "";
	os << "Gateway_AgentHangupCall:" 
		<< ";" << "agentID=" << ipccMessage->agentID
		<< ";" << "ipNo_AgentExt=" << ipccMessage->ipNo_AgentExt
		<< ";" << "outDialType=" << ipccMessage->outDialType
		<< ";" << "natip=" << ip
		<< ";" << "natport=" << port;
	sLog = os.str();
	
	g_Logger.MakeLog( sLog);
	
	//置 processing
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->agentID);
	if(pAgentInfo == NULL)
	{
		SIPCCMsg_ACD_CallReleased_WpAgent CallReleased_WpAgent;
		CallReleased_WpAgent.agentLineNo = ipccMessage->ipNo_AgentExt;
		CallReleased_WpAgent.handleId=ipccMessage->handleId; // by heyj 2011-08-15
		this->SendIPCCMsg_UDP(CallReleased_WpAgent, ip, port);
		return;
	}
	
	if(pAgentInfo->GetCurState() == Agent_Ringing)
	{
		//stopRing
		SIPCCMsg_ACD_StopRingAgentExt StopRingAgentExt;
		StopRingAgentExt.agentID = pAgentInfo->m_sAgentID;
		StopRingAgentExt.ipNo_AgentExt = ipccMessage->ipNo_AgentExt;
		StopRingAgentExt.handleId =ipccMessage->handleId;		//by heyj2011-08-15
		StopRingAgentExt.userName = pAgentInfo->m_userInfo.m_sUserName;
		CUtility::GetInstance()->SendIPCCMsg_UDP(StopRingAgentExt, ip, port);

		if ( !(pAgentInfo->m_bSendHangupCall) )
		{
			SIPCCMsg_WebService_HangupCall webService_HangupCall;
			webService_HangupCall.agentId = pAgentInfo->m_sAgentID;
			webService_HangupCall.station = pAgentInfo->m_sAgentPhoneNo;
			//CTime ct = CTime::GetCurrentTime();
			//webService_HangupCall.operationTime = ct.Format("%Y-%m-%d %H:%M:%S");

			boost::posix_time::ptime pt = boost::posix_time::second_clock::local_time();
			string strTime = boost::gregorian::to_iso_extended_string(pt.date())+" "+boost::posix_time::to_simple_string(pt.time_of_day());
			webService_HangupCall.operationTime = strTime;
			webService_HangupCall.event = "Hangup";
			webService_HangupCall.hangpuCallType = 1;
			webService_HangupCall.ucid = pAgentInfo->m_sCallID;
			this->SendIPCCMsg_UDP(webService_HangupCall, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

			string sLog("");
			ostringstream os;
			os.str("");
			os <<  "WebService_HangupCall: "
				<< "agentId=" << pAgentInfo->m_sAgentID
				<< ";station=" << pAgentInfo->m_sAgentPhoneNo
				<< ";ucid=" << pAgentInfo->m_sCallID
				<< ";hangpuCallType=1" 
				<< ";ip=" << pAgentInfo->m_ulAgentIP
				<< ";port=" << pAgentInfo->m_usAgentPort;

			sLog = os.str();
			g_Logger.MakeLog(sLog);

			pAgentInfo->m_bSendHangupCall = true;
		}
				
		return;
	}

	if(pAgentInfo->m_iLoginType == LoginType_WebPSTN)
	{
		if(pAgentInfo->m_iCurState == Agent_Dialing)
		{
			if(pAgentInfo->m_iCalledPartyType == CalledPartyType_Department)
			{
				//从队列中清除
				CDepartmentInfo* pDepartmentInfo = 
					CCorporationMgr::GetInstance()->GetDepartmentInfo(pAgentInfo->m_sCalledCorp, pAgentInfo->m_sCalledDepart );
				if(pDepartmentInfo != NULL)
                    pDepartmentInfo->RemoveFromCQueue(pAgentInfo->m_sAgentID);
				
				//通知被叫坐席小休
				CAgentInfo* pAgentInfo_Called = CAgentMgr::GetInstance()->GetAgentInfo(pAgentInfo->m_sTheOtherAgentID);
				if(pAgentInfo_Called != NULL)
				{
					CAgentMgr::GetInstance()->SetAgentState(pAgentInfo->m_sTheOtherAgentID, Agent_Parking);

					pAgentInfo_Called->SetClientAgentState(Agent_Parking);
				}	

				CAgentMgr::GetInstance()->SetAgentState(ipccMessage->agentID, Agent_BackProcess);
				
				pAgentInfo->SetClientAgentState(Agent_BackProcess);
			}
			else //wpAgent 呼叫 pstnUser, 硬电话挂机
			{
				SIPCCMsg_ACD_CallReleased_WpAgent CallReleased_WpAgent;
				CallReleased_WpAgent.agentLineNo = pAgentInfo->m_WebPstnAgentCurGwLine;
				CallReleased_WpAgent.userLineNo = pAgentInfo->m_CurGwUserLineNo;
				CallReleased_WpAgent.handleId=ipccMessage->handleId; // by heyj 2011-08-15
				this->SendIPCCMsg_UDP(CallReleased_WpAgent, ip, port);
				
				CAgentMgr::GetInstance()->SetAgentState(ipccMessage->agentID, Agent_BackProcess);
				
				pAgentInfo->SetClientAgentState(Agent_BackProcess);
			}
		}
		else if(pAgentInfo->m_iCurState == Agent_Holding)
		{
			SIPCCMsg_ACD_CallReleased_WpAgent CallReleased_WpAgent;
			CallReleased_WpAgent.agentLineNo = pAgentInfo->m_WebPstnAgentCurGwLine;
			CallReleased_WpAgent.userLineNo = pAgentInfo->m_CurGwUserLineNo;
			CallReleased_WpAgent.handleId=ipccMessage->handleId; // by heyj 2011-08-15
			this->SendIPCCMsg_UDP(CallReleased_WpAgent, ip, port);

			CAgentMgr::GetInstance()->SetAgentState(ipccMessage->agentID, Agent_BackProcess);
			
			pAgentInfo->SetClientAgentState(Agent_BackProcess);
		}
		else if(pAgentInfo->m_iCurState == Agent_DialUnderHold || pAgentInfo->m_iCurState == Agent_TalkUnderHold)
		{
			SIPCCMsg_ACD_CallReleased_WpAgent CallReleased_WpAgent;
			CallReleased_WpAgent.agentLineNo = pAgentInfo->m_WebPstnAgentCurGwLine;
			CallReleased_WpAgent.userLineNo = pAgentInfo->m_CurGwUserLineNo;
			CallReleased_WpAgent.handleId=ipccMessage->handleId; // by heyj 2011-08-15
			this->SendIPCCMsg_UDP(CallReleased_WpAgent, ip, port);
			
			if(pAgentInfo->m_sCalledParty == "")
			{
				if(pAgentInfo->m_iCalledPartyType == CalledPartyType_Department)
				{
/*
					//发送出 VDN 结果, 呼损
					SIPCCMsg_ACD_VDNStatusInfo vdnStatusInfo;
					vdnStatusInfo.callType = IPCCCallType_Agent;
					vdnStatusInfo.queueTimeSpan = CalTimeSpan(pAgentInfo->m_iAgentEnqueueTime, ::gettickcount());
					//responseTimeSpan 此时表示放弃时长
					vdnStatusInfo.responseTimeSpan = vdnStatusInfo.queueTimeSpan;
					vdnStatusInfo.SetCorporationName(pAgentInfo->m_sCalledCorp);
					vdnStatusInfo.SetTheDepartment(pAgentInfo->m_sCalledDepart);
					vdnStatusInfo.SetCallerNo(pAgentInfo->m_sAgentID);
					vdnStatusInfo.SetCalledNo(pAgentInfo->m_sCalledDepart);
					vdnStatusInfo.SetEndTime(CLog::GetLocalTime());
					vdnStatusInfo.endCause = VDNEndCause_CallAbandon;
					
					//DataCollect
					CUtility::GetInstance()->SendIPCCMsg_UDP(
						vdnStatusInfo, 
						CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_IP, 
						ConvertPort(ServerInfoDef::Port_DataCollectServer));
					//Monitor
					CUtility::GetInstance()->SendIPCCMsg_UDP(
						vdnStatusInfo, 
						CUtility::GetInstance()->m_IPCCConfig.MonitorServer_IP, 
						CUtility::GetInstance()->m_IPCCConfig.MonitorServer_Port);
*/
				}
				
				//从队列中清除
				CDepartmentInfo* pDepartmentInfo = 
					CCorporationMgr::GetInstance()->GetDepartmentInfo(pAgentInfo->m_sCalledCorp, pAgentInfo->m_sCalledDepart );
				if(pDepartmentInfo != NULL)
                    pDepartmentInfo->RemoveFromCQueue(pAgentInfo->m_sAgentID);
			}
			else
			{
				CAgentInfo* pCalledAgent = CAgentMgr::GetInstance()->GetAgentInfo(pAgentInfo->m_sCalledParty);
				if(pCalledAgent != NULL)
				{
					SIPCCMsg_ACD_CallReleased ACD_CallReleased;
					ACD_CallReleased.SetReleasedParty(pAgentInfo->m_sAgentID);
					CUtility::GetInstance()->SendIPCCMsg_UDP(ACD_CallReleased, pCalledAgent->m_ulAgentIP, pCalledAgent->m_usAgentPort);
				}
			}

			CAgentMgr::GetInstance()->SetAgentState(ipccMessage->agentID, Agent_BackProcess);
			
			pAgentInfo->SetClientAgentState(Agent_BackProcess);
		}
		else
		{
			SIPCCMsg_ACD_CallReleased CallReleased;
			CallReleased.SetReleasedParty(pAgentInfo->m_sAgentID);
			CUtility::GetInstance()->SendIPCCMsg_UDP(CallReleased, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

			if(pAgentInfo->m_iCurState == Agent_Talking)
			{
				CAgentInfo* pOtherAgent = CAgentMgr::GetInstance()->GetAgentInfo(pAgentInfo->m_sCalledParty);
				if(pOtherAgent != NULL && pOtherAgent->m_iCurState == Agent_OpTalking)
				{
					SIPCCMsg_ACD_CallReleased CallReleased;
					CallReleased.SetReleasedParty(pAgentInfo->m_sAgentID);
					CUtility::GetInstance()->SendIPCCMsg_UDP(CallReleased, pOtherAgent->m_ulAgentIP, pOtherAgent->m_usAgentPort);
				}
			}
			else if(pAgentInfo->m_iCurState == Agent_OpTalking)
			{
				SIPCCMsg_ACD_CallReleased_WpAgent CallReleased_WpAgent;
				CallReleased_WpAgent.agentLineNo = pAgentInfo->m_WebPstnAgentCurGwLine;
				CallReleased_WpAgent.handleId=ipccMessage->handleId; // by heyj 2011-08-15
				CUtility::GetInstance()->SendIPCCMsg_UDP(CallReleased_WpAgent, ip, port);

				CAgentInfo* pCalledAgent = CAgentMgr::GetInstance()->GetAgentInfo(pAgentInfo->m_sTheOtherAgentID);
				if(pCalledAgent != NULL)
				{
					SIPCCMsg_ACD_CallReleased ACD_CallReleased;
					ACD_CallReleased.SetReleasedParty(pAgentInfo->m_sAgentID);
					CUtility::GetInstance()->SendIPCCMsg_UDP(ACD_CallReleased, pCalledAgent->m_ulAgentIP, pCalledAgent->m_usAgentPort);
				}
			}
		}
	}
	else
	{
		if ( pAgentInfo->m_iCurState == Agent_Dialing )
		{
			SIPCCMsg_ACD_CallReleased_WpAgent CallReleased_WpAgent;
			CallReleased_WpAgent.agentLineNo = pAgentInfo->m_WebPstnAgentCurGwLine;
			CallReleased_WpAgent.userLineNo = pAgentInfo->m_CurGwUserLineNo;
			CallReleased_WpAgent.handleId=ipccMessage->handleId;
			this->SendIPCCMsg_UDP(CallReleased_WpAgent, ip, port);
		}		

		if ( !(pAgentInfo->m_bSendHangupCall) )
		{
			SIPCCMsg_WebService_HangupCall webService_HangupCall;
			webService_HangupCall.agentId = pAgentInfo->m_sAgentID;
			webService_HangupCall.station = pAgentInfo->m_sAgentPhoneNo;
			//CTime ct = CTime::GetCurrentTime();
			//webService_HangupCall.operationTime = ct.Format("%Y-%m-%d %H:%M:%S");
			boost::posix_time::ptime pt = boost::posix_time::second_clock::local_time();
			string strTime = boost::gregorian::to_iso_extended_string(pt.date())+" "+boost::posix_time::to_simple_string(pt.time_of_day());
			webService_HangupCall.operationTime =strTime;
			webService_HangupCall.event = "Hangup";
			webService_HangupCall.hangpuCallType = 1;
			webService_HangupCall.ucid = pAgentInfo->m_sCallID;
			this->SendIPCCMsg_UDP(webService_HangupCall, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

			string sLog("");
			ostringstream os;
			os.str("");
			os <<  "WebService_HangupCall: "
				<< "agentId=" << pAgentInfo->m_sAgentID
				<< ";station=" << pAgentInfo->m_sAgentPhoneNo
				<< ";ucid=" << pAgentInfo->m_sCallID
				<< ";hangpuCallType=1" 
				<< ";ip=" << pAgentInfo->m_ulAgentIP
				<< ";port=" << pAgentInfo->m_usAgentPort;
			sLog = os.str();
			g_Logger.MakeLog(sLog);

			pAgentInfo->m_bSendHangupCall = true;
		}
		
		if ( pAgentInfo->m_iCurState != Agent_Waiting )
		{
            pAgentInfo->SetState(Agent_Waiting);
        }
				
	}
}

void CUtility::OnGateway_StopRingAgentExtConf(SIPCCMsg_Gateway_StopRingAgentExtConf *ipccMessage, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");

	string sLog = "";
	os << "Gateway_StopRingAgentExtConf:" 
		<< ";" << "agentID=" << ipccMessage->agentID
		<< ";" << "ipNo_AgentExt=" << ipccMessage->ipNo_AgentExt
		<< ";" << "natip=" << ip
		<< ";" << "natport=" << port;
	sLog = os.str();

	g_Logger.MakeLog( sLog);

	//置 processing
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->agentID);
	if(pAgentInfo == NULL)
		return;

	if(pAgentInfo->m_iLoginType == LoginType_WebPSTN)
	{
		if(pAgentInfo->m_iCurState != Agent_Ringing)
			return;

		CAgentMgr::GetInstance()->SetAgentState(ipccMessage->agentID, Agent_Parking);

		pAgentInfo->SetClientAgentState(Agent_Parking);
	}
	else if (pAgentInfo->m_iLoginType == LoginType_OnlyPSTN)
	{
		if ( !(pAgentInfo->m_bSendHangupCall) )
		{
			SIPCCMsg_WebService_HangupCall webService_HangupCall;
			webService_HangupCall.agentId = pAgentInfo->m_sAgentID;
			webService_HangupCall.station = pAgentInfo->m_sAgentPhoneNo;
			//CTime ct = CTime::GetCurrentTime();
			//webService_HangupCall.operationTime = ct.Format("%Y-%m-%d %H:%M:%S");
			boost::posix_time::ptime pt = boost::posix_time::second_clock::local_time();
			string strTime = boost::gregorian::to_iso_extended_string(pt.date())+" "+boost::posix_time::to_simple_string(pt.time_of_day());
			webService_HangupCall.operationTime =strTime;
			webService_HangupCall.event = "Hangup";
			webService_HangupCall.hangpuCallType = 2;
			webService_HangupCall.ucid = pAgentInfo->m_sCallID;
			this->SendIPCCMsg_UDP(webService_HangupCall, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

			string sLog("");
			ostringstream os;
			os.str("");
			os <<  "WebService_HangupCall: "
				<< "agentId=" << pAgentInfo->m_sAgentID
				<< ";station=" << pAgentInfo->m_sAgentPhoneNo
				<< ";ucid=" << pAgentInfo->m_sCallID
				<< ";hangpuCallType=1" 
				<< ";ip=" << pAgentInfo->m_ulAgentIP
				<< ";port=" << pAgentInfo->m_usAgentPort;

			sLog = os.str();
			g_Logger.MakeLog(sLog);

			pAgentInfo->m_bSendHangupCall = true;
		}

        pAgentInfo->SetState(Agent_Waiting);
    }
	else
	{
        pAgentInfo->SetState(Agent_Waiting);
    }
}

void CUtility::OnGateway_CancelRingAgentExt(SIPCCMsg_Gateway_CancelRingAgentExt *ipccMessage, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");

	string sLog = "";
	os << "Gateway_CancelRingAgentExt:" 
		<< ";" << "agentID=" << ipccMessage->agentID
		<< ";" << "natip=" << ip
		<< ";" << "natport=" << port;
	sLog = os.str();

	g_Logger.MakeLog( sLog);

	//置 processing
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->agentID);
	if(pAgentInfo == NULL)
		return;

	if(pAgentInfo->m_iLoginType == LoginType_WebPSTN)
	{
		CAgentMgr::GetInstance()->SetAgentState(ipccMessage->agentID, Agent_BackProcess);

		pAgentInfo->SetClientAgentState(Agent_BackProcess);
	}
	else if (pAgentInfo->m_iLoginType == LoginType_OnlyPSTN)
	{
		if ( !(pAgentInfo->m_bSendHangupCall) )
		{
			SIPCCMsg_WebService_HangupCall webService_HangupCall;
			webService_HangupCall.agentId = pAgentInfo->m_sAgentID;
			webService_HangupCall.station = pAgentInfo->m_sAgentPhoneNo;
		//	CTime ct = CTime::GetCurrentTime();
			//webService_HangupCall.operationTime = ct.Format("%Y-%m-%d %H:%M:%S");
			
			boost::posix_time::ptime pt = boost::posix_time::second_clock::local_time();
			string strTime = boost::gregorian::to_iso_extended_string(pt.date())+" "+boost::posix_time::to_simple_string(pt.time_of_day());
			webService_HangupCall.operationTime=strTime;
			webService_HangupCall.event = "Hangup";
			webService_HangupCall.hangpuCallType = 1;
			webService_HangupCall.ucid = pAgentInfo->m_sCallID;
			this->SendIPCCMsg_UDP(webService_HangupCall, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

			string sLog("");
			ostringstream os;
			os.str("");
			os <<  "WebService_HangupCall: "
				<< "agentId=" << pAgentInfo->m_sAgentID
				<< ";station=" << pAgentInfo->m_sAgentPhoneNo
				<< ";ucid=" << pAgentInfo->m_sCallID
				<< ";hangpuCallType=1" 
				<< ";ip=" << pAgentInfo->m_ulAgentIP
				<< ";port=" << pAgentInfo->m_usAgentPort;

			sLog = os.str();
			g_Logger.MakeLog(sLog);

			pAgentInfo->m_bSendHangupCall = true;
		}

        pAgentInfo->SetState(Agent_Waiting);
	}
	else
	{
        pAgentInfo->SetState(Agent_Waiting);
    }
}

void CUtility::OnGateway_IsAgentLogin(SIPCCMsg_Gateway_IsAgentLogin *ipccMessage, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");

	string sLog = "";
	os << "Gateway_IsAgentLogin:" 
		<< ";" << "agentID=" << ipccMessage->agentID
		<< ";" << "natip=" << ip
		<< ";" << "natport=" << port;
	sLog = os.str();

	g_Logger.MakeLog( sLog);
	
	SIPCCMsg_ACD_IsAgentLoginConf IsAgentLoginConf;
	IsAgentLoginConf.agentID = ipccMessage->agentID;
	IsAgentLoginConf.handleId=ipccMessage->handleId;//by heyj 2011-08-15
	bool bIsAgentLogin = CAgentMgr::GetInstance()->IsAgentExist(ipccMessage->agentID);

	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->agentID);

	if(bIsAgentLogin)
		IsAgentLoginConf.bIsAgentLogin = 1;
	else
		IsAgentLoginConf.bIsAgentLogin = 0;
	IsAgentLoginConf.invokeId = ipccMessage->invokeId;
	IsAgentLoginConf.invokeType = ipccMessage->invokeType;

	if(pAgentInfo != NULL)
	{
		IsAgentLoginConf.agentState = pAgentInfo->m_iCurState;
		IsAgentLoginConf.loginType = pAgentInfo->m_iLoginType;
	}
	else
		IsAgentLoginConf.bIsAgentLogin = 0;

	CUtility::GetInstance()->SendIPCCMsg_UDP(IsAgentLoginConf, ip, port);	
}

void CUtility::OnGateway_CallAgentExtFailed(SIPCCMsg_Gateway_CallAgentExtFailed *ipccMessage, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");

	string sLog = "";
	os << "Gateway_CallAgentExtFailed:" 
		<< ";" << "agentID=" << ipccMessage->agentID
		<< ";" << "natip=" << ip
		<< ";" << "natport=" << port;
	sLog = os.str();

	g_Logger.MakeLog( sLog);
	//置 processing
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->agentID);
	if(pAgentInfo == NULL)
		return;

	if(pAgentInfo->m_iLoginType == LoginType_WebPSTN)
	{
		CAgentMgr::GetInstance()->SetAgentState(ipccMessage->agentID, Agent_BackProcess);

		pAgentInfo->SetClientAgentState(Agent_BackProcess);
		//pAgentInfo->SetClientAgentState(Agent_Waiting);
	}
	else if (pAgentInfo->m_iLoginType == LoginType_OnlyPSTN)
	{
		if ( !(pAgentInfo->m_bSendHangupCall) )
		{
			SIPCCMsg_WebService_HangupCall webService_HangupCall;
			webService_HangupCall.agentId = pAgentInfo->m_sAgentID;
			webService_HangupCall.station = pAgentInfo->m_sAgentPhoneNo;
			//CTime ct = CTime::GetCurrentTime();
			//webService_HangupCall.operationTime = ct.Format("%Y-%m-%d %H:%M:%S");
			boost::posix_time::ptime pt = boost::posix_time::second_clock::local_time();
			string strTime = boost::gregorian::to_iso_extended_string(pt.date())+" "+boost::posix_time::to_simple_string(pt.time_of_day());
			webService_HangupCall.operationTime = strTime;
			webService_HangupCall.event = "Hangup";
			webService_HangupCall.hangpuCallType = 1;
			webService_HangupCall.ucid = pAgentInfo->m_sCallID;
			this->SendIPCCMsg_UDP(webService_HangupCall, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

			string sLog("");
			ostringstream os;
			os.str("");
			os <<  "WebService_HangupCall: "
				<< "agentId=" << pAgentInfo->m_sAgentID
				<< ";station=" << pAgentInfo->m_sAgentPhoneNo
				<< ";ucid=" << pAgentInfo->m_sCallID
				<< ";hangpuCallType=1" 
				<< ";ip=" << pAgentInfo->m_ulAgentIP
				<< ";port=" << pAgentInfo->m_usAgentPort;

			sLog = os.str();
			g_Logger.MakeLog(sLog);

			pAgentInfo->m_bSendHangupCall = true;
		}

        pAgentInfo->SetState(Agent_Waiting);
	}
}

void CUtility::OnGateway_CallAgentFailed(SIPCCMsg_Gateway_CallAgentFailed *ipccMessage, unsigned long ip, unsigned short port)
{
	//坐席呼叫邦电话坐席，绑电话坐席状态空闲，但是绑电话坐席硬电话呼叫失败
	ostringstream os;
	os.str("");

	os << "Gateway_CallAgentFailed:" 
		<< ";" << "bCallFailed=" << (int)ipccMessage->bCallFailed
		<< ";" << "calledAgentID=" << ipccMessage->CalledAgentID
		<< ";" << "calledAgentLineNo=" << ipccMessage->calledAgentLineNo
		<< ";" << "calledAgentLineIP=" << ipccMessage->calledAgentLineIp
		<< ";" << "calledAgentLinePort=" << ipccMessage->calledAgentLinePort
		<< ";" << "calledNo=" << ipccMessage->calledNo
		<< ";" << "failedCause=" << ipccMessage->failedCause
		<< ";" << "originAgentID=" << ipccMessage->originAgentID
		<< ";" << "originAgentLineNo=" << ipccMessage->originAgentLineNo
		<< ";" << "originAgentIp=" << ipccMessage->originAgentIp
		<< ";" << "originAgentPort=" << ipccMessage->originAgentPort
		<< ";" << "natip=" << ip
		<< ";" << "natport=" << port;
	string sLog = os.str();

	g_Logger.MakeLog( sLog);
	
	//通知主叫坐席 呼叫坐席失败
	CAgentInfo* pCalledWpAgent = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->CalledAgentID);
	if(pCalledWpAgent == NULL)
		return;

	CAgentInfo* pCallingAgent = CAgentMgr::GetInstance()->GetAgentInfo(pCalledWpAgent->m_sTheOtherAgentID);
	if(pCallingAgent == NULL)
		return;

	SIPCCMsg_ACD_CallWpAgentExtFailed ACD_CallWpAgentExtFailed;
	ACD_CallWpAgentExtFailed.wpAgentID = ipccMessage->CalledAgentID;
	this->SendIPCCMsg_UDP(ACD_CallWpAgentExtFailed, pCallingAgent->m_ulAgentIP, pCallingAgent->m_usAgentPort);

	//释放 wpAgentExt
	SIPCCMsg_ACD_CallReleased_WpAgent ACD_CallReleased_WpAgent;
	ACD_CallReleased_WpAgent.agentLineNo = ipccMessage->calledAgentLineNo;
	ACD_CallReleased_WpAgent.handleId=ipccMessage->handleId; // by heyj 2011-08-15
	this->SendIPCCMsg_UDP(ACD_CallReleased_WpAgent, ip, port);

	//置被叫坐席小休
	CAgentMgr::GetInstance()->SetAgentState(ipccMessage->CalledAgentID, Agent_Parking);

	pCalledWpAgent->SetClientAgentState(Agent_Parking);

}

void CUtility::OnGateway_AgentExtCallSucc(SIPCCMsg_Gateway_AgentExtCallSucc *ipccMessage, unsigned long ip, unsigned short port)
{
	//webPSTN 电话外呼 pstn 用户成功
	ostringstream os;
	os.str("");

	string sLog = "";
	os << "Gateway_AgentExtCallSucc:" 
		<< ";" << "agentID=" << ipccMessage->agentID
		<< ";" << "gwAgentLine=" << ipccMessage->gwAgentLine
		<< ";" << "gwUserLine=" << ipccMessage->gwUserLine
		<< ";" << "gwAgentLineIP=" << ipccMessage->gwAgentLineIp
		<< ";" << "gwAgentLinePort=" << ipccMessage->gwAgentLinePort
		<< ";" << "gwUserLineIP=" << ipccMessage->gwUserLineIp
		<< ";" << "gwUserLinePort=" << ipccMessage->gwUserLinePort
		<< ";" << "natport=" << port;
	sLog = os.str();

	g_Logger.MakeLog( sLog);

	//置 waiting
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->agentID);
	if(pAgentInfo == NULL)
		return;

	pAgentInfo->m_CurGwUserLineNo = ipccMessage->gwUserLine;
	pAgentInfo->m_WebPstnAgentCurGwLine = ipccMessage->gwAgentLine;
	pAgentInfo->m_CurGwLineIP = ipccMessage->gwAgentLineIp;
	pAgentInfo->m_CurGwLinePort = ipccMessage->gwAgentLinePort;
	pAgentInfo->m_CurUserLineIP = ipccMessage->gwUserLineIp;
	pAgentInfo->m_CurUserLinePort = ipccMessage->gwUserLinePort;
	pAgentInfo->m_gwCtrlIP = ip;
	pAgentInfo->m_gwCtrlPort = port;

	CAgentMgr::GetInstance()->SetAgentState(ipccMessage->agentID, Agent_Talking);

	pAgentInfo->SetClientAgentState(Agent_Talking, 0, ipccMessage->gwAgentLine, ipccMessage->gwUserLine, ipccMessage->gwAgentLine, ""
		,ipccMessage->gwAgentLineIp, ipccMessage->gwAgentLinePort, ipccMessage->gwUserLineIp, ipccMessage->gwUserLinePort);

	if(pAgentInfo->m_iLoginType == LoginType_OnlyPSTN)
	{
		CSpliter sp(pAgentInfo->m_sPrivateData, ";");

		if ( sp[0] != "WebPSTNDial" )
		{
			return;
		}

		string sCallerNo = sp[3];
		if ( sCallerNo.length() > 1 )
		{
			sCallerNo = sCallerNo.substr(1, sCallerNo.length());
		}
		string sCalledNo = sp[4];
		if ( sCalledNo.length() > 1 )
		{
			sCalledNo = sCalledNo.substr(1, sCalledNo.length());
		}
		string sInvokeid = sp[7];

		SIPCCMsg_WebService_TalkingCall webService_TalkingCall;
		webService_TalkingCall.ani = sCallerNo;
		webService_TalkingCall.dnis = sCalledNo;
		webService_TalkingCall.ucid = sInvokeid;
		webService_TalkingCall.agentId = pAgentInfo->m_sAgentID;
		webService_TalkingCall.station = pAgentInfo->m_sAgentPhoneNo;
		//CTime ct = CTime::GetCurrentTime();
		//webService_TalkingCall.operationTime = ct.Format("%Y-%m-%d %H:%M:%S");
		boost::posix_time::ptime pt = boost::posix_time::second_clock::local_time();
		string strTime = boost::gregorian::to_iso_extended_string(pt.date())+" "+boost::posix_time::to_simple_string(pt.time_of_day());
		webService_TalkingCall.operationTime = strTime;
		webService_TalkingCall.event = "Establish";

		SendIPCCMsg_UDP(webService_TalkingCall, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

		string sLog("");
		ostringstream os;
		os.str("");
		os <<  "WebService_TalkingCall: "
			<< "ani=" << sCallerNo
			<< ";dnis=" << sCalledNo
			<< ";ucid=" << sInvokeid
			<< ";agentId=" << pAgentInfo->m_sAgentID
			<< ";station=" << pAgentInfo->m_sAgentPhoneNo
			<< ";ip=" << pAgentInfo->m_ulAgentIP
			<< ";port=" << pAgentInfo->m_usAgentPort;

		sLog = os.str();
		g_Logger.MakeLog(sLog);
	}
}

void CUtility::OnGateway_UserHangupCall(SIPCCMsg_Gateway_UserHangupCall *ipccMessage, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");

	string sLog = "";
	os << "Gateway_UserHangupCall:" 
		<< ";" << "agentID=" << ipccMessage->agentID
		<< ";" << "userLineNo=" << ipccMessage->userLineNo
		<< ";" << "outDialType=" << ipccMessage->outDialType
		<< ";" << "natip=" << ip
		<< ";" << "natport=" << port;
	sLog = os.str();

	g_Logger.MakeLog( sLog);

	//置 processing
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->agentID);
	if(pAgentInfo == NULL)
	{
		SIPCCMsg_ACD_CallReleased_WpAgent CallReleased_WpAgent;
		CallReleased_WpAgent.userLineNo = ipccMessage->userLineNo;
		CallReleased_WpAgent.handleId=ipccMessage->handleId; // by heyj 2011-08-15
		this->SendIPCCMsg_UDP(CallReleased_WpAgent, ip, port);
		return;
	}

	//pstn呼入接通前用户挂机发送的是 client_userhangupcall
	//所以 gateway_userhangupcall 不用处理 agent_Ringing 的情况

	//发送 CallReleased 消息给座席
	SIPCCMsg_ACD_CallReleased callReleased;
	callReleased.SetReleasedParty(ipccMessage->userLineNo);
	this->SendIPCCMsg_UDP(callReleased, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

	///对纯手机坐席的处理 -begin
	if(pAgentInfo->m_iLoginType == LoginType_OnlyPSTN)
	{
		if ( !(pAgentInfo->m_bSendHangupCall) )
		{
			SIPCCMsg_WebService_HangupCall webService_HangupCall;
			webService_HangupCall.agentId = pAgentInfo->m_sAgentID;
			webService_HangupCall.station = pAgentInfo->m_sAgentPhoneNo;
			boost::posix_time::ptime pt = boost::posix_time::second_clock::local_time();
			string strTime = boost::gregorian::to_iso_extended_string(pt.date())+" "+boost::posix_time::to_simple_string(pt.time_of_day());
			webService_HangupCall.operationTime = strTime;
			webService_HangupCall.event = "Hangup";
			webService_HangupCall.hangpuCallType = 2;
			webService_HangupCall.ucid = pAgentInfo->m_sCallID;
			this->SendIPCCMsg_UDP(webService_HangupCall, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

			string sLog("");
			ostringstream os;
			os.str("");
			os <<  "WebService_HangupCall: "
				<< "agentId=" << pAgentInfo->m_sAgentID
				<< ";station=" << pAgentInfo->m_sAgentPhoneNo
				<< ";ucid=" << pAgentInfo->m_sCallID
				<< ";hangpuCallType=2" 
				<< ";ip=" << pAgentInfo->m_ulAgentIP
				<< ";port=" << pAgentInfo->m_usAgentPort;

			sLog = os.str();
			g_Logger.MakeLog(sLog);

			pAgentInfo->m_bSendHangupCall = true;
		}
		
		m_RingAgentExtMap.erase(ipccMessage->agentID); //todo ?这是什么逻辑
        pAgentInfo->SetState(Agent_Waiting);

		return;
	}
	///对纯手机坐席的处理 -end

	if(pAgentInfo->m_iCurState == Agent_Talking)
	{
		//挂掉双方， 并给邦电话坐席发送 CallReleased
		SIPCCMsg_ACD_CallReleased_WpAgent ACD_CallReleased_WpAgent;
		ACD_CallReleased_WpAgent.agentLineNo = pAgentInfo->m_WebPstnAgentCurGwLine;
		ACD_CallReleased_WpAgent.userLineNo = ipccMessage->userLineNo;
		ACD_CallReleased_WpAgent.handleId=ipccMessage->handleId; // by heyj 2011-08-15
		this->SendIPCCMsg_UDP(ACD_CallReleased_WpAgent, ip, port);

		CAgentMgr::GetInstance()->SetAgentState(ipccMessage->agentID, Agent_BackProcess);

		pAgentInfo->SetClientAgentState(Agent_BackProcess);
	}
	else if(pAgentInfo->m_iCurState == Agent_Holding)
	{
		SIPCCMsg_ACD_CallReleased_WpAgent CallReleased_WpAgent;
		CallReleased_WpAgent.agentLineNo = pAgentInfo->m_WebPstnAgentCurGwLine;
		CallReleased_WpAgent.userLineNo = ipccMessage->userLineNo;
		CallReleased_WpAgent.handleId=ipccMessage->handleId; // by heyj 2011-08-15
		this->SendIPCCMsg_UDP(CallReleased_WpAgent, ip, port);
	}
	else if(pAgentInfo->m_iCurState == Agent_DialUnderHold || pAgentInfo->m_iCurState == Agent_TalkUnderHold)
	{
		SIPCCMsg_ACD_CallReleased_WpAgent CallReleased_WpAgent;
		CallReleased_WpAgent.userLineNo = ipccMessage->userLineNo;
		CallReleased_WpAgent.handleId=ipccMessage->handleId; // by heyj 2011-08-15
		this->SendIPCCMsg_UDP(CallReleased_WpAgent, ip, port);
	}
}

void CUtility::OnGateway_CallAgentConf(SIPCCMsg_Gateway_CallAgentConf *ipccMessage, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");

	string sLog = "";
	os << "Gateway_CallAgentConf:" 
		<< ";" << "bCallFailed=" << int(ipccMessage->bCallFailed)
		<< ";" << "calledNo=" << ipccMessage->calledNo
		<< ";" << "originAgentID=" << ipccMessage->originAgentID
		<< ";" << "originAgentLineNo=" << ipccMessage->originAgentLineNo
		<< ";" << "originAgentIp=" << ipccMessage->originAgentIp
		<< ";" << "originAgentPort=" << ipccMessage->originAgentPort
		<< ";" << "CalledAgentID=" << ipccMessage->CalledAgentID
		<< ";" << "calledAgentLineNo=" << ipccMessage->calledAgentLineNo
		<< ";" << "calledAgentLineIp=" << ipccMessage->calledAgentLineIp
		<< ";" << "calledAgentLinePort=" << ipccMessage->calledAgentLinePort
		<< ";" << "natip=" << ip
		<< ";" << "natport=" << port;
	sLog = os.str();

	g_Logger.MakeLog( sLog);

	CAgentInfo* pCallerAgent = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->originAgentID);
	if(pCallerAgent == NULL)
		return;
	
	CAgentInfo* pCalledAgent = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->CalledAgentID);
	if(pCalledAgent == NULL)
		return;
	
	pCalledAgent->m_WebPstnAgentCurGwLine = ipccMessage->calledAgentLineNo;
	pCalledAgent->m_CurGwLineIP = ipccMessage->calledAgentLineIp;
	pCalledAgent->m_CurGwLinePort = ipccMessage->calledAgentLinePort;
	pCalledAgent->m_gwCtrlIP = ip;
	pCalledAgent->m_gwCtrlPort = port;

	//返回 ACD_CallAgentConf
	SIPCCMsg_ACD_CallWpAgentConf ACD_CallWpAgentConf;
	ACD_CallWpAgentConf.bCallFailed = ipccMessage->bCallFailed;
	ACD_CallWpAgentConf.invokeID = ipccMessage->invokeID;
	ACD_CallWpAgentConf.lanIp = pCalledAgent->m_ulLanIP;
	ACD_CallWpAgentConf.wpAgentID = pCalledAgent->m_sAgentID;
	ACD_CallWpAgentConf.wpAgentLineNo = ipccMessage->calledAgentLineNo;
	ACD_CallWpAgentConf.wpAgentLineIp = ipccMessage->calledAgentLineIp;
	ACD_CallWpAgentConf.wpAgentLinePort = ipccMessage->calledAgentLinePort;
	this->SendIPCCMsg_UDP(ACD_CallWpAgentConf, pCallerAgent->m_ulAgentIP, pCallerAgent->m_usAgentPort);

	return;
}

void CUtility::OnClient_BeforeTransfer(SIPCCMsg_Client_BeforeTransfer *ipccMessage, unsigned long ip, unsigned short port)
{
	ostringstream ostream;
	ostream.str("");

	string sLog = "";
	ostream.str("");
	ostream << "Client_BeforeTransfer:" 
		<< ";" << "theParty=" << ipccMessage->GetTheParty()
		<< ";" << "presideAgent=" << ipccMessage->GetPresideAgent()
		<< ";" << "otherParty=" << ipccMessage->GetOtherParty()
		<< ";" << "natIP=" << ip
		<< ";" << "natport=" << port;
	sLog = ostream.str();
	
	g_Logger.MakeLog( sLog);

	string destAgent = ipccMessage->GetTheParty();
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(destAgent);
	if(pAgentInfo == NULL)
		return;
	
	if(pAgentInfo->m_iLoginType == LoginType_WebPSTN)
	{
		if(ipccMessage->GetPresideAgent() == destAgent)
			return;
		else
		{
			SIPCCMsg_ACD_BeforeTransferCall_WpAgent ACD_BeforeTransferCall_WpAgent;
			ACD_BeforeTransferCall_WpAgent.SetPresideAgent(ipccMessage->GetPresideAgent());
			ACD_BeforeTransferCall_WpAgent.SetOtherParty(ipccMessage->GetOtherParty());
			ACD_BeforeTransferCall_WpAgent.otherPartyType = ipccMessage->otherPartyType;
			ACD_BeforeTransferCall_WpAgent.wpAgentID = destAgent;
			ACD_BeforeTransferCall_WpAgent.wpAgentLineNo = pAgentInfo->m_WebPstnAgentCurGwLine;
			SendIPCCMsg_UDP(ACD_BeforeTransferCall_WpAgent, pAgentInfo->m_gwCtrlIP, pAgentInfo->m_gwCtrlPort);
		}
	}
}

void CUtility::OnGateway_CallUserFailed_WpAgent(SIPCCMsg_Gateway_CallUserFailed_WpAgent *ipccMessage, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");

	string sLog = "";
	os << "Gateway_CallUserFailed_WpAgent:" 
		<< ";" << "agentID=" << ipccMessage->agentID
		<< ";" << "natip=" << ip
		<< ";" << "natport=" << port;
	sLog = os.str();

	g_Logger.MakeLog( sLog);
	//置 processing
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->agentID);
	if(pAgentInfo == NULL)
		return;

	if(pAgentInfo->m_iLoginType == LoginType_WebPSTN)
	{
		CAgentMgr::GetInstance()->SetAgentState(ipccMessage->agentID, Agent_BackProcess);
		
		pAgentInfo->SetClientAgentState(Agent_BackProcess);
	}
}

void CUtility::OnGateway_CallCleared(SIPCCMsg_Gateway_CallCleared *ipccMessage, unsigned long ip, unsigned short port)
{
/*
	ostringstream os;
	os.str("");

	string sLog = "";
	os << "Gateway_CallCleared:" 
		<< ";" << "agentID=" << ipccMessage->agentid
		<< ";" << "userLineNo=" << ipccMessage->userLineNo
		<< ";" << "natip=" << ip
		<< ";" << "natport=" << port;
	sLog = os.str();

	g_Logger.MakeLog( sLog);

	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->agentid);
	if(pAgentInfo == NULL)
		return;

	if(pAgentInfo->m_iLoginType == LoginType_WebPSTN)
	{
		SIPCCMsg_ACD_StopRingAgentExt StopRingAgentExt;
		StopRingAgentExt.agentID = pAgentInfo->m_sAgentID;
		StopRingAgentExt.ipNo_AgentExt = ipccMessage->userLineNo;
		CUtility::GetInstance()->SendIPCCMsg_UDP(StopRingAgentExt, ip, port);
	}

	//....

*/
}

void CUtility::OnRecord_RecInfo2ACD(SIPCCMsg_Record_RecInfo2ACD *ipccMessage, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");

	string sLog = "";
	os << "Record_RecInfo2ACD:" 
		<< ";" << "agentID=" << ipccMessage->agentID
		<< ";" << "invokeID=" << ipccMessage->invokeID
		<< ";" << "fullPathFileName=" << ipccMessage->fullPathFileName
		<< ";" << "natip=" << ip
		<< ";" << "natport=" << port;
	sLog = os.str();

	g_Logger.MakeLog( sLog);

	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->agentID);
	if(pAgentInfo == NULL)
		return;

	if(pAgentInfo->m_iLoginType == LoginType_OnlyPSTN)
	{
		SIPCCMsg_WebService_RecordInfo webService_RecordInfo;
		webService_RecordInfo.agentId = pAgentInfo->m_sAgentID;
		webService_RecordInfo.station = pAgentInfo->m_sAgentPhoneNo;
		//CTime ct = CTime::GetCurrentTime();
		//webService_RecordInfo.operationTime = ct.Format("%Y-%m-%d %H:%M:%S");
		boost::posix_time::ptime pt = boost::posix_time::second_clock::local_time();
		string strTime = boost::gregorian::to_iso_extended_string(pt.date())+" "+boost::posix_time::to_simple_string(pt.time_of_day());
        webService_RecordInfo.operationTime = strTime;
		webService_RecordInfo.event = "Record";
		webService_RecordInfo.recordId = ipccMessage->invokeID;
		webService_RecordInfo.RecordType = 1;

		SendIPCCMsg_UDP(webService_RecordInfo, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

		string sLog("");
		ostringstream os;
		os.str("");
		os <<  "WebService_RecordInfo: "
			<< "agentId=" << pAgentInfo->m_sAgentID
			<< ";station=" << pAgentInfo->m_sAgentPhoneNo
			<< ";recordId=" << ipccMessage->invokeID
			<< ";RecordType=1"
			<< ";ip=" << pAgentInfo->m_ulAgentIP
			<< ";port=" << pAgentInfo->m_usAgentPort;

		sLog = os.str();
		g_Logger.MakeLog(sLog);
	}

	SIPCCMsg_ACD_RecInfo2Client RecInfo2Client;
	RecInfo2Client.invokeID = ipccMessage->invokeID;
	RecInfo2Client.fullPathFileName = ipccMessage->fullPathFileName;
	SendIPCCMsg_UDP(RecInfo2Client, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);
}

void CUtility::OnClient_GetTalkingAgent(SIPCCMsg_Client_GetTalkingAgent *ipccMessage, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");

	string sLog = "";
	os << "Client_GetTalkingAgent:" 
		<< ";" << "group=" << ipccMessage->Group
		<< ";" << "invokeID=" << ipccMessage->invokeID
		<< ";" << "natip=" << ip
		<< ";" << "natport=" << port;
	sLog = os.str();

	g_Logger.MakeLog( sLog);

	string corpName("");
	string deptName("");
	string skillName("");

	//get corp , get department
	/*
	list<string> tempList;
	CSplitString::SplitString(ipccMessage->Group, "%^#", tempList);
	if(tempList.size() == 2)
	{
		corpName = tempList.front();
		tempList.pop_front();
		deptName = tempList.front();
	}
	*/

	CSpliter strSp(ipccMessage->Group, "%^#");
	corpName = strSp[0];
	deptName = strSp[1];
	skillName = strSp[2];

	CAgentMgr::GetInstance()->SendTalkingAgents(ipccMessage->invokeID, corpName, deptName, skillName, ip, port);
}

void CUtility::OnClient_SuperDisconnect(SIPCCMsg_Client_SuperDisconnect *ipccMessage, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");

	string sLog = "";
	os << "Client_SuperDisconnect:" 
		<< ";" << "agentid=" << ipccMessage->agentID
		<< ";" << "natip=" << ip
		<< ";" << "natport=" << port;
	sLog = os.str();

	g_Logger.MakeLog( sLog);

	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->agentID);
	if(pAgentInfo == NULL)
		return;

	SIPCCMsg_ACD_SuperDisconnect SuperDisconnect;
	SendIPCCMsg_UDP(SuperDisconnect, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);
}

void CUtility::OnClient_MonitRequest(SIPCCMsg_Client_MonitRequest* ipccMessage, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");

	string sLog = "";
	os << "OnClient_MonitRequest:" 
		<< ";" << "agent_src=" << ipccMessage->agentName_Src
		<< ";" << "agent_dest=" << ipccMessage->agentName_Dest
		<< ";" << "natip=" << ip
		<< ";" << "natport=" << port;
	sLog = os.str();

	g_Logger.MakeLog(sLog);

	CAgentInfo* pAgentInfo_Dest = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->agentName_Dest);
	if(pAgentInfo_Dest == NULL)
		return;

	//conf
	SIPCCMsg_ACD_MonitRequestConf MonitRequestConf;
	MonitRequestConf.agentName_Src = ipccMessage->agentName_Src;
	MonitRequestConf.agentName_Dest = ipccMessage ->agentName_Dest;
	MonitRequestConf.CallId = pAgentInfo_Dest->m_sCallID;
	MonitRequestConf.trunkDn = pAgentInfo_Dest->m_CurGwUserLineNo;
	MonitRequestConf.ip = pAgentInfo_Dest->m_CurUserLineIP;
	MonitRequestConf.ip = pAgentInfo_Dest->m_CurUserLinePort;
	SendIPCCMsg_UDP(MonitRequestConf, ip, port);
	//request to gw
	SIPCCMsg_ACD_MonitRequest MonitRequest;
	MonitRequest.agentName_Src = ipccMessage->agentName_Src;
	MonitRequest.agentName_Dest = ipccMessage ->agentName_Dest;
	MonitRequest.trunkDn = pAgentInfo_Dest->m_CurGwUserLineNo;
	SendIPCCMsg_UDP(MonitRequestConf, pAgentInfo_Dest->m_CurGwLineIP, pAgentInfo_Dest->m_CurGwLinePort);

}

void CUtility::OnGateway_GetDeptQueueNum(SIPCCMsg_Gateway_GetDeptQueueNum* ipccMessage, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");

	string sLog = "";
	os << "OnGateway_GetDeptQueueNum:" 
		<< ";invokeID=" << ipccMessage->invokeId
		<< ";m_ssCorpName=" << ipccMessage->m_ssCorpName
		<< ";m_ssDeptName=" << ipccMessage->m_ssDeptName
		<< ";natip=" << ip
		<< ";natport=" << port;
	sLog = os.str();

	int iCount = 0;
	CDepartmentInfo* pDepartmentInfo = NULL;
	CSkillGroupInfo* pSkillGroupInfo = NULL;

	pSkillGroupInfo	= CCorporationMgr::GetInstance()->GetSkillGroupInfo(ipccMessage->m_ssCorpName, ipccMessage->m_ssDeptName);

	if ( pSkillGroupInfo == NULL )
	{
		pDepartmentInfo	= CCorporationMgr::GetInstance()->GetDepartmentInfo(ipccMessage->m_ssCorpName, ipccMessage->m_ssDeptName);
		if(pDepartmentInfo == NULL)
			return;

        iCount = pDepartmentInfo->GetCQueueSize();
	}
	else
	{
        iCount = pSkillGroupInfo->GetCQueueSize();
	}
	
	SIPCCMsg_ACD_GetDeptQueueNumConf ACD_GetDeptQueueNumConf;
	ACD_GetDeptQueueNumConf.invokeId = ipccMessage->invokeId;
	ACD_GetDeptQueueNumConf.m_ssDeptName = ipccMessage->m_ssDeptName;
	ACD_GetDeptQueueNumConf.m_uCount = iCount;
	ACD_GetDeptQueueNumConf.handleId=ipccMessage->handleId; //by heyj 2011-08-15

	SendIPCCMsg_UDP(ACD_GetDeptQueueNumConf, ip, port);
}

void CUtility::OnGateway_GetAgentState(SIPCCMsg_Gateway_GetAgentState* ipccMessage, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");

	string sLog = "";
	os << "OnGateway_GetAgentState:" 
		<< ";" << "invokeId=" << ipccMessage->invokeId
		<< ";" << "m_ssAgentName=" << ipccMessage->m_ssAgentName
		<< ";" << "natip=" << ip
		<< ";" << "natport=" << port;
	sLog = os.str();

	SIPCCMsg_ACD_GetAgentStateConf ACD_GetAgentStateConf;
	ACD_GetAgentStateConf.invokeId = ipccMessage->invokeId;
	ACD_GetAgentStateConf.m_ssAgentName = ipccMessage->m_ssAgentName;
	ACD_GetAgentStateConf.handleId=ipccMessage->handleId;//by heyj 2011-08-15

	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->m_ssAgentName);
	if(pAgentInfo == NULL)
	{
		ACD_GetAgentStateConf.m_ssState = "未登录";
	}
	else
	{
		ACD_GetAgentStateConf.m_ssState = pAgentInfo->GetAgentStateByStr();
	}
	SendIPCCMsg_UDP(ACD_GetAgentStateConf, ip, port);
}

void CUtility::OnGateway_GetDeptWaitingAgentNum(SIPCCMsg_Gateway_GetDeptWaitingAgentNum* ipccMessage, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");

	string sLog = "";
	os << "OnGateway_GetDeptWaitingAgentNum:" 
		<< ";" << "invokeId=" << ipccMessage->invokeId
		<< ";" << "m_ssCorpName=" << ipccMessage->m_ssCorpName
		<< ";" << "m_ssDeptName=" << ipccMessage->m_ssDeptName
		<< ";" << "natip=" << ip
		<< ";" << "natport=" << port;
	sLog = os.str();

	int iCount = 0;
	CDepartmentInfo* pDepartmentInfo = NULL;
	CSkillGroupInfo* pSkillGroupInfo = NULL;

	pSkillGroupInfo	= CCorporationMgr::GetInstance()->GetSkillGroupInfo(ipccMessage->m_ssCorpName, ipccMessage->m_ssDeptName);

	if ( pSkillGroupInfo == NULL )
	{
		pDepartmentInfo	= CCorporationMgr::GetInstance()->GetDepartmentInfo(ipccMessage->m_ssCorpName, ipccMessage->m_ssDeptName);
		if(pDepartmentInfo == NULL)
			return;

        iCount = pDepartmentInfo->GetWAQueueSize();
	}
	else
	{
        iCount = pSkillGroupInfo->GetWAQueueSize();
	}

	SIPCCMsg_ACD_GetDeptWaitingAgentNumConf ACD_GetDeptWaitingAgentNumConf;
	ACD_GetDeptWaitingAgentNumConf.invokeId = ipccMessage->invokeId;
	ACD_GetDeptWaitingAgentNumConf.m_uCount = iCount;
	ACD_GetDeptWaitingAgentNumConf.handleId=ipccMessage->handleId;//by 2011-08-15
	SendIPCCMsg_UDP(ACD_GetDeptWaitingAgentNumConf, ip, port);
}

void CUtility::OnGateway_GetDeptLoginAgentNum(SIPCCMsg_Gateway_GetDeptLoginAgentNum* ipccMessage, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");

	string sLog = "";
	os << "OnGateway_GetDeptLoginAgentNum:" 
		<< ";" << "invokeId=" << ipccMessage->invokeId
		<< ";" << "m_ssCorpName=" << ipccMessage->m_ssCorpName
		<< ";" << "m_ssDeptName=" << ipccMessage->m_ssDeptName
		<< ";" << "natip=" << ip
		<< ";" << "natport=" << port;
	sLog = os.str();

	int iLoginAgentNum = 0;
	if ( ipccMessage->m_ssDeptName == "" )
	{
		if ( ipccMessage->m_ssCorpName == "" )
		{
			return;
		}
		
		CCorporationInfo* pCorporationInfo = CCorporationMgr::GetInstance()->GetCorporation(ipccMessage->m_ssCorpName);
		if(pCorporationInfo == NULL)
			return;

		map<string, CDepartmentInfo*>::iterator pIter;
		for(pIter=pCorporationInfo->m_mapDepartments.begin(); pIter!=pCorporationInfo->m_mapDepartments.end(); ++pIter)
		{
			if(pIter->second != NULL)
				iLoginAgentNum += pIter->second->GetAgentNum();
		}

		/*map<string, CSkillGroupInfo*>::iterator pIter2;
		for(pIter2=pCorporationInfo->m_mapSkillGroups.begin(); pIter2!=pCorporationInfo->m_mapSkillGroups.end(); ++pIter2)
		{
			if(pIter2->second != NULL)
				iLoginAgentNum += pIter2->second->GetAgentNum();
		}*/
	}
	else
	{
		if ( ipccMessage->m_ssCorpName == "" )
		{
			return;
		}

		CDepartmentInfo* pDepartmentInfo = NULL;
		CSkillGroupInfo* pSkillGroupInfo = NULL;

		pSkillGroupInfo	= CCorporationMgr::GetInstance()->GetSkillGroupInfo(ipccMessage->m_ssCorpName, ipccMessage->m_ssDeptName);

		if ( pSkillGroupInfo == NULL )
		{
			pDepartmentInfo	= CCorporationMgr::GetInstance()->GetDepartmentInfo(ipccMessage->m_ssCorpName, ipccMessage->m_ssDeptName);
			if(pDepartmentInfo == NULL)
				return;

			iLoginAgentNum = pDepartmentInfo->GetAgentNum();
		}
		else
		{
			iLoginAgentNum = pSkillGroupInfo->GetAgentNum();
		}
		
	}

	SIPCCMsg_ACD_GetDeptLoginAgentNumConf ACD_GetDeptLoginAgentNumConf;
	ACD_GetDeptLoginAgentNumConf.invokeId = ipccMessage->invokeId;
	ACD_GetDeptLoginAgentNumConf.m_uCount = iLoginAgentNum;
	ACD_GetDeptLoginAgentNumConf.handleId=ipccMessage->handleId;// by heyj 2011-08-15

	SendIPCCMsg_UDP(ACD_GetDeptLoginAgentNumConf, ip, port);
}

void CUtility::OnGetACDLicence(SIPCCMsg_SysManage_NotifyACDLicence* licence, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");
	
	string sLog = "";
	os << "OnGetACDLicence:" 
		<< ";" << "licence=" << licence->licence
		<< ";" << "natip=" << ip
		<< ";" << "natport=" << port;
	sLog = os.str();
	g_Logger.MakeLog(sLog);

	CSpliter spliter(licence->licence, ";",",");

	for(int i=0; i<spliter.GetSize(); i++)
	{
		if (spliter[i] == "")
		{
			continue;
		}

		string corpname = spliter[i][0];

		CCorporationInfo* pCorp = CCorporationMgr::GetInstance()->GetCorporation(corpname);
		if(pCorp == NULL)
		{
			g_Logger.MakeLog("Cannt find corp=" + corpname + ";all=" + string(spliter[i]));
			continue;
		}

		if (spliter[i][1] == "")
		{
			pCorp->m_dynamicWebAgentLicence = 0x7FFFFFFF;
		}
		else
		{
			pCorp->m_dynamicWebAgentLicence = atoi(spliter[i][1].c_str());
		}

		if (spliter[i][2] == "")
		{
			pCorp->m_dynamicMobileAgentLicence = 0x7FFFFFFF;
		}
		else
		{
			pCorp->m_dynamicMobileAgentLicence = atoi(spliter[i][2].c_str());
		}
	}
}

void CUtility::CheckRingAgentExtTime()
{
	unsigned long iCurTimeMark = gettickcount();
	if(iCurTimeMark - m_CheckRingAgentExtTime > 1000)
	{
		MyCheckRingAgentExtTime();
		m_CheckRingAgentExtTime = iCurTimeMark;
	}
}

void CUtility::MyCheckRingAgentExtTime()
{
	for (map<string, long>::iterator it = m_RingAgentExtMap.begin()
		; it != m_RingAgentExtMap.end(); )
	{
		map<string, long>::iterator it2 = it++;
		CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo( it2->first );
		if(pAgentInfo == NULL)
		{
			m_RingAgentExtMap.erase(it2);
			continue;
		}

		CDepartmentInfo* pDepartmentInfo = CCorporationMgr::GetInstance()->GetDepartmentInfo( pAgentInfo->m_sCorporation, pAgentInfo->m_sDepartment );
		if(pDepartmentInfo == NULL)
		{
			m_RingAgentExtMap.erase(it2);
			continue;
		}

//		if (gettickcount() - it->second >= 20000)
		if ( gettickcount() - it2->second >= pDepartmentInfo->m_iMaxRingTime )
		{
//			map<string, DWORD>::iterator it2 = it++;
			OnRingAgentExtTimeOut(it2->first);
			m_RingAgentExtMap.erase(it2);
		}
		
	}
}

void CUtility::OnRingAgentExtTimeOut(string sAgent)
{
////////////	ostringstream os;
////////////	os.str("");
////////////
////////////	string sLog = "";
////////////	os << "OnRingAgentExtTimeOut:" 
////////////		<< ";" << "sAgent=" << sAgent;
////////////	sLog = os.str();
////////////
////////////	g_Logger.MakeLog( sLog);
////////////
////////////	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(sAgent);
////////////	if(pAgentInfo == NULL)
////////////		return;
////////////
////////////	bool bFailed = true;
////////////	string sNextAgentId = "";
////////////
////////////	CUserInfo& user = pAgentInfo->m_userInfo;
////////////	if (CallParty_VoiceGateWay == user.m_iUserType
////////////		&& user.m_CalledIsDept
////////////		&& pAgentInfo->m_iLoginType == LoginType_OnlyPSTN)
////////////	{
////////////		user.m_CallFailedAgents.insert(sAgent);
////////////
////////////		CDepartmentInfo* pDepartmentInfo = 
////////////			CCorporationMgr::GetInstance()->GetDepartmentInfo(user.m_sOriginCalledTarget);
////////////
////////////		if(pDepartmentInfo == NULL)
////////////		{
////////////			goto conf;
////////////		}
////////////
////////////		int iAgentNumOfTheDepart = pDepartmentInfo->GetAgentNum();
////////////
////////////		if(0 == iAgentNumOfTheDepart)	//无座席登录
////////////		{
////////////			goto conf;
////////////		}
////////////
////////////		sNextAgentId = pDepartmentInfo->FrontOfWAQueue();//参数确定用户类型
////////////		if(sNextAgentId == "" || user.m_CallFailedAgents.find(sNextAgentId) != user.m_CallFailedAgents.end())
////////////		{
////////////			goto conf;
////////////		}
////////////
////////////        //todo ?什么逻辑没看懂
////////////		pDepartmentInfo->QueueToCQueue(true, user);
////////////		sNextAgentId = pDepartmentInfo->PopFromWAQueue();
////////////		pDepartmentInfo->func_AgentWaiting(sNextAgentId, true);
////////////		bFailed = false;
////////////	}
////////////
////////////conf:
////////////	if (!bFailed)
////////////	{
////////////		SIPCCMsg_ACD_StopRingAgentExt StopRingAgentExt;
////////////		StopRingAgentExt.agentID = pAgentInfo->m_sAgentID;
////////////		StopRingAgentExt.ipNo_AgentExt = pAgentInfo->m_sIPNo_TheAgent;
////////////		StopRingAgentExt.userName = user.m_sUserName;
////////////		CUtility::GetInstance()->SendIPCCMsg_UDP(StopRingAgentExt, user.m_ulUserIP, user.m_usUserPort);
////////////	}
}

void CUtility::OnGetDeptOverflow(SIPCCMsg_Gateway_GetDeptOverflow* licence, unsigned long ip, unsigned short port)
{
	ostringstream os;
	os.str("");

	string sLog = "";
	os << "OnGetDeptOverflow:" 
		<< ";" << "invokeId=" << licence->invokeId
		<< ";" << "m_ssCorpName=" << licence->m_ssCorpName
		<< ";" << "m_ssDeptName=" << licence->m_ssDeptName
		<< ";" << "natip=" << ip
		<< ";" << "natport=" << port;
	sLog = os.str();
	g_Logger.MakeLog(sLog);

	SIPCCMsg_ACD_GetDeptOverflowConf conf;
	conf.invokeId = licence->invokeId;
	conf.handleId=licence->handleId;//by heyj 2011-08-15
	CDepartmentInfo* pDept = CCorporationMgr::GetInstance()->GetDepartmentInfo(licence->m_ssCorpName, licence->m_ssDeptName);
	if (pDept)
	{
		conf.timeoutValue = pDept->m_iOverflowTime;
		conf.overFlowDest = pDept->m_sOverflowTarget;
		conf.overFlowDestType = pDept->m_iOverFlowDestType;
	}
	else
	{
		g_Logger.MakeLog("部门不存在");
	}

	SendIPCCMsg_UDP(conf, ip, port);
}

void CUtility::OutDial_SetAgentCount(string corpName, string deptName)
{
	//指定组
	CDepartmentInfo* pDepart = 
		CCorporationMgr::GetInstance()->GetDepartmentInfo(corpName, deptName);
	
	if(pDepart == NULL)
		return;

	pDepart->SendOutDialSys_DeptAgentsInfo();
}

void CUtility::OutDial_SetAgentCount()
{
	//轮循corp
	CCorporationMgr::GetInstance()->OutDial_SetAgentCount();
}

void CUtility::OnClientSendStrMsg( SIPCCMsg_Client_SendStrMsg* clientSendStrMsg, unsigned long ip, unsigned short port )
{

	//查看座席状态
	CAgentInfo* pAgentInfo_caller = CAgentMgr::GetInstance()->GetAgentInfo(clientSendStrMsg->theSender);
	if(pAgentInfo_caller == NULL)
		return;
	
	char acDestAgents[BUFSIZ] = {0};
	strncpy(acDestAgents, clientSendStrMsg->destAgents.c_str(), BUFSIZ-1); // clientSendStrMsg->destAgents;	 格式 1@1.com|1@2.com|...	
	char seps[]   = "|";
	char *called;
	// Establish string and get the first token:
	called = strtok( acDestAgents, seps ); // C4996
	// Note: strtok is deprecated; consider using strtok_s instead
	while( called != NULL )
	{
		// While there are tokens in "string"
//////////////////////////////////////////////////////////////////////////
		string sAgentID_caller = clientSendStrMsg->theSender;
		CAgentInfo* pAgentInfo_caller = CAgentMgr::GetInstance()->GetAgentInfo(sAgentID_caller);
		CAgentInfo* pAgentInfo_called = CAgentMgr::GetInstance()->GetAgentInfo(called);

		if(pAgentInfo_caller==NULL || pAgentInfo_called==NULL)	
			return;
		if(pAgentInfo_called->m_iLoginType == LoginType_OnlyWeb)
			pAgentInfo_called->m_iCalledPartyType = CalledPartyType_WebAgent;
		else if(pAgentInfo_called->m_iLoginType == LoginType_WebPSTN)
			pAgentInfo_called->m_iCalledPartyType = CalledPartyType_WpAgent;

		pAgentInfo_caller->m_sCalledParty = called;
		pAgentInfo_caller->m_sTheOtherAgentID = called;
		pAgentInfo_called->m_sTheOtherAgentID = pAgentInfo_caller->m_sAgentID;

		SIPCCMsg_ACD_AgentMakeCallConf agentMakeCallConf;

		if(pAgentInfo_called == NULL)
		{
			//座席未登录
			agentMakeCallConf.callingState = AgentCalling_AgentNoLogin;	
			this->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);

			/*SIPCCMsg_ACD_NotifyAgentStateEvent notifyAgentStateEvent;
			notifyAgentStateEvent.SetCorporationName(pAgentInfo_caller->m_sCorporation);
			notifyAgentStateEvent.SetTheDepartment(pAgentInfo_caller->m_sDepartment);
			notifyAgentStateEvent.SetAgentID(sAgentID_caller);
			notifyAgentStateEvent.stateEvent = Event_DialFailed;
			notifyAgentStateEvent.SetCallTarger(called);
			notifyAgentStateEvent.SetCurrentTime(CLog::GetLocalTime());*/
			//this->SendIPCCMsg_UDP(notifyAgentStateEvent, this->m_IPCCConfig.DataCollectServer_IP, ConvertPort(ServerInfoDef::Port_DataCollectServer));

			return;
		}

		if(pAgentInfo_caller->m_iLoginType == LoginType_WebPSTN 
			&& pAgentInfo_called->m_iLoginType != LoginType_OnlyWeb)
		{
			agentMakeCallConf.callingState = AgentCalling_AgentNoLogin;	
			this->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);
			return;
		}

		//发送 ACD_SendStrMsg
		SIPCCMsg_ACD_SendStrMsg ACDSendStrMsg;

		ACDSendStrMsg.theSender = clientSendStrMsg->theSender;
		ACDSendStrMsg.msgContent = clientSendStrMsg->msgContent;

		this->SendIPCCMsg_UDP(ACDSendStrMsg, pAgentInfo_called->m_ulAgentIP, pAgentInfo_called->m_usAgentPort);
//////////////////////////////////////////////////////////////////////////

		// Get next token: 
		called = strtok( NULL, seps ); // C4996
	}
}

void CUtility::OnClient_ReturnToAgent(SIPCCMsg_Client_ReturnToAgent *returnToAgent, unsigned long ip, unsigned short port)
{
	string sAgentID = returnToAgent->agentID;

	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(sAgentID);
	if(pAgentInfo == NULL)
	{
		if ( returnToAgent->bIsSuccess != 0 )
		{
			SIPCCMsg_ACD_ReturnToAgent callReturned;
			callReturned.IVRUUIData = returnToAgent->IVRUUIData;
			callReturned.bIsSuccess = returnToAgent->bIsSuccess;
			callReturned.bIsAgentWaiting = false;
			callReturned.handleId=returnToAgent->handleId;//by heyj 2011-08-15

			CUtility::GetInstance()->SendIPCCMsg_UDP(callReturned, ip, port);

			//log
			string sLog;
			ostringstream ostream;
			ostream.str("");
			ostream << "OnClient_ReturnToAgent:" << "agentID=" << sAgentID
				<< ";" << "IVRUUIData=" << callReturned.IVRUUIData
				<< ";" << "bIsSuccess=" << callReturned.bIsSuccess
				<< ";" << "bIsAgentWaiting=" << callReturned.bIsAgentWaiting
				<< ";" << "natip=" << ip
				<< ";" << "natport=" << port;
			sLog = ostream.str();
			g_Logger.MakeLog( sLog);
		}

		return;
	}

	if(pAgentInfo->m_iCurState != Agent_IVRnWait)
	{
		if ( returnToAgent->bIsSuccess != 0 )
		{
			SIPCCMsg_ACD_ReturnToAgent callReturned;
			callReturned.IVRUUIData = returnToAgent->IVRUUIData;
			callReturned.bIsSuccess = returnToAgent->bIsSuccess;
			callReturned.bIsAgentWaiting = false;
			callReturned.handleId=returnToAgent->handleId;//by heyj 2011-08-15

			CUtility::GetInstance()->SendIPCCMsg_UDP(callReturned, ip, port);

			//log
			string sLog;
			ostringstream ostream;
			ostream.str("");
			ostream << "OnClient_ReturnToAgent:" << "agentID=" << sAgentID
				<< ";" << "IVRUUIData=" << callReturned.IVRUUIData
				<< ";" << "bIsSuccess=" << callReturned.bIsSuccess
				<< ";" << "bIsAgentWaiting=" << callReturned.bIsAgentWaiting
				<< ";" << "natip=" << ip
				<< ";" << "natport=" << port;
			sLog = ostream.str();
			g_Logger.MakeLog( sLog);
		}

		return;
	}

	SIPCCMsg_ACD_ReturnToAgent callReturned;
	callReturned.IVRUUIData = returnToAgent->IVRUUIData;
	callReturned.bIsSuccess = returnToAgent->bIsSuccess;
	callReturned.bIsAgentWaiting = true;
	callReturned.handleId=returnToAgent->handleId;//by heyj 2011-08-15

	CUtility::GetInstance()->SendIPCCMsg_UDP(callReturned, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

	//log
	string sLog;
	ostringstream ostream;
	ostream.str("");
	ostream << "OnClient_ReturnToAgent:" << "agentID=" << sAgentID
		<< ";" << "IVRUUIData=" << callReturned.IVRUUIData
		<< ";" << "bIsSuccess=" << callReturned.bIsSuccess
		<< ";" << "bIsAgentWaiting=" << callReturned.bIsAgentWaiting
		<< ";" << "natip=" << pAgentInfo->m_ulAgentIP
		<< ";" << "natport=" << pAgentInfo->m_usAgentPort;
	sLog = ostream.str();
	g_Logger.MakeLog( sLog);
}

void CUtility::OnGateway_WpAgentCallAnswerred(SIPCCMsg_Gateway_WpAgentCallAnswerred *ipccMessage, unsigned long ip, unsigned short port)
{
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage->agentID);
	if(pAgentInfo == NULL)
		return;

	pAgentInfo->m_CurGwUserLineNo = ipccMessage->gwUserLineNo;
	pAgentInfo->m_WebPstnAgentCurGwLine = ipccMessage->gwAgentLineNo;
	pAgentInfo->m_CurGwLineIP = ipccMessage->gwAgentLineIp;
	pAgentInfo->m_CurGwLinePort = ipccMessage->gwAgentLinePort;
	pAgentInfo->m_CurUserLineIP = ipccMessage->gwUserLineIp;
	pAgentInfo->m_CurUserLinePort = ipccMessage->gwUserLinePort;
	pAgentInfo->m_gwCtrlIP = ip;
	pAgentInfo->m_gwCtrlPort = port;

	CAgentMgr::GetInstance()->SetAgentState(ipccMessage->agentID, Agent_Talking);

	pAgentInfo->SetClientAgentState(Agent_Talking, 0, ipccMessage->gwAgentLineNo, ipccMessage->gwUserLineNo, ipccMessage->gwAgentLineNo, ""
		,ipccMessage->gwAgentLineIp, ipccMessage->gwAgentLinePort, ipccMessage->gwUserLineIp, ipccMessage->gwUserLinePort);

}

void CUtility::OnLicence_QueryConf(SIPCCMsg_Licence_QueryConf* ipccMessage, unsigned long ip, unsigned short port)
{
	if ( m_iLicenceConfirmMode < 2 )
	{
		return;
	}

	if ( m_sLicenceConfirmTag != ipccMessage->queryTag )
	{
		return;
	}

	if ( ipccMessage->LicenceConfirmPassed != 0 )
	{
		if ( m_iLicenceConfirmMode == 2 )
		{
		  boost::thread thrd(Thread_NotifyDataCollect,this);

			time_t ltime;
			ltime = time(NULL);
			char str[32];
			strftime(str, sizeof(str), "%Y-%m-%d %H:%M:00", localtime(&ltime));

			SIPCCMsg_ACD_ResetDataCollect resetDataCollect;
			resetDataCollect.curTime = str;
			CUtility::GetInstance()->SendIPCCMsg_UDP(
				resetDataCollect, 
				CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_IP, 
				CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_Port);

			SIPCCMsg_ACD_ResetMonitor resetMonitor;
			resetMonitor.curTime = str;
			CUtility::GetInstance()->SendToMonitorServer(resetMonitor);				

		}

		m_iLicenceConfirmMode = 1;
	}
	else
	{
		if ( ipccMessage->FailedCause == 1 )
		{
			m_bShouldCNT = true;
			m_MsgCNT = 0;
		}
		else if ( m_iLicenceConfirmMode == 2 )
		{
			m_bShouldCNT = true;
			m_MsgCNT = 0;
		}
		else
		{
			m_iAgentsLicence = 0;
		}

		m_iLicenceConfirmMode = 0;
	}
}

//会议开始
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void  CUtility::OnGateway_ConferenceStartConf(SIPCCMsg_Gateway_ConferenceStartConf& ipccMessage, unsigned long ip, unsigned short port)
{
    bool bConfereneSuccess=(bool)(ipccMessage.bSucc);
	////////////begin write log/////////////////////////////////////////////////
	{
		ostringstream ostream;
		string sLog = "";
		ostream.str("");
		ostream << "SIPCCMsg_Gateway_ConferenceStartConf : " 
			<< "firstName=" << ipccMessage.firstName
			<< ", firstLineNo=" << ipccMessage.firstLineNo
			<< ", thirdName=" << ipccMessage.thirdName
			<< ", conferenceType=" << ipccMessage.conferenceType
			<< ", managerid=" << ipccMessage.managerid
			<< ", bSucc=" << bConfereneSuccess
			<< ", firstConferencePort=" <<ipccMessage.firstConferencePort
			<< ", secondConferencePort=" << ipccMessage.secondConferencePort
			<< ", firstPartyLineNo=" <<ipccMessage.firstPartyLineNo
			<< ", secondPartyLineNo=" << ipccMessage.secondPartyLineNo;
		sLog = ostream.str();			
		g_Logger.MakeLog( sLog );
	}
	
    ////////////end  write log/////////////////////////////////////////////////////////////
	
    PrintConferenceTypeMessage(ipccMessage.conferenceType);
	
	 CAgentInfo* pAgentInfo1 = CAgentMgr::GetInstance()->GetAgentInfo( ipccMessage.firstName);
	 if(pAgentInfo1 == NULL)
	 { 
		 string slog=" ERRROR: 会议主控方坐席不存在 agnetid=" + ipccMessage.firstName+"无法开启本次会义议";
		 g_Logger.MakeLog( slog );	
		 return ;

	 }
	 //////////////////////////////////////////////////////////////////////////////////////
     SIPCCMsg_ACD_ConferenceStartConf  ConferenceStartConf;//add 发给会议主控方
	 ConferenceStartConf.firstName=ipccMessage.firstName;
	 ConferenceStartConf.firstLineNo=ipccMessage.firstLineNo;
	 ConferenceStartConf.thirdName=ipccMessage.thirdName;
	 ConferenceStartConf.conferenceType=ipccMessage.conferenceType;
	 ConferenceStartConf.managerid=ipccMessage.managerid;
	 ConferenceStartConf.bSucc=ipccMessage.bSucc;
	 ConferenceStartConf.ConferencePort=ipccMessage.firstConferencePort;
	 ConferenceStartConf.thePartyLineNo=ipccMessage.firstPartyLineNo;
	 ConferenceStartConf.theOtherPartyLineNo=ipccMessage.secondPartyLineNo;


	 //
	 SIPCCMsg_ACD_ConferenceStart   ACD_ConferenceStart;//ACD发给会议参与方
     ACD_ConferenceStart.firstName=ipccMessage.firstName;
	 ACD_ConferenceStart.firstLineNo=ipccMessage.firstLineNo;
	 ACD_ConferenceStart.ConferencePort=ipccMessage.secondConferencePort;
	 ACD_ConferenceStart.thePartyLineNo=ipccMessage.secondPartyLineNo;
	 ACD_ConferenceStart.theOtherPartyLineNo=ipccMessage.firstPartyLineNo;
	 ACD_ConferenceStart.audioStreamDescription = ipccMessage.audioStreamDescription;
     ////////////////////////////////////////////////////////////////////////////////
	 string strConferenceID="";//会议ID,会议成功是会生产一个会议ID
	 if(bConfereneSuccess==true)
	 {
       
	    strConferenceID=NewGlobalInvokeID();
		pAgentInfo1->strConferenceID=strConferenceID;
		pAgentInfo1->firstName=ipccMessage.firstName;		
		pAgentInfo1->firstLineNo=ipccMessage.firstLineNo;
		pAgentInfo1->thirdName=ipccMessage.thirdName;
		pAgentInfo1->conferenceType=ipccMessage.conferenceType;
		pAgentInfo1->ConferenceRole=AgentJoin_Active;  
		pAgentInfo1->firstPartyLineNo=ipccMessage.firstPartyLineNo;
		pAgentInfo1->secondPartyLineNo= ipccMessage.secondPartyLineNo;
	    string tsLog = "会议成功，生成一个 conferenceid="+strConferenceID ;
		g_Logger.MakeLog( tsLog );

	 }
	
	 //发送给主控方
	if( pAgentInfo1->bApplyforConference==true)
	{
	    SendIPCCMsg_UDP(ConferenceStartConf, pAgentInfo1->m_ulAgentIP, pAgentInfo1->m_usAgentPort);


		{
			ostringstream ostream;
			string sLog = "";
			ostream.str("");
			ostream << "sAgentID= " <<pAgentInfo1->m_sAgentID
				<< "ip=" <<pAgentInfo1->m_ulAgentIP
				<< ", port=" <<  pAgentInfo1->m_usAgentPort;

			sLog = ostream.str();			
			g_Logger.MakeLog( sLog );
		}
	}
	else
	{
	   	g_Logger.MakeLog( "会议主控坐席，未申请过会会议，信息将不转发" );
		return;
	
	}
	if ( bConfereneSuccess==false )
	{
		g_Logger.MakeLog( "会议创建失败！！！" );
		return ;
	}
	switch(ipccMessage.conferenceType)
	{
	case 1: //1表示两座席一外线
		{					
			CAgentInfo* pAgentInfo2 = CAgentMgr::GetInstance()->GetAgentInfo( ipccMessage.thirdName);
			if(pAgentInfo2 == NULL)
			{
				string slog="EEEOR:会议参与坐席不存在 agentid="+ ipccMessage.thirdName+ ",conferenceid="+strConferenceID;
				g_Logger.MakeLog( slog );
				return ;
			}
			
			this->SendIPCCMsg_UDP(ACD_ConferenceStart, pAgentInfo2->m_ulAgentIP, pAgentInfo2->m_usAgentPort);

			{
				ostringstream ostream;
				string sLog = "";
				ostream.str("");
				ostream << "参与坐席 sAgentID= " <<pAgentInfo2->m_sAgentID
					<< "ip=" <<pAgentInfo2->m_ulAgentIP
					<< ", port=" <<  pAgentInfo2->m_usAgentPort;

				sLog = ostream.str();			
				g_Logger.MakeLog( sLog );
			}
			if(bConfereneSuccess==true)//
			{			
				//////////
				/*pAgentInfo2->firstLineNo=ipccMessage.firstLineNo;
				pAgentInfo2->thirdName=ipccMessage.thirdName;
				pAgentInfo2->conferenceType=ipccMessage.conferenceType;
				pAgentInfo2->ConferenceRole=AgentJoin_Passive;
				pAgentInfo2->strConferenceID=strConferenceID;*/

				pAgentInfo2->strConferenceID=strConferenceID;
				pAgentInfo2->firstName=ipccMessage.firstName;
				pAgentInfo2->firstLineNo=ipccMessage.firstLineNo;
				pAgentInfo2->thirdName=ipccMessage.thirdName;
				pAgentInfo2->conferenceType=ipccMessage.conferenceType;
				pAgentInfo2->ConferenceRole=AgentJoin_Passive;  
				pAgentInfo2->firstPartyLineNo=ipccMessage.firstPartyLineNo;
				pAgentInfo2->secondPartyLineNo= ipccMessage.secondPartyLineNo;
				

			}
			
   

		}
		break;
	case 2://表示一座席两外线
		{
           //启动，但不需要处理

		}
		break;
	case 3://表示一座席一外线	
		{
               //本次不启动

		}
		break;
	case 4 ://表示两外线
		{  
           //本次不启动

		}
		break;
	case 5://5表示两座席
		{

             //本次不启动
		}
		break;
	default:
		break;




	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//结束会议,只有主持人才能结束会议
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void   CUtility::OnClient_ConferenceEnd(SIPCCMsg_Client_ConferenceEnd& ipccMessage, unsigned long ip, unsigned short port)
{
	ostringstream ostream;
	string sLog = "";
	ostream.str("");
	ostream << "OnClient_ConferenceEnd : " 
		<< "firstName=" << ipccMessage.firstName
		<< ", firstLineNo=" << ipccMessage.firstLineNo
		<< ", thirdName=" << ipccMessage.thirdName
		<< ", conferenceType=" << ipccMessage.conferenceType
		<< ", ip=" << ip<<",port="<<port;	
	sLog = ostream.str();			
	g_Logger.MakeLog( sLog );
	//会议主控与坐席
	CAgentInfo* pAgentInfo1 = CAgentMgr::GetInstance()->GetAgentInfo( ipccMessage.firstName);
	if(pAgentInfo1 == NULL)
	{
		g_Logger.MakeLog( "ERROR: 会议主控坐席不存在,本会议出问题" );
		return;

	}
	////////////////////////////////////////////////////////////////////////////////////////
	PrintConferenceTypeMessage(ipccMessage.conferenceType);		
	//发给会议参议的
	SIPCCMsg_ACD_ConferenceEnd  ACD_ConferenceEnd;
	ACD_ConferenceEnd.firstName=ipccMessage.firstName;
	ACD_ConferenceEnd.firstLineNo=ipccMessage.firstLineNo;
	/////////////////////////////////////////////////////////////////////////////////////////
	g_Logger.MakeLog( "会议ID="+pAgentInfo1->strConferenceID );
	switch(ipccMessage.conferenceType)
	{
	case 1://1表示两座席一外线	
		{		
			if(ipccMessage.thirdName!="")
			{
				CAgentInfo* pAgentInfo2 = CAgentMgr::GetInstance()->GetAgentInfo(ipccMessage.thirdName);	
				if(pAgentInfo2 == NULL)
				{
					g_Logger.MakeLog( "ERROR:参与方：" +ipccMessage.thirdName+"坐席不存在");
					return;
				}
				else
				{
				  SendIPCCMsg_UDP(ACD_ConferenceEnd, pAgentInfo2->m_ulAgentIP, pAgentInfo2->m_usAgentPort);
				}

			}
			else
			{
				g_Logger.MakeLog( "参与方为空值，已经退出了" );

			}
		}
		break;
	case 2://表示一座席两外线
		{
			// 此不处理，到AgnetQueryState中处理			
		}
		break;
	case 3://表示一座席一外线
		{
		   // 此不处理，到AgnetQueryState中处理

		}
		break;
	case 4 ://表示两外线
		{ 
			// 此不处理
		}
		break;
	case 5://5表示两座席
		{

		}
		break;
	default:
		break;
	}
	return ;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//离开会议
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void  CUtility:: OnGateway_UserLeaveConference(SIPCCMsg_Gateway_UserLeaveConference& ipccMessage, unsigned long ip, unsigned short port)
{	
	ostringstream ostream;
	string sLog = "";
	ostream.str("");
	ostream << "OnGateway_UserLeaveConference leaveUserName=" << ipccMessage.leaveUserName
		<< ", initiateAgentName=" << ipccMessage.initiateAgentName
		<< ", joinName=" << ipccMessage.joinName
		<< ", conferenceType=" << ipccMessage.conferenceType
		<< ", managerid=" << ipccMessage.managerid;	
	sLog = ostream.str();
	///////////////////////////////////////////////////////////////////////////////////////////////////
    CAgentInfo* pAgentInfo1 = CAgentMgr::GetInstance()->GetAgentInfo( ipccMessage.initiateAgentName);
	if(pAgentInfo1== NULL)
	{
		
		g_Logger.MakeLog( sLog );
		g_Logger.MakeLog( "会议主控坐席不存在" );
		return ;

	}
	else
	{

		sLog .append("conferenceid= "+pAgentInfo1->strConferenceID);		
		g_Logger.MakeLog( sLog );		
		
	}
    PrintConferenceTypeMessage(ipccMessage.conferenceType);
	///////////////////////////////////////////////////////////////////////////////////
	SIPCCMsg_ACD_UserLeaveConference    UserLeaveConference;
	UserLeaveConference.leaveUserName=ipccMessage.leaveUserName;
	UserLeaveConference.initiateAgentName=ipccMessage.initiateAgentName;
	UserLeaveConference.joinName=ipccMessage.joinName;
	UserLeaveConference.conferenceType=ipccMessage.conferenceType;
	UserLeaveConference.managerid=ipccMessage.managerid;
    ///////////////////////////////////////////////////////////////////////////////////	
	bool bManinAgentLeaveLineout=false;
	if(ipccMessage.initiateAgentName== ipccMessage.leaveUserName)
	{
	  	g_Logger.MakeLog( "会议主控方线，情况比较少见！！！！！！" );
		bManinAgentLeaveLineout=true;	  
	
	}
	////////////////////////////////////////////////////////////////////////////////////
	switch(ipccMessage.conferenceType)
	{
	case 1: //1表示两座席一外线
		{ 		
			
			if(bManinAgentLeaveLineout==false)//非主控离线，常见
			{
				int nLeaveType=1;
				if( ipccMessage.leaveUserName==pAgentInfo1->firstLineNo)
				{
					nLeaveType=1;//用户离线

				}
				if( ipccMessage.leaveUserName==pAgentInfo1->thirdName)
				{
					nLeaveType=2;//参与离线			
				}
				//发送给主控方离会信息
				this->SendIPCCMsg_UDP(UserLeaveConference, pAgentInfo1->m_ulAgentIP, pAgentInfo1->m_usAgentPort);				
				if( nLeaveType==1)//用户离开，转化成两坐席会议
				{
					//pAgentInfo1->conferenceType=5;//表示两座席 (本次不启动)
					pAgentInfo1->firstLineNo="";//
					g_Logger.MakeLog( "用户离开" );
					//将会议类型改为两坐席会议
					if(pAgentInfo1->thirdName=="")
					{
						g_Logger.MakeLog( "会议参与坐席为空，不再处理" );
						return ;

					}
					CAgentInfo* pAgentInfo2 = CAgentMgr::GetInstance()->GetAgentInfo( pAgentInfo1->thirdName);
					if(pAgentInfo2 == NULL)
					{
						g_Logger.MakeLog( "会议参与坐席不存在" );					

					}
					else
					{
						//pAgentInfo2->conferenceType=5;//表示两座席 (本次不启动)
						pAgentInfo2->firstLineNo="";//
						//向会议参与坐席发送用户离会消息
						this->SendIPCCMsg_UDP(UserLeaveConference, pAgentInfo2->m_ulAgentIP, pAgentInfo2->m_usAgentPort);
					}
                      return ;
   
				}
				else//坐席方与离开，转化成一座席一外线会议		
				{

					//pAgentInfo1->conferenceType=3;//一座席一外线 (本次不启动)					
               		g_Logger.MakeLog( "坐席参与离开会议" );
					if(pAgentInfo1->thirdName=="")
					{
						g_Logger.MakeLog( "会议参与坐席为空，不在处理" );
						 return ;

					}			
					CAgentInfo* pAgentInfo2 = CAgentMgr::GetInstance()->GetAgentInfo( pAgentInfo1->thirdName);
					if(pAgentInfo2 == NULL)
					{
						g_Logger.MakeLog( "会议参与坐席不存在" );						
						
					}
					else
					{
						pAgentInfo2->ClearConferenceParam();					
						SendIPCCMsg_UDP(UserLeaveConference, pAgentInfo1->m_ulAgentIP, pAgentInfo1->m_usAgentPort);
					}

					pAgentInfo1->thirdName="";
					return ;
				}
		
			}
			else//主控坐席离开会，一般情况不会发生，但不能排除,或者认为它是一项错误
			{ 
			 ////////////////////////////////////////////////////////////////////////////////////////////
				CAgentInfo* pAgentInfo2 = CAgentMgr::GetInstance()->GetAgentInfo( pAgentInfo1->thirdName);
				if(pAgentInfo2 == NULL)
				{
					g_Logger.MakeLog( "会议参与坐席不存在" );
					return ;

				}
				else
				{	//pAgentInfo2->conferenceType=4;//表示两座席(本次不启动)
					pAgentInfo2->ConferenceRole=AgentJoin_Active;
					pAgentInfo2->firstLineNo="";//
					SendIPCCMsg_UDP(UserLeaveConference, pAgentInfo2->m_ulAgentIP, pAgentInfo2->m_usAgentPort);
					pAgentInfo1->ClearConferenceParam();
				



				}
			////////////////////////////////////////////////////////////////////////////////////////////
			}

		}
		break;
	case 2://表示一座席两外线，一外离开后改化成，
		{
			if(bManinAgentLeaveLineout==false)//非主控离线-》常见
			{
				if( ipccMessage.leaveUserName==pAgentInfo1->firstLineNo)
				{
					g_Logger.MakeLog( "会议外线firstLineNo离线" );
					pAgentInfo1->firstLineNo="";
				}
				if( ipccMessage.leaveUserName==pAgentInfo1->thirdName)
				{
					g_Logger.MakeLog( "会议外线thirdName离线" );
					pAgentInfo1->thirdName="";			
				}
				//pAgentInfo1->conferenceType=3;//转化成一座席一外线 (本次不启动)
				//向主控坐席发送用户离会消息
				this->SendIPCCMsg_UDP(UserLeaveConference, pAgentInfo1->m_ulAgentIP, pAgentInfo1->m_usAgentPort);
			}
			else////主控坐席离开会，一般情况不会发生，但不能排除
			{
			   pAgentInfo1->ClearConferenceParam();

			
			}
		

		}
		break;
	case 3://表示一座席一外线  (本次不启动)
		{
			/*if(bManinAgentLeaveLineout==false)//非主控离线，常见
			{

				if( ipccMessage.leaveUserName==pAgentInfo1->firstLineNo)
				{
					pAgentInfo1->firstLineNo="";
				}
				if( ipccMessage.leaveUserName==pAgentInfo1->thirdName)
				{
					pAgentInfo1->thirdName="";
				}
				//pAgentInfo->conferenceType=0;？？是否需要改会议类型
				//向主控坐席发送用户离会消息
				this->SendIPCCMsg_UDP(UserLeaveConference, pAgentInfo1->m_ulAgentIP, pAgentInfo1->m_usAgentPort);
			}
			else
			{

				pAgentInfo1->ClearConferenceParam();

			}
			*/

		}
		break;
	case 4 ://表示两外线
		{
			//不需要处理
		}
		break;
	case 5://5表示两座席   (本次不启动)
		{
			/*CAgentInfo* pAgentInfo2 = CAgentMgr::GetInstance()->GetAgentInfo( pAgentInfo1->thirdName);
			if(pAgentInfo2 == NULL)
			{
			g_Logger.MakeLog( "会议参与坐席不存在" );
			return ;

			}
			if(bManinAgentLeaveLineout==false)//非主控离线，常见
			{

			pAgentInfo1->thirdName="";//
			pAgentInfo2->ClearConferenceParam();			
			//向会议主控坐席发送用户离会消息
			SendIPCCMsg_UDP(UserLeaveConference, pAgentInfo1->m_ulAgentIP, pAgentInfo1->m_usAgentPort);

			}
			else//主控离开，罕见
			{
			pAgentInfo1->ClearConferenceParam();
			pAgentInfo2->ConferenceRole=2;
			SendIPCCMsg_UDP(UserLeaveConference, pAgentInfo2->m_ulAgentIP, pAgentInfo2->m_usAgentPort);
			}

			*/
		}
		break;
	default:
		break;
	}

   return ;

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void  CUtility:: PrintConferenceTypeMessage(int nType)
{
   switch(nType)
	{
	case 1: //1表示两座席一外线
		{	
			g_Logger.MakeLog( "1 类型_两座席一外线" );
		}
		break;
	case 2://表示一座席两外线
		{

			g_Logger.MakeLog( "2 类型_座席两外线" );

		}
		break;
	case 3://表示一座席一外线
		{

        g_Logger.MakeLog( "3 类型_座席一外线" );

		}
		break;
	case 4 ://表示两外线
		{
			 g_Logger.MakeLog("4 类型_表示两外线" );

		}
		break;
	case 5://5表示两座席
		{

         g_Logger.MakeLog( "5 类型_两座席" );
		}
		break;
	default:
		break;
	}


}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int  CUtility:: ConferenceAgentHuageup(CAgentInfo* pAgentInfo,unsigned long ip, unsigned short port,string strmsg,bool bsendUStatusserver,bool reLogin)
{
    int nresult=0;	
	if(pAgentInfo==NULL)
	{
		g_Logger.MakeLog("ConferenceAgentHuageup pAgentInfo==NULL");
		return nresult;

	}
	if( pAgentInfo->ConferenceRole!=AgentJoin_UnKnown)//在会议中
	{ 
	  
		{
			//begin  write log//////////////////////////////////////////////////////
			string sLog;
			ostringstream ostream;
			ostream.str("");
			ostream <<"在会议中"<<pAgentInfo->strConferenceID<<";"<< strmsg<< ";agentID=" << pAgentInfo->firstName
				<<";otheragentID=" << pAgentInfo->thirdName
				<<";conferenceType=" << pAgentInfo->conferenceType
				<<";firstPartyLineNo="<<pAgentInfo->firstPartyLineNo
				<<";secondPartyLineNo="<<pAgentInfo->secondPartyLineNo
				<< ";  natip=" << ip
				<< "; natport=" << port;
			sLog = ostream.str();
			g_Logger.MakeLog(sLog);
			//end write log//////////////////////////////////////////////////////////


		}
		switch(pAgentInfo->conferenceType)
		{
		case 1: //1表示两座席一外线		
			{
				if(pAgentInfo->ConferenceRole==AgentJoin_Active)//会议主持Hangeup
				{
					{
						string tstrLog= "退出会议 ,会议ID="+ pAgentInfo->strConferenceID+" ,会议主控先挂, 主控 firstName=" +pAgentInfo->firstName+",第三方="+pAgentInfo->thirdName;
						g_Logger.MakeLog( tstrLog );
					}
					int nConferecneNum=0;
					if(pAgentInfo->firstLineNo !="")
					{
						nConferecneNum+=1;

					}
					if(pAgentInfo->thirdName !="")
					{
					    nConferecneNum+=1;
					
					}
					{

						ostringstream ostream;
						ostream.str("");
						ostream <<"在会议中ID"<<pAgentInfo->strConferenceID<<";在线数="<<nConferecneNum;						
						g_Logger.MakeLog( ostream.str());

					}				   		
					//发送 CallReleased 消息给源座席
					SIPCCMsg_ACD_CallReleased  ACD_CallReleased;
					ACD_CallReleased.SetReleasedParty(pAgentInfo->firstName);//自己
					ACD_CallReleased.bIsPresideAgent=1;
					this->SendIPCCMsg_UDP(ACD_CallReleased, ip,port);	
					{
					
						string sLog;
						ostringstream ostream;
						ostream.str("");
						ostream << strmsg<< ";agentID=" << pAgentInfo->firstName						
							<< ";  natip=" << ip
							<< "; natport=" << port;
						sLog = ostream.str();
						g_Logger.MakeLog(sLog);
					
					
					}
					/////////////////////////////////////////////////////////////////////
					if(bsendUStatusserver)
					{
						if(nConferecneNum !=1)
						{
							SIPCCMsg_Client_AgentHangupCall AgentHangupCall;
							AgentHangupCall.SetAgentID(pAgentInfo->m_sAgentID);
							AgentHangupCall.SetTheOtherParty(pAgentInfo->firstPartyLineNo);
							AgentHangupCall.agentJoinType=AgentJoin_Active;
							unsigned long ip = CUtility::GetInstance()->m_IPCCConfig.UStateServer_IP;
							CUtility::GetInstance()->SendIPCCMsg_UDP(AgentHangupCall, CUtility::GetInstance()->m_IPCCConfig.UStateServer_IP, CUtility::GetInstance()->m_IPCCConfig.UStateServer_Port);
							g_Logger.MakeLog( "主控 SIPCCMsg_Client_AgentHangupCall 到UStatus" );
						}
						else
						{

							SIPCCMsg_Client_ConferenceEnd  ConferenceEnd;
							ConferenceEnd.firstName=pAgentInfo->firstPartyLineNo;
							ConferenceEnd.firstLineNo=pAgentInfo->firstLineNo;
							ConferenceEnd.thirdName=pAgentInfo->secondPartyLineNo;
							CUtility::GetInstance()->SendIPCCMsg_UDP(ConferenceEnd, CUtility::GetInstance()->m_IPCCConfig.UStateServer_IP, CUtility::GetInstance()->m_IPCCConfig.UStateServer_Port);
							g_Logger.MakeLog( " 只剩一条 主控SIPCCMsg_Client_ConferenceEnd  到UStatus" );

						}

					}
					//////////////////////////////////////////////////////////////////////

					if(pAgentInfo->thirdName !="")//确定是两坐席
					{
						CAgentInfo*  pTmpAgentInfo=CAgentMgr::GetInstance()->GetAgentInfo(pAgentInfo->thirdName);
						if(pTmpAgentInfo!=NULL)
						{ 
							//发送参与议
							this->SendIPCCMsg_UDP(ACD_CallReleased, pTmpAgentInfo->m_ulAgentIP, pTmpAgentInfo->m_usAgentPort);
							pTmpAgentInfo->ConferenceRole=AgentJoin_Active;//参与方，改为主控方
							pTmpAgentInfo->firstName=pAgentInfo->thirdName;//角色转化
							pTmpAgentInfo->thirdName="";
							pTmpAgentInfo->firstPartyLineNo=pAgentInfo->secondPartyLineNo;
							pTmpAgentInfo->secondPartyLineNo="";
							{

								string sLog;
								ostringstream ostream;
								ostream.str("");
								ostream << strmsg<< ";agentID=" << pAgentInfo->firstName						
									<< ";  natip=" << pTmpAgentInfo->m_ulAgentIP
									<< "; natport=" << pTmpAgentInfo->m_usAgentPort
								<<";firstPartyLineNo="<<pAgentInfo->firstPartyLineNo
				                 <<";secondPartyLineNo="<<pAgentInfo->secondPartyLineNo;
								sLog = ostream.str();
								g_Logger.MakeLog(sLog);


							}


						}

					}
					else
					{
						string tstrLog= "AgentLogout 会议挂机，无参与方 ,会议ID="+pAgentInfo->strConferenceID+",anent="+pAgentInfo->thirdName;
						g_Logger.MakeLog( tstrLog );


					}
					pAgentInfo->ClearConferenceParam();
					return 1;

				}
				else//会议参与Hangeup
				{

					int nConferecneNum=0;
					if(pAgentInfo->firstLineNo !="")
					{
						nConferecneNum+=1;

					}
					if(pAgentInfo->firstName !="")
					{
					    nConferecneNum+=1;
					
					}
                   
					{

						ostringstream ostream;
						ostream.str("");
						ostream <<"在会议中ID"<<pAgentInfo->strConferenceID<<";在线数="<<nConferecneNum;						
						g_Logger.MakeLog( ostream.str());

					}			
					if(bsendUStatusserver)
					{
						if(nConferecneNum != 1)
						{
							SIPCCMsg_Client_AgentHangupCall AgentHangupCall;
							AgentHangupCall.SetAgentID(pAgentInfo->m_sAgentID);
							AgentHangupCall.SetTheOtherParty(pAgentInfo->secondPartyLineNo);
							AgentHangupCall.agentJoinType=AgentJoin_Passive;						
							CUtility::GetInstance()->SendIPCCMsg_UDP(AgentHangupCall, CUtility::GetInstance()->m_IPCCConfig.UStateServer_IP, CUtility::GetInstance()->m_IPCCConfig.UStateServer_Port);

							g_Logger.MakeLog( "参与 ，SIPCCMsg_Client_AgentHangupCall 到UStatus" );
						}
						else
						{

							SIPCCMsg_Client_ConferenceEnd  ConferenceEnd;
							ConferenceEnd.firstName=pAgentInfo->firstPartyLineNo;
							ConferenceEnd.firstLineNo=pAgentInfo->firstLineNo;
							ConferenceEnd.thirdName=pAgentInfo->secondPartyLineNo;
							CUtility::GetInstance()->SendIPCCMsg_UDP(ConferenceEnd, CUtility::GetInstance()->m_IPCCConfig.UStateServer_IP, CUtility::GetInstance()->m_IPCCConfig.UStateServer_Port);
							g_Logger.MakeLog( "只剩一条 主控SIPCCMsg_Client_ConferenceEnd  到UStatus" );


						}

					}
					///
					SIPCCMsg_ACD_CallReleased ACD_CallReleased;
					ACD_CallReleased.SetReleasedParty(pAgentInfo->thirdName);
					ACD_CallReleased.bIsPresideAgent=0;
					this->SendIPCCMsg_UDP(ACD_CallReleased, ip, port);	
					CAgentInfo*  pTmpAgentInfo=CAgentMgr::GetInstance()->GetAgentInfo(pAgentInfo->firstName);
					if(pTmpAgentInfo!=NULL)
					{ 
						pTmpAgentInfo->thirdName="";
						string tstrLog= "退出会议 ,会议ID="+pTmpAgentInfo->strConferenceID+",会议参与先挂, 主控 firstName="+pAgentInfo->firstName+",第三方="+pAgentInfo->thirdName;
						g_Logger.MakeLog( tstrLog );

						//发送 CallReleased 消息给主控座席	
						this->SendIPCCMsg_UDP(ACD_CallReleased, pTmpAgentInfo->m_ulAgentIP, pTmpAgentInfo->m_usAgentPort);
						///是否需要，详细处理，对处报表或监控
                        

					}
					else
					{
						g_Logger.MakeLog( pAgentInfo->firstName );
						g_Logger.MakeLog( "OnAgentHangupCall 会议主控不存在" );

					}

					//清除会议记录
					pAgentInfo->ClearConferenceParam();
					return 1;

				}

			}
			break;
		case 2:///2表示一座席两外线
			{

				int nConferecneNum=0;
				if(pAgentInfo->firstLineNo !="")
				{
					nConferecneNum+=1;

				}
				if(pAgentInfo->thirdName !="")
				{
					nConferecneNum+=1;

				}
				{

					ostringstream ostream;
					ostream.str("");
					ostream <<"在会议中ID"<<pAgentInfo->strConferenceID<<";在线数="<<nConferecneNum;						
					g_Logger.MakeLog( ostream.str());

				}			
				if(reLogin)
				{

					g_Logger.MakeLog( "2 一座席两外线 坐席重登录" );
					SIPCCMsg_ACD_CallReleased  ACD_CallReleased;
					ACD_CallReleased.SetReleasedParty(pAgentInfo->firstName);//自己
					ACD_CallReleased.bIsPresideAgent=1;
					this->SendIPCCMsg_UDP(ACD_CallReleased, ip,port);	
					{

						string sLog;
						ostringstream ostream;
						ostream.str("");
						ostream << strmsg<< ";agentID=" << pAgentInfo->firstName						
							<< ";  natip=" << ip
							<< "; natport=" << port;
						sLog = ostream.str();
						g_Logger.MakeLog(sLog);


					}
				
				}           

			   /////////////////////////////////////////////////////////////////////
				if(bsendUStatusserver)
				{
					if(nConferecneNum !=1)
					{

						SIPCCMsg_Client_AgentHangupCall AgentHangupCall;
						AgentHangupCall.SetAgentID(pAgentInfo->m_sAgentID);
						AgentHangupCall.SetTheOtherParty(pAgentInfo->firstPartyLineNo);
						AgentHangupCall.agentJoinType=AgentJoin_Active;
						unsigned long ip = CUtility::GetInstance()->m_IPCCConfig.UStateServer_IP;
						CUtility::GetInstance()->SendIPCCMsg_UDP(AgentHangupCall, CUtility::GetInstance()->m_IPCCConfig.UStateServer_IP, CUtility::GetInstance()->m_IPCCConfig.UStateServer_Port);
						g_Logger.MakeLog( "2 一坐席两外线主控SIPCCMsg_Client_AgentHangupCall 到UStatus" );
					}
					else
					{

						SIPCCMsg_Client_ConferenceEnd  ConferenceEnd;
						ConferenceEnd.firstName=pAgentInfo->firstPartyLineNo;
						ConferenceEnd.firstLineNo=pAgentInfo->firstLineNo;
						ConferenceEnd.thirdName=pAgentInfo->secondPartyLineNo;
						CUtility::GetInstance()->SendIPCCMsg_UDP(ConferenceEnd, CUtility::GetInstance()->m_IPCCConfig.UStateServer_IP, CUtility::GetInstance()->m_IPCCConfig.UStateServer_Port);
						g_Logger.MakeLog( " 只剩一条 主控 SIPCCMsg_Client_ConferenceEnd  到UStatus" );



					}
				}
				////////////////////////////////////////////////////////////////////
				pAgentInfo->ClearConferenceParam();
				return 1;
			}
			break;
		case 3://，3表示一座席一外线
			{
				// return;////返回还是继续执行？？？
			}
			break;
		case 4://4表示两外线
			{
				//return ;//返回还是继续执行？？？

			}
		case 5: //5表示两座席
			{
				//return ;//返回还是继续执行？？？

			}
			break;		
		default:
			break;


		}


	}	

 return nresult;

}

//todo
//////////void CUtility::OnServerHeartBeatConf(SIPCCMsg_OneKeySwitch_ServerHeartBeatConf& msg, unsigned long ip, unsigned short port)
//////////{
//////////
//////////
//////////	if(msg.sendtime-m_HeartBeatTimeoutime_A<0 && msg.sendtime-m_HeartBeatTimeoutime_B<0)
//////////	{
//////////
//////////		return ;
//////////
//////////	}
//////////
//////////	bool bChangeHeartBeatServer = false;
//////////	if (m_HeartBeatServerIp1 == ip && m_HeartBeatServerPort1 == port)
//////////	{
//////////
//////////
//////////		m_HeartBeatServerIp1 = 0;
//////////		m_HeartBeatServerPort1 = 0;
//////////
//////////		bChangeHeartBeatServer = true;
//////////		m_HeartBeatTimeoutime_A=gettickcount();
//////////
//////////	}
//////////	else if (m_HeartBeatServerIp2 == ip && m_HeartBeatServerPort2 == port)
//////////	{
//////////
//////////
//////////		m_HeartBeatServerIp2 = 0;
//////////		m_HeartBeatServerPort2 = 0;
//////////		m_HeartBeatTimeoutime_B=gettickcount();
//////////
//////////
//////////		bChangeHeartBeatServer = true;
//////////	}
//////////
//////////	if ( bChangeHeartBeatServer )
//////////	{
//////////		ostringstream ostream;
//////////		string sLog = "";
//////////		ostream.str("");
//////////		ostream << "SIPCCMsg_OneKeySwitch_ServerHeartBeatConf : " 
//////////			<< "resultCode=" << msg.resultCode
//////////			<< "sendtime=" << msg.sendtime
//////////			<< ", HeartBeatServerIp=" << ip
//////////			<< ", HeartBeatServerPort=" << port;
//////////		sLog = ostream.str();			
//////////		g_Logger.MakeLog(sLog);
//////////		unsigned long lcurtime=gettickcount();
//////////		m_bIsBeginHBTimer = true;
//////////		GetHeartBeatServer(ip, port);
//////////	}
//////////	
//////////}

void CUtility::OnAddSkillGroup(SIPCCMsg_SysManage_AddSkillGroup* addSkillGroup, unsigned long ip, unsigned short port)
{
	string sTempStr = addSkillGroup->SGId;
	string sCorp = "";
	string sSkillGroup = "";

	if(!SplitCorpAndDepart(sTempStr, sCorp, sSkillGroup))
		return;

	if(sCorp.length() <= 0 || sSkillGroup.length() <= 0)
		return;

	CCorporationInfo* corp;
	corp = CCorporationMgr::GetInstance()->GetCorporation(sCorp);
	if(corp == NULL)
	{
		CCorporationMgr::GetInstance()->AddCorporationInfo(sCorp, addSkillGroup->managerid);
		corp = CCorporationMgr::GetInstance()->GetCorporation(sCorp);
		if (NULL == corp)
		{
			return;
		}
	}

	corp->AddSkillGroup(addSkillGroup->SGName, sSkillGroup);
	corp->AddTXTSkillGroup(addSkillGroup->SGName, sSkillGroup, addSkillGroup->queuingAlgorithm);
}

void CUtility::OnDelSkillGroup(SIPCCMsg_SysManage_DelSkillGroup* delSkillGroup, unsigned long ip, unsigned short port)
{
	string sTempStr = delSkillGroup->SGId;
	string sCorp = "";
	string sSkillGroup = "";

	if(!SplitCorpAndDepart(sTempStr, sCorp, sSkillGroup))
		return;

	CCorporationInfo* corp;
	corp = CCorporationMgr::GetInstance()->GetCorporation(sCorp);

	if (corp != NULL)
	{
		corp->DelSkillGroup(sSkillGroup);
		corp->DelTXTSkillGroup(sSkillGroup);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
string   CUtility::GetMsgLogTransContext(SIPCCMessage* pMsg)
{
	CIpccMsgLogTrans ipccmsglogtrans;
	string slogtrans=ipccmsglogtrans.MsgToLog(pMsg);
	return  slogtrans;
		
}

string   CUtility::GetMsgLogTransContext(SIPCCMessage* pMsg, int iPrefix)
{
	CIpccMsgLogTrans ipccmsglogtrans;
	string slogtrans=ipccmsglogtrans.MsgToLog(pMsg);

	switch( iPrefix )
	{
	case 0:
		{
			slogtrans = m_sLogTransSign0+slogtrans;
		}
		break;
	case 1:
		{
			slogtrans = m_sLogTransSign1+slogtrans;
		}
		break;
	case 2:
		{
			slogtrans = m_sLogTransSign2+slogtrans;
		}
		break;
	case 3:
		{
			slogtrans = m_sLogTransSign3+slogtrans;
		}
		break;
	default:
		break;
	}

	g_Logger.MakeLog(slogtrans);

	return  slogtrans;

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string  CUtility:: ConvertNatIP2HostIP(long lip)
{
    using namespace boost::asio::ip;
    string strip=address_v4( ntohl(lip)).to_string();
	return   strip;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string  CUtility:: ConvertNatPort2HostPort(int nport)
{
    using namespace boost::asio::ip;
   string strport= boost::lexical_cast<string>(ntohs(nport));
   return strport;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////void CUtility::SendRegMSG2RegServer()
////////////////{
////////////////	m_tRegisterServerA = gettickcount();
////////////////	m_tRegisterServerB = m_tRegisterServerA;
////////////////	if (this->m_IPCCConfig.RegisterServerMode != 0)
////////////////	{
////////////////		if ( this->m_IPCCConfig.RegisterServerA_IP != 0 )
////////////////		{
////////////////			//发送 SIPCCMsg_OneKeySwitch_ServiceRegister 的消息给注册中心A
////////////////			SIPCCMsg_OneKeySwitch_ServiceRegister ACDRegister;
////////////////			ACDRegister.ServerName = "WIPCC_ACDServer";
////////////////			ACDRegister.ServerType = EST_ACDServer;
////////////////			ACDRegister.wanIP = this->m_IPCCConfig.localIP;
////////////////			ACDRegister.wanPort = ConvertPort(ServerInfoDef::Port_ACDServer);
////////////////			this->SendIPCCMsg_UDP(
////////////////				ACDRegister,
////////////////				this->m_IPCCConfig.RegisterServerA_IP,
////////////////				this->m_IPCCConfig.RegisterServerA_Port
////////////////				);
////////////////
////////////////			ostringstream os;
////////////////			os.str("");
////////////////			os << "Repeat, SIPCCMsg_OneKeySwitch_ServiceRegister, RegisterServerA_IP="
////////////////				<< this->m_IPCCConfig.RegisterServerA_IP
////////////////				<< "; RegisterServerA_Port=" << this->m_IPCCConfig.RegisterServerA_Port;
////////////////			g_Logger.MakeLog(os.str());
////////////////
////////////////			//m_tRegisterServerA = GetTickCount();
////////////////		}
////////////////
////////////////		if ( this->m_IPCCConfig.RegisterServerB_IP != 0 )
////////////////		{
////////////////			//发送 SIPCCMsg_OneKeySwitch_ServiceRegister 的消息给注册中心B
////////////////			SIPCCMsg_OneKeySwitch_ServiceRegister ACDRegister;
////////////////			ACDRegister.ServerName = "WIPCC_ACDServer";
////////////////			ACDRegister.ServerType = EST_ACDServer;
////////////////			ACDRegister.wanIP = this->m_IPCCConfig.localIP;
////////////////			ACDRegister.wanPort = ConvertPort(ServerInfoDef::Port_ACDServer);
////////////////			this->SendIPCCMsg_UDP(
////////////////				ACDRegister,
////////////////				this->m_IPCCConfig.RegisterServerB_IP,
////////////////				this->m_IPCCConfig.RegisterServerB_Port
////////////////				);
////////////////
////////////////			ostringstream os;
////////////////			os.str("");
////////////////			os << "Repeat, SIPCCMsg_OneKeySwitch_ServiceRegister, RegisterServerB_IP="
////////////////				<< this->m_IPCCConfig.RegisterServerB_IP
////////////////				<< "; RegisterServerB_Port=" << this->m_IPCCConfig.RegisterServerB_Port;
////////////////		   g_Logger.MakeLog(os.str());
////////////////
////////////////			//m_tRegisterServerB = GetTickCount();
////////////////		}
////////////////	}
////////////////}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//坐席状态描述
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string  CUtility:: ShowAgentState(int state)
{
  
	string strAgentstate="无效状态";
	switch(state)
	{
	     case Agent_NoLogin:
			 strAgentstate="未登录";
		      break;
		 case Agent_Waiting:
			 strAgentstate="等待";
			 break;
		 case Agent_Ringing:
			 strAgentstate="震铃";
			 break;
		 case Agent_Parking:
			  strAgentstate="小休";
			 break;
		 case Agent_Talking:
			 strAgentstate="通话";
			 break;
		 case Agent_OpTalking:
			 strAgentstate="被动席间通话";
			 break;
		 case Agent_ForeProcess:
			 strAgentstate="话前处理";
			 break;
		 case Agent_BackProcess:
			 strAgentstate="话后处理";
			 break;
		 case Agent_Holding:
			 strAgentstate="保持";
			 break;
		 case Agent_Held:
			 strAgentstate="被保持";
			 break;

		 case Agent_Dialing:
			 strAgentstate="拨号";
			 break;
		 case Agent_DialUnderHold:
			 strAgentstate="保持下拨号";
			 break;
		 case Agent_TalkUnderHold:
			 strAgentstate="保持下通话";
			 break;
		 case Agent_ConnSucc_NoSend:
			 strAgentstate="Agent_ConnSucc_NoSend";
			 break;

		 case Agent_PreDial:
			 strAgentstate="预拨号";
			 break;
		 case Agent_ConferencePreside:
			 strAgentstate="会议主持";
			 break;
		 case Agent_ConferenceJoin:
			 strAgentstate="会议参与者";
			 break;
	}
	
	return  strAgentstate;
}

void CUtility::LoadTestContext()
{
	bool bIsReLoad = false;
	ostringstream ostream;

	//发送 LoadDepartments 的消息给维护程序
	SIPCCMsg_ACD_LoadDepartments loadDepartments;
	loadDepartments.serverIP = this->m_IPCCConfig.localserverIP;
	loadDepartments.acdserverID = this->m_IPCCConfig.ServiceID;
	this->SendIPCCMsg_UDP(
		loadDepartments,
		this->m_IPCCConfig.SysManageServer_IP,
		ConvertPort(ServerInfoDef::Port_SysManageServer)
		);

	this->m_bEgnRun = true;

	boost::thread th_SockEnginet(CUtility::ThreadSockEngine, this);

	boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
}

void CUtility::OnPhoneSearch_SMSInfoConf(SIPCCMsg_PhoneSearch_SMSInfoConf *smsInfoConf, unsigned long ip, unsigned short port)
{
	return;
}

void CUtility::OnFileTransfer_UploadRequest(SIPCCMsg_FileTransfer_UploadRequest *pMsg, unsigned long ip, unsigned short port)
{
	string sAgentID = pMsg->agentID;

	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(sAgentID);
	if (pAgentInfo == NULL)
	{
		//log
		string sLog;
		ostringstream ostream;
		ostream.str("");
		ostream << "FileTransfer_UploadRequest:" << "agentID=" << sAgentID << "; Agent Not Find";
		sLog = ostream.str();
		g_Logger.MakeLog( sLog);

		return;
	}

	SIPCCMsg_FileTransfer_UploadRequest uploadRequest;
	uploadRequest.agentID = sAgentID;
	uploadRequest.description = pMsg->description;
	uploadRequest.address = pMsg->address;
	this->SendIPCCMsg_UDP(uploadRequest, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//bool CUtility::LoadRedisConfig()
//{
//	//MyXML::XMLResults res;
//	string sConfigFile = (GetAppPath() / "RedisConfig.xml").string();
//
//	MyXML::XMLNode xmlRedisServer = MyXML::XMLNode::openFileHelper(sConfigFile.c_str(), "RedisServer");
//	if(xmlRedisServer.isEmpty())
//	{
//		return false;
//	}
//
//	XMLCSTR pszRedisConfig = NULL;
//	int iRedisType = 0;
//	string sRedisSvrIP = "";
//	int iRedisSvrPort = 0;
//	int iRedisSvrID = -1;
//	int iRedisCount = xmlRedisServer.nChildNode( "Redis" );
//	for ( int i=0; i<iRedisCount; i++ )
//	{
//		MyXML::XMLNode xmlRedis = xmlRedisServer.getChildNode( "Redis", i );
//		if(xmlRedis.isEmpty())
//		{
//			return false;
//		}
//
//		pszRedisConfig = xmlRedis.getAttribute("ServerType");
//		if (NULL == pszRedisConfig)
//		{
//			iRedisType = 0;
//		}
//		else
//		{
//			iRedisType = atoi(pszRedisConfig);
//		}
//
//		pszRedisConfig = xmlRedis.getAttribute("ServerIP");
//		if (NULL == pszRedisConfig)
//		{
//			sRedisSvrIP = "";
//		}
//		else
//		{
//			sRedisSvrIP = string(pszRedisConfig);
//		}
//		pszRedisConfig = xmlRedis.getAttribute("ServerPort");
//		if (NULL == pszRedisConfig)
//		{
//			iRedisSvrPort= 6379;
//		}
//		else
//		{
//			iRedisSvrPort= atoi(pszRedisConfig);
//		}
//		pszRedisConfig = xmlRedis.getAttribute("ServerID");
//		if (NULL == pszRedisConfig || 0 == strcmp(pszRedisConfig, ""))
//		{
//			iRedisSvrID = 0;
//		}
//		else
//		{
//			iRedisSvrID = atoi(pszRedisConfig);
//		}
//
//		switch ( iRedisType )
//		{
//		case 1:
//			{
//				m_RedisCorp.m_strredisip = sRedisSvrIP;
//				m_RedisCorp.m_nredisport = iRedisSvrPort;
//				m_RedisCorp.m_nrdisidex = iRedisSvrID;
//
//				m_RedisCorp.m_redisScriptMap["GetCorpInfo"] = 
//					"local corpInfo "
//					"corpInfo = redis.call(\"HGET\", \"CorpInfo\", KEYS[1]) "
//					"if not(corpInfo) then return -1 end "
//					"return corpInfo ";
//
//				m_RedisCorp.m_redisScriptMap["SetCorpInfo"] = 
//					"local status = redis.call(\"HSET\", \"CorpInfo\", KEYS[1], ARGV[1]) "
//					"return status ";
//
//				m_RedisCorp.m_redisScriptMap["GetDeptInfo"] = 
//					"local content = {} "
//					"content.corpInfo = redis.call(\"HGET\", \"CorpInfo\", KEYS[1]) "
//					"if not(content.corpInfo) then return -1 end "
//					"content.key = ARGV[1] .. \"@DeptInfo\" "
//					"content.deptInfo = redis.call(\"HGET\", content.key, KEYS[2]) "
//					"if not(content.deptInfo) then return -2 end "
//					"local result = {} "
//					"result[1] = content.corpInfo "
//					"result[2] = content.deptInfo "
//					"return result ";
//
//				m_RedisCorp.m_redisScriptMap["SetDeptInfo"] = 
//					"local content = {} "
//					"content.key = ARGV[1] .. \"@DeptInfo\" "
//					"local status = redis.call(\"HSET\", content.key, KEYS[1], ARGV[2]) "
//					"return status ";
//
//				m_RedisCorp.m_redisScriptMap["GetSkillInfo"] = 
//					"local content = {} "
//					"content.corpInfo = redis.call(\"HGET\", \"CorpInfo\", KEYS[1]) "
//					"if not(content.corpInfo) then return -1 end "
//					"content.key = ARGV[1] .. \"@SkillInfo\" "
//					"content.skillInfo = redis.call(\"HGET\", content.key, KEYS[2]) "
//					"if not(content.skillInfo) then return -2 end "
//					"local result = {} "
//					"result[1] = content.corpInfo "
//					"result[2] = content.skillInfo "
//					"return result ";
//
//				m_RedisCorp.m_redisScriptMap["SetSkillInfo"] = 
//					"local content = {} "
//					"content.key = ARGV[1] .. \"@SkillInfo\" "
//					"local status = redis.call(\"HSET\", content.key, KEYS[1], ARGV[2]) "
//					"return status ";
//				
//				m_RedisCorp.m_redisScriptMap["GetAgentRelatedInfo"] = 
//					"do "
//					"local function Split(szFullString, szSeparator) "
//					"if not(szFullString) or szFullString == \"\" then return nil end "
//					"if not(szSeparator) or szSeparator == \"\" then return szFullString end "
//					"local nFindStartIndex = 1 "
//					"local nSplitIndex = 1 "
//					"local nSplitArray = {} "
//					"while true do "
//					"local nFindLastIndex = string.find(szFullString, szSeparator, nFindStartIndex) "
//					"if not nFindLastIndex then "
//					"nSplitArray[nSplitIndex] = string.sub(szFullString, nFindStartIndex, string.len(szFullString)) "
//					"break end "
//					"nSplitArray[nSplitIndex] = string.sub(szFullString, nFindStartIndex, nFindLastIndex - 1) "
//					"nFindStartIndex = nFindLastIndex + string.len(szSeparator) "
//					"nSplitIndex = nSplitIndex + 1 "
//					"end "
//					"return nSplitArray "
//					"end "
//					"local TempSp = Split(ARGV[1], \";\") "
//					"local content = {} "
//					"local result = {} "
//					"content.corpInfo = redis.call(\"HGET\", \"CorpInfo\", KEYS[1]) "
//					"if not(content.corpInfo) then return -2 end "
//					"result[1] = content.corpInfo "
//					"local TempSp2 = Split(content.corpInfo, \";\") "
//					"content.key1 = TempSp2[2] .. \"@DeptInfo\" "
//					"content.deptInfo = redis.call(\"HGET\", content.key1, KEYS[2]) "
//					"if not(content.deptInfo) then return -3 end "
//					"result[2] = content.deptInfo "
//					"content.key2 = TempSp2[2] .. \"@SkillInfo\" "
//					"content.index = 3 "
//					"for key, value in ipairs(TempSp) do "
//					"content.skillInfo = redis.call(\"HGET\", content.key2, value) "
//					"if not(content.skillInfo) then content.skillInfo = -3 end "
//					"result[content.index] = content.skillInfo "
//					"content.index = content.index + 1 "
//					"end "
//					"return result "
//					"end ";
//
//				bool bloadResult = m_RedisCorp.InitLoadSrcipt();
//				if ( !bloadResult )
//				{
//					return false;
//				}
//			}
//			break;
//		case 2:
//			{
//				m_RedisUser.m_strredisip = sRedisSvrIP;
//				m_RedisUser.m_nredisport = iRedisSvrPort;
//				m_RedisUser.m_nrdisidex = iRedisSvrID;
//			}
//			break;
//		case 3:
//			{
//				m_RedisAgent.m_strredisip = sRedisSvrIP;
//				m_RedisAgent.m_nredisport = iRedisSvrPort;
//				m_RedisAgent.m_nrdisidex = iRedisSvrID;
//			}
//			break;
//		default:
//			break;
//		}
//	}
//	return true;
//}

void CUtility::OnSysManage_AgentInfoChange(SIPCCMsg_SysManage_AgentInfoChange* pMsg, unsigned long ip, unsigned short port)
{
	//获取座席
	CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(pMsg->agentName);
	if(pAgentInfo == NULL)
	{
		ostringstream ostream;
		ostream << "=+= Agent do not exist =+= :" << "agentID=" << pMsg->agentName;
		string sLog = ostream.str();
		g_Logger.MakeLog( sLog );

		return;
	}

	//置座席状态为 Agent_NoLogin
	//CAgentMgr::GetInstance()->SetAgentState(pMsg->agentName, Agent_NoLogin);

	//返回 AgentLogoutConf消息
	SIPCCMsg_ACD_ForceLogout agentLogout;
	agentLogout.agentName = pMsg->agentName;

	switch ( pMsg->cause ) 
	{
	case 1:
		{
			agentLogout.cause = ForceLogoutCause_ChangeLoginType;
		}
		break;
	case 2:
		{
			agentLogout.cause = ForceLogoutCause_SwitchDept;
		}
		break;
	case 3:
		{
			agentLogout.cause = ForceLogoutCause_DelAgent;
		}
		break;
	case 4:
		{
			agentLogout.cause = ForceLogoutCause_ChangeAgentInfo;
		}
		break;
	default:
		break;
	}

	this->SendIPCCMsg_UDP(agentLogout, pAgentInfo->m_ulAgentIP, pAgentInfo->m_usAgentPort);

	//从 agentMap 中删除座席
	//CAgentMgr::GetInstance()->DelAgent(pMsg->agentName);
}

void CUtility::OnCProxy_CMakeCall(SIPCCMsg_CProxy_CMakeCall* cclientMakeCall, unsigned long ip, unsigned short port)
{

	switch (cclientMakeCall->callType)
	{
	case E_MESSAGE_CALL:case E_WEBCHAT_CALL:case E_EMAIL_CALL:
	{
		return OnMakeMessageCall(*cclientMakeCall, ip, port);
	}
	case E_NORMAL_CALL:case E_VIDEO_CALL:
	{
		if (cclientMakeCall->targetType == CallParty_Agent)
		{
			func_CClentCallAgent(cclientMakeCall, ip, port);
		}
		else if (cclientMakeCall->targetType == CallParty_Department)
		{
			func_CClientCallDepartment(cclientMakeCall, ip, port);
		}
		return;
	}
		
	default:
		break;
		
	}

	ostringstream os;
	string sTemp("");
	os << "OnCProxy_CMakeCall: unknown calltype =  " << cclientMakeCall->callType;
	sTemp = os.str();
	g_Logger.MakeLog(sTemp);


	/*
	if (cclientMakeCall->callType == E_MESSAGE_CALL 
		|| cclientMakeCall->callType == E_WEBCHAT_CALL
		|| cclientMakeCall->callType == E_WEBCHAT_CALL)
	{
		return OnMakeMessageCall(*cclientMakeCall, ip, port);
	}

	if (cclientMakeCall->targetType == CallParty_Agent)
	{
		func_CClentCallAgent(cclientMakeCall, ip, port);
	}
	else if (cclientMakeCall->targetType == CallParty_Department)
	{
		func_CClientCallDepartment(cclientMakeCall, ip, port);
	}
	*/
}

void CUtility::func_CClentCallAgent(SIPCCMsg_CProxy_CMakeCall *cclientMakeCall, unsigned long ip, unsigned short port)
{
	SIPCCMsg_ACD_AgentMakeCallConf agentMakeCallConf;
	agentMakeCallConf.callingAgentID = cclientMakeCall->account;
	agentMakeCallConf.callID = cclientMakeCall->callID;
	agentMakeCallConf.callType = cclientMakeCall->callType;

	CAgentInfo* pAgentInfo_called = CAgentMgr::GetInstance()->GetAgentInfo(cclientMakeCall->targetName);

	if (pAgentInfo_called == NULL)
	{
		//座席未登录
		agentMakeCallConf.callingState = AgentCalling_AgentNoLogin;	
		this->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);

		return;
	}

	if (pAgentInfo_called->m_iLoginType != LoginType_OnlyWeb)
	{
		agentMakeCallConf.callingState = AgentCalling_AgentNoLogin;	
		this->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);

		return;
	}

	//查看被叫座席状态
	if(pAgentInfo_called->m_iCurState != Agent_Waiting)
	{
		//座席忙
		agentMakeCallConf.callingState = AgentCalling_AgentBusy;
		this->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);

		return;
	}

	string sAccount_caller = "CClient_" + cclientMakeCall->account;

    pAgentInfo_called->m_bCallFromVDN = false;
    pAgentInfo_called->m_bCallFromQueue = false;
    pAgentInfo_called->SetCallID(cclientMakeCall->callID);
	pAgentInfo_called->bCallin = true;
	pAgentInfo_called->bInnerCall = false;
	pAgentInfo_called->bPSTNCall = false;
	pAgentInfo_called->m_sTheOtherAgentID = sAccount_caller;

	pAgentInfo_called->SetState(Agent_Ringing);

	agentMakeCallConf.callingState = AgentCalling_AgentRinging;
	agentMakeCallConf.SetCalledParty(pAgentInfo_called->m_sAgentID);
	agentMakeCallConf.lanIP = pAgentInfo_called->m_ulLanIP;
	agentMakeCallConf.lanMainSockPort = pAgentInfo_called->m_usLanMainSockPort;
	agentMakeCallConf.callingAgentID = sAccount_caller;
	agentMakeCallConf.callID = cclientMakeCall->callID;
	this->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);

	CCClientInfo* pCClient_caller = CCClientMgr::GetInstance()->AddCClient(
		sAccount_caller, 
		ip, 
		port, 
		cclientMakeCall->targetName, 
		cclientMakeCall->uuiDate,
		cclientMakeCall->callID, 
		cclientMakeCall->audioStreamDescription,
		cclientMakeCall->callType);
	CCClientMgr::GetInstance()->SetResponseAgentID(pCClient_caller->m_sAccount, pAgentInfo_called->m_sAgentID, __FILE__, __LINE__);

	//发送 Delivered
	boost::shared_ptr<SIPCCMsg_ACD_CallDelivered> callDelivered(new SIPCCMsg_ACD_CallDelivered);
	callDelivered->SetCallerName(sAccount_caller);
	callDelivered->callerType = CallParty_CClient;
	callDelivered->SetCallID(cclientMakeCall->callID);
	callDelivered->SetUUIData(cclientMakeCall->uuiDate);
	callDelivered->audioStreamDescription = cclientMakeCall->audioStreamDescription;
	callDelivered->callType = cclientMakeCall->callType;
	callDelivered->mediaRequest = cclientMakeCall->mediaRequest;

	if (pAgentInfo_called->IsAllowedPush())
	{
		pAgentInfo_called->addListCallDelivered(callDelivered);
		Push_Message(pAgentInfo_called->m_sAgentID,E_NORMAL_CALL,callDelivered->GetCallID());
	}

	ostringstream os;
	string sTemp("");
	os << "func_AgentCallAgent mediaInfo:" << callDelivered->audioStreamDescription.to_str();
	sTemp = os.str();
	g_Logger.MakeLog(sTemp);

	this->SendIPCCMsg_UDP(*callDelivered.get(), pAgentInfo_called->m_ulAgentIP, pAgentInfo_called->m_usAgentPort);


}

void CUtility::func_CClientCallDepartment(SIPCCMsg_CProxy_CMakeCall *cclientMakeCall, unsigned long ip, unsigned short port)
{
    //1、conf通用属性设置
    SIPCCMsg_ACD_AgentMakeCallConf agentMakeCallConf;
    agentMakeCallConf.SetCalledParty(cclientMakeCall->targetName);
    agentMakeCallConf.callingAgentID = cclientMakeCall->account;
    agentMakeCallConf.callID = cclientMakeCall->callID;
    agentMakeCallConf.callType = cclientMakeCall->callType;

    //2、查找呼叫目标
    bool bCallSG = true;
    CSkillGroupInfo* pSkillGroupInfo = NULL;
    CDepartmentInfo* pDepartmentInfo = NULL;

    pSkillGroupInfo = CCorporationMgr::GetInstance()->GetSkillGroupInfo(cclientMakeCall->targetName);
    if (pSkillGroupInfo == NULL) {
        bCallSG = false;

        pDepartmentInfo = CCorporationMgr::GetInstance()->GetDepartmentInfo(cclientMakeCall->targetName);
        if (pDepartmentInfo == NULL) {
            agentMakeCallConf.callingState = AgentCalling_DepartmentInexistence;
            this->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);
            return;
        }
    }

    //3、保存主叫信息
	CCClientInfo* pCClient_caller = CCClientMgr::GetInstance()->AddCClient(
		cclientMakeCall->account, 
		ip, 
		port, 
		cclientMakeCall->targetName, 
		cclientMakeCall->uuiDate,
		cclientMakeCall->callID, 
		cclientMakeCall->audioStreamDescription,
		cclientMakeCall->callType);

	pCClient_caller->setMediaRequest(cclientMakeCall->mediaRequest);

    //4、组合dn消息
    SIPCCMsg_ACD_CallEnterVDN CallEnterVDN;
    if (bCallSG) {
        CallEnterVDN.vdnName = pSkillGroupInfo->m_sSkillGroupid;
        CallEnterVDN.vdnNo = pSkillGroupInfo->m_sSkillGroupName;
        CallEnterVDN.managerid = pSkillGroupInfo->m_sManagerid;
        CallEnterVDN.SkillGroupID = pSkillGroupInfo->m_sSkillGroupid;
        CallEnterVDN.SkillGroupName = pSkillGroupInfo->m_sSkillGroupName;
    }
    else {
        CallEnterVDN.vdnName = pDepartmentInfo->m_sDepartmentName;
        CallEnterVDN.vdnNo = pDepartmentInfo->m_sDepartmentName;
        CallEnterVDN.managerid = pDepartmentInfo->m_sManagerid;
        CallEnterVDN.deptid = pDepartmentInfo->m_sDeptid;
    }

    CallEnterVDN.callerID = cclientMakeCall->account;
    CallEnterVDN.calleeID = pCClient_caller->m_sCalledDepartment;
    CallEnterVDN.callType = CalledPartyType_Department;
    CallEnterVDN.enterTime = CLog::GetLocalTime();
    CallEnterVDN.callID = pCClient_caller->m_CallID_sCallIn;
    CallEnterVDN.corpName = pCClient_caller->m_sCorpName;
    CallEnterVDN.callSrc = "";
    CallEnterVDN.extDevID = "";
    CallEnterVDN.status = 0;
    CallEnterVDN.bInnerCall = 0;
    CallEnterVDN.bIntoQueue = 0;
    CallEnterVDN.CurQueueNum = 0;


    //5、分配呼叫（结果：分配到座席，或排队）
    //0错误, 1呼叫成功分配， 2排队
    pair<int, boost::any> deliverState =
        (pDepartmentInfo == NULL) ? pSkillGroupInfo->CallIn(pCClient_caller->m_sAccount, CallParty_CClient)
                                  : pDepartmentInfo->CallIn(pCClient_caller->m_sAccount, CallParty_CClient);
    pCClient_caller = NULL;

    //6、设置一些本地变量
    CAgentInfo* pAgentInfo = NULL;
    if (deliverState.first == 1) { //分配到座席
        pAgentInfo = any_cast<CAgentInfo*>(deliverState.second);
        pAgentInfo->m_bCallFromVDN = true;
        pAgentInfo->m_bCallFromQueue = false;
    }

    //7、发送conf消息
    switch (deliverState.first) {
    case 1: {//分配到座席
        agentMakeCallConf.callingState = AgentCalling_AgentRinging;
        agentMakeCallConf.SetCalledParty(pAgentInfo->m_sAgentID);
        agentMakeCallConf.lanIP = pAgentInfo->m_ulLanIP;
        agentMakeCallConf.lanMainSockPort = pAgentInfo->m_usLanMainSockPort;
        break;
    }
    case 2: {//排队
        agentMakeCallConf.callingState = AgentCalling_AgentQueuing;
        agentMakeCallConf.userNumInQueue = any_cast<int>(deliverState.second);
        break;
    }
    }
    this->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);

    //8、发送vdn消息
    if (deliverState.first == 2) {
        //排队
        CallEnterVDN.bIntoQueue = true;
        CallEnterVDN.CurQueueNum = any_cast<int>(deliverState.second);
    }

    CUtility::GetInstance()->SendIPCCMsg_UDP(
        CallEnterVDN,
        CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_IP,
        CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_Port);

    CUtility::GetInstance()->SendToMonitorServer(CallEnterVDN);
}

void CUtility::OnCProxy_HangupCall(SIPCCMsg_CProxy_HangupCall* cclientHangupCall, unsigned long ip, unsigned short port)
{
	switch (cclientHangupCall->callType)
	{
	case E_MESSAGE_CALL:
	case E_WEBCHAT_CALL:
	case E_EMAIL_CALL:
	{
		return OnHangupTXTCall(*cclientHangupCall, ip, port);
	}
	default:
		break;

	}

	string sResponseAgent = "";
	string sOriginalTarget = "";
	CCClientInfo* pCClientInfo_main = CCClientMgr::GetInstance()->GetCClientInfo(cclientHangupCall->account);

	if ( pCClientInfo_main == NULL )
	{
		SIPCCMsg_ACD_CHangupCallConf changupCallConf;
		changupCallConf.callID = cclientHangupCall->callID;
		this->SendIPCCMsg_UDP(changupCallConf, ip, port);

		g_Logger.MakeLog("not found cclient info, account:" + cclientHangupCall->account);

		return;
	}
	else
	{
		sResponseAgent = pCClientInfo_main->m_sResponseAgentID;
		sOriginalTarget = pCClientInfo_main->m_sOriginCalledTarget;
		CCClientMgr::GetInstance()->DelCClient(cclientHangupCall->account);
	}

	if ( sResponseAgent.empty() )
	{
        if (cclientHangupCall->targetType == CallParty_Department)
		{
			//从队列中清除

			string corpAndDepart = cclientHangupCall->targetName;
			if (corpAndDepart.empty())
			{
				corpAndDepart = sOriginalTarget;
			}

			CDepartmentInfo* pDepartmentInfo = CCorporationMgr::GetInstance()->GetDepartmentInfo(corpAndDepart);

			if(pDepartmentInfo == NULL)
			{
				SIPCCMsg_ACD_CHangupCallConf changupCallConf;
				changupCallConf.callID = cclientHangupCall->callID;
				this->SendIPCCMsg_UDP(changupCallConf, ip, port);
				g_Logger.MakeLog("not found Department Info , " + cclientHangupCall->targetName);
				return;
			}

			{
				SIPCCMsg_ACD_CallLeaveVDN CallLeaveVDN;
				CallLeaveVDN.vdnNo = pDepartmentInfo->m_sDepartmentName;
				CallLeaveVDN.corpName = pDepartmentInfo->m_sCorpName;
				CallLeaveVDN.callID = cclientHangupCall->callID;
				CallLeaveVDN.leaveTime = CLog::GetLocalTime();
				CallLeaveVDN.leaveCause = VDNEndCause_CallAbandon;
				CallLeaveVDN.managerid = pDepartmentInfo->m_sManagerid;
				CallLeaveVDN.deptid = pDepartmentInfo->m_sDeptid;
				CallLeaveVDN.LeaveQueueTime = CLog::GetLocalTime();
				CallLeaveVDN.agentID = "";
				CallLeaveVDN.bIntoQueue = true;
                CallLeaveVDN.CurQueueNum = pDepartmentInfo->GetCQueueSize();

				//DataCollect
				CUtility::GetInstance()->SendIPCCMsg_UDP(
					CallLeaveVDN, 
					CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_IP, 
					CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_Port);

				CUtility::GetInstance()->SendToMonitorServer(CallLeaveVDN);
			}

			pDepartmentInfo->RemoveFromCQueue(cclientHangupCall->account);
		}
		
		SIPCCMsg_ACD_CHangupCallConf changupCallConf;
		changupCallConf.callID = cclientHangupCall->callID;
		changupCallConf.keyword = cclientHangupCall->targetName;
		this->SendIPCCMsg_UDP(changupCallConf, ip, port);

		g_Logger.MakeLog("ResponseAgentID is empty");

		return;
	}

	CAgentInfo* pAgentInfo_other = CAgentMgr::GetInstance()->GetAgentInfo(sResponseAgent);
	if (pAgentInfo_other==NULL)
	{
		//发送 CHangupCallConf 消息给CClient
		SIPCCMsg_ACD_CHangupCallConf changupCallConf;
		changupCallConf.callID = cclientHangupCall->callID;
		changupCallConf.agentName = sResponseAgent;
		this->SendIPCCMsg_UDP(changupCallConf, ip, port);

		g_Logger.MakeLog("agent not found " + sResponseAgent);
		return;
	}

	//删除还没有推送的calldelivered
	pAgentInfo_other->delCallDelivered(cclientHangupCall->callID);

	//发送 CHangupCallConf 消息给源CClient
	SIPCCMsg_ACD_CHangupCallConf changupCallConf;
	changupCallConf.callID = cclientHangupCall->callID;
	changupCallConf.agentName = sResponseAgent;
	this->SendIPCCMsg_UDP(changupCallConf, ip, port);

	string sAccount = "CClient_";
	sAccount += cclientHangupCall->account;

	//发送 CallReleased 消息给目的座席
	SIPCCMsg_ACD_CallReleased callReleased_other;
	callReleased_other.SetReleasedParty(sAccount);
	callReleased_other.callID = cclientHangupCall->callID;
	callReleased_other.agentName = pAgentInfo_other->m_sAgentID;
	this->SendIPCCMsg_UDP(callReleased_other, pAgentInfo_other->m_ulAgentIP, pAgentInfo_other->m_usAgentPort);

	if ( cclientHangupCall->targetType == 3 )
	{
		if (pAgentInfo_other->m_iCurState != Agent_Ringing)
		{
			g_Logger.MakeLog("agent is ringing, " + sResponseAgent);
			return;
		}

		string corpAndDepart = cclientHangupCall->targetName;
		if (corpAndDepart.empty())
		{
			corpAndDepart = sOriginalTarget;
		}

		CDepartmentInfo* pDepartmentInfo = CCorporationMgr::GetInstance()->GetDepartmentInfo(corpAndDepart);
		if ( pDepartmentInfo == NULL )
		{
			g_Logger.MakeLog("Department Info not exist, " + corpAndDepart);
			return;
		}

		SIPCCMsg_ACD_CallLeaveVDN CallLeaveVDN;
		CallLeaveVDN.vdnNo = pDepartmentInfo->m_sDepartmentName;
		CallLeaveVDN.corpName = pDepartmentInfo->m_sCorpName;
		CallLeaveVDN.callID = cclientHangupCall->callID;
		CallLeaveVDN.leaveTime = CLog::GetLocalTime();
		CallLeaveVDN.leaveCause = VDNEndCause_CallAbandon;
		CallLeaveVDN.managerid = pDepartmentInfo->m_sManagerid;
		CallLeaveVDN.deptid = pDepartmentInfo->m_sDeptid;
		CallLeaveVDN.LeaveQueueTime = pAgentInfo_other->m_sQueueArriveTime;
		CallLeaveVDN.agentID = pAgentInfo_other->m_sAgentID;
		CallLeaveVDN.bIntoQueue = pAgentInfo_other->m_bCallFromQueue;
        CallLeaveVDN.CurQueueNum = pDepartmentInfo->GetCQueueSize();

		CUtility::GetInstance()->SendIPCCMsg_UDP(
			CallLeaveVDN, 
			CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_IP, 
			CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_Port);

		CUtility::GetInstance()->SendToMonitorServer(CallLeaveVDN);
	}
}

void CUtility::OnNotifyMsg(SIPCCMsg_OneKeySwitch_NotifyMsg* notifyMsg, unsigned long ip, unsigned short port)
{
	SIPCCMsg_OneKeySwitch_NotifyMsg msgcopy(*notifyMsg);
	for (size_t j = 0; j < notifyMsg->targets.size(); j++)
	{
		SIPCCMsg_OneKeySwitch_NotifyMsg::Target &target = notifyMsg->targets[j];
		if (target.type == "CORP")
		{
			for (size_t i = 0; i < target.targetNames.size(); i++)
			{
				CCorporationInfo *pCI = CCorporationMgr::GetInstance()->FindWithManagerID(target.targetNames[i]);
				if (pCI)
				{
					pCI->NotifyMsg(msgcopy);
				}
			}
		}
		else if (target.type == "DEPT")
		{
			for (size_t i = 0; i < target.targetNames.size(); i++)
			{
				CSpliter sp(target.targetNames[i], ":");
				if (sp.GetSize() > 1)
				{
					CCorporationInfo *pCI = CCorporationMgr::GetInstance()->FindWithManagerID(sp[0]);
					if (pCI)
					{
						CDepartmentInfo* pdept = pCI->GetDepartmentInfo(sp[1]);
						if (pdept)
						{
							pdept->NotifyMsg(msgcopy);
						}
					}
				}
			}
		}
		else if (target.type == "AGENT")
		{
			for (size_t i = 0; i < target.targetNames.size(); i++)
			{
				CSpliter sp(target.targetNames[i], ":");
				string agentid = (sp.GetSize() > 1) ? sp[1] : sp[0];
				CAgentInfo* pAgent = CAgentMgr::GetInstance()->GetAgentInfo(agentid);
				if (pAgent)
				{
					pAgent->NotifyMsg(msgcopy);
				}
			}
		}
	}
}

void CUtility::OnOperatorSendStrMsg(SIPCCMsg_Operator2ACD_SendStrMsg* OperatorSendStrMsg, unsigned long ip, unsigned short port)
{
	////查看座席状态
	//CAgentInfo* pAgentInfo_caller = CAgentMgr::GetInstance()->GetAgentInfo(OperatorSendStrMsg->theSender);
	//if(pAgentInfo_caller == NULL)
	//	return;
	switch (OperatorSendStrMsg->eNT)
	{
	case Notice_All:	// 发给所有在线坐席
	{
		map<string, CAgentInfo*> & mapAgents = CAgentMgr::GetInstance()->MapAgents();
		map<string, CAgentInfo*>::iterator it = mapAgents.begin(), itEnd = mapAgents.end();
		for (; it != itEnd; it++)
		{
			//发送 ACD_SendStrMsg
			SIPCCMsg_ACD_SendStrMsg ACDSendStrMsg;

			ACDSendStrMsg.theSender = OperatorSendStrMsg->theSender;
			ACDSendStrMsg.msgContent = OperatorSendStrMsg->msgContent;
			if (it->second)
			{
				this->SendIPCCMsg_UDP(ACDSendStrMsg, it->second->m_ulAgentIP, it->second->m_usAgentPort);
			}
		}
	}
	break;
	case Notice_Company:	// 发给某一公司的所有在线坐席
	{
		CCorporationMgr *pCM = CCorporationMgr::GetInstance();
		CCorporationInfo *pCI = pCM->GetCorporation(OperatorSendStrMsg->strCompany);
		if (!pCI)
		{
			return;
		}
		map<string, CDepartmentInfo*>::iterator it = pCI->m_mapDepartments.begin(), itEnd = pCI->m_mapDepartments.end();
		for (; it != itEnd; it++)
		{
			map<string, string> &mapAgentsInDepart = it->second->MapAgentsInDepart();
			map<string, string>::iterator itTem = mapAgentsInDepart.begin(), itTemEnd = mapAgentsInDepart.end();
			for (; itTem != itTemEnd; itTem++)
			{
				CAgentInfo* pAgentDest = CAgentMgr::GetInstance()->GetAgentInfo(itTem->first);
				if (!pAgentDest)
				{
					continue;
				}
				//发送 ACD_SendStrMsg
				SIPCCMsg_ACD_SendStrMsg ACDSendStrMsg;

				ACDSendStrMsg.theSender = OperatorSendStrMsg->theSender;
				ACDSendStrMsg.msgContent = OperatorSendStrMsg->msgContent;
				this->SendIPCCMsg_UDP(ACDSendStrMsg, pAgentDest->m_ulAgentIP, pAgentDest->m_usAgentPort);
			}
		}
	}
	break;
	case Notice_Agents:
	{
		char acDestAgents[BUFSIZ] = { 0 };
		strncpy(acDestAgents, OperatorSendStrMsg->destAgents.c_str(), BUFSIZ - 1); // OperatorSendStrMsg->destAgents;	 格式 1@1.com|1@2.com|...	
		char seps[] = "|";
		char *called;
		// Establish string and get the first token:
		called = strtok(acDestAgents, seps); // C4996
		// Note: strtok is deprecated; consider using strtok_s instead
		while (called != NULL)
		{
			// While there are tokens in "string"
			//////////////////////////////////////////////////////////////////////////
			CAgentInfo* pAgentInfo_called = CAgentMgr::GetInstance()->GetAgentInfo(called);

			if (pAgentInfo_called == NULL)
				return;

			//发送 ACD_SendStrMsg
			SIPCCMsg_ACD_SendStrMsg ACDSendStrMsg;

			ACDSendStrMsg.theSender = OperatorSendStrMsg->theSender;
			ACDSendStrMsg.msgContent = OperatorSendStrMsg->msgContent;

			this->SendIPCCMsg_UDP(ACDSendStrMsg, pAgentInfo_called->m_ulAgentIP, pAgentInfo_called->m_usAgentPort);
			//////////////////////////////////////////////////////////////////////////

			// Get next token: 
			called = strtok(NULL, seps); // C4996
		}
	}
	break;
	default:
		break;
	}
}

void CUtility::OnMakeMessageCall(SIPCCMsg_CProxy_CMakeCall & makecall, unsigned long ip, unsigned short port)
{
	if (makecall.targetType == CallParty_Agent)
	{
		OnMakeMessageCallToAgent(makecall, ip, port);
	}
	else if (makecall.targetType == CallParty_Department)
	{
		OnMakeMessageCallToDepartment(makecall, ip, port);
	}
}

void CUtility::OnMakeMessageCall(SIPCCMsg_Client_AgentMakeCall & makecall, unsigned long ip, unsigned short port)
{
	if (makecall.targetType == CallParty_Department)
	{
		OnMakeMessageCallToDepartment(makecall, ip, port);
	}
	else
	{
		OnMakeMessageCallToAgent(makecall, ip, port);
	}
}

void CUtility::OnMakeMessageCallToAgent(SIPCCMsg_CProxy_CMakeCall & makecall, unsigned long ip, unsigned short port)
{
	CAgentInfo* pAgentInfo_called = CAgentMgr::GetInstance()->GetAgentInfo(makecall.targetName);

	if (pAgentInfo_called == NULL ||
		pAgentInfo_called->m_iLoginType == LoginType_WebPSTN)
	{
		SIPCCMsg_ACD_AgentMakeCallConf agentMakeCallConf;
		agentMakeCallConf.callID = makecall.callID;
		agentMakeCallConf.callingAgentID = makecall.account;
		agentMakeCallConf.callingState = AgentCalling_AgentNoLogin;
		agentMakeCallConf.callingAgentID = makecall.account;
		agentMakeCallConf.callID = makecall.callID;
		agentMakeCallConf.callType = makecall.callType;
		this->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);
		return;
	}

	if (makecall.callType == E_EMAIL_CALL)
	{
		int state = pAgentInfo_called->MakeCall(makecall, ip, port);

		SIPCCMsg_ACD_AgentMakeCallConf agentMakeCallConf;
		agentMakeCallConf.callID = makecall.callID;
		agentMakeCallConf.callingAgentID = makecall.account;
		agentMakeCallConf.callingState = state;
		agentMakeCallConf.callingAgentID = makecall.account;
		agentMakeCallConf.callID = makecall.callID;
		agentMakeCallConf.callType = makecall.callType;
		this->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);

		return;
	}

	pAgentInfo_called->DeliverCall(makecall, ip, port);
}

void CUtility::OnMakeMessageCallToDepartment(SIPCCMsg_CProxy_CMakeCall & makecall, unsigned long ip, unsigned short port)
{
	CTXTSkillGroupPtr pSkillGroupInfo =
		CCorporationMgr::GetInstance()->GetTXTSkillGroupInfo(makecall.targetName);

	if (pSkillGroupInfo)
	{
		pSkillGroupInfo->func_CallIn(makecall, ip, port);
		return;
	}
	else
	{
		CDepartmentInfo* pDepartmentInfo =
			CCorporationMgr::GetInstance()->GetDepartmentInfo(makecall.targetName);

		if (pDepartmentInfo)
		{
			pDepartmentInfo->On_SIPCCMsg_CProxy_CMakeCall(makecall, ip, port);
			return;
		}
	}

	SIPCCMsg_ACD_AgentMakeCallConf agentMakeCallConf;
	agentMakeCallConf.callingState = AgentCalling_DepartmentInexistence;
	agentMakeCallConf.SetCalledParty(makecall.targetName);
	agentMakeCallConf.callingAgentID = makecall.account;
	agentMakeCallConf.callID = makecall.callID;
	agentMakeCallConf.callType = makecall.callType;
	this->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);
}

void CUtility::OnMakeMessageCallToDepartment(SIPCCMsg_Client_AgentMakeCall & makecall, unsigned long ip, unsigned short port)
{
	CTXTSkillGroupPtr pSkillGroupInfo =
		CCorporationMgr::GetInstance()->GetTXTSkillGroupInfo(makecall.GetTargetPosition());

	if (pSkillGroupInfo)
	{
		pSkillGroupInfo->func_CallIn(makecall, ip, port);
		return;
	}
	else
	{
		CDepartmentInfo* pDepartmentInfo =
			CCorporationMgr::GetInstance()->GetDepartmentInfo(makecall.GetTargetPosition());

		if (pDepartmentInfo)
		{
			pDepartmentInfo->On_SIPCCMsg_Client_AgentMakeCall(makecall, ip, port);
			return;
		}
	}

	SIPCCMsg_ACD_AgentMakeCallConf agentMakeCallConf;
	agentMakeCallConf.callingState = AgentCalling_DepartmentInexistence;
	agentMakeCallConf.SetCalledParty(makecall.GetTargetPosition());
	agentMakeCallConf.callingAgentID = makecall.GetAgentID();
	agentMakeCallConf.callID = makecall.GetCallID();
	agentMakeCallConf.callType = makecall.callType;
	this->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);
}

void CUtility::OnMakeMessageCallToAgent(SIPCCMsg_Client_AgentMakeCall & makecall, unsigned long ip, unsigned short port)
{
	int state;
	CAgentInfo* pAgentInfo_called = CAgentMgr::GetInstance()->GetAgentInfo(makecall.GetTargetPosition());
	if (NULL == pAgentInfo_called)
	{
		state = AgentCalling_AgentNoLogin;
	}
	else
	{
		state = pAgentInfo_called->MakeCall(makecall, ip, port);
	}

	SIPCCMsg_ACD_AgentMakeCallConf agentMakeCallConf;
	agentMakeCallConf.callingState = state;
	agentMakeCallConf.SetCalledParty(makecall.GetTargetPosition());
	agentMakeCallConf.callingAgentID = makecall.GetAgentID();
	agentMakeCallConf.callID = makecall.GetCallID();
	agentMakeCallConf.callType = makecall.callType;
	this->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);

	return;
}

void CUtility::OnHangupTXTCall(SIPCCMsg_CProxy_HangupCall & hangupcall, unsigned long ip, unsigned short port)
{
	string agentid = hangupcall.targetName;

	//座席为空或者为部门,通过callid找座席
	if (agentid.empty() || agentid.find("%^#") != std::string::npos) {
		ACDServer::call_ptr callptr = CCallMgr::GetInstance()->Get(hangupcall.callID);
		if (callptr) {
			agentid = callptr->call_.alerting_device;
		}
	}

	CAgentInfo* pAgentInfo_called = NULL;
	if (!agentid.empty())
	{
		pAgentInfo_called = CAgentMgr::GetInstance()->GetAgentInfo(agentid);
	}

	if (pAgentInfo_called != NULL)
	{
        pAgentInfo_called->OnCClientHangupTXTCall(hangupcall, ip, port);
	}
	else
	{
		CTXTSkillGroupPtr pSkillGroupInfo =
			CCorporationMgr::GetInstance()->GetTXTSkillGroupInfo(hangupcall.targetName);

		if (pSkillGroupInfo)
		{
			pSkillGroupInfo->On_SIPCCMsg_CProxy_HangupCall(hangupcall, ip, port);
			return;
		}
		else
		{
			CDepartmentInfo* pDepartmentInfo =
				CCorporationMgr::GetInstance()->GetDepartmentInfo(hangupcall.targetName);
			if (pDepartmentInfo != NULL)
			{
				pDepartmentInfo->On_SIPCCMsg_CProxy_HangupCall(hangupcall, ip, port);
				return;
			}
		}

		SIPCCMsg_ACD_CHangupCallConf changupCallConf;
		changupCallConf.callID = hangupcall.callID;
		changupCallConf.agentName = hangupcall.targetName;
		changupCallConf.account = hangupcall.account;
		CUtility::GetInstance()->SendIPCCMsg_UDP(changupCallConf, ip, port);
	}
}

void CUtility::OnHangupTXTCall(SIPCCMsg_Client_AgentHangupCall & hangupcall, unsigned long ip, unsigned short port)
{
	//解除发起方呼叫限制
    string agentid = hangupcall.GetAgentID();

	//座席为空或者为部门,通过callid找座席
	if (agentid.empty() || agentid.find("%^#") != std::string::npos) {
        ACDServer::call_ptr callptr = CCallMgr::GetInstance()->Get(hangupcall.GetCallID());
        if (callptr) {
            agentid = callptr->call_.alerting_device;
        }
    }

    CAgentInfo* pAgentInfo_called = NULL;
    if (!agentid.empty()) {
        pAgentInfo_called = CAgentMgr::GetInstance()->GetAgentInfo(agentid);
    }

	if (pAgentInfo_called != NULL)
	{
        pAgentInfo_called->OnHangupTXTCall(hangupcall, ip, port);
	}
	else
	{
		CTXTSkillGroupPtr pSkillGroupInfo =
			CCorporationMgr::GetInstance()->GetTXTSkillGroupInfo(hangupcall.GetAgentID());

		if (pSkillGroupInfo)
		{
			pSkillGroupInfo->On_SIPCCMsg_Client_AgentHangupCall(hangupcall, ip, port);
			return;
		}
		else
		{
			CDepartmentInfo* pDepartmentInfo =
				CCorporationMgr::GetInstance()->GetDepartmentInfo(hangupcall.GetAgentID());
			if (pDepartmentInfo != NULL)
			{
				pDepartmentInfo->On_SIPCCMsg_Client_AgentHangupCall(hangupcall, ip, port);
				return;
			}
		}

		SIPCCMsg_ACD_CallReleased callReleased;
		callReleased.callID = hangupcall.GetCallID();
		callReleased.callType = hangupcall.callType;
		callReleased.SetReleasedParty(hangupcall.GetAgentID());
		CUtility::GetInstance()->SendIPCCMsg_UDP(callReleased, ip, port);

		{
			ostringstream os;
			string sTemp("");
			os << "!!!Warning  CUtility::OnHangupTXTCall(): agentID<" << hangupcall.GetAgentID() << "> not find,"
				<< "callID<" << hangupcall.GetCallID() << "> not released!!!";

			sTemp = os.str();
			g_Logger.MakeLog(sTemp);
		}
	}
}

void CUtility::OnHangupEMailCall(SIPCCMsg_CProxy_HangupCall & hangupcall, unsigned long ip, unsigned short port)
{
	string agentid = hangupcall.targetName;

	//座席为空或者为部门,通过callid找座席
	if (agentid.empty() || agentid.find("%^#") != std::string::npos) {
		ACDServer::call_ptr callptr = CCallMgr::GetInstance()->Get(hangupcall.callID);
		if (callptr) {
			agentid = callptr->call_.alerting_device;
		}
	}

	CAgentInfo* pAgentInfo_called = NULL;
	if (!agentid.empty())
	{
		pAgentInfo_called = CAgentMgr::GetInstance()->GetAgentInfo(agentid);
	}

	if (pAgentInfo_called != NULL)
	{
		pAgentInfo_called->OnHangupEmailCall(hangupcall, ip, port);
	}
	else
	{
		CTXTSkillGroupPtr pSkillGroupInfo =
			CCorporationMgr::GetInstance()->GetTXTSkillGroupInfo(hangupcall.targetName);

		if (pSkillGroupInfo)
		{
			pSkillGroupInfo->On_SIPCCMsg_CProxy_HangupCall(hangupcall, ip, port);
			return;
		}
		else
		{
			CDepartmentInfo* pDepartmentInfo =
				CCorporationMgr::GetInstance()->GetDepartmentInfo(hangupcall.targetName);
			if (pDepartmentInfo != NULL)
			{
				pDepartmentInfo->On_SIPCCMsg_CProxy_HangupCall(hangupcall, ip, port);
				return;
			}
		}

		SIPCCMsg_ACD_CHangupCallConf changupCallConf;
		changupCallConf.callID = hangupcall.callID;
		changupCallConf.agentName = hangupcall.targetName;
		changupCallConf.account = hangupcall.account;
		CUtility::GetInstance()->SendIPCCMsg_UDP(changupCallConf, ip, port);
	}
}

void CUtility::OnHangupEMailCall(SIPCCMsg_Client_AgentHangupCall & hangupcall, unsigned long ip, unsigned short port)
{
	string agentid = hangupcall.GetAgentID();

	//座席为空或者为部门,通过callid找座席
	if (agentid.empty() || agentid.find("%^#") != std::string::npos) {
		ACDServer::call_ptr callptr = CCallMgr::GetInstance()->Get(hangupcall.GetCallID());
		if (callptr) {
			agentid = callptr->call_.alerting_device;
		}
	}

	CAgentInfo* pAgentInfo_called = NULL;
	if (!agentid.empty()) {
		pAgentInfo_called = CAgentMgr::GetInstance()->GetAgentInfo(agentid);
	}

	if (pAgentInfo_called != NULL)
	{
		pAgentInfo_called->OnHangupEmailCall(hangupcall, ip, port);
	}
	else
	{
		CTXTSkillGroupPtr pSkillGroupInfo =
			CCorporationMgr::GetInstance()->GetTXTSkillGroupInfo(hangupcall.GetAgentID());

		if (pSkillGroupInfo)
		{
			pSkillGroupInfo->On_SIPCCMsg_Client_AgentHangupCall(hangupcall, ip, port);
			return;
		}
		else
		{
			CDepartmentInfo* pDepartmentInfo =
				CCorporationMgr::GetInstance()->GetDepartmentInfo(hangupcall.GetAgentID());
			if (pDepartmentInfo != NULL)
			{
				pDepartmentInfo->On_SIPCCMsg_Client_AgentHangupCall(hangupcall, ip, port);
				return;
			}
		}

		SIPCCMsg_ACD_CallReleased callReleased;
		callReleased.callID = hangupcall.GetCallID();
		callReleased.callType = hangupcall.callType;
		callReleased.SetReleasedParty(hangupcall.GetAgentID());
		CUtility::GetInstance()->SendIPCCMsg_UDP(callReleased, ip, port);

		{
			ostringstream os;
			string sTemp("");
			os << "!!!Warning  CUtility::OnHangupEmailCall(): agentID<" << hangupcall.GetAgentID() << "> not find,"
				<< "callID<" << hangupcall.GetCallID() << "> not released!!!";

			sTemp = os.str();
			g_Logger.MakeLog(sTemp);
		}
	}
}

bool CUtility::GetService(string querystr)
{
	if (m_redirectServer)
	{
		m_redirectServer->GetService(querystr, EST_ACDServer,
			boost::bind(&CUtility::OnGetService_Redirect, this, _1, _2, _3),
			__FILE__, __LINE__);
		return true;
	}
	else if (m_registerServer)
	{
		if (m_registerServer->IsOK())
		{
			CSpliter sp_service(querystr, ";");

			for (int i = 0; i < sp_service.GetSize()-1; i += 4)
			{
				int Service1Type = (i + 0 >= sp_service.GetSize()-1) ? 0:boost::lexical_cast<int>(sp_service[i + 0].c_str());
				int Service2Type = (i + 1 >= sp_service.GetSize()-1) ? 0:boost::lexical_cast<int>(sp_service[i + 1].c_str());
				int Service3Type = (i + 2 >= sp_service.GetSize()-1) ? 0:boost::lexical_cast<int>(sp_service[i + 2].c_str());
				int Service4Type = (i + 3 >= sp_service.GetSize()-1) ? 0:boost::lexical_cast<int>(sp_service[i + 3].c_str());
				m_registerServer->GetService(boost::bind(&CUtility::OnGetService, this, _1, _2),
					Service1Type, Service2Type, Service3Type, Service4Type
					);
			}
			return true;
		}
	}

	return false;
}

void CUtility::OnGetService_Redirect(string invokeid, vector<ServiceInfo> & serviceVec, vector<RedisDBInfo> redisVec)
{
	OnGetService(invokeid, serviceVec);
}

void CUtility::OnGetService(string invokeid, vector<ServiceInfo> & serviceVec)
{
	for (size_t i = 0; i < serviceVec.size(); i++)
	{
		switch (serviceVec[i].serviceType)
		{
		case EST_ConfigurationServer:
		{
			m_IPCCConfig.SysManageServer_ID = serviceVec[i].serviceID;
			m_IPCCConfig.SysManageServer_IP = serviceVec[i].ip;
			m_IPCCConfig.SysManageServer_Port = serviceVec[i].port == 0 ? ConvertPort(ServerInfoDef::Port_SysManageServer) : serviceVec[i].port;

			SIPCCMsg_ACD_LoadDepartments loadDepartments;
			loadDepartments.serverIP = this->m_IPCCConfig.localserverIP;
			this->SendIPCCMsg_UDP(
				loadDepartments,
				this->m_IPCCConfig.SysManageServer_IP,
				m_IPCCConfig.SysManageServer_Port
				);

			break;
		}
		case EST_UStateServer:
		{
			m_IPCCConfig.UStateServer_ID = serviceVec[i].serviceID;
			m_IPCCConfig.UStateServer_IP = serviceVec[i].ip;
			m_IPCCConfig.UStateServer_Port = serviceVec[i].port;
			break;
		}
		case EST_DataCollectServe:
		{
			m_IPCCConfig.DataCollectServer_ID = serviceVec[i].serviceID;
			m_IPCCConfig.DataCollectServer_IP = serviceVec[i].ip;
			m_IPCCConfig.DataCollectServer_Port = serviceVec[i].port;

			time_t ltime;
			ltime = time(NULL);
			char str[32];
			strftime(str, sizeof(str), "%Y-%m-%d %H:%M:00", localtime(&ltime));

			SIPCCMsg_ACD_ResetDataCollect resetDataCollect;
			resetDataCollect.curTime = str;
			CUtility::GetInstance()->SendIPCCMsg_UDP(
				resetDataCollect,
				m_IPCCConfig.DataCollectServer_IP,
				m_IPCCConfig.DataCollectServer_Port);

			break;
		}
		case EST_MonitiorServer:
		{
			m_IPCCConfig.MonitorServer_ID = serviceVec[i].serviceID;
			m_IPCCConfig.MonitorServer_IP = serviceVec[i].ip;
			m_IPCCConfig.MonitorServer_Port = serviceVec[i].port;

			time_t ltime;
			ltime = time(NULL);
			char str[32];
			strftime(str, sizeof(str), "%Y-%m-%d %H:%M:00", localtime(&ltime));

			SIPCCMsg_ACD_ResetMonitor resetMonitor;
			resetMonitor.curTime = str;
			CUtility::GetInstance()->SendIPCCMsg_UDP(
				resetMonitor,
				m_IPCCConfig.MonitorServer_IP,
				m_IPCCConfig.MonitorServer_Port);

			break;
		}
		case EST_AreaCodeServer:
		{
			m_IPCCConfig.AreaCodeServer_ID = serviceVec[i].serviceID;
			m_IPCCConfig.AreaCodeServer_IP = serviceVec[i].ip;
			m_IPCCConfig.AreaCodeServer_Port = serviceVec[i].port;
			break;
		}
		default:
			;
		}
	}
}

bool CUtility::CheckCallerStateBeforeMakeCall(CAgentInfo* caller, int& reason)
{
	bool ret = true;
	switch (caller->GetCurState())
	{
		case Agent_Dialing: case Agent_DialUnderHold:
			break;
		default:
		{
			ret = false;
			reason = AgentCalling_MakeCallNoAllowed;
		}
	}

	return ret;
}

bool CUtility::CheckCalleeStateBeforeMakeCall(CAgentInfo* callee, int& reason)
{
	if (callee->GetCurState() != Agent_Waiting)
	{
		reason = AgentCalling_AgentBusy;
		return false;
	}
	else
	{
		return true;
	}
}

bool CUtility::CheckStateBeforeMakeCall(CAgentInfo* caller, CAgentInfo* callee, int& reason)
{
	bool ret = CheckCallerStateBeforeMakeCall(caller, reason);
	if (ret)
	{
		ret = CheckCalleeStateBeforeMakeCall(callee, reason);
	}

	if (ret)
	{
		if (caller->m_iLoginType == LoginType_WebPSTN
			&& callee->m_iLoginType != LoginType_OnlyWeb)
		{
			ret = false;
			reason = AgentCalling_AgentNoLogin;
		}
	}

	return ret;
}

void CUtility::MakeLog(string logmsg)
{
	g_Logger.MakeLog(logmsg);
}

string CUtility::ReadText(CIni& ini_file, string category, string key, string defaultValue)
{
	boost::scoped_ptr<char> value(ini_file.ReadText((char*)category.c_str(), (char*)key.c_str()));
	return ((value.get() == NULL) || (strlen(value.get()) == 0)) ? defaultValue : value.get();
}

void CUtility::On_SIPCCMsg_OneKeySwitch_ServiceUnavailable(SIPCCMsg_OneKeySwitch_ServiceUnavailable* msg,
	unsigned long src_ip, unsigned short src_port)
{
	ostringstream oss;
	if (msg->unavailableServiceID == m_IPCCConfig.SysManageServer_ID)
	{
		m_IPCCConfig.SysManageServer_IP = 0;
		m_IPCCConfig.SysManageServer_Port = 0;
		oss << EST_ConfigurationServer << ";";
	}
	if (msg->unavailableServiceID == m_IPCCConfig.UStateServer_ID)
	{
		m_IPCCConfig.UStateServer_IP = 0;
		m_IPCCConfig.UStateServer_Port = 0;
		oss << EST_UStateServer << ";";
	}
	if (msg->unavailableServiceID == m_IPCCConfig.DataCollectServer_ID)
	{
		m_IPCCConfig.DataCollectServer_IP = 0;
		m_IPCCConfig.DataCollectServer_Port = 0;
		oss << EST_DataCollectServe << ";";
	}
	if (msg->unavailableServiceID == m_IPCCConfig.MonitorServer_ID)
	{
		m_IPCCConfig.MonitorServer_IP = 0;
		m_IPCCConfig.MonitorServer_Port = 0;
		oss << EST_MonitiorServer << ";";
	}

	if (msg->unavailableServiceID == m_IPCCConfig.AreaCodeServer_ID)
	{
		m_IPCCConfig.AreaCodeServer_IP = 0;
		m_IPCCConfig.AreaCodeServer_Port = 0;
		oss << EST_AreaCodeServer << ";";
	}

	string querystr = oss.str();
	if (!querystr.empty())
	{
		GetService(oss.str());
	}
}

void CUtility::On_SIPCCMsg_Client_QueryAgentsState(SIPCCMsg_Client_QueryAgentsState* msg,
    unsigned long src_ip, unsigned short src_port)
{
    stringstream ss;
    CSpliter sp(msg->queryList, ";");
    for (int i = 0; i < sp.GetSize(); i++)
    {
        CAgentInfo* pagent = CAgentMgr::GetInstance()->GetAgentInfo((string)sp[i]);
        if (pagent != NULL)
        {
            ss << (string)sp[i] << ":" << pagent->GetCurState() << ";";
        }
        else
        {
            ss << (string)sp[i] << ":" << Agent_NoLogin << ";";
        }
    }

    SIPCCMsg_Client_QueryAgentsStateConf conf;
    conf.invokeID = msg->invokeID;
    conf.agentID = msg->agentID;
    conf.agentList = ss.str();

    CUtility::GetInstance()->SendIPCCMsg_UDP(
        conf, src_ip, src_port);

}

void CUtility::Push_Message(std::string account, ECallType calltype, std::string callid)
{
	CAgentInfo *pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(account);
	if (NULL == pAgentInfo || pAgentInfo->IsPC())
		return;

	pAgentInfo->PushFlag++;

	SPushMessage_ptr spmsg(new SPushMessage());
	spmsg->account = account;
	spmsg->content = "您有一个客户来电";
	spmsg->devicetoken = pAgentInfo->deviceToken;
	spmsg->pushway = (pAgentInfo->IsIOS() && calltype == E_NORMAL_CALL) ? Push_APNs : Push_XG;
	spmsg->ttype = pAgentInfo->IsIOS() ? Terminal_IOS : Terminal_Android;
	spmsg->custom_info.insert(std::pair<string,string>("callid", callid));

	CMessagePushMgr::GetInstance()->PushMessage(spmsg);
		
}

void CUtility::RemoveAgentForPushFailed(string agentID)
{
	CAgentInfo *pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(agentID);
	if (NULL == pAgentInfo || pAgentInfo->bAgentDormancy == false)
		return;
	
	//退出会议	
	CAgentMgr::GetInstance()->OnferenceinAgentTimeout(pAgentInfo);

	CDepartmentInfo* pDepartmentInfo = NULL;
	pDepartmentInfo = CCorporationMgr::GetInstance()->GetDepartmentInfo(pAgentInfo->m_sCorporation, pAgentInfo->m_sDepartment);
	if (pDepartmentInfo)
	{
		pDepartmentInfo->RemoveFromWAQueue(pAgentInfo->m_sAgentID);

		if (pAgentInfo->m_iCurState == Agent_Talking && pAgentInfo->bPSTNCall)
		{
			SIPCCMsg_Client_AgentHangupCall AgentHangupCall;
			AgentHangupCall.SetAgentID(pAgentInfo->m_sAgentID);

			if (pAgentInfo->bCallin)
			{
				AgentHangupCall.SetTheOtherParty(pAgentInfo->m_userInfo.m_sUserName);
			}
			else
			{
				AgentHangupCall.SetTheOtherParty(pAgentInfo->m_sCalledParty);
			}

			this->SendIPCCMsg_UDP(AgentHangupCall, this->m_IPCCConfig.UStateServer_IP, this->m_IPCCConfig.UStateServer_Port);

		}
	}
	pAgentInfo->SetState(Agent_NoLogin);

	CAgentMgr::GetInstance()->DelAgent(agentID);

}

void  CUtility::send_SIPCCMsg_Client_MessageCallInfo(
	ECallType callType,
	string callID,     //呼叫ID
	string callerName, //主叫
	string agentname,  //客服账号
	string startTime,  //开始时间
	string endTime,    //结束时间
	string managerID,
	string deptName,
	int32_t callerType,    //主叫类型
	string calledKeyword,  //呼叫关键字
	string skillGroupId,   //路由到的目的技能组ID
	string skillGroupName, //路由到的目的技能自名称
	int32_t established    //0:会话未形成 1：形成会话（acd收到座席应答）
	)
{
	SIPCCMsg_Client_MessageCallInfo messagecallinfo;

	messagecallinfo.callType = callType;
	messagecallinfo.callID = callID;
	messagecallinfo.phoneNumber = "";
	string::size_type pos = messagecallinfo.callerName.find("@");
	if (pos != string::npos)
	{
		messagecallinfo.phoneNumber = messagecallinfo.callerName.substr(0, pos);
	}
	messagecallinfo.callerName = callerName;
	messagecallinfo.agentname = agentname;
	messagecallinfo.startTime = startTime;
	messagecallinfo.endTime = endTime;

	time_duration span = time_from_string(endTime) - time_from_string(startTime);
	if (span.total_seconds() < 0)
	{
		endTime = GetLocalTimeForMessageCall();
	}

	messagecallinfo.managerID = managerID;
	messagecallinfo.deptName = deptName;	
	messagecallinfo.callerType = callerType;			
	messagecallinfo.established = established;
	messagecallinfo.calledKeyword = calledKeyword;
	messagecallinfo.skillGroupId =skillGroupId;
	messagecallinfo.skillGroupName = skillGroupName;

	this->SendIPCCMsg_UDP(messagecallinfo,this->m_IPCCConfig.AreaCodeServer_IP, this->m_IPCCConfig.AreaCodeServer_Port);
}



/*************************************
bool CUtility::APNs_Push_Test()
{
	std::string provider_tokens = m_JWTEncode_ptr->get_provider_authentication_tokens(m_IPCCConfig.AuthKeyID, m_IPCCConfig.TeamID);

	std::string url = "https://api.development.push.apple.com:443/3/device/7feffa5e587f221b03aca86871ad63f6f45f1bc19845d7a78db3807096c7e276";

	std::map<string, string> headers;
	headers["content-type"] = "application/json";
	headers["apns-topic"] = m_IPCCConfig.BundleId;
	headers["authorization"] = string("bearer ") + provider_tokens;

	std::cout << "send http2 post,"
		<< "full url = " << url
		<< ",keyID=" << m_IPCCConfig.AuthKeyID
		<< ",teamID=" << m_IPCCConfig.TeamID
		<< ",BundleId=" << m_IPCCConfig.BundleId
		<< "token=" << provider_tokens
		<< std::endl;


	std::string msg = "{\"aps\":{\"alert\":\"Hello\"}}";


	//client.HttpsPost(url, msg, headers, "GeoTrust_Global_CA.pem",
	m_AsyncHttpClient_ptr->Http2Post(url, msg, headers, "GeoTrust_Global_CA.pem",
		[=](bool bOk, std::string& strRsp)
	{

		std::cout << "http2 post response: bOk="
			<< (bOk ? "true" : "false")
			<< ",strRsp=" << strRsp 
			<< std::endl;

	});

	return true;
}
**************************************************/
