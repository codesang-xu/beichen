// UserInfo.cpp: implementation of the CUserInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "public/StdAfx.h"
#include "common/util.h"
#include "public/MemLog.h"
#include "public/GlobalOperation.h"
#include "UserInfo.h"

extern CMemLog   g_Logger;

using namespace ib::common;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUserInfo::CUserInfo()
{
	m_sUserName = "";
	m_sCalledKeyWord = "";
	m_sOriginCalledTarget = "";
	m_iUserType = -1;
	m_audioIP = 0;
	m_audioPort = 0;

	m_sPartNo = "";
	m_sComSymbol = "";

	this->m_sResponseAgentID = "";

	m_ulUserIP = 0;
	m_usUserPort = 0;
	m_sUUIData = "";
	m_iUserLevel = -1;
	m_lanIP = 0;
	m_iCallType = -1;
	m_sCallerNo = "";
	m_sCalledNo = "";
	m_iEnqueueTime = 0;
	m_CallID_sCallIn = "";
	ClearCallFailed();

    ////ostringstream oss;
    ////oss << "user created at 0x" << this;
    ////g_Logger.MakeLog(oss.str());
}

CUserInfo::~CUserInfo()
{
    ////ostringstream oss;
    ////oss << "user destroyed at 0x" << this;
    ////g_Logger.MakeLog(oss.str());
}

string CUserInfo::ToString()
{
    ostringstream oss;
    oss << "0|"
        << m_CallID_sCallIn      << "|"
        << m_sCallerNo           << "|"
        << m_sUserName           << "|"
        << m_sCalledNo           << "|"
        << m_sCorpName           << "|"
        << m_sOriginCalledTarget << "|"
        << m_sCalledKeyWord      << "|"
        << m_sResponseAgentID    << "|"
        << m_sUUIData            << "|"
        << m_iUserLevel          << "|"
        << m_iUserType           << "|"
        << m_iCallType           << "|"
        << m_sPartNo             << "|"
        << m_sComSymbol          << "|"
        << m_asd.to_str()        << "|"
        << format_ip_addr(m_lanIP) << "|"
        << format_ip_addr(m_audioIP) << "|"
        << m_audioPort           << "|"
        << m_audioRtpPort        << "|"
        << format_ip_addr(m_ulUserIP) << "|"
        << m_usUserPort          << "|"
        << m_iQueueType          << "|"
        << m_sQueueID            << "|"
        << m_sQueueName;
        return oss.str();
}

bool CUserInfo::FromString(string & str)
{
    ib::common::splitor spl(str, "|");

    if (spl[0] != "0") {
        return false;
    }
    int i = 1;
    m_CallID_sCallIn      = spl[i++];
    m_sCallerNo           = spl[i++];
    m_sUserName           = spl[i++];
    m_sCalledNo           = spl[i++];
    m_sCorpName           = spl[i++];
    m_sOriginCalledTarget = spl[i++];
    m_sCalledKeyWord      = spl[i++];
    m_sResponseAgentID    = spl[i++];
    m_sUUIData            = spl[i++];
    m_iUserLevel          = atoi(spl[i++].c_str());
    m_iUserType           = atoi(spl[i++].c_str());
    m_iCallType           = atoi(spl[i++].c_str());
    m_sPartNo             = spl[i++];
    m_sComSymbol          = spl[i++];
    m_asd.from_str(         spl[i++]);
    m_lanIP               = inet_addr(spl[i++].c_str());
    m_audioIP             = inet_addr(spl[i++].c_str());
    m_audioPort           = atoi(spl[i++].c_str());
    m_audioRtpPort        = atoi(spl[i++].c_str());
    m_ulUserIP            = inet_addr(spl[i++].c_str());
    m_iQueueType          = atoi(spl[i++].c_str());
    m_sQueueID            = spl[i++];
    m_sQueueName          = spl[i++];

    return 1;
}

void CUserInfo::ClearCallFailed()
{
	m_CallFailedAgents.clear();
	m_sUUIData = "";
	m_sCallerNo = "";
	m_sCalledNo = "";
	m_sUserName = "";
}