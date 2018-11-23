#include "MemLog.h"
#include <boost/thread.hpp>




CMemLog::CMemLog()
{
	m_cSeptor  = '|';
	m_nLogType = TIME_LOG;
#ifdef _MSC_VER
	m_strPath = "C:\\IPCC_Run_Log\\";
#else
	m_strPath="/var/log/IPCC_Run_Log";
#endif

	m_strPreFileName = "Info";
	m_strFileNameExt = ".LOG";

}

CMemLog::~CMemLog()
{

}

bool CMemLog::Init(int nLogType, string strPath, string strPreFileName, string strLogExt)
{
	if (strPath.empty())
		return false;

	m_strPath = strPath;
	m_nLogType = nLogType;
	m_strPreFileName = strPreFileName;
	m_strFileNameExt = strLogExt;

	return true;
}

bool CMemLog::Start()
{
	// 启动写日志线程
	cout << "Location of the log files are in the path "<< m_strPath << endl;
	startThread();
	return true;
}

bool CMemLog::Stop()
{
	stopThread();

	// 等待线程退出，不然可能跳错；不能死等。
	boost::unique_lock<boost::mutex> lock(m_mutexCond);
	boost::system_time const timeout=boost::get_system_time()+boost::posix_time::milliseconds(2000);
	m_hThreadStopEvent.timed_wait(lock,timeout);

	//////////////////////////////////////////////
	/*if (m_hEvent)
	{
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
	if (m_hThreadStopEvent)
	{
		CloseHandle(m_hThreadStopEvent);
		m_hThreadStopEvent = NULL;
	}*/

	return true;
}


bool CMemLog::startThread()
{
	m_bEnableRun = true;
	thrdLog tl(this);
	boost::thread thrd(tl);
	return true;
}

bool CMemLog::stopThread()
{
	m_bEnableRun = false;

	return true;
}

char CMemLog::GetSeptor()
{
	return m_cSeptor;
}

// 消息前加上时间
// 类型为 TIME_LOG : 后两参数应该为空，并且不会使用后两参数。
bool CMemLog::MakeLog( const string &msgContent, enuLogLevel eLL/*=LOG_LEVEL_NORMAL*/, string subPath /*= ""*/, string strSufFileName /*= ""*/ )
{
	char acLog[LogLen]={0};
	try
	{
		// Get Time
		boost::posix_time::ptime pt = boost::posix_time::second_clock::local_time();
		string strTime = boost::gregorian::to_iso_extended_string(pt.date())+" "+boost::posix_time::to_simple_string(pt.time_of_day());

		if (m_nLogType == TIME_LOG)
		{

			switch (eLL)
			{
			case LOG_LEVEL_NORMAL:
				snprintf(acLog,LogLen,"%s %s", strTime.c_str(), msgContent.c_str());
				break;
			case LOG_LEVEL__WARNING:
				snprintf(acLog,LogLen,"%s warning: %s", strTime.c_str(), msgContent.c_str());
				break;
			case LOG_LEVEL__ERROR:
				snprintf(acLog,LogLen,"%s error: %s", strTime.c_str(), msgContent.c_str());
				break;
			}

		}
		else if (m_nLogType == FILENAME_TIME_LOG)
		{
			if (!subPath.empty()  && subPath[subPath.length() - 1] != '\\')
			{
				subPath += "\\";
			}

			switch (eLL)
			{
			case LOG_LEVEL_NORMAL:
				snprintf(acLog,LogLen,"%s%s%s\\%s%s%s%c%s %s", m_strPath.c_str(), subPath.c_str(),
						strTime.substr(0, 10).c_str(),m_strPreFileName.c_str(), strSufFileName.c_str(),
						m_strFileNameExt.c_str(),GetSeptor(), strTime.c_str(), msgContent.c_str());   // 完整文件名|时间 日志信息
				break;
			case LOG_LEVEL__WARNING:
				snprintf(acLog,LogLen,"%s%s%s\\%s%s%s%c%s warning: %s", m_strPath.c_str(), subPath.c_str(),
						strTime.substr(0, 10).c_str(),m_strPreFileName.c_str(), strSufFileName.c_str(),
						m_strFileNameExt.c_str(),GetSeptor(), strTime.c_str(), msgContent.c_str());   // 完整文件名|时间 日志信息
				break;
			case LOG_LEVEL__ERROR:
				snprintf(acLog,LogLen,"%s%s%s\\%s%s%s%c%s error: %s", m_strPath.c_str(), subPath.c_str(), strTime.substr(0, 10).c_str(),
					m_strPreFileName.c_str(), strSufFileName.c_str(), m_strFileNameExt.c_str(),
					GetSeptor(), strTime.c_str(), msgContent.c_str());   // 完整文件名|时间 日志信息
				break;
			}
		}

		// first send msg
		{
			boost::mutex::scoped_lock sl(m_LogQueueLock);
			this->m_queueLogData.push(acLog);
		}

		// then signal event
		m_cond.notify_one();
	}
	catch (...)
	{
		return false;
	}
	return true;
}


bool CMemLog::getMsg(string &msgContent)
{
	bool bRet = true;

	try
	{
		boost::mutex::scoped_lock sl(m_LogQueueLock);
		if(this->m_queueLogData.size() > 0)
		{
			msgContent = this->m_queueLogData.front();
			this->m_queueLogData.pop();
			bRet = true;
		}
		else
		{
			bRet = false;
		}
	}
	catch (...)
	{
		bRet = false;
	}

	return bRet;
}

//boost::uint32_t  CMemLog::MakeLogThread(void * pParam)
//{
//	CMemLog *pThis = (CMemLog*)pParam;
//
//	pThis->Run();
//
//	return 0;
//}

// In : strTime = "2010-05-14 10:00:00 InfoContent";
// Out: "C:\\log\\2010-05-14\\SesTestLog_10.txt"
bool CMemLog::GetFileName(string &strFileName, string &strLog)
{
	if (m_nLogType == TIME_LOG)
	{
			boost::filesystem::path p=boost::filesystem::path(m_strPath)
			/ boost::filesystem::path(strLog.substr(0,10))
			/boost::filesystem::path(m_strPreFileName+strLog.substr(11, 2)+m_strFileNameExt);
			strFileName = p.string();
	}
	else if (m_nLogType == FILENAME_TIME_LOG)
	{
		int nSeptorPos = 0;
		nSeptorPos = strLog.find(GetSeptor());  // '|'
		if (nSeptorPos >= 0)
		{
			strFileName = strLog.substr(0,nSeptorPos);
			strLog = strLog.substr(nSeptorPos+1,strlen(strLog.c_str()));
			//strLog.delete(0,nSeptorPos + 1);
		}
	}

	return true;
}


/*bool CMemLog::CreateMultipleLevelDirectory(string strDir)
{
	file*		fFile;					// File Handle
	//WIN32_FIND_DATA fileinfo;			// File Information Structure
	deque<string>	m_arr;				// deque to hold Directory Structures
	bool tt;							// BOOL used to test if Create Directory was successful
	int x1 = 0;							// Counter
	string tem = "";					// Temporary string Object

	//进行容错处理，使之能处理以"\"结尾的路径
	if (strDir.length() >0 )
	{
		if (strDir[strDir.length()-1]== '\\')
		{//最后是一个反斜杠
			strDir.erase(strDir.end()-1);
		}
	}

	// Before we go to a lot of work.
	// Does the file exist

	fFile = FindFirstFile(strDir.c_str(),&fileinfo);

	// if the file exists and it is a directory
	if(fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	{
		//  Directory Exists close file and return
		FindClose(fFile);
		return TRUE;
	}

	m_arr.clear();			// Not really necessary - Just habit
	for(x1=0;x1<strDir.length();x1++)	// Parse the supplied string Directory String
	{
		if(strDir[x1] != '\\')		// if the Charachter is not a \
			tem += strDir[x1];		// else add character to Temp String
		else
		{
			m_arr.push_back(tem);			// if the Character is a \ Add the Temp string to the deque
			tem += "\\";				// Now add the \ to the temp string
		}
		if(x1 == strDir.length()-1)		// If we reached the end of the file add the remaining string
			m_arr.push_back(tem);
	}

	// Close the file
	FindClose(fFile);

	// Now lets cycle through the String Array and create each directory in turn
	deque<string>::iterator itItem = m_arr.begin();
	while(itItem != m_arr.end())
	{
		tem = *itItem; itItem++;
		tt = (::CreateDirectory(tem.c_str(),NULL)==FALSE)?false:true;
		// If the Directory exists it will return a false
		if(tt)
		{
			SetFileAttributes(tem.c_str(),FILE_ATTRIBUTE_NORMAL);
		}
		// If we were successful we set the attributes to normal
	}
	m_arr.clear();
	//  Now lets see if the directory was successfully created
	fFile = FindFirstFile(strDir.c_str(),&fileinfo);

	// if the file exists and it is a directory
	if(fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	{
		//  Directory Exists close file and return
		FindClose(fFile);
		return TRUE;
	}
	else
	{
		FindClose(fFile);
		return FALSE;
	}
}
*/
int CMemLog::Run()
{
	string strLog = "";
	string strFileName = "";
	string sPreFilename = "";

	int nWriteCount = 0;

	FILE *fp = NULL;
	while (m_bEnableRun)
	{

		// Wait for event
		boost::unique_lock<boost::mutex> lock(m_mutexCond);
		boost::system_time const timeout=boost::get_system_time()+boost::posix_time::milliseconds(1000);
		m_cond.timed_wait(lock, timeout);

			try
			{
				// 取出并处理RecvQueue中的所有消息
				while (getMsg(strLog) && m_bEnableRun)
				{
					if (strLog.length() < 19)	// 2010-05-19 11:06:13 InfoContent
						continue;

					GetFileName(strFileName, strLog);
					if (strFileName.empty())
						continue;

					//////////////////////////////////////////////////////////////////////////
					// 修改思路：连续写入很多次需要关闭休息下。写入的文件名不同也要关闭。

					// 是否还是连续写入上次相同的文件中。
					if (strFileName.compare(sPreFilename) != 0)
					{
						sPreFilename = strFileName;
						if (fp != NULL)		// 不同文件, 需要先关闭以前的。
						{
							fclose(fp);
							fp = NULL;
						}
					}
					else
					{
						//	相同文件. 如果文件描述符仍然打开，就直接写入。

					}

					if (fp == NULL)
					{
						// 一次打开不了就创建该路径
						fp = fopen(strFileName.c_str(),"a");
						if (fp == NULL)
						{
							boost::filesystem::path p = strFileName;

							boost::filesystem::create_directories(p.parent_path());
							fp = fopen(strFileName.c_str(),"a");
						}

					}
					if (fp != NULL)
					{
						fprintf(fp,"%s\n",strLog.c_str());
					}

					//	连续写入或打开了很多次,写入的文件每次都不一样都需要休息。都需要休息一下。
					nWriteCount++;
					if (nWriteCount >= 1000)
					{
						if (fp != NULL)
						{
							fclose(fp);
							fp = NULL;
						}

						nWriteCount = 0;
						//Sleep(1);
                        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
					}
				}//while

				if (fp)
				{
					fclose(fp);
					fp = NULL;
				}
			}
			catch (std::exception& e)
			{
			}
			catch (...)
			{
			}


	}

	if (fp)
	{
		fclose(fp);
		fp = NULL;
	}

	//if (m_hThreadStopEvent)
	//{
	//	//SetEvent(m_hThreadStopEvent);
		m_hThreadStopEvent.notify_all();
	//}

	return 0;
}
