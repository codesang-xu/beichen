// CClientMgr.cpp: implementation of the CUserMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "public/StdAfx.h"
#include "CClientMgr.h"
#include "SplitString.h"
#include "public/IPCCMessage.h"
#include "Utility.h"
#include "public/Spliter.h"
#include "public/ServerInfoDef.h"

#include "public/GlobalOperation.h"
#include "DepartmentInfo.h"
#include "CorporationMgr.h"
#include "public/MemLog.h"
extern CMemLog   g_Logger;

#include <list>
using namespace std;
#include "public/MemLog.h"
extern CMemLog   g_Logger;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCClientMgr* CCClientMgr::__CClientMgr = 0;

CCClientMgr::CCClientMgr()
{

}

CCClientMgr::~CCClientMgr()
{
	map<string, CCClientInfo*>::iterator pIter;
	for(pIter=m_mapCClients.begin(); pIter!=m_mapCClients.end(); ++pIter)
	{
		if(pIter->second != NULL)
			delete pIter->second;
	}
	
	this->m_mapCClients.clear();
}

CCClientMgr* CCClientMgr::GetInstance()
{
	if(0 == __CClientMgr)
		__CClientMgr = new CCClientMgr();

	return __CClientMgr;
}

void CCClientMgr::destroy()
{
	if(0 != __CClientMgr)
		delete __CClientMgr;

	__CClientMgr = 0;
}

CCClientInfo* CCClientMgr::GetCClientInfo(string account)
{
	if(account.length() <= 0)
		return NULL;	

	map<string, CCClientInfo*>::iterator pIter;
	if((pIter=this->m_mapCClients.find(account)) == m_mapCClients.end())
		return NULL;

	return pIter->second;	
}

CCClientInfo* CCClientMgr::AddCClient(
	string account, 
	unsigned long ip, 
	unsigned short port, 
	string sCalledDepartment, 
	string sUuiData, 
	string sCallTransID,
	CAudioStreamDescription asd,
	ECallType callType,
	bool bCallSG, 
	int iUserLevel)
{
	if(account.length() <= 0)
		return NULL;

	//首先清空原有的 cclient (避免cclient重复呼叫引起重复)
	this->DelCClient(account);

	CCClientInfo* pCClientInfo = new CCClientInfo();
	pCClientInfo->m_sAccount = account;
	pCClientInfo->m_ulUserIP = ip;
	pCClientInfo->m_usUserPort = port;

	pCClientInfo->m_sOriginCalledTarget = sCalledDepartment;
	pCClientInfo->m_sUUIData = sUuiData;

	if ( sCallTransID.empty() )
	{
		pCClientInfo->m_CallID_sCallIn = GetNewVersionInvokeID();
	}
	else
	{
		pCClientInfo->m_CallID_sCallIn = sCallTransID;
	}
	
	string sCorp("");
	string sDepartment("");
	CUtility::GetInstance()->SplitCorpAndDepart(sCalledDepartment, sCorp, sDepartment);
	pCClientInfo->m_sCorpName = sCorp;

	if ( bCallSG )
	{
		pCClientInfo->m_sCalledSkillGroup = sDepartment;
	}
	else
	{
		pCClientInfo->m_sCalledDepartment = sDepartment;
	}
	pCClientInfo->m_bCallSkillGroup = bCallSG;
	pCClientInfo->m_iUserLevel = iUserLevel;

	pCClientInfo->m_iEnqueueTime = gettickcount();

    pCClientInfo->audioStreamDescription = asd;
	pCClientInfo->callType = callType;

	m_mapCClients[account] = pCClientInfo;

	return pCClientInfo;
}

void CCClientMgr::AddCClient(CCClientInfo& cclient)
{
	if(cclient.m_sAccount.length() <= 0)
		return;

	//首先清空原有的 cclient (避免cclient重复呼叫引起重复)
	this->DelCClient(cclient.m_sAccount);

	CCClientInfo* pCClientInfo = new CCClientInfo();
	*pCClientInfo = cclient;

	m_mapCClients[cclient.m_sAccount] = pCClientInfo;
}

void CCClientMgr::DelCClient(string account)
{
	if(account.length() <= 0)
		return;

	map<string, CCClientInfo*>::iterator pIter = this->m_mapCClients.find(account);
	if(pIter == this->m_mapCClients.end())
		return;

	delete pIter->second;
	this->m_mapCClients.erase(account);
}

void CCClientMgr::SetResponseAgentID(string account, string agentid, string file, int line)
{
	{
		ostringstream oss;
		oss << "SetResponseAgentID [" << account << "," << agentid << "] " << file << ", " << line;
		g_Logger.MakeLog(oss.str());
	}

	CCClientInfo* pInfo = GetCClientInfo(account);
	if (pInfo == NULL)
	{
		g_Logger.MakeLog("SetResponseAgentID not found clientinfo, " + account);
		return;
	}

	pInfo->m_sResponseAgentID = agentid;
}