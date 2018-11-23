// CClientInfo.h: interface for the CCClientInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__CCLIENTINFO_H)
#define __CCLIENTINFO_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <set>
using namespace std;

#include "public/IPCCMessage.h"

class CCClientInfo  
{
public:
	CCClientInfo();
	virtual ~CCClientInfo();

	void setMediaRequest(MediaRequestPara mediarequest){ mediaRequest = mediarequest; }

    string ToString();
    bool FromString(string & str);

public:
	string m_sResponseAgentID;

	string m_sOriginCalledTarget;
	string m_sAccount;
	string m_sCorpName;
	string m_sCalledDepartment;
	string m_sUUIData;
	bool m_bCallSkillGroup;
	string m_sCalledSkillGroup;
	int m_iUserLevel;

	unsigned long m_iEnqueueTime;		//用户进入队列时间
	unsigned long m_iQueueSpanTime;

	string m_CallID_sCallIn;

	bool m_CalledIsDept;

	unsigned long m_ulUserIP;
	unsigned short m_usUserPort;

	CAudioStreamDescription audioStreamDescription;
	ECallType callType;

	MediaRequestPara mediaRequest;
};

#endif // !defined(__CCLIENTINFO_H)