#pragma once
#include<string>
#include<queue>
#include <map>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "httpclient/asynchttpclient.h"
#include "jwt.h"

using namespace std;

enum EPushWay
{
	Push_APNs = 0, //苹果APNs服务，只能推送IOS终端
	Push_XG = 1   //腾讯信鸽推送平台，可推送IOS和Andriod 终端
};

enum ETerminalType
{
	Terminal_IOS = 0,
	Terminal_Android = 1
};

typedef struct __SPushServerConfig
{
	string sXG_IOS_AccessID;
	string sXG_IOS_SecretKey;
	int iXG_IOS_Environment;//默认为1，生产环境；2表示开发环境；

	string sXG_Android_AccessID;
	string sXG_Android_SecretKey;

	string BundleId;
	string AuthKey;
	string AuthKeyID;
	string TeamID;
	string CertFile;

}SPushServerConfig;

typedef struct __SPushMessage
{
	EPushWay pushway;
	ETerminalType ttype;
	string account;
	string content;
	string devicetoken;
	map<string, string> custom_info;

}SPushMessage;

typedef boost::shared_ptr<SPushMessage> SPushMessage_ptr;

class CMessagePushMgr
{
public:	
	typedef boost::function<void(const string &)> makelogfunc;

public:
	~CMessagePushMgr();
	static CMessagePushMgr* GetInstance();
	void Init(SPushServerConfig config, makelogfunc logfunc);
	bool PushMessage(SPushMessage_ptr pmsg);
	void Start();
	void Stop();
	int Run();

protected:
	CMessagePushMgr();
	CMessagePushMgr(const CMessagePushMgr&);

private:

	bool getMsg(SPushMessage_ptr& pmsg);
	void XG_Push_Message(ETerminalType ttype, string account, string message);
	void APNs_Push_Message(string devicetoken, string account, string message);

	SPushServerConfig m_config;
	makelogfunc m_logfunc;

	boost::condition_variable m_hThreadStopEvent;
	boost::mutex m_mutexCond;
	boost::condition_variable m_cond;
	bool   m_bEnableRun;

	std::queue<SPushMessage_ptr> m_queueMsg;
	boost::mutex m_MsgQueueLock;

	boost::scoped_ptr<CAsyncHttpClient> m_AsyncHttpClient_ptr;
	boost::scoped_ptr<JWTEncode> m_JWTEncode_ptr;
};

class thrdPush
{
public:
	CMessagePushMgr *pThis;
	thrdPush(CMessagePushMgr *p) :pThis(p){}
	void operator()()
	{
		pThis->Run();
	}

};

