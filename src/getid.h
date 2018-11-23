#ifndef __GETID_H__ 
#define __GETID_H__


#include <stdio.h>
#ifdef _WIN32
#include <sys\timeb.h>
 #include <windows.h>
  #include <TCHAR.H >
#else
#include<sys/types.h>
#include <sys/timeb.h>
#include <time.h>
#include <sys/times.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h> 

#include <stdlib.h>
#include <stdio.h>
#include<time.h>
#include<sys/time.h>
#endif
#include <string>
using namespace std;
#define ID_LENGTH 24+1
#define NAME_LENGTH 4+1
#ifndef _MAC
#define MAX_COMPUTERNAME_LENGTH 15
#else
#define MAX_COMPUTERNAME_LENGTH 31
#endif

#ifdef _WIN32
class GetGId
{
public:
	GetGId()
	{
		bstr = ::SysAllocStringByteLen(NULL,12);
		static long loopup = 0;
		static TCHAR name[MAX_COMPUTERNAME_LENGTH + 1];
		unsigned long size = MAX_COMPUTERNAME_LENGTH + 1;
		static BOOL bName = FALSE;
		if( ! bName ){
			::GetComputerName(name,&size);
			bName = TRUE;
		}
		_timeb systime;
		_ftime(&systime);		

		long *p = (long *)(bstr);
		*p++ = (long)systime.time;
		unsigned long tmp = (long)(systime.millitm << 22);
		tmp |= ((loopup & 0x200000) ? (loopup = 0) : loopup++);
		*p++ = tmp;

		if(bName){
			char *pc = (char *)p;
			for(int i=0; i<4;i++){
				*pc++ = (char)name[i];	//只取前面四位；
			}
		}
	}
	~GetGId(){ ::SysFreeString(bstr); };
	inline operator BSTR() const
	{
		return bstr;
	}
protected:
	BSTR bstr;
};

class BSTRToSessionId
{
public:
	BSTRToSessionId(BSTR src):m_str(NULL)
	{
		if(::SysStringByteLen(src) == 12){
			long *p = (long *)src;
			m_str = new TCHAR[ID_LENGTH];
			long time = *p++;
			unsigned long milt = *p;			milt >>= 22;
			long lookup = 0x3FFFFF  & *p++;
			TCHAR name[5];
			char *pc = (char *)p;
			for(int i=0;i<4;i++){
				name[i] = (TCHAR)(*pc);
				++pc;
			}
			name[4] = _T('\0');
			_stprintf(m_str,_T("%010d%03d%07d%s"),time,milt,lookup,name);
		}
	};
	~BSTRToSessionId()
	{
		if(m_str)	delete m_str;
	};
	inline operator LPCTSTR() const
	{
		return m_str;
	}
	inline TCHAR *GetString() const
	{
		return m_str;
	}
protected:
	TCHAR *m_str;
};
#else
class GetGId
{
public:
	GetGId()
	{
            char   tmepch[100];
		static long loopup = 0;
		char name[MAX_COMPUTERNAME_LENGTH + 1];
		unsigned long size = MAX_COMPUTERNAME_LENGTH + 1;
		static bool bName = false;
		if( ! bName )
                {
			string sthostname = getenv("HOSTNAME");
                        strcpy(name,sthostname.c_str());
			bName = false;
		}
		time_t systime;
                struct tm *timenow; 
		time(&systime);   
                timenow = localtime(&systime);  		
                struct timeval t;
                gettimeofday(&t, NULL); 
		long *p = (long *)(tmepch);
		*p++ = (long)systime;
		unsigned long tmp = (long)(t.tv_usec<< 22);
		tmp |= ((loopup & 0x200000) ? (loopup = 0) : loopup++);
		*p++ = tmp;
         bstr=tmepch;
         bstr= bstr.substr(0,12);
           
	}
	~GetGId(){ };
	inline operator string () const
	{
		return bstr;
	}
public:
	string  bstr;
};

class BSTRToSessionId
{
public:
	BSTRToSessionId(char*  src):m_str(NULL)
	{
            string temp=src;
		if(temp.length() == 12){
			long *p = (long *)src;
			m_str = new char[ID_LENGTH];
			long time = *p++;
			unsigned long milt = *p;			milt >>= 22;
			long lookup = 0x3FFFFF  & *p++;
			char name[5];
			char *pc = (char *)p;
			for(int i=0;i<4;i++){
				name[i] = (char)(*pc);
				++pc;
			}
			name[4] = '\0';
			sprintf(m_str,"%010d%03d%07d%s",time,milt,lookup,name);
		}
	};
	~BSTRToSessionId()
	{
		if(m_str)	delete m_str;
	};
	inline operator char*() const
	{
		return m_str;
	}
	inline char  *GetString() const
	{
		return m_str;
	}
protected:
	char  *m_str;
};

#endif

#endif
