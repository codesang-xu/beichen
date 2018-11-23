// Spliter.h: interface for the CSpliter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPLITER_H__99EEC8A2_5C32_40EA_9B38_5D3E8A9C5085__INCLUDED_)
#define AFX_SPLITER_H__99EEC8A2_5C32_40EA_9B38_5D3E8A9C5085__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
using namespace std;

class CSpliter
{
public:
	CSpliter(string sSource, string sSplit1 = "", string sSplit2 = "", string sSplit3 = "");
	virtual ~CSpliter();

public:
	int GetSize() {return m_Vec.size();}
	
	CSpliter& operator[](int i)
	{
		if ( i >= 0 && i < m_Vec.size() )
			return m_Vec[i];
		else
			return CSpliter::Empty;
	};

	operator string&(){return m_sSource;};
	bool operator==(string sSource){return m_sSource == sSource;};
	bool operator!=(string sSource){return m_sSource != sSource;};
	const char* c_str(){return m_sSource.c_str();};
	
	CSpliter& operator =(const string sSource) { m_sSource=sSource; return *this; };
	
private:
	vector<CSpliter> m_Vec;
	string m_sSource;
	static CSpliter Empty;
	
};

#endif // !defined(AFX_SPLITER_H__99EEC8A2_5C32_40EA_9B38_5D3E8A9C5085__INCLUDED_)
