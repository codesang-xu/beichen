#include "public/StdAfx.h"
#include "DepartmentMgr.h"
#include "DepartmentInfo.h"

CDepartmentMgr* CDepartmentMgr::__DepartmentMgr = NULL;

CDepartmentMgr::CDepartmentMgr(void)
{
}

CDepartmentMgr::~CDepartmentMgr(void)
{
}

CDepartmentMgr* CDepartmentMgr::GetInstance()
{
	if(0 == __DepartmentMgr)
		__DepartmentMgr = new CDepartmentMgr();

	return __DepartmentMgr;	
}

CDepartmentInfo* CDepartmentMgr::GetDept(string sDeptId)
{
	map<string, CDepartmentInfo*>::iterator it = m_DeptMap.find(sDeptId);
	if (it != m_DeptMap.end())
	{
		return it->second;
	}
	else
	{
		return NULL;
	}
}

void CDepartmentMgr::AddDept(CDepartmentInfo* pDept)
{
	m_DeptMap[pDept->m_sDeptid] = pDept;
}

void CDepartmentMgr::MoveDept(string sDeptId)
{
	m_DeptMap.erase(sDeptId);
}

void CDepartmentMgr::MoveDept(CDepartmentInfo* pDept)
{
	MoveDept(pDept->m_sDeptid);
}