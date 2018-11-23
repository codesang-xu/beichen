// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOG_H__58E47164_922D_44FA_8244_455F06089EFD__INCLUDED_)
#define AFX_LOG_H__58E47164_922D_44FA_8244_455F06089EFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)
#include <string>
#include <iostream>
#include <sstream>
#include <deque>
using namespace std;

#include <stdarg.h>

class CLog  
{	
public:
	static int GetLocalHour();
	void release();
	//HANDLE m_StopThreadEvent;
	//HANDLE m_LogEvent;
	bool GetLogFromQueue(string& log);
	void AddLogToQueue(string log);
	//CRITICAL_SECTION m_LogQueueLock;
	deque<string> m_LogQueue;
	static void DirectWriteFile(string path, string fileName,string msg);
	void ResumeMakeLog();
	void StopMakeLog();
	string m_sDirectory;
	string m_sFileName;
	bool m_bIsStop;
	virtual void MakeLog(string logType, string logID, string devID, string logContent){};
	virtual void MakeLog(string logContent){};
	virtual void init(string directory, string fileName){};
//	virtual CLog* GetInstance(){return NULL;}; 
	CLog();
	virtual ~CLog();
	bool m_bMakeLog;
	static string GetLocalTime();
};


#endif // !defined(AFX_LOG_H__58E47164_922D_44FA_8244_455F06089EFD__INCLUDED_)
