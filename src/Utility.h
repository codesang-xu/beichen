// Utility.h: interface for the CUtility class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTILITY_H__B96D7F2F_9644_4E77_AA0C_7CF932223D4E__INCLUDED_)
#define AFX_UTILITY_H__B96D7F2F_9644_4E77_AA0C_7CF932223D4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public/UDPSocket.h"	// Added by ClassView
#include "public/IPCCMessage.h"
#include "public/CreditUDP.h"
#include "AgentInfo.h"

#include <string>
#include <list>

#include "AgentMgr.h"
#include "common/creditudp_transport.h"
#include "CorporationMgr.h"
////////////#include "HandleRedisThread.h"
#include "RedirectServer.h"
#include "RedisServer_ACD.h"
#include "RegisterServer.h"
#include "common/thread.h"

using namespace std;
using namespace ib_common;

class CIni;

//配置文件结构
typedef struct __SIPCCConfig
{
	int ServiceID;
	string ACDServerName;
	string localserverIP;
	uint32_t localIP;

	int SysManageServer_ID;
	uint32_t SysManageServer_IP;
	uint16_t SysManageServer_Port;

	int UStateServer_ID;
	uint32_t UStateServer_IP;
	uint16_t UStateServer_Port;

	int	DataCollectServer_ID;
	uint32_t DataCollectServer_IP;
	uint16_t DataCollectServer_Port;

	int	MonitorServer_ID;
	uint32_t MonitorServer_IP;
	uint16_t MonitorServer_Port;

	int	MonitorServer_Backup_ID;
	uint32_t MonitorServer_Backup_IP;
	uint16_t MonitorServer_Backup_Port;

	int AreaCodeServer_ID;
	uint32_t AreaCodeServer_IP;
	uint16_t AreaCodeServer_Port;
	
	bool bIsEnterpriseVersion;
	bool bAllowForceLogout;

	bool bIsLocalDog;
	unsigned long LicenceConfirmServer1IP;
	unsigned short LicenceConfirmServer1Port;
	unsigned long LicenceConfirmServer2IP;
	unsigned short LicenceConfirmServer2Port;

	unsigned long RegisterServerA_IP;
	unsigned short RegisterServerA_Port;
	unsigned long RegisterServerB_IP;
	unsigned short RegisterServerB_Port;

	int  PortConfigMode;
	int iCallerNoForSend_PiccMode;	//默认为0，不透传；1表示透传第一方主叫；2表示透传第一方被叫；
	
    string ACDRedisIP;
    uint16_t ACDRedisPort;
    int ACDRedisDB;
    string ACDRedisPassword;

}SIPCCConfig;

enum ERegisterMode{
	REGISTER_NONE = 0,
	REGISTER_DOUBLE,
	REGISTER_DOMAIN,
};

typedef struct __CallDeliveredCache
{
	unsigned long createTime;
	boost::shared_ptr<SIPCCMsg_ACD_CallDelivered> callDelivered;

}CallDeliveredCache;

class CUtility  
{
public:
    //已整理部分
    void func_UserCallDepart(SIPCCMsg_Client_UserMakeCall *userMakeCall, unsigned long ip, unsigned short port, bool bIsOverflow);
    void func_AgentCallDepartment(SIPCCMsg_Client_AgentMakeCall *agentMakeCall, unsigned long ip, unsigned short port);
    void func_CClientCallDepartment(SIPCCMsg_CProxy_CMakeCall *cclientMakeCall, unsigned long ip, unsigned short port);
    void func_UserCallAgent(SIPCCMsg_Client_UserMakeCall *userMakeCall, unsigned long ip, unsigned short port, bool bIsOverflow);
    void func_AgentCallAgent(SIPCCMsg_Client_AgentMakeCall *agentMakeCall, unsigned long ip, unsigned short port);
    void func_CClentCallAgent(SIPCCMsg_CProxy_CMakeCall *cclientMakeCall, unsigned long ip, unsigned short port);

public:
	void OutDial_SetAgentCount();
	void OutDial_SetAgentCount(string corpName, string deptName);
	void OnClient_MonitRequest(SIPCCMsg_Client_MonitRequest* ipccMessage, unsigned long ip, unsigned short port);
	void OnClient_SuperDisconnect(SIPCCMsg_Client_SuperDisconnect* ipccMessage, unsigned long ip, unsigned short port);
	void OnClient_GetTalkingAgent(SIPCCMsg_Client_GetTalkingAgent* ipccMessage, unsigned long ip, unsigned short port);
	void OnRecord_RecInfo2ACD(SIPCCMsg_Record_RecInfo2ACD* ipccMessage, unsigned long ip, unsigned short port);
	void OnGateway_CallCleared(SIPCCMsg_Gateway_CallCleared* ipccMessage, unsigned long ip, unsigned short port);
	void OnGateway_CallUserFailed_WpAgent(SIPCCMsg_Gateway_CallUserFailed_WpAgent* ipccMessage, unsigned long ip, unsigned short port);
	void OnClient_BeforeTransfer(SIPCCMsg_Client_BeforeTransfer* ipccMessage, unsigned long ip, unsigned short port);
	void OnGateway_CallAgentConf(SIPCCMsg_Gateway_CallAgentConf* ipccMessage, unsigned long ip, unsigned short port);
	void OnGateway_UserHangupCall(SIPCCMsg_Gateway_UserHangupCall* ipccMessage, unsigned long ip, unsigned short port);
	void OnGateway_AgentExtCallSucc(SIPCCMsg_Gateway_AgentExtCallSucc* ipccMessage, unsigned long ip, unsigned short port);
	void OnGateway_CallAgentFailed(SIPCCMsg_Gateway_CallAgentFailed* ipccMessage, unsigned long ip, unsigned short port);
	void OnGateway_CallAgentExtFailed(SIPCCMsg_Gateway_CallAgentExtFailed* ipccMessage, unsigned long ip, unsigned short port);
	void OnGateway_IsAgentLogin(SIPCCMsg_Gateway_IsAgentLogin* ipccMessage, unsigned long ip, unsigned short port);
	void OnGateway_CancelRingAgentExt(SIPCCMsg_Gateway_CancelRingAgentExt* ipccMessage, unsigned long ip, unsigned short port);
	void OnGateway_StopRingAgentExtConf(SIPCCMsg_Gateway_StopRingAgentExtConf* ipccMessage, unsigned long ip, unsigned short port);
	void OnGateway_AgentHangupCall(SIPCCMsg_Gateway_AgentHangupCall* ipccMessage, unsigned long ip, unsigned short port);
	void OnGateway_AgentAnswerCall(SIPCCMsg_Gateway_AgentAnswerCall* ipccMessage, unsigned long ip, unsigned short port);
	void OnGateway_RingAgentExtFailed(SIPCCMsg_Gateway_RingAgentExtFailed* ipccMessage, unsigned long ip, unsigned short port);
	void OnGateway_RingAgentExtConf(SIPCCMsg_Gateway_RingAgentExtConf* ipccMessage, unsigned long ip, unsigned short port);
	void OnGateway_QueryGWConf(SIPCCMsg_Gateway_QueryGWConf* ipccMessage, unsigned long ip, unsigned short port);
	static UINT  ThreadDelayHangup(void* lp);
	void OnSysManage_DelACDCorp(SIPCCMsg_SysManage_DelACDCorp *delACDCorp, unsigned long ip, unsigned short port);
	void OnClient_MediaReady(SIPCCMsg_Client_MediaReady* mediaReady, unsigned long ip, unsigned short port);
	void MakeSendMsgLog(SIPCCMessage* ipccmessage, unsigned long ip, unsigned short port);
	void OnClient_GetGroupRequest(SIPCCMsg_Client_GetGroupRequest* getGroupRequest, unsigned long ip, unsigned short port);
	void OnClient_GetDepartNAgentInfo(SIPCCMsg_Client_GetDepartNAgentInfo* getDepartNAgentInfo, unsigned long ip, unsigned short port);
	bool SplitCorpAndDepart(string sTempStr, string& sCorp, string& sDepart);
	void OnClient_AgentUnHoldCall(SIPCCMsg_Client_AgentUnHoldCall *agentUnHoldCall, unsigned long ip, unsigned short port);
	void OnClient_AgentHoldCall(SIPCCMsg_Client_AgentHoldCall *agentHoldCall, unsigned long ip, unsigned short port);
	void OnClient_ClearPreConnection(SIPCCMsg_Client_ClearPreConnection* clearPreConnection, unsigned long ip, unsigned short port);
	void OnClient_TransferCall(SIPCCMsg_Client_TransferCall* transferCall, unsigned long ip, unsigned short port);
	void OnClient_TransReady(SIPCCMsg_Client_TransReady* transReady, unsigned long ip, unsigned short port);
	void OnClient_PrepareTransfer(SIPCCMsg_Client_PrepareTransfer* prepareTransfer, unsigned long ip, unsigned short port);
	void OnClient_CallUserFailed(SIPCCMsg_Client_CallUserFailed* ipccMessage, unsigned long ip, unsigned short port);
	void OnClient_AgentSwitchToFrontDesk(SIPCCMsg_Client_AgentSwitchToFrontDesk* ipccMessage, unsigned long ip, unsigned short port);
	void OnOutDial_SetOutdialGroup(SIPCCMSG_OutDial_SetOutdialGroup* ipccMessage, unsigned long ip, unsigned short port);
	void OnOutDial_ExitOutDial(SIPCCMSG_OutDial_ExitOutDial* ipccMessage, unsigned long ip, unsigned short port);
	void OnOutDial_JoinOutDial(SIPCCMSG_OutDial_JoinOutDial* ipccMessage, unsigned long ip, unsigned short port);
	void OnSysManage_MoveAgent(SIPCCMSG_SysManage_MoveAgent* ipccMessage, unsigned long ip, unsigned short port);
	void OnLicence_QueryConf(SIPCCMsg_Licence_QueryConf* ipccMessage, unsigned long ip, unsigned short port);

	bool m_bNotifyDataCollect;
	int m_iPreNotifyMinute;
	static UINT Thread_NotifyDataCollect(void* lpThis);
	string CreateCallID();
	string GetNewInvokeID();
	SIPCCConfig m_IPCCConfig;
	void SendIPCCMsg_UDP(SIPCCMessage &ipccmessage, unsigned long ip, unsigned short port, string file = "", int line = 0, int nReSendCount = 10, bool makelog = true);
	void SendToMonitorServer(SIPCCMessage &ipccmessage);
	static void destroy();
	static CUtility* GetInstance();
	static UINT   ThreadSockEngine(void* lpThis);
	void Release();
	void Init();
	virtual ~CUtility();
	void OnUserMakeCall(SIPCCMsg_Client_UserMakeCall* userMakeCall, unsigned long ip, unsigned short port, bool bIsOverflow = false);	//IsOverflow是溢出标识，标注OnUserMakeCall是否由溢出引起
	void OnClient_ReturnToAgent(SIPCCMsg_Client_ReturnToAgent* returnToAgent, unsigned long ip, unsigned short port);
	unsigned long m_sOutdialIp;
	unsigned short m_sOutdialPort;

	int m_iLicenceConfirmMode;
	unsigned long m_tLicenceConfirmTime;
	int m_iLicenceConfirmCount;
	string m_sLicenceConfirmTag;

	int ConferenceAgentHuageup(CAgentInfo* pAgentInfo,unsigned long ip, unsigned short port,string strmsg,bool bsendUStatusserver=true,bool reLogin=false);

	void OnAddSkillGroup(SIPCCMsg_SysManage_AddSkillGroup* addSkillGroup, unsigned long ip, unsigned short port);
	void OnDelSkillGroup(SIPCCMsg_SysManage_DelSkillGroup* delSkillGroup, unsigned long ip, unsigned short port);

	void LoadTestContext();

	void OnPhoneSearch_SMSInfoConf(SIPCCMsg_PhoneSearch_SMSInfoConf *smsInfoConf, unsigned long ip, unsigned short port);
	void OnSysManage_AgentInfoChange(SIPCCMsg_SysManage_AgentInfoChange* pMsg, unsigned long ip, unsigned short port);

	void OnFileTransfer_UploadRequest(SIPCCMsg_FileTransfer_UploadRequest *pMsg, unsigned long ip, unsigned short port);

	void OnCProxy_CMakeCall(SIPCCMsg_CProxy_CMakeCall* cclientMakeCall, unsigned long ip, unsigned short port);
	void OnCProxy_HangupCall(SIPCCMsg_CProxy_HangupCall* cclientHangupCall, unsigned long ip, unsigned short port);
	void OnNotifyMsg(SIPCCMsg_OneKeySwitch_NotifyMsg* notifyMsg, unsigned long ip, unsigned short port);
	void NotifyMsg(SIPCCMsg_OneKeySwitch_NotifyMsg* notifyMsg, unsigned long ip, unsigned short port);
    void On_SIPCCMsg_OneKeySwitch_ServiceUnavailable(SIPCCMsg_OneKeySwitch_ServiceUnavailable* msg,
        unsigned long src_ip, unsigned short src_port);
    void On_SIPCCMsg_Client_QueryAgentsState(SIPCCMsg_Client_QueryAgentsState* msg,
        unsigned long src_ip, unsigned short src_port);
private:
    void OnMakeCallViaClient(SIPCCMsg_MakeCallViaClient* makeCallViaClient, unsigned long ip, unsigned short port);
	void On_SIPCCMsg_Client_AgentMakeCall(SIPCCMsg_Client_AgentMakeCall* agentMakeCall, unsigned long ip, unsigned short port);
	void OnClient_UserAnswerCall(SIPCCMsg_Client_UserAnswerCall* userAnswerCall, unsigned long ip, unsigned short port);
	void OnClient_UserAnswerCall_Real(SIPCCMsg_Client_UserAnswerCall_Real* userAnswerCall, unsigned long ip, unsigned short port);
	void OnClient_CallUserConf(SIPCCMsg_Client_CallUserConf* callUserConf, unsigned long ip, unsigned short port);
	void OnAgentReset(SIPCCMsg_Client_AgentReset* agentReset, unsigned long ip, unsigned short port);
	void OnAgentQueryState(SIPCCMsg_Client_AgentQueryState* agentQueryState, unsigned long ip, unsigned short port);
	void OnGetCallingQueueInfo(SIPCCMsg_Client_GetCallingQueueInfo* getCallingQueueInfo, unsigned long ip, unsigned short port);
	void OnAgentCallReceived(SIPCCMsg_Client_AgentCallReceived* agentCallReceived, unsigned long ip, unsigned short port);
	void ScanAgentMap();
	void OnHeartBeat(SIPCCMsg_HeartBeat* heartBeat, unsigned long ip, unsigned short port);
	bool LoadConfig();
	void OnMediaRequest(SIPCCMsg_Client_MediaRequest* mediaRequest, unsigned long ip, unsigned short port);
	void OnDelDepartment(SIPCCMsg_SysManage_DelDepartment* delDepartment, unsigned long ip, unsigned short port);
	void SetRcvSockThreadEnd();
	void OnAgentHangupCall(SIPCCMsg_Client_AgentHangupCall *agentHangupCall, unsigned long ip, unsigned short port);
	void OnAgentAnswerCall(SIPCCMsg_Client_AgentAnswerCall *agentAnswerCall, unsigned long ip, unsigned short port);
	void OnAgentUnPark(SIPCCMsg_Client_AgentUnPark *agentUnPark, unsigned long ip, unsigned short port);
	void OnAgentPark(SIPCCMsg_Client_AgentPark *agentPark, unsigned long ip, unsigned short port);
	void OnAgentLogout(SIPCCMsg_Client_AgentLogout *agentLogout, unsigned long ip, unsigned short port);
	void OnAgentLogoutDeparts(SIPCCMsg_Client_AgentLogoutDeparts *agentLogout, unsigned long ip, unsigned short port);	// [scq/2010-04-23]
	void OnAgentLogin(SIPCCMsg_Client_AgentLogin *agentLogin, unsigned long ip, unsigned short port);
	void OnUserHangupCall(SIPCCMsg_Client_UserHangupCall *userHangupCall, unsigned long ip, unsigned short port);
	void OnAddDepartment(SIPCCMsg_SysManage_AddDepartment* addDepartment, unsigned long ip, unsigned short port);
	void OnSetOutdialCase(SIPCCMsg_Client_SetOutdialCase* pMsg, unsigned long ip, unsigned short port);
	//bool DisposeSockMsg(char* pmsg, int msgsize, unsigned long ip, unsigned short port);
	bool DisposeSockMsg(char *pmsg, int msgsize, const string &strIp, const string &strPort);
	bool m_bEgnRun;
	void MainSockEngine();
	unsigned long m_iPreTimeMark;

	int m_iAgentsLicence;
public://add code by hyj 2013-8-27
	string m_sLogTransSign0;
	string m_sLogTransSign1;
	string m_sLogTransSign2;
	string m_sLogTransSign3;
	string GetMsgLogTransContext(SIPCCMessage* pMsg);
	string GetMsgLogTransContext(SIPCCMessage* pMsg, int iPrefix);
	string ConvertNatIP2HostIP(long lip);
	string ConvertNatPort2HostPort(int nport);
public:
	int           m_ServerHeartoperationMode;	
	unsigned long m_Abnormatime;
	//////////////void          SendRegMSG2RegServer();
protected:
	CUtility();
	static CUtility* __Utility;
private:
	int m_iLicenceCode;
	bool m_bShouldCNT;
	int m_MsgCNT;
	void OnGateway_GetDeptQueueNum(SIPCCMsg_Gateway_GetDeptQueueNum* ipccMessage, unsigned long ip, unsigned short port);
	void OnGateway_GetAgentState(SIPCCMsg_Gateway_GetAgentState* ipccMessage, unsigned long ip, unsigned short port);
	void OnGateway_GetDeptWaitingAgentNum(SIPCCMsg_Gateway_GetDeptWaitingAgentNum* ipccMessage, unsigned long ip, unsigned short port);
	void OnGateway_GetDeptLoginAgentNum(SIPCCMsg_Gateway_GetDeptLoginAgentNum* ipccMessage, unsigned long ip, unsigned short port);
	void OnGetACDLicence(SIPCCMsg_SysManage_NotifyACDLicence* licence, unsigned long ip, unsigned short port);
	void OnGetDeptOverflow(SIPCCMsg_Gateway_GetDeptOverflow* licence, unsigned long ip, unsigned short port);
	void OnWpAgent_LineNo(SIPCCMsg_Gateway_WpAgent_LineNo* pMsg, unsigned long ip, unsigned short port);
	void OnWpAgentRelease(SIPCCMsg_Gateway_WpAgentRelease* pMsg, unsigned long ip, unsigned short port);
	void OnGateway_WpAgentCallAnswerred(SIPCCMsg_Gateway_WpAgentCallAnswerred* ipccMessage, unsigned long ip, unsigned short port);

public://Conference being add code by hyj 2012-12-12

	void OnGateway_ConferenceStartConf(SIPCCMsg_Gateway_ConferenceStartConf& ipccMessage, unsigned long ip, unsigned short port);
	void OnClient_ConferenceEnd(SIPCCMsg_Client_ConferenceEnd& ipccMessage, unsigned long ip, unsigned short port);
    void OnGateway_UserLeaveConference(SIPCCMsg_Gateway_UserLeaveConference& ipccMessage, unsigned long ip, unsigned short port);
    void PrintConferenceTypeMessage(int nType);
public: //end  add code 
	void OneKeySwitch_GetServiceConf(SIPCCMsg_OneKeySwitch_GetServiceConf& ipccMessage, unsigned long ip, unsigned short port);
	void OneKeySwitch_ServiceConfirmConf(SIPCCMsg_OneKeySwitch_ServiceConfirmConf& msg, unsigned long ip, unsigned short port);
	void CheckGetServer();
	void OneKeySwitch_SwitchService_Ctrl(SIPCCMsg_OneKeySwitch_SwitchService_Ctrl& msg, unsigned long ip, unsigned short port);

	unsigned long m_DataCenterServerTime;
	unsigned long m_UstateServerTime;
	unsigned long m_SysManageServerTime;

public:
	string   ShowAgentState(int state);
public:
	unsigned long m_iSendOutDialStateTime;
	void CheckSendOutDialState();
	
	unsigned long m_iCheckOverflowTime;
	void CheckOverflow();

	map<string, long> m_RingAgentExtMap;
	unsigned long m_CheckRingAgentExtTime;
	void CheckRingAgentExtTime();
	void MyCheckRingAgentExtTime();
	void OnRingAgentExtTimeOut(string sAgent);
	void OnClientSendStrMsg(SIPCCMsg_Client_SendStrMsg* clientSendStrMsg, unsigned long ip, unsigned short port);	//  [3/2/2009 lixg]
	void OnOperatorSendStrMsg(SIPCCMsg_Operator2ACD_SendStrMsg* OperatorSendStrMsg, unsigned long ip, unsigned short port);

public:
    //文本会话呼入入口
	void OnMakeMessageCall(SIPCCMsg_CProxy_CMakeCall & makecall, unsigned long ip, unsigned short port);
    void OnMakeMessageCall(SIPCCMsg_Client_AgentMakeCall & makecall, unsigned long ip, unsigned short port);
	
    //文本会话结束入口
    void OnHangupTXTCall(SIPCCMsg_CProxy_HangupCall & hangupcall, unsigned long ip, unsigned short port);
    void OnHangupTXTCall(SIPCCMsg_Client_AgentHangupCall & hangupcall, unsigned long ip, unsigned short port);

	void OnHangupEMailCall(SIPCCMsg_CProxy_HangupCall & hangupcall, unsigned long ip, unsigned short port);
	void OnHangupEMailCall(SIPCCMsg_Client_AgentHangupCall & hangupcall, unsigned long ip, unsigned short port);
    
    //文本会话座席挂断入口
    void OnAnswerTXTCall(SIPCCMsg_Client_AgentAnswerCall & answerCall, unsigned long ip, unsigned short port);
	void OnAnswerEmailCall(SIPCCMsg_Client_AgentAnswerCall & answerCall, unsigned long ip, unsigned short port);

	//文本会话记录入库
	void send_SIPCCMsg_Client_MessageCallInfo(ECallType callType,
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
											);

	//文本会话超时清理
	void ScanTXTSessionTimeout();
	unsigned long m_iPreSessionTimeMark;

public:
	RedisServer_ACD * Get_RedisServer_ACD() { return m_redisServer_ACD; };

private:
	void OnMakeMessageCallToAgent(SIPCCMsg_CProxy_CMakeCall & makecall, unsigned long ip, unsigned short port);
	void OnMakeMessageCallToDepartment(SIPCCMsg_CProxy_CMakeCall & makecall, unsigned long ip, unsigned short port);
	void OnMakeMessageCallToAgent(SIPCCMsg_Client_AgentMakeCall & makecall, unsigned long ip, unsigned short port);
	void OnMakeMessageCallToDepartment(SIPCCMsg_Client_AgentMakeCall & makecall, unsigned long ip, unsigned short port);

	////////////std::multimap<string, string> m_mapUserWebChatCall; //user->callid

public:
	//APP Agent call delivered 超时检测

	bool IsAllowedPush(){ return  m_bAllowedPush; }
	
	void Push_Message(std::string account, ECallType calltype, std::string callid );

	bool APNs_Push_Test();
	void RemoveAgentForPushFailed(string agentID);
	std::map<string, CallDeliveredCache> m_CallDeliveredMap;  //<agentID, >
	bool m_bAllowedPush;

private:
	unsigned long m_iCheckCallDeliveredTime;
	//boost::scoped_ptr<CAsyncHttpClient> m_AsyncHttpClient_ptr;
	//boost::scoped_ptr<JWTEncode> m_JWTEncode_ptr;

private:
	bool CheckCallerStateBeforeMakeCall(CAgentInfo* caller, int& reason);
	bool CheckCalleeStateBeforeMakeCall(CAgentInfo* callee, int& reason);
	bool CheckStateBeforeMakeCall(CAgentInfo* caller, CAgentInfo* callee, int& reason);

private:
	string ReadText(CIni& ini_file, string category, string key, string defaultValue = "");
///////////////////////////////[ get service from server //////////////////
private:
	bool GetService(string querystr);
	void OnGetService(string invokeid, vector<ServiceInfo> & serviceVec);
	void OnGetService_Redirect(string invokeid, vector<ServiceInfo> & serviceVec, vector<RedisDBInfo> redisVec);
	/////////////////////////////// get service from server ]//////////////////

private:
	void MakeLog(string logmsg);

private:
	RegisterServer *m_registerServer;

	string	m_sRedirectServerDomain;
	RedirectServer *m_redirectServer;

	RedisServer_ACD *m_redisServer_ACD;
private:
	CCreditUDP *m_mySock;
	creditudp_transport_ptr m_SockHolder;

};

inline ACDServer::call_type_t to_call_type_t(ECallType calltype)
{
    switch (calltype)
    {
    case E_NORMAL_CALL:
        return ACDServer::PSTN_CALL;
    case E_MESSAGE_CALL:
        return ACDServer::MESSAGE_CALL;
    case E_WEBCHAT_CALL:
        return ACDServer::WEBCHAT_CALL;
	case E_EMAIL_CALL:
		return ACDServer::EMAIL_CALL;
    }

    return ACDServer::PSTN_CALL;
}

#endif // !defined(AFX_UTILITY_H__B96D7F2F_9644_4E77_AA0C_7CF932223D4E__INCLUDED_)
