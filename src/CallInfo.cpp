// CallInfo.cpp: implementation of the CCallInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "public/StdAfx.h"
#include "CallInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCallInfo::CCallInfo(string callID, string agentID, string UserName)
{
	this->m_sCallID = callID;
	this->m_sAgentID = agentID;
	this->m_sUserName = UserName;
}

CCallInfo::~CCallInfo()
{

}
