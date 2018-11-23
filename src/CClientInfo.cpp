// CClientInfo.cpp: implementation of the CCClientInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "public/StdAfx.h"
#include "CClientInfo.h"
#include "common/util.h"
#include "public/GlobalOperation.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCClientInfo::CCClientInfo()
{
	m_sAccount = "";
	m_sCorpName = "";
	m_sCalledDepartment = "";
	m_sOriginCalledTarget = "";
	m_iUserLevel = -1;

	this->m_sResponseAgentID = "";
	m_ulUserIP = 0;
	m_usUserPort = 0;
	m_sUUIData = "";
	m_bCallSkillGroup = false;
	m_sCalledSkillGroup = "";
	m_CalledIsDept = false;
	
	m_iEnqueueTime = 0;
	m_iQueueSpanTime = 0;
	m_CallID_sCallIn = "";

	audioStreamDescription = CAudioStreamDescription();
	mediaRequest = MediaRequestPara();

}

CCClientInfo::~CCClientInfo()
{}

string CCClientInfo::ToString()
{
    ostringstream oss;
    oss << "1|"
        << m_sResponseAgentID      << "|"
        << m_sOriginCalledTarget   << "|"
        << m_sAccount              << "|"
        << m_sCorpName             << "|"
        << m_sCalledDepartment     << "|"
        << m_sUUIData              << "|"
        << (int)m_bCallSkillGroup  << "|"
        << m_sCalledSkillGroup     << "|"
        << m_iUserLevel            << "|"
        << m_iEnqueueTime          << "|"
        << m_iQueueSpanTime        << "|"
        << m_CallID_sCallIn        << "|"
        << (int)m_CalledIsDept     << "|"
        << m_ulUserIP              << "|"
        << m_usUserPort            << "|"
        << audioStreamDescription.to_str()  << "|"
        << callType                << "|"
        << mediaRequest.to_str();

    return oss.str();
}

bool CCClientInfo::FromString(string & str)
{
    ib::common::splitor spl(str, "|");

    if (spl[0] != "0") {
        return false;
    }

    m_sResponseAgentID       = spl[1];
    m_sOriginCalledTarget    = spl[2];
    m_sAccount               = spl[3];
    m_sCorpName              = spl[4];
    m_sCalledDepartment      = spl[5];
    m_sUUIData               = spl[6];
    m_bCallSkillGroup        = atoi(spl[7].c_str());
    m_sCalledSkillGroup      = spl[8];
    m_iUserLevel             = atoi(spl[9].c_str());
    m_iEnqueueTime           = atoi(spl[10].c_str());
    m_iQueueSpanTime         = atoi(spl[11].c_str());
    m_CallID_sCallIn         = spl[12];
    m_CalledIsDept           = atoi(spl[13].c_str());
    m_ulUserIP               = atoi(spl[14].c_str());
    m_usUserPort             = atoi(spl[15].c_str());
    //audioStreamDescription.from_str(spl[16]);
    callType                 = (ECallType)atoi(spl[17].c_str());
    mediaRequest.from_str(spl[18]);

    return 1;

}

