// AgentMgr.h: interface for the CAgentMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGENTMGR_H__587CF644_556E_4E51_86DD_4C39DD590E14__INCLUDED_)
#define AFX_AGENTMGR_H__587CF644_556E_4E51_86DD_4C39DD590E14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgentInfo.h"
#include <string>
#include <map>
using namespace std;

class CAgentMgr  
{
public:
	void SendTalkingAgents(string invokeID, string corpName, string deptName, string skillName, unsigned long ip, unsigned short port);
	bool IsAgentExist(string agentID);
	CAgentInfo* GetAnyAgent(string agentID);
	void SetAgentDormancy(string agentid, bool dormancy);
	string GetStateStr(int state);
	void CleanUpAgentMap();
	CAgentInfo* GetAgentInfo(string agentID);
	void SetAgentState(string agentID, int agentState, bool bIsAgentLogin = false);
	void DelAgent(string agentID);
	
	CAgentInfo* AddAgent(string agentID, string nickName, string corpName, string departmentName, int natType, 
		unsigned long ip, unsigned short port, unsigned long lanip, unsigned short lanMainSockPort, 
		int loginType, string agentPhoneNo, int NeedDisposal, string managerid, string deptid, 
		string extNo, string deptNickName, bool allowAudioCall, bool allowTXTCall, int maxTXTCallCount, int32_t loginPlatform,
		string skillGroupId = "", bool needHeartBeat = true);
	virtual ~CAgentMgr();
	static CAgentMgr* GetInstance();
	static void destroy();
	int GetAgentCount();
	map<string, CAgentInfo*> &MapAgents() { return m_mapAgents; }

	bool m_bCheckBak;
	map<string, CAgentInfo*> m_mapCheckOnlineAgents;
	map<string, CAgentInfo*> m_mapCheckOnlineAgents_bak;
	map<string, CAgentInfo*> m_mapPSTNAgents;

protected:
	CAgentMgr();
	static CAgentMgr* __AgentMgr;
private:
	map<string, CAgentInfo*> m_mapAgents;	//agentID->agentInfo
public:
	map<string, string> m_mapUserCallAgent;
	void  OnferenceinAgentTimeout(CAgentInfo* pAgentInfo);

	//////void RemoveFromWAQueue(string agentID);

	void ClearTXTSessionTimeout();
};

#endif // !defined(AFX_AGENTMGR_H__587CF644_556E_4E51_86DD_4C39DD590E14__INCLUDED_)
