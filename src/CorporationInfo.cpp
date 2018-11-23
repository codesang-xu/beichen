// CorporationInfo.cpp: implementation of the CCorporationInfo class.
//
//////////////////////////////////////////////////////////////////////
#include "public/StdAfx.h"
#include "CorporationInfo.h"
#include "public/IPCCMessage.h"
#include "public/MemLog.h"
#include "DepartmentInfo.h"
#include "Utility.h"
#include "Log.h"
#include "DepartmentMgr.h"


extern CMemLog   g_Logger;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCorporationInfo::CCorporationInfo()
{
	m_dynamicWebAgentLicence = 0;
	m_dynamicMobileAgentLicence = 0;
	m_managerid = "";
	m_ulLoadInfoTime = 0;
}

CCorporationInfo::~CCorporationInfo()
{
	//clear map
	map<string, CDepartmentInfo*>::iterator pIter;
	for(pIter=m_mapDepartments.begin(); pIter!=m_mapDepartments.end(); ++pIter)
	{
		if(pIter->second != NULL)
		{
			CDepartmentMgr::GetInstance()->MoveDept(pIter->second);
			delete pIter->second;
		}
	}
	
	this->m_mapDepartments.clear();
}

CDepartmentInfo* CCorporationInfo::AddDepartment(string departmentName, string deptid, bool* pIsReLoad)
{
	if(departmentName.length() <= 0)
		return NULL;

	if(this->m_mapDepartments.find(departmentName) != m_mapDepartments.end())
	{
		if ( pIsReLoad )
		{
			*pIsReLoad = true;
		}
		
		return GetDepartmentInfo(departmentName);
		//return NULL;
	}

    CDepartmentInfo* departmentInfo = new CDepartmentInfo(m_managerid, m_corporationName, deptid, departmentName);
	m_mapDepartments[departmentName] = departmentInfo;

	CDepartmentMgr::GetInstance()->AddDept(departmentInfo);

	return departmentInfo;
}

int CCorporationInfo::GetWebAgentCount()
{
	int iRet = 0;
	map<string, CDepartmentInfo*>::iterator it = m_mapDepartments.begin();
	for (; it != m_mapDepartments.end(); it++)
	{
		iRet += it->second->GetWebAgentCount();
	}
	return iRet;
}

int CCorporationInfo::GetMobileAgentCount()
{
	int iRet = 0;
	map<string, CDepartmentInfo*>::iterator it = m_mapDepartments.begin();
	for (; it != m_mapDepartments.end(); it++)
	{
		iRet += it->second->GetMobileAgentCount();
	}
	return iRet;
}

void CCorporationInfo::DelDepartment(string departmentName)
{
	if(departmentName.length() <= 0)
		return;	

	map<string, CDepartmentInfo*>::iterator pIter;
	if((pIter=this->m_mapDepartments.find(departmentName)) == m_mapDepartments.end())
		return;

	CDepartmentMgr::GetInstance()->MoveDept(pIter->second);
	delete pIter->second;
	m_mapDepartments.erase(departmentName);
}

CDepartmentInfo* CCorporationInfo::GetDepartmentInfo(string departName)
{
	if(departName.length() <= 0)
		return NULL;	

	map<string, CDepartmentInfo*>::iterator pIter;
	if((pIter=this->m_mapDepartments.find(departName)) != m_mapDepartments.end())
	{
		return pIter->second;		
	}

	/*if ( CUtility::GetInstance()->m_bHaveRedis )
	{
	return GetDepartmentInfo_Redis(departName);
	}*/

	return NULL;
}

void CCorporationInfo::NotifyGetDepartNAgentInfo(string invokeID, int mode, string depart, unsigned long ip, unsigned short port)
{
	switch(mode)
	{
	case Mode_MyDepart:
		break;
	case Mode_MyDepartIdleAgents:
		func_GetMyDepartIdleAgents(invokeID, depart, ip, port);
		break;
	case Mode_OtherDeparts:
		func_GetOtherDeparts(invokeID, depart, ip, port);
		break;
	case Mode_OtherDepartsIdleAgents:
		func_GetOtherDepartsIdleAgents(invokeID, depart, ip, port);
		break;
	case Mode_AllDepartsNAgents:
		func_GetAllDepartsNAgents(invokeID, depart, ip, port);
		break;
	case Mode_IdleAgentsForTXTCall:
		func_GetTXTIdleAgents(invokeID, depart, ip, port);
		break;
	default:
		break;
	}
}

void CCorporationInfo::func_GetMyDepartIdleAgents(string invokeID, string depart, unsigned long ip, unsigned short port)
{
	CDepartmentInfo* pDepart = this->GetDepartmentInfo(depart);
	if (pDepart == NULL || pDepart->m_iDeptType != DeptType_Skill
		|| depart == "Dept_400CallBack")
		return;

	pDepart->func_SendIdleAgents(invokeID, ip, port);

	SIPCCMsg_ACD_DepartNAgentInfo info;
	info.invokeID = invokeID;
	info.SetTheInfo("");
	info.bEnd = 1;
	CUtility::GetInstance()->SendIPCCMsg_UDP(info, ip, port);
}

void CCorporationInfo::func_GetTXTIdleAgents(string invokeID, string groupid, unsigned long ip, unsigned short port)
{
	CTXTSkillGroupPtr skillgroup = this->GetTXTSkillGroupInfo(groupid);
	if (skillgroup != NULL)
	{
		skillgroup->func_SendIdleAgents(invokeID, ip, port);
	}
	else
	{
		CDepartmentInfo* deptinfo = this->GetDepartmentInfo(groupid);
		if (deptinfo != NULL)
		{
			deptinfo->func_SendTXTIdleAgents(invokeID, ip, port);
		}
	}

}

void CCorporationInfo::CheckOverflow()
{
	map<string, CDepartmentInfo*>::iterator it = m_mapDepartments.begin();
	for (; it != m_mapDepartments.end(); it++)
	{
		it->second->CheckOverflow();
	}
}

void CCorporationInfo::func_GetOtherDeparts(string invokeID, string depart, unsigned long ip, unsigned short port)
{
	string sTempInfo("");
	string sTempNickInfo("");
	string sTemp("");

	CDepartmentInfo* pDeptInfo = NULL;

	map<string, CDepartmentInfo*>::iterator pIter;
	for(pIter=m_mapDepartments.begin(); pIter!=m_mapDepartments.end(); ++pIter)
	{
		if(pIter->first == depart)
			continue;

		if(pIter->second == NULL || pIter->second->m_iDeptType != DeptType_Skill
			|| pIter->first == "Dept_400CallBack")
			continue;

		pDeptInfo = pIter->second;

		sTempInfo += pIter->first;
		sTempInfo += "|";

		sTempNickInfo += pDeptInfo->m_sNickname;
		sTempNickInfo += "|";

		sTemp = sTempInfo + sTempNickInfo;
		
		if( sTemp.length() > 600 )
		{
			//send
			SIPCCMsg_ACD_DepartNAgentInfo info;
			info.invokeID = invokeID;
			info.SetTheInfo(sTempInfo);
			info.deptNickname = sTempNickInfo;
			info.bEnd = 0;
			CUtility::GetInstance()->SendIPCCMsg_UDP(info, ip, port);

			sTempInfo = "";
			sTempNickInfo = "";
			sTemp = "";
		}
	}

//	if(sTempInfo.length() > 0)
//	{
		//send
		SIPCCMsg_ACD_DepartNAgentInfo info;
		info.invokeID = invokeID;
		info.SetTheInfo(sTempInfo);
		info.deptNickname = sTempNickInfo;
		info.bEnd = 1;
		CUtility::GetInstance()->SendIPCCMsg_UDP(info, ip, port);
//	}
}

void CCorporationInfo::func_GetOtherDepartsIdleAgents(string invokeID, string depart, unsigned long ip, unsigned short port)
{
	CDepartmentInfo* pDepart;

	map<string, CDepartmentInfo*>::iterator pIter;
	for(pIter=m_mapDepartments.begin(); pIter!=m_mapDepartments.end(); ++pIter)
	{
		if(pIter->first == "" || pIter->second == NULL)
			continue;

		if(pIter->second == NULL || pIter->second->m_iDeptType != DeptType_Skill
			|| pIter->first == "Dept_400CallBack")
			continue;

		pDepart = pIter->second;
		pDepart->func_SendIdleAgents(invokeID, ip, port);
	}

	SIPCCMsg_ACD_DepartNAgentInfo info;
	info.invokeID = invokeID;
	info.SetTheInfo("");
	info.bEnd = 1;
	CUtility::GetInstance()->SendIPCCMsg_UDP(info, ip, port);
}

void CCorporationInfo::func_GetAllDepartsNAgents(string invokeID, string depart, unsigned long ip, unsigned short port)
{
	
}

void CCorporationInfo::func_SendDepartment(string invokeId, unsigned long ip, unsigned short port)
{
	int iDepartNum = this->m_mapDepartments.size();

	string sDeparts;

	map<string, CDepartmentInfo*>::iterator pIter;
	for(pIter=m_mapDepartments.begin(); pIter!=m_mapDepartments.end(); ++pIter)
	{
		sDeparts += pIter->first;
		sDeparts += ",";
		if(sDeparts.length() > 600)
		{
			//send
			SIPCCMsg_ACD_GroupInfo groupInfo;
			groupInfo.invokeID = invokeId;
			groupInfo.SetCorporationName(this->m_corporationName);
			groupInfo.SetTheInfo(sDeparts);
			groupInfo.currentTime = CLog::GetLocalHour();
			groupInfo.groupNum = iDepartNum;
			CUtility::GetInstance()->SendIPCCMsg_UDP(groupInfo, ip, port);

			sDeparts = "";
		}
	}

	SIPCCMsg_ACD_GroupInfo groupInfo;
	groupInfo.invokeID = invokeId;
	groupInfo.SetCorporationName(this->m_corporationName);
	groupInfo.SetTheInfo(sDeparts);
	groupInfo.currentTime = CLog::GetLocalHour();
	groupInfo.groupNum = iDepartNum;
	CUtility::GetInstance()->SendIPCCMsg_UDP(groupInfo, ip, port);
}

void CCorporationInfo::OutDial_SetAgentCount()
{
	map<string, CDepartmentInfo*>::iterator pIter;
	CDepartmentInfo* pDept;
	for(pIter=m_mapDepartments.begin(); pIter!=m_mapDepartments.end(); ++pIter)
	{
		if(pIter->second != NULL)
		{
			pDept = pIter->second;
			pDept->SendOutDialSys_DeptAgentsInfo();
		}
	}
}

CSkillGroupInfo* CCorporationInfo::GetSkillGroupInfo(string skillGroupId)
{
	if (skillGroupId.length() <= 0)
		return NULL;

	map<string, CSkillGroupInfo*>::iterator pIter;
	if ((pIter = this->m_mapSkillGroups.find(skillGroupId)) == m_mapSkillGroups.end())
	{
		/*if ( CUtility::GetInstance()->m_bHaveRedis )
		{
		return GetSkillGroupInfo_Redis(skillGroupId);
		}*/

		return NULL;
	}

	return pIter->second;
}

CSkillGroupInfo* CCorporationInfo::AddSkillGroup(string skillGroupName, string skillGroupId, bool* pIsReLoad)
{
	if (skillGroupName.length() <= 0)
		return NULL;

	//DelSkillGroup(skillGroupId);

    CSkillGroupInfo* skillGroupInfo = m_mapSkillGroups[skillGroupId];
    if (skillGroupInfo != NULL) {
        skillGroupInfo->m_sSkillGroupName = skillGroupName;
    }
    else{
        skillGroupInfo = new CSkillGroupInfo(m_managerid, m_corporationName, skillGroupId, skillGroupName);
        m_mapSkillGroups[skillGroupId] = skillGroupInfo;
    }

	return skillGroupInfo;
}

void CCorporationInfo::DelSkillGroup(string skillGroupId)
{
	if (skillGroupId.length() <= 0)
		return;

	map<string, CSkillGroupInfo*>::iterator pIter;
	if ((pIter = this->m_mapSkillGroups.find(skillGroupId)) == m_mapSkillGroups.end())
		return;

	delete pIter->second;
	m_mapSkillGroups.erase(skillGroupId);
}

CTXTSkillGroupPtr CCorporationInfo::GetTXTSkillGroupInfo(string skillGroupId)
{
	if (skillGroupId.length() <= 0)
		return NULL;

	map<string, CTXTSkillGroupPtr>::iterator pIter;
	if ((pIter = this->m_mapTXTSkillGroups.find(skillGroupId)) == m_mapTXTSkillGroups.end())
	{
		return NULL;
	}

	return pIter->second;
}

CTXTSkillGroupPtr CCorporationInfo::AddTXTSkillGroup(string skillGroupName, string skillGroupId, int32_t queuingAlgorithm, bool* pIsReLoad)
{
	if (skillGroupName.length() <= 0)
		return NULL;
	
    CTXTSkillGroupPtr skillGroupInfo = m_mapTXTSkillGroups[skillGroupId];
    if (skillGroupInfo) {
        stringstream ss;
        ss << "update txt skill group"
            << ", corp:" << m_corporationName
            << ", groupid:" << skillGroupId
            << ", groupname:" << skillGroupName;
        g_Logger.MakeLog(ss.str());

        skillGroupInfo->m_sSkillGroupName = skillGroupName;
        skillGroupInfo->m_iQueuingAlgorithm = queuingAlgorithm;
        skillGroupInfo->SetQueuingAlgorithm(queuingAlgorithm);
    }
    else {
	    stringstream ss;
	    ss << "add txt skill group"
		    << ", corp:" << m_corporationName
		    << ", groupid:" << skillGroupId
		    << ", groupname:" << skillGroupName;
	    g_Logger.MakeLog(ss.str());

        skillGroupInfo.reset(new CSkillGroupTXTCall());
        skillGroupInfo->m_sSkillGroupName = skillGroupName;
        skillGroupInfo->m_sCorpName = this->m_corporationName;
        skillGroupInfo->m_sManagerid = this->m_managerid;
        skillGroupInfo->m_sSkillGroupid = skillGroupId;
        skillGroupInfo->m_iQueuingAlgorithm = queuingAlgorithm;
        skillGroupInfo->SetQueuingAlgorithm(queuingAlgorithm);
        m_mapTXTSkillGroups[skillGroupId] = skillGroupInfo;
    }

	return skillGroupInfo;
}

void CCorporationInfo::DelTXTSkillGroup(string skillGroupId)
{
	if (skillGroupId.length() <= 0)
		return;
	m_mapSkillGroups.erase(skillGroupId);
}

void CCorporationInfo::NotifyMsg(SIPCCMsg_OneKeySwitch_NotifyMsg notifyMsg)
{
	for (map<string, CDepartmentInfo*>::iterator it = m_mapDepartments.begin(); it != m_mapDepartments.end(); it++)
	{
		it->second->NotifyMsg(notifyMsg);
	}
}

