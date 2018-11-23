// CorporationMgr.cpp: implementation of the CCorporationMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "public/StdAfx.h"
#include "CorporationMgr.h"
#include "Utility.h"
#include "public/MemLog.h"

extern CMemLog   g_Logger;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCorporationMgr* CCorporationMgr::__CorporationMgr = 0;

CCorporationMgr::CCorporationMgr()
{

}

CCorporationMgr::~CCorporationMgr()
{
	//clear map
	map<string, CCorporationInfo*>::iterator pIter;
	for(pIter=m_mapCorporations.begin(); pIter!=m_mapCorporations.end(); ++pIter)
	{
		if(pIter->second != NULL)
			delete pIter->second;
	}
	
	this->m_mapCorporations.clear();
}

void CCorporationMgr::AddCorporationInfo(string corpname, string managerid)
{
	if(corpname.length() <= 0)
		return;

	if(this->m_mapCorporations.find(corpname) != m_mapCorporations.end())
		return;

	CCorporationInfo* corpInfo = new CCorporationInfo();
	corpInfo->m_corporationName = corpname;
	corpInfo->m_managerid = managerid;
	m_mapCorporations[corpname] = corpInfo;	
}

void CCorporationMgr::DelCorporationInfo(string corpname)
{
	if(corpname.length() <= 0)
		return;	

	map<string, CCorporationInfo*>::iterator pIter;
	if((pIter=this->m_mapCorporations.find(corpname)) == m_mapCorporations.end())
		return;

	delete pIter->second;
	m_mapCorporations.erase(corpname);
}

CCorporationInfo* CCorporationMgr::GetCorporation(string corpname)
{
	if(corpname.length() <= 0)
		return NULL;	

	map<string, CCorporationInfo*>::iterator pIter;
	if((pIter=this->m_mapCorporations.find(corpname)) != m_mapCorporations.end())
	{
		return pIter->second;		
	}

	return NULL;
}

CCorporationInfo* CCorporationMgr::FindWithManagerID(string managerid)
{
	for (map<string, CCorporationInfo*>::iterator it = m_mapCorporations.begin();
		it != m_mapCorporations.end(); it++)
	{
		if (it->second->m_managerid == managerid)
		{
			return it->second;
		}
	}

	return NULL;
}

CCorporationMgr* CCorporationMgr::GetInstance()
{
	if(0 == __CorporationMgr)
		__CorporationMgr = new CCorporationMgr();

	return __CorporationMgr;	
}

CDepartmentInfo* CCorporationMgr::GetDepartmentInfo(string corpName, string departName)
{
	CCorporationInfo* corp = GetCorporation(corpName);
	if(corp == NULL)
		return NULL;
	
	return corp->GetDepartmentInfo(departName);
}

void CCorporationMgr::CheckOverflow()
{
	map<string, CCorporationInfo*>::iterator it = m_mapCorporations.begin();
	for (; it != m_mapCorporations.end(); it++)
	{
		it->second->CheckOverflow();
	}
}

void CCorporationMgr::destroy()
{
	if(0 != __CorporationMgr)
		delete __CorporationMgr;

	__CorporationMgr = 0;
}

CDepartmentInfo* CCorporationMgr::GetDepartmentInfo(string corpAnddepart)
{
	string sCorp("");
	string sDepartment("");
	if(!CUtility::GetInstance()->SplitCorpAndDepart(corpAnddepart, sCorp, sDepartment))
		return NULL;

	return GetDepartmentInfo(sCorp, sDepartment);
}

void CCorporationMgr::SendCorpGroupInfo(string invokeID, unsigned long ip, unsigned short port)
{
	//clear map
	map<string, CCorporationInfo*>::iterator pIter;
	CCorporationInfo* corp;
	for(pIter=m_mapCorporations.begin(); pIter!=m_mapCorporations.end(); ++pIter)
	{
		if(pIter->second != NULL)
		{
			corp = pIter->second;
			corp->func_SendDepartment(invokeID, ip, port);
		}
	}
}

void CCorporationMgr::OutDial_SetAgentCount()
{
	CCorporationInfo* pCorpInfo = NULL;
	map<string, CCorporationInfo*>::iterator pIter;
	for(pIter=m_mapCorporations.begin(); pIter!=m_mapCorporations.end(); ++pIter)
	{
		if(pIter->second != NULL)
		{
			pCorpInfo = pIter->second;
			pCorpInfo->OutDial_SetAgentCount();
		}
	}
}

CSkillGroupInfo* CCorporationMgr::GetSkillGroupInfo(string corpName, string skillGroupId)
{
	if(corpName.length() <= 0)
		return NULL;

	CCorporationInfo* corp = GetCorporation(corpName);
	if(corp == NULL)
		return NULL;

	return corp->GetSkillGroupInfo(skillGroupId);
}

CSkillGroupInfo* CCorporationMgr::GetSkillGroupInfo(string corpAnddepart)
{
	string sCorp("");
	string skillGroupId("");
	if (!CUtility::GetInstance()->SplitCorpAndDepart(corpAnddepart, sCorp, skillGroupId))
		return NULL;

	return GetSkillGroupInfo(sCorp, skillGroupId);
}

CTXTSkillGroupPtr CCorporationMgr::GetTXTSkillGroupInfo(string corpName, string skillGroupId)
{
	if (corpName.length() <= 0)
		return NULL;

	CCorporationInfo* corp = GetCorporation(corpName);
	if (corp == NULL)
		return NULL;

	return corp->GetTXTSkillGroupInfo(skillGroupId);
}

CTXTSkillGroupPtr CCorporationMgr::GetTXTSkillGroupInfo(string corpAnddepart)
{
	string sCorp("");
	string skillGroupId("");
	if (!CUtility::GetInstance()->SplitCorpAndDepart(corpAnddepart, sCorp, skillGroupId))
		return NULL;

	return GetTXTSkillGroupInfo(sCorp, skillGroupId);
}

//CCorporationInfo* CCorporationMgr::GetCorporation_Redis(string corpname)
//{
//	if(corpname.length() <= 0)
//		return NULL;
//	
//	string strRedisCmmd;
//	Format(strRedisCmmd,"HGET CorpInfo %s", corpname.c_str());
//
//	char* pOutStr = NULL;
//	long long iOutInt = 0;
//	int iResult = CUtility::GetInstance()->m_rcRuntime.EexcRedisCommand(strRedisCmmd, pOutStr, &iOutInt);
//	
//	CCorporationInfo* corpInfo = NULL;
//	if ( iResult == REPLY_STRING && pOutStr != NULL )
//	{
//		string sCorpInfo = string(pOutStr);
//		CorporationInfo_Redis rCorpInfo(sCorpInfo);
//		corpInfo = AddCorporationInfo(rCorpInfo.m_sCorporationName, rCorpInfo.m_sManagerid, rCorpInfo.m_iDynamicWebAgentLicence, rCorpInfo.m_iDynamicMobileAgentLicence, rCorpInfo.m_iCurWebAgentCount, rCorpInfo.m_iCurMoblieAgentCount);
//
//		delete pOutStr;
//		pOutStr = NULL;
//	}
//
//	return corpInfo;
//}

CCorporationInfo* CCorporationMgr::AddCorporationInfo(string corpname, string managerid, int dynamicWebAgentLicence, int dynamicMobileAgentLicence, int dynamicWebAgentCount, int dynamicMobileAgentCount)
{
	if(corpname.empty() || managerid.empty())
	{
		return NULL;
	}

	/*CCorporationInfo* corpInfo = NULL;
	map<string, CCorporationInfo*>::iterator iter = this->m_mapCorporations.find(corpname);
	if( iter == m_mapCorporations.end())
	{
		corpInfo = new CCorporationInfo();
	}
	else
	{
		corpInfo = iter->second;
	}*/

	CCorporationInfo* corpInfo = new CCorporationInfo();

	if ( corpInfo )
	{
		corpInfo->m_corporationName = corpname;
		corpInfo->m_managerid = managerid;
		corpInfo->m_dynamicWebAgentLicence = dynamicWebAgentLicence;
		corpInfo->m_dynamicMobileAgentLicence = dynamicMobileAgentLicence;
		corpInfo->m_dynamicWebAgentCount = dynamicWebAgentCount;
		corpInfo->m_dynamicMobileAgentCount = dynamicMobileAgentCount;
		m_mapCorporations[corpname] = corpInfo;	

		return corpInfo;
	}
	
	return NULL;
}
