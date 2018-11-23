#pragma once

#include <string>
using namespace std;

class SIPCCMessage;
class CIpccMsgLogTrans
{
public:
	CIpccMsgLogTrans(void);
public:
	virtual ~CIpccMsgLogTrans(void);
public:
	static bool LogToMsg(string sLog, SIPCCMessage* msg);
	static string MsgToLog(SIPCCMessage* pMsg);
};
