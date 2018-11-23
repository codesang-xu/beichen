#ifndef  _XG_URL_H
#define  _XG_URL_H

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include<map>
#include<assert.h>

#include <boost/lexical_cast.hpp>

const std::string XG_HOST = "http://openapi.xg.qq.com";
const std::string XG_PATH = "/v2/push/single_account";

class xg_url
{
public:
	xg_url(std::string accessid, std::string secretkey, std::string account, 
		std::string message = "", uint32_t enviroment = 0, uint32_t message_type = 1);
	
	std::string get_strParams();
	std::string get_parameter(std::string name);	

private:	
	uint32_t gen_timestamp();
	std::string gen_sign();
private:
	std::string accessid_;
	std::string secretkey_;
	std::map<std::string, std::string> m_mapParams;
};


#endif
