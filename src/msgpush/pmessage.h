#pragma once

#ifdef PROJECT_INT_DEF
#include "public/intdef.h"
#else
#ifdef CSTD_INT
#include <cstdint>
#else
#include <boost/cstdint.hpp>
using namespace boost;
#endif
#endif

#include <vector>
#include <map>
#include <string>

using namespace std;

const string STR_MSG_PUSH_TITLE = "Ѷ���ƶ��ͷ�"; 
const string STR_MSG_PUSH_BODY = "����һ���ͻ�����";

struct SXG_Message_Andriod
{
	string title;
	string content;
	int n_id;
	int builder_id;
	int ring;
	int vibrate;
	std::map<string, string> custom_content;

	SXG_Message_Andriod()
	{
        title = STR_MSG_PUSH_TITLE;
        content = STR_MSG_PUSH_BODY;
		n_id = 0;
		builder_id = 0;
		ring = 1;
		vibrate = 1;
	}

	string ToJson();

};

struct SXG_Message_IOS
{
	string title;
	string body;
	int badge;
	std::map<string, string> custom;

	SXG_Message_IOS()
	{
		title = STR_MSG_PUSH_TITLE;
		body = STR_MSG_PUSH_BODY;
		badge = 1;
	}
	string ToJson();
};

struct SAPNs_Message
{
	string title;
	string body;
	int badge;
	std::map<string, string> userinfo;

	SAPNs_Message()
	{
		title = STR_MSG_PUSH_TITLE;
		body = STR_MSG_PUSH_BODY;
		badge = 1;
	}
	string ToJson();
};
