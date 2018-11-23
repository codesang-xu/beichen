/*/////////////////////////////////////////////////////////////////////////
�����ã�������д��־�ࡣ
˵  ����
ʹ  �ã�
(1).TIME_LOG ����ʹ�ã�
	������е���� ��2010-05-19 11:06:13 ---Start SessionEngine Succeeded in normal mode!
	д����ļ����?? ��C:\\Log\\2010-05-19\\SesTestLog_11.LOG
	д���ļ������?? ��2010-05-19 11:06:13 ---Start SessionEngine Succeeded in normal mode!

	CMemLog mLog;
	mLog.Init(CMemLog::TIME_LOG, "C:\\Log\\", "SesTestLog_", ".LOG");
	mLog.Start();
	mLog.MakeLog("---Start SessionEngine Succeeded in normal mode!", "", "");

	mLog.Stop();		// �ͷ�ʱ���á�

(2).FILENAME_TIME_LOG ����ʹ�ã�
	������е���� ��C:\\utility\\Ӧ��\\����\\2010-05-19\\12700011788430000141INFO.LOG|2010-05-19 11:06:13 12700011788430000141INFO,2036 ---Start SessionEngine Succeeded in normal mode!
	д����ļ����?? ��C:\\utility\\Ӧ��\\����\\2010-05-19\\12700011788430000141INFO.LOG
	д���ļ������?? ��2010-05-19 11:06:13 12700011788430000141INFO,2036 ---Start SessionEngine Succeeded in normal mode!

	CMemLog mLog;
	//   mLog.Init(CMemLog::FILENAME_TIME_LOG, "C:\\utility\\", "SesFlowLog_", ".LOG");
	���� mLog.Init(CMemLog::FILENAME_TIME_LOG, "C:\\utility\\", "", ".LOG");
	mLog.Start();
	mLog.MakeLog("---Start SessionEngine Succeeded in normal mode!",
				 "Ӧ��\\����\\", "12700011788430000141INFO" );

	mLog.Stop();		// �ͷ�ʱ���á�

/*////////////////////////////////////////////////////////////////////////
// #pragma once
#ifndef __MEMLOG_H__
#define __MEMLOG_H__

#include <queue>
#include <cstdio>
#include <string.h>
#include <boost/thread/mutex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
using namespace std;
#define BOOST_DATE_TIME_SOURCE

const int LogLen=2048;

#if defined(_MSC_VER) || defined(__BORLANDC__)
#define snprintf _snprintf
#elif defined (__GNUC__)

#endif

enum enuLogLevel
{
	LOG_LEVEL_NORMAL = 0,	//
	LOG_LEVEL__ERROR = 1,	//
	LOG_LEVEL__WARNING = 2
};

class CMemLog
{
public:
	enum
	{
		TIME_LOG = 0,			// ��Ϣ��ʽΪ��2010-05-19 11:06:13 InfoContent
		FILENAME_TIME_LOG = 1	// ��Ϣ��ʽΪ��C:\\log\\Ӧ��\\����\\2010-05-19\\12700011745780000137INFO.LOG|2010-05-19 11:06:13 InfoContent"
	};
public:
	CMemLog();
	~CMemLog();
	bool Init(int nLogType, string strPath, string strLogSpec = "", string strLogExt = ".LOG");
	bool Start();
	bool MakeLog(const string &msgContent, enuLogLevel eLL=LOG_LEVEL_NORMAL, string subPath = "", string strSufFileName = "");
	bool Stop();

	int  Run();

	//static boost::uint32_t  MakeLogThread(void * pParam);
private:

	bool startThread();
	bool stopThread();
	bool getMsg(string &msgContent);

	char GetSeptor();
	bool GetFileName(string &strFileName, string &strLog);

private:
	string m_strPath;				//��־�ļ���Ŀ¼ "C:\\log\\"; log�ļ�����·��, �����ķ�б��
	string m_strPreFileName;		//����ļ����ǰ׺�����磺 "SesTestLog_";
	string m_strSufFileName;		//����ļ���ĺ�׺�����磺 "17"; ���ﲻʹ�á�����������ļ������ɲ��֡�??
	string m_strFileNameExt;		//����ļ������չ��,���� ".txt"

	int     m_nLogType;				//��־����
	char    m_cSeptor;				//�ļ�������־��Ϣ�ķָ���š�?? FILENAME_TIME_LOG ��Ч

	boost::condition_variable m_hThreadStopEvent;
	boost::mutex m_mutexCond;
	boost::condition_variable m_cond;
	bool   m_bEnableRun;			// �Ƿ�����д��־�̼߳������С�

	queue<string> m_queueLogData;
	boost::mutex m_LogQueueLock;
};

class thrdLog
{
public:
	CMemLog *pThis;
	thrdLog(CMemLog *p):pThis(p){}
	void operator()()
	{
		pThis->Run();
	}

};

#endif // __MEMLOG_H__