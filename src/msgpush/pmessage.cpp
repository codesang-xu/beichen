#include <boost/locale.hpp>
#include "pmessage.h"
#include "json/json.h"

string SXG_Message_Andriod::ToJson()
{
	Json::Value root;
	Json::FastWriter fw;

    root["title"] = boost::locale::conv::between(title,"UTF-8","GBK");
    root["content"] = boost::locale::conv::between(content, "UTF-8", "GBK");;
	root["n_id"] = n_id;
	root["builder_id"] = builder_id;
	root["vibrate"] = vibrate;

	if (!custom_content.empty())
	{
		Json::Value custominfo;
		std::map<string, string>::const_iterator iter = custom_content.begin();
		while (iter != custom_content.end())
		{
			custominfo[iter->first]= iter->second;
			iter++;
		}
		root["custom_content"] = custominfo;
	}

	return fw.write(root);

}

string SXG_Message_IOS::ToJson()
{
	Json::Value root,aps,alert;
	Json::FastWriter fw;

    alert["title"] = boost::locale::conv::between(title, "UTF-8", "GBK");
    alert["body"] = boost::locale::conv::between(body, "UTF-8", "GBK");

	aps["alert"] = alert;
	aps["badge"] = badge;

	root["aps"] = aps;
	if (!custom.empty())
	{
		Json::Value custominfo;
		std::map<string, string>::const_iterator iter = custom.begin();
		while (iter != custom.end())
		{
			custominfo[iter->first] = iter->second;
			iter++;
		}
		root["custom"] = custominfo;
	}

	return fw.write(root);	 
}

string SAPNs_Message::ToJson()
{
	Json::Value root, aps, alert;
	Json::FastWriter fw;

    alert["title"] = boost::locale::conv::between(title, "UTF-8", "GBK");;
    alert["body"] = boost::locale::conv::between(body, "UTF-8", "GBK");;

	aps["alert"] = alert;
	aps["badge"] = badge;
	
	if (!userinfo.empty())
	{
		Json::Value custominfo;
		std::map<string, string>::const_iterator iter = userinfo.begin();
		while (iter != userinfo.end())
		{
			custominfo[iter->first] = iter->second;
			iter++;
		}
		aps["userinfo"] = custominfo;
	}

	root["aps"] = aps;

	return fw.write(root);

}