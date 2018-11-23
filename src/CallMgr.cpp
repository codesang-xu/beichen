// CallMgr.cpp: implementation of the CCallMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "public/StdAfx.h"
#include "CallMgr.h"
#include "getid.h"
#include "AgentMgr.h"
#include "UserMgr.h"
#include "DepartmentInfo.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCallMgr* CCallMgr::__CallMgr = 0;

CCallMgr::CCallMgr()
{
}

CCallMgr::~CCallMgr()
{
}

CCallMgr* CCallMgr::GetInstance()
{
	if(0 == __CallMgr)
		__CallMgr = new CCallMgr();

	return __CallMgr;
}

bool CCallMgr::Add(ACDServer::call_ptr callptr)
{
    std::pair<CallContainer::iterator, bool> ret 
        = m_CallContainer.insert(SCallIndex(callptr));
    return ret.second;
}

void CCallMgr::Remove(std::string id)
{
    m_CallContainer.get<0>().erase(id);
}

ACDServer::call_ptr CCallMgr::Get(std::string id)
{
    typedef CallContainer::nth_index<0>::type CallContainerByID;
    CallContainerByID& id_index = m_CallContainer.get<0>();
    CallContainerByID::iterator it = id_index.find(id);
    if (it != id_index.end()) {
        return it->callptr;
    }

    return NULL;
}

size_t CCallMgr::GetCount(std::string caller)
{
    typedef CallContainer::nth_index<1>::type CallContainerByID;
    return m_CallContainer.get<1>().count(caller);
}
