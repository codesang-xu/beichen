// UserMgr.h: interface for the CUserMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERMGR_H__5B1E91F3_4EE9_4FC1_A9F2_E04482D6AF39__INCLUDED_)
#define AFX_USERMGR_H__5B1E91F3_4EE9_4FC1_A9F2_E04482D6AF39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UserInfo.h"
#include <string>
#include <map>
using namespace std;

class CUserMgr  
{
public:
	void SetResponseAgentID(string username, string agentid);
	void DelUser(string userName);
    CUserInfo* AddUser(string userName, int userType, int iUserNatType, string sCalledKeyWord, 
        string sCalledDepartment, string sUuiData, unsigned long audioIP, unsigned short audioPort, 
        unsigned long lanIP, string partNo, string comSymbol, string sCallTransID, unsigned long sipBoardIP,
        unsigned short sipBoardRtpPort, unsigned short sipBoardRtcpPort, unsigned long sipTransIP, 
        unsigned short sipTransRtpPort, unsigned short sipTransRtcpPort, string trans, string codec, 
        int unit, CAudioStreamDescription &asd, unsigned short audioRtpPort, bool bCallSG, 
        int iUserLevel, string departID);

    CUserInfo*  AddUser(CUserInfo& user);
    CUserInfo* AddUser(string userName, int natType, unsigned long localIP, unsigned short localPort, unsigned long ip, unsigned short port);
	CUserInfo* GetUserInfo(string userName);
	virtual ~CUserMgr();
	static CUserMgr* GetInstance();
	static void destroy();

	//CUserInfo* CreateUser_Redis(string userName);
protected:
	CUserMgr();
	static CUserMgr* __UserMgr;
private:
	map<string, CUserInfo*> m_mapUsers;
};

#endif // !defined(AFX_USERMGR_H__5B1E91F3_4EE9_4FC1_A9F2_E04482D6AF39__INCLUDED_)
