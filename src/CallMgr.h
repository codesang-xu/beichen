// CallMgr.h: interface for the CCallMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALLMGR_H__699B017E_0273_4C42_ABE1_7880CB853FF4__INCLUDED_)
#define AFX_CALLMGR_H__699B017E_0273_4C42_ABE1_7880CB853FF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CallInfo.h"

#include <string>
#include <map>
using namespace std;

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

using boost::multi_index_container;
using namespace boost::multi_index;

#include "call.h"

struct SCallIndex
{
    std::string id;
    std::string caller;
    ACDServer::call_ptr callptr;

    SCallIndex(ACDServer::call_ptr callptr_)
        :id(callptr_->call_.call_id)
        , caller(callptr_->call_.calling_device)
        , callptr(callptr_)
    {}


    bool operator<(const SCallIndex& cidx)const{ return id<cidx.id; }
};

typedef multi_index_container<
    SCallIndex,
    indexed_by<
    // sort by less<string> on id
    ordered_unique<member<SCallIndex, std::string, &SCallIndex::id> >,

    // sort by less<string> on caller
    ordered_non_unique<member<SCallIndex, std::string, &SCallIndex::caller> >
    >
> CallContainer;

class CCallMgr  
{
public:
	static CCallMgr* GetInstance();
    virtual ~CCallMgr();

    bool Add(ACDServer::call_ptr callptr);
    void Remove(std::string id);

    ACDServer::call_ptr Get(std::string id);
    size_t GetCount(std::string caller);

protected:
	CCallMgr();
	static CCallMgr* __CallMgr;

private:
    CallContainer m_CallContainer;

};

#endif // !defined(AFX_CALLMGR_H__699B017E_0273_4C42_ABE1_7880CB853FF4__INCLUDED_)
