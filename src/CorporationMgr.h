// CorporationMgr.h: interface for the CCorporationMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CORPORATIONMGR_H__074BBD33_EFDD_4DA5_8788_AAD6D86CE1E5__INCLUDED_)
#define AFX_CORPORATIONMGR_H__074BBD33_EFDD_4DA5_8788_AAD6D86CE1E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CorporationInfo.h"

class CCorporationMgr  
{
public:
	void OutDial_SetAgentCount();
	void SendCorpGroupInfo(string invokeID, unsigned long ip, unsigned short port);
	CDepartmentInfo* GetDepartmentInfo(string corpAnddepart);
	static void destroy();
	CDepartmentInfo* GetDepartmentInfo(string corpName, string departName);
	static CCorporationMgr* GetInstance();
	CCorporationInfo* GetCorporation(string corpname);
	CCorporationInfo* FindWithManagerID(string managerid);
	//CCorporationInfo* GetCorporation_Redis(string corpname);
	void DelCorporationInfo(string corpname);
	void AddCorporationInfo(string corpname, string managerid);
	CCorporationInfo* AddCorporationInfo(string corpname, string managerid, int dynamicWebAgentLicence, int dynamicMobileAgentLicence, int dynamicWebAgentCount, int dynamicMobileAgentCount);
	virtual ~CCorporationMgr();

	map<string, CCorporationInfo*> m_mapCorporations;

	void CheckOverflow();

	CSkillGroupInfo* GetSkillGroupInfo(string corpAnddepart);
	CSkillGroupInfo* GetSkillGroupInfo(string corpName, string skillGroupId);
	CTXTSkillGroupPtr GetTXTSkillGroupInfo(string corpAnddepart);
	CTXTSkillGroupPtr GetTXTSkillGroupInfo(string corpName, string skillGroupId);

protected:
	CCorporationMgr();
	static CCorporationMgr* __CorporationMgr;
};

#endif // !defined(AFX_CORPORATIONMGR_H__074BBD33_EFDD_4DA5_8788_AAD6D86CE1E5__INCLUDED_)
