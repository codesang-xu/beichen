// SplitString.h: interface for the CSplitString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPLITSTRING_H__18D6025A_7DA9_4C4D_9F6D_B0FA456298A6__INCLUDED_)
#define AFX_SPLITSTRING_H__18D6025A_7DA9_4C4D_9F6D_B0FA456298A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <list>
using namespace std;

class CSplitString
{
public:
	CSplitString();
	virtual ~CSplitString();

	static void SplitString(string& originString, string splitString, list<string>& retList);
};

#endif // !defined(AFX_SPLITSTRING_H__18D6025A_7DA9_4C4D_9F6D_B0FA456298A6__INCLUDED_)
