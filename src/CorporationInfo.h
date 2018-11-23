// CorporationInfo.h: interface for the CCorporationInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CORPORATIONINFO_H__D6DCF848_AA40_4368_8C3D_9BF58957E772__INCLUDED_)
#define AFX_CORPORATIONINFO_H__D6DCF848_AA40_4368_8C3D_9BF58957E772__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DepartmentInfo.h"

class CCorporationInfo  
{
public:
	void OutDial_SetAgentCount();
	void func_SendDepartment(string invokeId, unsigned long ip, unsigned short port);
	void func_GetAllDepartsNAgents(string invokeID, string depart, unsigned long ip, unsigned short port);
	void func_GetOtherDepartsIdleAgents(string invokeID, string depart, unsigned long ip, unsigned short port);
	void func_GetOtherDeparts(string invokeID, string depart, unsigned long ip, unsigned short port);
	void func_GetMyDepartIdleAgents(string invokeID, string depart, unsigned long ip, unsigned short port);
	void func_GetTXTIdleAgents(string invokeID, string groupid, unsigned long ip, unsigned short port);
	void NotifyGetDepartNAgentInfo(string invokeID, int mode, string depart, unsigned long ip, unsigned short port);
	CDepartmentInfo* GetDepartmentInfo(string departName);
	void DelDepartment(string departmentName);
	CDepartmentInfo* AddDepartment(string departmentName, string deptid, bool* pIsReLoad = NULL);
	CCorporationInfo();
	virtual ~CCorporationInfo();
	
	int GetWebAgentCount();
	int GetMobileAgentCount();

	void CheckOverflow();

	CSkillGroupInfo* GetSkillGroupInfo(string skillGroupId);
	CSkillGroupInfo* AddSkillGroup(string skillGroupName, string skillGroupId, bool* pIsReLoad = NULL);
	void DelSkillGroup(string skillGroupId);

	CTXTSkillGroupPtr GetTXTSkillGroupInfo(string skillGroupId);
	CTXTSkillGroupPtr AddTXTSkillGroup(string skillGroupName, string skillGroupId, int32_t queuingAlgorithm, bool* pIsReLoad = NULL);
	void DelTXTSkillGroup(string skillGroupId);

	void NotifyMsg(SIPCCMsg_OneKeySwitch_NotifyMsg notifyMsg);

public:
	string m_corporationName;
	string m_managerid;
	int m_dynamicWebAgentLicence;
	int m_dynamicMobileAgentLicence;
	int m_dynamicWebAgentCount;
	int m_dynamicMobileAgentCount;

	unsigned long m_ulLoadInfoTime;

	map<string, CDepartmentInfo*> m_mapDepartments;

	map<string, CSkillGroupInfo*> m_mapSkillGroups;
	map<string, CTXTSkillGroupPtr> m_mapTXTSkillGroups;
};

#endif // !defined(AFX_CORPORATIONINFO_H__D6DCF848_AA40_4368_8C3D_9BF58957E772__INCLUDED_)
