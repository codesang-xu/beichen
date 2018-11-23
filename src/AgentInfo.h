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
    //����ӿ�
    //1��������ϯ״̬
    void SetState(int state, bool bIsAgentLogin = false);

    //2������ӿ�
    bool CallIn(string callerid, int callertype, void * caller = NULL);
    bool CallIn(CUserInfo* pUser);
    bool CallIn(CAgentInfo* callerAgent);
    bool CallIn(CCClientInfo* callerCAgent);

    //��¼���м���������ı�������
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

	/******* TXT call ��ʱ���� begin ********/	
	void SetTXTSessionTimeout(int32_t timeout);
	int32_t GetTXTSessionTimeout() const
	{
		return txtSessionTimeout;
	}

	void UpdateTXTSessionTime(string callid, int32_t nomsgTime);
	void ClearTXTSessionTimeout();

	/********  TXT call ��ʱ���� end  *********/

	//���б���ϯ�����ʧ�ܸ��ͻ��˷���ȷ��
	void On_SIPCCMsg_Client_AgentMakeCall(SIPCCMsg_Client_AgentMakeCall & makecall, unsigned long src_ip, unsigned short src_port);
	void On_SIPCCMsg_CProxy_CMakeCall(SIPCCMsg_CProxy_CMakeCall & makecall, unsigned long src_ip, unsigned short src_port);

	//����Ƿ���Է������
	int CheckForMakeCall(ECallType callType, bool ignoreCallCountLimt = false);

	//���б���ϯ�����ʧ��û���κζ�������
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
	//����Ƿ����������Ϣ
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
	string m_sTheOtherAgentID; //��һ��AgentID
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
	int m_iCurState;				//��ϯ��ǰ״̬
	string m_sCurStateBeginTime;	//��ǰ״̬��ʼʱ��
	unsigned long m_iStateBeginTime;			//
	int m_iPreState;				//��ϯǰһ״̬
	string m_sPreStateBeginTime;	//ǰһ״̬��ʼʱ��

	string m_sUUIData;				//uuiData

	bool bCallin;
	bool bInnerCall;
	bool bPSTNCall;
	bool m_bPSTNHungup;				//�Ƿ����߹һ�

	//����ϯ�Ǳ���
	CUserInfo m_userInfo; //���Ƴ�Ӧ��ʹ��map��userinfoȡ��
    bool m_bCallFromVDN;
    bool m_bCallFromQueue;
    unsigned long m_iAnswerTimeSpan;			//��Ӧʱ��

	//����ϯ������
	int m_iCallType;                //��������
	int m_iCalledPartyType;			//��������
	string m_sCalledParty;			//���з�
	string m_sCalledCorp;			//���еĹ�˾
	string m_sCalledDepart;			//���еĲ���
	unsigned long m_iAgentEnqueueTime;		//����ϯ������ж���ʱ��
	unsigned long m_iAgentQueueSpanTime;		//����ϯ�ڶ������Ŷ�ʱ��
	string m_sCalledNo;				//��ϯ�������_��������ͨ�������

	bool bAgentDormancy;			//�Ƿ�����ֹͣ
	unsigned long m_iDormancyBeginTime;		//

	int m_iParkReasonID;

	string m_sParkReason;

	unsigned long sipAgentPhoneIP1;
	unsigned short sipAgentPhoneRtpPort1;
	unsigned short sipAgentPhoneRtcpPort1;
    CAudioStreamDescription audioStreamDescription;

	OutDialSys_Info m_OutDialSys_Info;
	string agentUUIData;	// ��·���� [2/23/2009 lixg]

	string originalData;	//��ת�Ƶ�ԭ����
	string originalAgentID; //ԭ��ϯID
	string originalCallID;  //ԭcallid

	string m_sPrivateData;

	string comSymbol;

	string m_sQueueArriveTime;		//�ͻ����Ŷӵ�����ϯ��ʱ��

	bool bCallin_Hold;
	bool bInnerCall_Hold;
	bool bPSTNCall_Hold;
	string m_sCallID_Hold;

	bool bExitOutdial;

	string m_sExtNo;	//��ϯ�ֻ���
	//���� hold conference mssage add by hyj 2012-12-12
	string      firstName;//������//���ط���ϯID
    string      firstLineNo;//��һ����UserName
    string      thirdName;//�����λ᷽���������ϯ������ϯID��������м̾���UserName
	int         conferenceType;//���������ͣ�1��ʾ����ϯһ���ߣ�2��ʾһ��ϯ�����ߣ�3��ʾһ��ϯһ���ߣ�4��ʾ�����ߣ�5��ʾ����ϯ��
    enum __AgentJoinType         ConferenceRole;//-1 ���ڻ����У�0�������ˣ�1������
	string      strConferenceID;//����ID
	bool        bApplyforConference;

	string       firstPartyLineNo;//���ط���ϯ��Ӧ��ConferenceAgent���
    string       secondPartyLineNo;	//�λ᷽��ϯ��Ӧ��ConferenceAgent���
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

	int32_t m_iLoginPlatform;        //��ϯ��¼ƽ̨��0:WindowsPC��1��Andriod,2:IOS,
	map<string,boost::shared_ptr<SIPCCMsg_ACD_CallDelivered> > m_mapCallDelivered;  //��app�Ѿ������̨���������������ģ��ȴ����Ѻ��ٷ���
	string latestSuspendTime;//���һ�ι���ʱ��
	int PushFlag;//���ͱ�ʶ�������ָ�����˱�ʶ,�������app�Ƿ�Ӧ�ñ�ɾ��

	int32_t txtSessionTimeout; //�ı��Ự����Ϣ��ʱʱ��,��λ��(s)

	string preHeartTime;
	string deviceToken;

	//int SetState_Redis();
};

#endif // !defined(AFX_AGENTINFO_H__C194E369_B009_41B4_B9A1_6EAB5F42C582__INCLUDED_)
