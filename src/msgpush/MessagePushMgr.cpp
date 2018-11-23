#include <boost/thread.hpp>

#include "MessagePushMgr.h"
#include "xg_url.h"
#include "pmessage.h"
#include "json/json.h"

CMessagePushMgr::CMessagePushMgr()
{
}


CMessagePushMgr::~CMessagePushMgr()
{
	boost::mutex::scoped_lock sl(m_MsgQueueLock);
	while (m_queueMsg.size() > 0)
	{
		m_queueMsg.pop();
	}
}


CMessagePushMgr* CMessagePushMgr::GetInstance()
{
	static CMessagePushMgr pushMgr;
	return &pushMgr;
}

void  CMessagePushMgr::Init(SPushServerConfig config, makelogfunc logfunc)
{
	m_config = config;
	m_logfunc = logfunc;
}

bool  CMessagePushMgr::PushMessage(SPushMessage_ptr pmsg)
{

	{
		boost::mutex::scoped_lock sl(m_MsgQueueLock);
		this->m_queueMsg.push(pmsg);
	}

	m_cond.notify_one();

	return true;
}

void  CMessagePushMgr::Start()
{
	m_bEnableRun = true;

	m_AsyncHttpClient_ptr.reset(new CAsyncHttpClient());
	m_AsyncHttpClient_ptr->init(30);

	m_JWTEncode_ptr.reset(new JWTEncode(m_config.AuthKey));
	
	thrdPush tl(this);
	boost::thread thrd(tl);

}

void  CMessagePushMgr::Stop()
{
	m_bEnableRun = false;
	// 等待线程退出，不然可能跳错；不能死等。
	boost::unique_lock<boost::mutex> lock(m_mutexCond);
	boost::system_time const timeout = boost::get_system_time() + boost::posix_time::milliseconds(2000);
	m_hThreadStopEvent.timed_wait(lock, timeout);
}

bool CMessagePushMgr::getMsg(SPushMessage_ptr& pmsg)
{
	bool bRet = true;

	try
	{
		boost::mutex::scoped_lock sl(m_MsgQueueLock);
		if (this->m_queueMsg.size() > 0)
		{
			pmsg = this->m_queueMsg.front();
			this->m_queueMsg.pop();
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

int  CMessagePushMgr::Run()
{
	while (m_bEnableRun)
	{
		// Wait for event
		boost::unique_lock<boost::mutex> lock(m_mutexCond);
		boost::system_time const timeout = boost::get_system_time() + boost::posix_time::milliseconds(1000);
		m_cond.timed_wait(lock, timeout);

		SPushMessage_ptr msg;
		while (getMsg(msg) && m_bEnableRun)
		{
			//通过苹果的APNs服务推送
			if (msg->pushway == Push_APNs)
			{
				SAPNs_Message apns_msg;
				apns_msg.userinfo = msg->custom_info;
				if (!msg->content.empty())
					apns_msg.body = msg->content;

				string msg_js = apns_msg.ToJson();
				string token = msg->devicetoken;
				string account = msg->account;

				APNs_Push_Message(token, account, msg_js);
			}
			else     //腾讯信鸽推送
			{	
				//IOS手机
				if (msg->ttype == Terminal_IOS)
				{
					SXG_Message_IOS xg_ios_msg;
					xg_ios_msg.custom = msg->custom_info;
					if (!msg->content.empty())
						xg_ios_msg.body = msg->content;
					string msg_js = xg_ios_msg.ToJson();
					string account = msg->account;

					XG_Push_Message(Terminal_IOS, account, msg_js);
				}
				else //Android手机
				{
					SXG_Message_Andriod xg_android_msg;
					xg_android_msg.custom_content = msg->custom_info;
					if (!msg->content.empty())
						xg_android_msg.content = msg->content;
					string msg_js = xg_android_msg.ToJson();
					string account = msg->account;

					XG_Push_Message(Terminal_Android, account, msg_js);

				}
				
			}
		}
	}
	m_hThreadStopEvent.notify_all();
	return 0;
}

void CMessagePushMgr::XG_Push_Message(ETerminalType ttype, string account, string message)

{
	string accessid("");
	string secretkey("");
	int env = 0;

	if (ttype == Terminal_IOS)
	{
		accessid = m_config.sXG_IOS_AccessID;
		secretkey = m_config.sXG_IOS_SecretKey;
		env = m_config.iXG_IOS_Environment;
	}
	else
	{
		accessid = m_config.sXG_Android_AccessID;
		secretkey = m_config.sXG_Android_SecretKey;
	}


	std::string url = "http://openapi.xg.qq.com/v2/push/single_account";
	xg_url xgurl(accessid, secretkey, account, message, env);
	std::string strParas = xgurl.get_strParams();

	{
		ostringstream os;
		string log("");
		os << "send http post,"
			<< "account=" << account
			<< ",full url = " << url << "?" << strParas
			<< ",msg=" << message;
		log = os.str();
		m_logfunc(log);
	}

	m_AsyncHttpClient_ptr->HttpPost(url, strParas,
		[&](bool bOk, std::string& strRsp)
	{
		stringstream os;
		string log("");
		os << "http post response: bOk="
			<< (bOk ? "true" : "false")
			<< ",strRsp=" << strRsp;
		log = os.str();
		m_logfunc(log);

		/*
		if (bOk)
		{
			Json::Reader reader;
			Json::Value root;
			reader.parse(strRsp, root);

			if (root != Json::nullValue)
			{
				int ret_code = root.get("ret_code", 0).asInt();
				if (ret_code != 0)
				{
					string err_msg = root.get("err_msg", "").asString();
					stringstream os;
					string log("");
					os << "http post response:ret_code="<<ret_code
						<<",err_msg="<<err_msg;
					log = os.str();
					m_logfunc(log);

				}
			}

		}*/
	});
}

void CMessagePushMgr::APNs_Push_Message(string devicetoken, string account, string message)
{
	std::string provider_tokens = m_JWTEncode_ptr->get_provider_authentication_tokens(m_config.AuthKeyID, m_config.TeamID);
	std::string url = "https://api.development.push.apple.com:443/3/device/" + devicetoken;

	std::map<string, string> headers;
	headers["content-type"] = "application/json";
	headers["apns-topic"] = m_config.BundleId;
	headers["authorization"] = string("bearer ") + provider_tokens;

	if (message.empty())
	{
		message = "{\"aps\":{\"alert\":\"Hello\"}}";
	}

	{
		ostringstream os;
		string log("");
		os << "send http2 post,"
			<< "account=" << account
			<< ",full url = " << url
			<< ",msg=" << message;
		log = os.str();
		m_logfunc(log);
	}

	m_AsyncHttpClient_ptr->Http2Post(url, message, headers, m_config.CertFile,
		[=](bool bOk, std::string& strRsp)
	{

		stringstream os;
		string log("");
		os << "http2 post response: bOk="
			<< (bOk ? "true" : "false")
			<< ",strRsp=" << strRsp;
		log = os.str();
		m_logfunc(log);


	});
}