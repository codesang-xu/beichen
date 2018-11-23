#include <boost/date_time/posix_time/posix_time.hpp>

#include "xg_url.h"
#include "md5.h"

using namespace boost::posix_time;
using namespace std;

//const std::string  ACCESS_ID = "2200260928";
//const std::string  SECRET_KEY = "f638a3040ce3155bd754029d46506b1e";

const std::string  IOS_PUSH_MESSAGE = "{\"aps\":{\"alert\":\"hello\"}}";
const std::string  ANDRIOD_PUSH_MESSAGE = "{ \"content\":\"hello\", \"title\" : \"QTB\", \"vibrate\" : 1 }";

static unsigned char ToHex(unsigned char x)
{
	return  x > 9 ? x + 55 : x + 48;
}

static unsigned char FromHex(unsigned char x)
{
	unsigned char y;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	else assert(0);
	return y;
}

static std::string UrlEncode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (isalnum((unsigned char)str[i]) ||
			(str[i] == '-') ||
			(str[i] == '_') ||
			(str[i] == '.') ||
			(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ')
			strTemp += "%20";
		else
		{
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] % 16);
		}
	}
	return strTemp;
}

static std::string UrlDecode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (str[i] == '+') strTemp += ' ';
		else if (str[i] == '%')
		{
			assert(i + 2 < length);
			unsigned char high = FromHex((unsigned char)str[++i]);
			unsigned char low = FromHex((unsigned char)str[++i]);
			strTemp += high * 16 + low;
		}
		else strTemp += str[i];
	}
	return strTemp;
}

xg_url::xg_url(std::string accessid, std::string secretkey, std::string account, 
	std::string message /* = "" */, uint32_t enviroment /* = 0 */, uint32_t message_type /* = 1 */)
	:accessid_(accessid),
	secretkey_(secretkey)
{

	/*
	注：仅列出必要参数 详细参数请看 http://developer.qq.com/wiki/xg/%E6%9C%8D%E5%8A%A1%E7%AB%AFAPI%E6%8E%A5%E5%85%A5/Rest%20API%20%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/Rest%20API%20%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97.html

	参数名称        类型       描述

	//通用参数

	access_id        uint       appID,唯一
	timestamp        uint       本请求的时间戳
	sign             string     签名

	//请求参数（单个账号）

	account         string      要推送的帐号
	message_type    uint       消息类型，1：通知，2：透传
	message         string     消息类容
	enviroment      uint       IOS选项，1：生产环境，2：开发环境，Andriod不填或填0
	*/

	m_mapParams["access_id"] = accessid;
	m_mapParams["timestamp"] = boost::lexical_cast<std::string>(gen_timestamp());
	m_mapParams["account"] = account;
	m_mapParams["message_type"] = boost::lexical_cast<std::string>(message_type);

	
	if (message == "")
	{
		if (enviroment == 0)
		{
			m_mapParams["message"] = ANDRIOD_PUSH_MESSAGE;
		} 
		else
		{
			m_mapParams["message"] = IOS_PUSH_MESSAGE;
		}
		
	}
	else
	{
		m_mapParams["message"] = message;
	}
	
	m_mapParams["environment"] = boost::lexical_cast<std::string>(enviroment);

	m_mapParams["sign"] = gen_sign();
	
}

std::string xg_url::get_strParams()
{
	ostringstream os;

	string str_access_id = get_parameter("access_id");
	string str_timestamp = get_parameter("timestamp");
	string str_sign = get_parameter("sign");

	string str_account = get_parameter("account");
	string str_message_type = get_parameter("message_type");
	string str_message = get_parameter("message");
	string str_enviroment = get_parameter("environment");

	os << "access_id=" << UrlEncode(str_access_id)
		<< "&timestamp=" << UrlEncode(str_timestamp)
		<< "&sign=" << UrlEncode(str_sign)
		<< "&account=" << UrlEncode(str_account)
		<< "&message_type=" << UrlEncode(str_message_type)
		<< "&message=" << UrlEncode(str_message)
		<< "&environment=" << UrlEncode(str_enviroment);

	return  os.str();

}

std::string xg_url::get_parameter(std::string name)
{
	std::map<string, string>::iterator pIter = m_mapParams.find(name);
	if (pIter != m_mapParams.end())
	{
		return pIter->second;
	}

	return "";
}

string xg_url::gen_sign()
{
	string strBase = "POSTopenapi.xg.qq.com/v2/push/single_account";

	std::map<string, string>::iterator pIter;
	for (pIter = m_mapParams.begin(); pIter != m_mapParams.end();pIter++)
	{
		strBase += pIter->first;
		strBase += "=";
		strBase += pIter->second;
	}
	
	strBase += secretkey_;
	
	char szMd5[33];
	MD5_Calculate(strBase.c_str(), strBase.length(), szMd5);
	
	return string(szMd5);

}

uint32_t xg_url::gen_timestamp()
{
	boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::gregorian::Jan, 1));

	boost::posix_time::time_duration time_from_epoch =
		//boost::posix_time::microsec_clock::universal_time() - epoch;
		boost::posix_time::second_clock::universal_time() - epoch;

	//return time_from_epoch.total_microseconds();

	return time_from_epoch.total_seconds();
}
