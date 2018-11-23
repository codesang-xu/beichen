// Spliter.cpp: implementation of the CSpliter class.
//
//////////////////////////////////////////////////////////////////////
#include "Spliter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpliter::CSpliter(string sSource, string sSplit1, string sSplit2, string sSplit3)
{
	m_sSource = sSource;
	if ( sSplit1 == "" )
	{
		return;
	}

	int iPos;
	try{
		while ( ( iPos = sSource.find( sSplit1 ) ) != string::npos )
		{
			string sLeft = sSource.substr( 0, iPos );
			sSource = sSource.substr( iPos + sSplit1.length() );
			if ( sSplit3 != "" )
			{
				m_Vec.push_back( CSpliter( sLeft, sSplit2, sSplit3 ) );
			}
			else if ( sSplit2 != "" )
			{
				m_Vec.push_back( CSpliter( sLeft, sSplit2 ) );
			}
			else
			{
				m_Vec.push_back( CSpliter( sLeft ) );
			}
		}
	}
	catch(...)
	{
	}

	if ( sSplit3 != "" )
	{
		m_Vec.push_back( CSpliter( sSource, sSplit2, sSplit3 ) );
	}
	else if ( sSplit2 != "" )
	{
		m_Vec.push_back( CSpliter( sSource, sSplit2 ) );
	}
	else
	{
		m_Vec.push_back( CSpliter( sSource ) );
	}
}

CSpliter::~CSpliter()
{

}

CSpliter CSpliter::Empty = string("");

