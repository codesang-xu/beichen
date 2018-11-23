#pragma once
#include <string>
#include <map>
using namespace std;

class CDepartmentInfo;
class CDepartmentMgr
{
public:
	static void destroy();
	static CDepartmentMgr* GetInstance();

	CDepartmentInfo* GetDept(string sDeptId);
	void AddDept(CDepartmentInfo* pDept);
	void MoveDept(string sDeptId);
	void MoveDept(CDepartmentInfo* pDept);
public:
	virtual ~CDepartmentMgr(void);
protected:
	CDepartmentMgr(void);
	static CDepartmentMgr* __DepartmentMgr;
	map<string, CDepartmentInfo*> m_DeptMap;
};
