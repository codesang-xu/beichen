// Department.h: interface for the CDepartment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEPARTMENT_H__4B8AFC44_F08D_4E6F_BC9F_9DEC3A0CB8EA__INCLUDED_)
#define AFX_DEPARTMENT_H__4B8AFC44_F08D_4E6F_BC9F_9DEC3A0CB8EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <list>
#include <map>

#include <boost/scoped_ptr.hpp>

#include "ACDAlgorithm.h"
#include "AgentInfo.h"
#include "GlobalDefs.h"
#include "PriorityQueue.h"
#include "public/GlobalOperation.h"
#include "public/IPCCMessage.h"
#include "UserInfo.h"

using namespace std;

typedef struct __CallerPartyInfo
{
	string partyName;
	int	partyType;

	bool operator==(const struct __CallerPartyInfo & info)
	{
		if(info.partyName==partyName && info.partyType==partyType)
			return true;
		return false;
	}
}CallerPartyInfo;

class CAgentQueueInfo
{
public:
	CAgentQueueInfo()
	{
		m_ulQueueInTime = gettickcount();
	}

	virtual ~CAgentQueueInfo()
	{

	}

	CAgentQueueInfo(int qualification, string agentid)
	{
		m_iQualification = qualification;
		m_ulQueueInTime = gettickcount();
		m_sAgentId = agentid;
	}

	bool operator < (CAgentQueueInfo tempInfo)
	{
		if ( m_iQualification < tempInfo.m_iQualification )
		{
			return true;
		}
		else if ( m_iQualification > tempInfo.m_iQualification )
		{
			return false;
		}
		else
		{
			if ( m_ulQueueInTime < tempInfo.m_ulQueueInTime )
			{
				return true;
			}
			else
			{
				return false;
			}
		}

	}

	bool operator > (CAgentQueueInfo tempInfo)
	{
		if ( m_iQualification > tempInfo.m_iQualification )
		{
			return true;
		}
		else if ( m_iQualification < tempInfo.m_iQualification )
		{
			return false;
		}
		else
		{
			if ( m_ulQueueInTime < tempInfo.m_ulQueueInTime )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	int	m_iQualification;
	unsigned long m_ulQueueInTime;
	string m_sAgentId;

};

class WaitingList_Finder
{
public:
	WaitingList_Finder(int iQualification, string sAgentId)
	{
		m_iQualification = iQualification;
		m_sAgentId = sAgentId;
	}

	bool operator () ( list<CAgentQueueInfo>::value_type &value )
	{
		return (value.m_iQualification == m_iQualification)&&(value.m_sAgentId == m_sAgentId);
	}

private:
	int m_iQualification;
	string m_sAgentId;

};

class WaitingList_FinderEx
{
public:
	WaitingList_FinderEx(string sAgentId)
	{
		m_sAgentId = sAgentId;
	}

	bool operator () ( list<CAgentQueueInfo>::value_type &value )
	{
		return (value.m_sAgentId == m_sAgentId);
	}

private:
	string m_sAgentId;

};

class MakeCallInfo
{
public:
	MakeCallInfo() {};
	MakeCallInfo(string callID, boost::any makeCallMsgPtr,
		uint32_t clientIP, uint16_t clientPort);
	boost::shared_ptr<vector<char> > serialize();
	bool deserialize(char * buf, int buf_size);
public:
	string callID;
	boost::any makeCallMsgPtr;
	uint32_t clientIP;
	uint16_t clientPort;
	string   startTime;

};

typedef list<MakeCallInfo> CallQueue;
class Agent_Finder;
class CDepartmentInfo  
{
public:
    CDepartmentInfo(string managerid, string corpName, string deptid, string departmentName);
    virtual ~CDepartmentInfo();

public:
    //顶层接口
    //1、C端呼入
    //返回值: 0:参数或内部错误， 1：呼叫分配成功； 2：呼叫排队
    //  first：0，second:空
    //  first：1，second:string 分配到的座席账号
    //  first：2，second:size_t 队列长度
    //  
    pair<int, boost::any> CallIn(string callerid, int callertype, void * caller = NULL, int priority = 3);

    bool AgentLogin(string agentID);
    void AgentLogout(string agentID);

public:
	void GetAgentCount(int& freeCount, int& workCount);
	int GetAgentNum();
    int GetWebAgentCount();
    int GetMobileAgentCount();

    void SendOutDialSys_DeptAgentsInfo();
	string GetAllAndFreeCount(int* pAll, int* pFree);

	void func_SendIdleAgents(string invokeID, unsigned ip, unsigned port);
	void func_SendTXTIdleAgents(string invokeID, unsigned ip, unsigned port);

	map<string, string> &MapAgentsInDepart() { return m_mapAgentsInDepart; }

	void CheckOverflow();
	bool IsTimeout(CUserInfo* pUser);
	void Overflow(CUserInfo* pUser);
	unsigned long m_iOverflowTime;
	string m_sOverflowTarget;
	int m_iOverFlowDestType;
	long m_iMaxRingTime;

	void NotifyMsg(SIPCCMsg_OneKeySwitch_NotifyMsg &notifyMsg);

	void On_SIPCCMsg_CProxy_CMakeCall(SIPCCMsg_CProxy_CMakeCall & makecall, unsigned long ip, unsigned short port);
	void On_SIPCCMsg_Client_AgentMakeCall(SIPCCMsg_Client_AgentMakeCall & makecall, unsigned long ip, unsigned short port);
	int  MakeCall(string caller, __CallPartyType callertype,
		string called, __CallPartyType calledtype, string keyword,
		string callid, ECallType callType,
		string uuidata, string recentContacts, CAudioStreamDescription & asd,
		string originalagentid, string originalcallid,
		unsigned long ip, unsigned short port);

	void OnAgentFreeForTXTCall(CAgentInfo * pAgent);

	void On_SIPCCMsg_CProxy_HangupCall(SIPCCMsg_CProxy_HangupCall & hangupcall, unsigned long ip, unsigned short port);
	void On_SIPCCMsg_Client_AgentHangupCall(SIPCCMsg_Client_AgentHangupCall & hangupcall, unsigned long ip, unsigned short port);

    void PushBackToCallQueue(const SIPCCMsg_Client_AgentMakeCall & makecall,
        unsigned long ip, unsigned short port);
    void PushBackToCallQueue(const SIPCCMsg_CProxy_CMakeCall & makecall,
		unsigned long ip, unsigned short port);

    MakeCallInfo PopFrontFromCallQueue(CallQueue& callqueue);
    bool RemoveFromCQueue(CallQueue& callqueue, string callID);

	int LoadTXTCallQueueFromRedis();

	size_t GetTXTCallQueueSize()
	{
		return m_TXTCallQueue.size();
	}

	void BlockInbound(uint32_t ip, uint16_t port);
	void UnblockInbound();

    void SetQueuingAlgorithm(int32_t iQueuingAlgorithm);

    inline CAgentInfo* GetAgentInfo1(map<string, string>::iterator& agentiter);
    inline CAgentInfo* GetAgentInfo2(string& agentid);
    int ACDGetTXTCallCount(map<string, string>::iterator& agentiter);
    int ACDGetEmailCallCount(map<string, string>::iterator& agentiter);
    inline bool CanAcceptTXTCall(string& agentid);
    inline bool CanAcceptEmailCall(string& agentid);

	void  send_SIPCCMsg_Client_MessageCallInfo(MakeCallInfo makeCallInfo);

public:
    //[排队相关函数
    void QueueToCQueue(bool bPushFront, string callerID, int callerType, int callerPriority = 3, string calldata = "", bool writeRedis = true);
    int PopFromCQueue(string& userName, bool writeRedis = true);
    void RemoveFromCQueue(string callerID, bool writeRedis = true);
    int GetCQueueSize();

    void QueueToWAQueue(string agentID, int iQualification = 3, bool writeRedis = true);
    void RemoveFromWAQueue(string agentID, int iQualification = 3, bool writeRedis = true);
    string PopFromWAQueue(bool writeRedis = true);
    string FrontOfWAQueue();
    bool IsInWAQueue(string agentid) { return m_WaitingAgentQueue.Exist(agentid); };
    int GetWAQueueSize();

    void LoadQueueFromRedis();
    //排队相关函数]

public:
    string m_sManagerid;
    string m_sCorpName;
    string m_sDeptid;
    string m_sDepartmentName;

	OutDialSys_Info m_OutDialSys_Info;
	bool IsmapChanged; //标记是否有座席登录登出

	int m_iDeptType;
	string m_sNickname;

private:
    CPriorityQueue<pair<string, int> > m_WaitingAgentQueue;

    CPriorityQueue<CallerPartyInfo> m_CallQueue;

	map<string, string> m_mapAgentsInDepart;	//agentID->agentInfo

    int32_t m_iQueuingAlgorithm; //排队算法 1最小工作量，2 轮询
    boost::scoped_ptr<ACDAlgorithm> m_AgentFinderForTXTCall_ptr_;
    boost::scoped_ptr<ACDAlgorithm> m_AgentFinderForEmailCall_ptr_;

    CallQueue m_TXTCallQueue;
    CallQueue m_EmailCallQueue;
    int txtCallQueueStatus;
	set<pair<uint32_t, uint16_t> > m_cproxyServers;

	const static int MAX_TXT_CALL_QUEUE_SIZE;

private:
    string m_sRedisAgentQueueName;

};

class CSkillGroupInfo
{
public:
    CSkillGroupInfo(string managerid, string sCorpName, string sSkillGroupid, string sSkillGroupName);
	virtual ~CSkillGroupInfo();

public:
    //顶层接口
    //返回值: 0:参数或内部错误， 1：呼叫分配成功； 2：呼叫排队
    //  first：0，second:空
    //  first：1，second:string 分配到的座席账号
    //  first：2，second:size_t 队列长度
    //  
    pair<int, boost::any> CallIn(string callerid, int callertype, void * caller = NULL, int priority = 3);

    bool AgentLogin(string agentID);
    void AgentLogout(string agentID);

    //排队相关函数
    void QueueToCQueue(bool bPushFront, string callerID, int callerType, int callPriority = 3, string calldata = "", bool writeRedis = true);
    int PopFromCQueue(string& userName, bool writeRedis = true);
    void RemoveFromCQueue(string userName, bool writeRedis = true);

    void QueueToWAQueue(string agentID, int iQualification = 3, bool writeRedis = true);
    void RemoveFromWAQueue(string agentID, int iQualification = 3, bool writeRedis = true);
    string PopFromWAQueue(bool writeRedis = true);
    string FrontOfWAQueue();
    bool IsInWAQueue(string agentid) { return m_WaitingAgentQueue.Exist(agentid); };

    void LoadQueueFromRedis();
    //

public:
    int GetWAQueueSize();
    int GetCQueueSize();
	int GetAgentNum();

public:
    string GetRedisAgentQueueName(){ return m_sRedisAgentQueueName; };

public:
	string m_sManagerid;
	string m_sCorpName;
	string m_sSkillGroupid;
	string m_sSkillGroupName;

    CPriorityQueue<CallerPartyInfo> m_CallQueue;
    CPriorityQueue<pair<string, int> > m_WaitingAgentQueue;/*1、agentid，2、技能等级(删除redis时的必须参数)*/

	map<string, string> m_mapAgentsInSkillGroup;

private:
    string m_sRedisAgentQueueName;


};

class Agent_Finder_InTXTSkill;
class CSkillGroupTXTCall
{
public:
	CSkillGroupTXTCall();
	virtual ~CSkillGroupTXTCall() {};

	void AgentLogin(CAgentInfo * agent_ptr);
	void AgentLogout(CAgentInfo * agent_ptr);

	void AgentWait(CAgentInfo * pAgent);

	int func_CallIn(SIPCCMsg_CProxy_CMakeCall & makecall, unsigned long ip, unsigned short port);
	int func_CallIn(SIPCCMsg_Client_AgentMakeCall & makecall, unsigned long ip, unsigned short port);

	void On_SIPCCMsg_CProxy_HangupCall(SIPCCMsg_CProxy_HangupCall & hangupcall, unsigned long ip, unsigned short port);
	void On_SIPCCMsg_Client_AgentHangupCall(SIPCCMsg_Client_AgentHangupCall & hangupcall, unsigned long ip, unsigned short port);

	void func_SendIdleAgents(string invokeID, unsigned ip, unsigned port);
	void SetQueuingAlgorithm(int32_t iQueuingAlgorithm);

    inline CAgentInfo* GetAgentInfo1(map<string, CAgentInfo*>::iterator& agentIter);
    inline CAgentInfo* GetAgentInfo2(CAgentInfo*& agentPtr);
    inline int ACDGetTXTCallCount(map<string, CAgentInfo*>::iterator& agentIter);
    inline int ACDGetEmailCallCount(map<string, CAgentInfo*>::iterator& agentIter);
    inline bool CanAcceptTXTCall(CAgentInfo*& agentid);
    inline bool CanAcceptEmailCall(CAgentInfo*& agentid);

	void send_SIPCCMsg_Client_MessageCallInfo(MakeCallInfo makeCallInfo);

private:
	int  MakeCall(string caller, __CallPartyType callertype,
		string called, __CallPartyType calledtype, string keyword,
		string callid, ECallType callType, string uuidata, string recentContacts,
		string originalagentid, string originalcallid,
		unsigned long ip, unsigned short port);
   
    void PushBackToCallQueue(const SIPCCMsg_Client_AgentMakeCall & makecall,
		unsigned long ip, unsigned short port);
    void PushBackToCallQueue(const SIPCCMsg_CProxy_CMakeCall & makecall,
		unsigned long ip, unsigned short port);

    MakeCallInfo PopFrontFromCallQueue(CallQueue& callqueue);
    bool RemoveFromCQueue(CallQueue& callqueue, string callID);

	size_t GetTXTCallQueueSize()
	{
		return m_TXTCallQueue.size();
	}

	void BlockInbound(uint32_t ip, uint16_t port);
	void UnblockInbound();

public:
	string m_sSkillGroupName;
	string m_sCorpName;
	string m_sManagerid;
	string m_sSkillGroupid;
	int32_t m_iQueuingAlgorithm; //排队算法 1最小工作量，2 轮询
    boost::scoped_ptr<ACDAlgorithm> m_AgentFinderForTXTCall_ptr_;
    boost::scoped_ptr<ACDAlgorithm> m_AgentFinderForEmailCall_ptr_;

private:
	map<string, CAgentInfo*> m_AgentMap;
    CallQueue m_TXTCallQueue;
    CallQueue m_EmailCallQueue;

	const static int MAX_TXT_CALL_QUEUE_SIZE = 10000;
};

typedef boost::shared_ptr<CSkillGroupTXTCall> CTXTSkillGroupPtr;

#endif // !defined(AFX_DEPARTMENT_H__4B8AFC44_F08D_4E6F_BC9F_9DEC3A0CB8EA__INCLUDED_)
