// UserMgr.cpp: implementation of the CUserMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "public/StdAfx.h"
#include "UserMgr.h"
#include "UserInfo.h"
#include "SplitString.h"
#include "public/IPCCMessage.h"
#include "Utility.h"
#include "public/Spliter.h"
#include "public/ServerInfoDef.h"

#include "public/GlobalOperation.h"
#include "DepartmentInfo.h"
#include "CorporationMgr.h"

#include <list>
using namespace std;
#include "public/MemLog.h"
extern CMemLog   g_Logger;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUserMgr* CUserMgr::__UserMgr = 0;

CUserMgr::CUserMgr()
{

}

CUserMgr::~CUserMgr()
{
	map<string, CUserInfo*>::iterator pIter;
	for(pIter=m_mapUsers.begin(); pIter!=m_mapUsers.end(); ++pIter)
	{
		if(pIter->second != NULL)
			delete pIter->second;
	}
	
	this->m_mapUsers.clear();
}

CUserMgr* CUserMgr::GetInstance()
{
	if(0 == __UserMgr)
		__UserMgr = new CUserMgr();

	return __UserMgr;
}

void CUserMgr::destroy()
{
	if(0 != __UserMgr)
		delete __UserMgr;

	__UserMgr = 0;
}

CUserInfo* CUserMgr::GetUserInfo(string userName)
{
	if(userName.length() <= 0)
		return NULL;	

	map<string, CUserInfo*>::iterator pIter;
	if((pIter=this->m_mapUsers.find(userName)) == m_mapUsers.end())
		return NULL;

	return pIter->second;	
}

CUserInfo* CUserMgr::AddUser(string userName, int userType, int iUserNatType, string sCalledKeyWord, 
    string sCalledDepartment, string sUuiData, unsigned long audioIP, unsigned short audioPort, 
    unsigned long lanIP, string partNo, string comSymbol, string sCallTransID, unsigned long sipBoardIP, 
    unsigned short sipBoardRtpPort, unsigned short sipBoardRtcpPort, unsigned long sipTransIP, 
    unsigned short sipTransRtpPort, unsigned short sipTransRtcpPort, string trans, string codec, 
    int unit, CAudioStreamDescription &asd, unsigned short audioRtpPort, bool bCallSG, int iUserLevel, 
    string departID)
{
	if(userName.length() <= 0)
		return NULL;

	//首先清空原有的 user (避免user重复呼叫引起重复)
	this->DelUser(userName);

	CUserInfo* pUserInfo = new CUserInfo();
    if (pUserInfo == NULL) {
        throw "out of memory";
    }

	pUserInfo->m_sUserName = userName;
	pUserInfo->m_iUserType = userType;
	pUserInfo->m_sCalledKeyWord = sCalledKeyWord;
	pUserInfo->m_sOriginCalledTarget = sCalledDepartment;
	pUserInfo->m_sUUIData = sUuiData;
	pUserInfo->m_audioIP = audioIP;
	pUserInfo->m_audioPort = audioPort;
	pUserInfo->m_audioRtpPort = audioRtpPort;
	pUserInfo->m_lanIP = lanIP;
	pUserInfo->m_sPartNo = partNo;
	pUserInfo->m_sComSymbol = comSymbol;
	if ( sCallTransID.empty() ) {
		pUserInfo->m_CallID_sCallIn = GetNewVersionInvokeID();
	}
	else {
		pUserInfo->m_CallID_sCallIn = sCallTransID;
	}
	
    pUserInfo->m_iQueueType = bCallSG ? 2 : 1;
    pUserInfo->m_sQueueID = departID;

	string sCorp("");
	string sDepartment("");
	CUtility::GetInstance()->SplitCorpAndDepart(sCalledDepartment, sCorp, sDepartment);
    pUserInfo->m_sCorpName = sCorp;
    pUserInfo->m_sQueueName = sDepartment;
    if (bCallSG) {
        CSkillGroupInfo *pSkillGroupInfo = CCorporationMgr::GetInstance()->GetSkillGroupInfo(sCalledDepartment);
        if (pSkillGroupInfo) {
            pUserInfo->m_sQueueName = pSkillGroupInfo->m_sSkillGroupName;
        }
    }

	pUserInfo->m_iUserLevel = iUserLevel;
	pUserInfo->m_iEnqueueTime = gettickcount();

	if(pUserInfo->m_iUserType == CallParty_VoiceGateWay)
	{
		CSpliter strSp(sUuiData, "%^#");
		pUserInfo->m_sCallerNo = strSp[0];
		pUserInfo->m_sCalledNo = strSp[1];
		pUserInfo->m_iCallType = IPCCCallType_PSTN;
	}
	else
	{
		pUserInfo->m_sCallerNo = userName;
		pUserInfo->m_sCalledNo = sCalledKeyWord;
		pUserInfo->m_iCallType = IPCCCallType_Internet;
	}
    pUserInfo->m_asd = asd;

	m_mapUsers[userName] = pUserInfo;

    return pUserInfo;
}

CUserInfo*  CUserMgr::AddUser(CUserInfo& user)
{
	if(user.m_sUserName.length() <= 0){
        return NULL;
    }

	//首先清空原有的 user (避免user重复呼叫引起重复)
	this->DelUser(user.m_sUserName);

	CUserInfo* pUserInfo = new CUserInfo();

    if (pUserInfo == NULL) {
        throw "out of memory";
    }

	*pUserInfo = user;

	m_mapUsers[user.m_sUserName] = pUserInfo;

    return pUserInfo;
}

CUserInfo* CUserMgr::AddUser(string userName, int natType, unsigned long localIP, unsigned short localPort, unsigned long ip, unsigned short port)
{
    if (userName.length() <= 0) {
        return NULL;
    }

	//首先清空原有的 user (避免user重复登录引起重复)
	this->DelUser(userName);

	CUserInfo* pUserInfo = new CUserInfo();

    if (pUserInfo == NULL) {
        throw "out of memory";
    }

	pUserInfo->m_sUserName = userName;
	pUserInfo->m_iUserType = CallParty_User;
	pUserInfo->m_ulUserIP = ip;
	pUserInfo->m_usUserPort = port;
	if(natType == -1)
	{
		pUserInfo->m_iUserType = CallParty_VoiceGateWay;
		if(localIP != 0)
			pUserInfo->m_ulUserIP = localIP;
		if(localPort != 0)
			pUserInfo->m_usUserPort = localPort;
	}

	m_mapUsers[userName] = pUserInfo;	

    return pUserInfo;
}

void CUserMgr::DelUser(string userName)
{
	if(userName.length() <= 0)
		return;

	map<string, CUserInfo*>::iterator pIter = this->m_mapUsers.find(userName);
	if(pIter == this->m_mapUsers.end())
		return;

	delete pIter->second;
	this->m_mapUsers.erase(userName);
}

void CUserMgr::SetResponseAgentID(string username, string agentid)
{
	CUserInfo* pInfo = GetUserInfo(username);
	if(pInfo == NULL)
		return;
	pInfo->m_sResponseAgentID = agentid;
}

