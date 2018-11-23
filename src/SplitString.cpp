// SplitString.cpp: implementation of the CSplitString class.
//
//////////////////////////////////////////////////////////////////////

#include "public/StdAfx.h"
#include "SplitString.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSplitString::CSplitString()
{

}

CSplitString::~CSplitString()
{

}

void CSplitString::SplitString(string& originString, string splitString, list<string>& retList)
{
	int splitStrLen = splitString.length();
	string sOneRetString("");

	if(originString.length() == 0)
		return;

	int iPos = originString.find(splitString);

	if(iPos == -1)
	{
		sOneRetString = originString;
		retList.push_back(sOneRetString);
		return;
	}
	else if(iPos == 0)
	{
		if(originString.length() == 1)
			return;
		originString = originString.substr(iPos+splitStrLen, originString.length()-splitStrLen);
		SplitString(originString, splitString, retList);
		return;
	}

	sOneRetString = originString.substr(0, iPos);
	retList.push_back(sOneRetString);
	if(iPos == originString.length()-splitStrLen)
		return;
	originString = originString.substr(iPos+splitStrLen, originString.length()-splitStrLen);
	SplitString(originString, splitString, retList);
	return;
}
