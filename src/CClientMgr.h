// UserMgr.h: interface for the CUserMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__CCLIENTMGR_H)
#define __CCLIENTMGR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CClientInfo.h"
#include <string>
#include <map>
using namespace std;

class CCClientMgr  
{
public:
	void SetResponseAgentID(string account, string agentid, string file, int line);
	void DelCClient(string account);
	CCClientInfo* AddCClient(
		string account, 
		unsigned long ip,
		unsigned short port,
		string sCalledDepartment,
		string sUuiData,
		string sCallTransID,
		CAudioStreamDescription asd,
		ECallType callType = E_NORMAL_CALL,
		bool bCallSG=false, 
		int iUserLevel=3);
	void AddCClient(CCClientInfo& cclient);
	CCClientInfo* GetCClientInfo(string account);

	virtual ~CCClientMgr();
	static CCClientMgr* GetInstance();
	static void destroy();

protected:
	CCClientMgr();
	static CCClientMgr* __CClientMgr;

private:
	map<string, CCClientInfo*> m_mapCClients;
};

#endif // !defined(__CCLIENTMGR_H)