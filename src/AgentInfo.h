// AgentInfo.h: interface for the CAgentInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGENTINFO_H__C194E369_B009_41B4_B9A1_6EAB5F42C582__INCLUDED_)
#define AFX_AGENTINFO_H__C194E369_B009_41B4_B9A1_6EAB5F42C582__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UserInfo.h"

#include <string>
#include <list>

#include "call.h"
#include "GlobalDefs.h"
#include "PriorityQueue.h"
#include "public/IPCCMessage.h"

using namespace std;

struct AgentSkillInfo
{
	AgentSkillInfo()
		: qualification(0)
		, level(0)
	{}

	AgentSkillInfo(string gid, string gName, int glevel, int gqualification)
		: id(gid)
		, name(gName)
		, qualification(gqualification)
		, level(glevel)
	{}

	string id;
	string name;
	int level;
	int qualification;
};

class CSkillGroupInfo;
class CDepartmentInfo;
class CCClientInfo;
class CAgentInfo  
{
public:
	CAgentInfo();
	virtual ~CAgentInfo();

public:
    //顶层接口
    //1、设置座席状态
    void SetState(int state, bool bIsAgentLogin = false);

    //2、来电接口
    bool CallIn(string callerid, int callertype, void * caller = NULL);
    bool CallIn(CUserInfo* pUser);
    bool CallIn(CAgentInfo* callerAgent);
    bool CallIn(CCClientInfo* callerCAgent);

    //登录所有技能组包括文本技能组
    void LoginAllSkillGroup();

    void LoginSkillGroup(string id, string name, int level = 3, int qualification = 3);
    void LogoutAllSkillGroup();
    void LogoutSkillGroup(string id);

    void LoginTXTSkillGroup(string id, string name, int level = 3, int qualification = 3);
    void LogoutAllTXTSkillGroup();
    void LogoutTXTSkillGroup(string id);

public:
	CDepartmentInfo* IsInOutDialDepartment(string sDepartment);	// [ scq/2010-02-23 ]
	void SetOutDialSysUUIDataInfo(string uuiData);
	bool IsAgentCallDepartOrAgent();
	void CheckPhoneNo();
	void SetClientAgentState(int state, int operType=0,string gwAgentLine="", string gwUserLine="", string transedAgentID="", string transedGwAgentLine="", unsigned long gwAgentLineIP=0, unsigned short gwAgentLinePort=0, unsigned long gwUserLineIP=0, unsigned short gwUserLinePort=0, unsigned long transedGwAgentLineIP=0, unsigned short transedGwAgentLinePort=0);
	void SetCallID(string callid);
	void SetDormancy(bool bDormancy);
	void AgentRelive();
	void SendVDNStatusInfo();
	string MakeCorpNDepart();
	void UpdateUserInfo(CUserInfo* pUserInfo);
	int GetCurState();
	//////void AddSkillToMap(string sSkillGroupName, string sSkillGroupid, int iSkillLevel, int iSkillQualification=0);
	//////void DelSkillFromMap(string sSkillGroupName, string sSkillGroupid, int iSkillLevel=-1);
	//////void ClearSkillMap();
	//////void DelFromSkillGroupMap();
	string GetAgentStateByStr(void);

	//////void AddTXTSkillToMap(string sSkillGroupid, string sSkillGroupName, int iSkillLevel, int iSkillQualification = 0);
	//////void DelTXTSkillFromMap(int iSkillLevel, string sSkillGroupid);
	//////void DelTXTSkillFromMap(string sSkillGroupid);
	//////void ClearTXTSkillMap();
	//////void LoginToTXTSkill();
	void ClearTXTCalls();

	/******* TXT call 超时处理 begin ********/	
	void SetTXTSessionTimeout(int32_t timeout);
	int32_t GetTXTSessionTimeout() const
	{
		return txtSessionTimeout;
	}

	void UpdateTXTSessionTime(string callid, int32_t nomsgTime);
	void ClearTXTSessionTimeout();

	/********  TXT call 超时处理 end  *********/

	//呼叫本座席，如果失败给客户端返回确认
	void On_SIPCCMsg_Client_AgentMakeCall(SIPCCMsg_Client_AgentMakeCall & makecall, unsigned long src_ip, unsigned short src_port);
	void On_SIPCCMsg_CProxy_CMakeCall(SIPCCMsg_CProxy_CMakeCall & makecall, unsigned long src_ip, unsigned short src_port);

	//检查是否可以发起呼叫
	int CheckForMakeCall(ECallType callType, bool ignoreCallCountLimt = false);

	//呼叫本座席，如果失败没有任何动作发生
	int MakeCall(SIPCCMsg_Client_AgentMakeCall & makecall, unsigned long src_ip, unsigned short src_port);
	int MakeCall(SIPCCMsg_CProxy_CMakeCall & makecall, unsigned long src_ip, unsigned short src_port);
    int MakeCall(string caller, __CallPartyType callertype,
        string called, __CallPartyType calledtype, string keyword,
        string callid, ECallType callType,
        string uuidata, CAudioStreamDescription & asd,
        string originalagentid, string originalcallid,
        unsigned long src_ip, unsigned short src_port, bool ignoreCallCountLimt = false);

    //Txt call, message call
    int MakeTXTCall(string caller, __CallPartyType callertype,
        string called, __CallPartyType calledtype, string keyword,
        string callid, ECallType callType, string uuidata, 
        string originalagentid, string originalcallid,
        unsigned long src_ip, unsigned short src_port, bool ignoreCallCountLimt = false);

    int MakeEmailCall(string caller, __CallPartyType callertype,
        string called, __CallPartyType calledtype, string keyword,
        string callid, string uuidata, string originalagentid, string originalcallid,
        unsigned long src_ip, unsigned short src_port);

public:	
	void DeliverCall(SIPCCMsg_CProxy_CMakeCall & makecall, unsigned long src_ip, unsigned short src_port);
    void OnCClientHangupTXTCall(SIPCCMsg_CProxy_HangupCall & hangupcall, unsigned long src_ip, unsigned short src_port);
	void OnHangupTXTCall(SIPCCMsg_Client_AgentHangupCall & hangupcall, unsigned long src_ip, unsigned short src_port);
    void OnAnswerTXTCall(SIPCCMsg_Client_AgentAnswerCall & answercall, unsigned long src_ip, unsigned short src_port);

	void OnHangupEmailCall(SIPCCMsg_CProxy_HangupCall & hangupcall, unsigned long src_ip, unsigned short src_port);
	void OnHangupEmailCall(SIPCCMsg_Client_AgentHangupCall & hangupcall, unsigned long src_ip, unsigned short src_port);

	void OnAnswerEmailCall(SIPCCMsg_Client_AgentAnswerCall & answercall, unsigned long src_ip, unsigned short src_port);
	void OnAgentSwitchToFrontDesk(SIPCCMsg_Client_AgentSwitchToFrontDesk &switchtofrontdesk, unsigned long src_ip, unsigned short src_port);

	bool IsFreeForTXTCall()
	{
		switch (m_iCurState)
		{
		case Agent_Waiting: case Agent_ForeProcess: case Agent_BackProcess:
			return true;
		default:
			return false;
		}
	}

    bool CanAcceptTXTCall(bool ignoreCallCountLimt = false)
    {
        return (m_bAllowTXTCall
            && IsFreeForTXTCall()
            && (ignoreCallCountLimt || (txt_calls.size() < m_iMaxTXTDialogCount)));
    };

    bool CanAcceptEmailCall(bool ignoreCallCountLimt = false)
    {
        return m_bAllowTXTCall;
    };

    bool IsPC()
	{
		return (m_iLoginPlatform == OSType_WindowsPC);
	}

	bool IsIOS()
	{
		return (m_iLoginPlatform == OSType_IOS);
	}

	bool IsSuspended()
	{
		return (bAgentDormancy && !IsPC());
	}

	bool ExistPushEvent()
	{
		return !m_mapCallDelivered.empty();
	}
	//检查是否可以推送消息
	bool IsAllowedPush();

	void SetSuspend();
	void SetDeviceToken(std::string token) { deviceToken = token; }

	void NotifyMsg(SIPCCMsg_OneKeySwitch_NotifyMsg &notifyMsg);
	void OnDeptmartmentRemoved();
	void SelectTXTCall();

	void addListCallDelivered(boost::shared_ptr<SIPCCMsg_ACD_CallDelivered> callDelivered);
	void delCallDelivered(string callid);
	void SendCallDelivered();
	void ClearListCallDelivered();

    int GetEmailCallCount()
    {
        return m_iEmailCallCount;
    };

public:
    string to_str() {
        stringstream ss;
        ss << m_sAgentID << "|"
            << m_bCallFromVDN << "|"
            << m_bCallFromQueue << "|"
            << m_sCalledCorp << "|"
            << m_sCalledDepart;
        return ss.str();
    }

private:
    bool func_AgentWaiting();
    bool func_AgentBusy();

public:
	unsigned long m_iTalkBeginTime;
	string m_sNickName;
	string m_sTheOtherAgentID; //另一个AgentID
	string m_CurGwUserLineNo;
	unsigned long m_gwCtrlIP;
	unsigned short m_gwCtrlPort;
	unsigned short m_CurGwLinePort;
	unsigned long m_CurGwLineIP;
	unsigned short m_CurUserLinePort;
	unsigned long m_CurUserLineIP;
	string m_WebPstnAgentCurGwLine;
	string m_sIPNo_FirstUser;
	string m_sIPNo_SecondUser;
	string m_sIPNo_TheAgent;
	string m_sIPNo_OtherAgent;
	int m_iLoginType;
	string m_sAgentPhoneNo;
	unsigned long m_iPreHeartBeatMark;
	unsigned long m_iRingStateMark_OnlyPSTN;
	int m_iAgentNatType;
	string m_sAgentID;
	string m_sCorporation;

	string m_sDeptNickName;
	string m_sDepartment;
	string m_sDeptid;
	AgentSkillInfo m_sCurSkillInfo;
	AgentSkillInfo m_sOriSkillInfo;

	string m_sSkillList;

	string m_sManagerid;
	unsigned long m_ulAgentIP;
	unsigned short m_usAgentPort;
	unsigned long m_ulLanIP;
	unsigned short m_usLanMainSockPort;
	string m_sCallID;
	int m_iCurState;				//座席当前状态
	string m_sCurStateBeginTime;	//当前状态开始时间
	unsigned long m_iStateBeginTime;			//
	int m_iPreState;				//座席前一状态
	string m_sPreStateBeginTime;	//前一状态开始时间

	string m_sUUIData;				//uuiData

	bool bCallin;
	bool bInnerCall;
	bool bPSTNCall;
	bool m_bPSTNHungup;				//是否外线挂机

	//本座席是被叫
	CUserInfo m_userInfo; //待移除应该使用map中userinfo取代
    bool m_bCallFromVDN;
    bool m_bCallFromQueue;
    unsigned long m_iAnswerTimeSpan;			//响应时长

	//本座席是主叫
	int m_iCallType;                //呼叫类型
	int m_iCalledPartyType;			//被叫类型
	string m_sCalledParty;			//被叫方
	string m_sCalledCorp;			//呼叫的公司
	string m_sCalledDepart;			//呼叫的部门
	unsigned long m_iAgentEnqueueTime;		//本座席进入呼叫队列时间
	unsigned long m_iAgentQueueSpanTime;		//本座席在队列中排队时长
	string m_sCalledNo;				//座席外呼号码_仅保存普通外呼号码

	bool bAgentDormancy;			//是否心跳停止
	unsigned long m_iDormancyBeginTime;		//

	int m_iParkReasonID;

	string m_sParkReason;

	unsigned long sipAgentPhoneIP1;
	unsigned short sipAgentPhoneRtpPort1;
	unsigned short sipAgentPhoneRtcpPort1;
    CAudioStreamDescription audioStreamDescription;

	OutDialSys_Info m_OutDialSys_Info;
	string agentUUIData;	// 随路数据 [2/23/2009 lixg]

	string originalData;	//待转移的原数据
	string originalAgentID; //原座席ID
	string originalCallID;  //原callid

	string m_sPrivateData;

	string comSymbol;

	string m_sQueueArriveTime;		//客户从排队到达坐席的时间

	bool bCallin_Hold;
	bool bInnerCall_Hold;
	bool bPSTNCall_Hold;
	string m_sCallID_Hold;

	bool bExitOutdial;

	string m_sExtNo;	//坐席分机号
	//会议 hold conference mssage add by hyj 2012-12-12
	string      firstName;//　　　//主控方座席ID
    string      firstLineNo;//第一外线UserName
    string      thirdName;//第三参会方，如果是座席就填座席ID，如果是中继就填UserName
	int         conferenceType;//　三方类型，1表示两座席一外线，2表示一座席两外线，3表示一座席一外线，4表示两外线，5表示两座席；
    enum __AgentJoinType         ConferenceRole;//-1 不在会议中，0是主持人，1参与者
	string      strConferenceID;//会议ID
	bool        bApplyforConference;

	string       firstPartyLineNo;//主控方座席对应的ConferenceAgent编号
    string       secondPartyLineNo;	//参会方座席对应的ConferenceAgent编号
	void        ClearConferenceParam();

	bool	m_bSendHangupCall;
	bool	m_bNeedHeartBeat;

	bool	m_bNeedCheck;

	bool m_bLoadFromRedis;
	unsigned long m_ulLoadInfoTime;

	map<string, ACDServer::call_ptr> txt_calls;
	int m_iTotalTXTCallCount;

	map<string, ACDServer::call_ptr> email_calls;

	bool   m_bAllowAudioCall;
	bool   m_bAllowTXTCall;
	uint8_t m_iMaxTXTDialogCount;

    CPriorityQueue<AgentSkillInfo> m_SkillList;
    CPriorityQueue<AgentSkillInfo> m_TXTSkillList;

    int m_iEmailCallCount;

	int32_t m_iLoginPlatform;        //座席登录平台，0:WindowsPC，1：Andriod,2:IOS,
	map<string,boost::shared_ptr<SIPCCMsg_ACD_CallDelivered> > m_mapCallDelivered;  //若app已经进入后台，保存信令上下文，等待唤醒后再发送
	string latestSuspendTime;//最后一次挂起时间
	int PushFlag;//推送标识，心跳恢复则清此标识,用来检测app是否应该被删除

	int32_t txtSessionTimeout; //文本会话无消息超时时长,单位秒(s)

	string preHeartTime;
	string deviceToken;

	//int SetState_Redis();
};

#endif // !defined(AFX_AGENTINFO_H__C194E369_B009_41B4_B9A1_6EAB5F42C582__INCLUDED_)
