// CallInfo.h: interface for the CCallInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALLINFO_H__8686370A_3605_44F4_9D60_6E226F1C29C7__INCLUDED_)
#define AFX_CALLINFO_H__8686370A_3605_44F4_9D60_6E226F1C29C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
using namespace std;

class CCallInfo  
{
public:
	CCallInfo(string callID, string agentID, string UserName);
	virtual ~CCallInfo();

	string m_sCallID;
	string m_sUserName;
	string m_sAgentID;
};

#endif // !defined(AFX_CALLINFO_H__8686370A_3605_44F4_9D60_6E226F1C29C7__INCLUDED_)
