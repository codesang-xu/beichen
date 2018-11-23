#ifndef __HTTP_CURL_H__
#define __HTTP_CURL_H__

#include <string>
#include <map>
#include "curl/curl.h"


class CHttpClient
{
public:
	CHttpClient(void);
	~CHttpClient(void);

    typedef std::map<std::string, std::string> map_req_headers;
public:
	/**
	* @brief HTTP POST请求
	* @param strUrl 输入参数,请求的Url地址,妿?http://www.baidu.com
	* @param strPost 输入参数,使用如下格式para1=val1&para2=val2&??
	* @param strResponse 输出参数,返回的内宿?
	* @return 返回是否Post成功
	*/
	CURLcode Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse);

	/**
	* @brief HTTP GET请求
	* @param strUrl 输入参数,请求的Url地址,妿?http://www.baidu.com
	* @param strResponse 输出参数,返回的内宿?
	* @return 返回是否Post成功
	*/
	CURLcode Get(const std::string & strUrl, std::string & strResponse);

	/**
	* @brief HTTPS POST请求,无证书版朿?
	* @param strUrl 输入参数,请求的Url地址,妿?https://www.alipay.com
	* @param strPost 输入参数,使用如下格式para1=val1&para2=val2&??
	* @param strResponse 输出参数,返回的内宿?
	* @param pCaPath 输入参数,为CA证书的路徿?.如果输入为NULL,则不验证服务器端证书的有效濿??.
	* @return 返回是否Post成功
	*/
	CURLcode Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath = NULL);

	/**
	* @brief HTTPS GET请求,无证书版朿?
	* @param strUrl 输入参数,请求的Url地址,妿?https://www.alipay.com
	* @param strResponse 输出参数,返回的内宿?
	* @param pCaPath 输入参数,为CA证书的路徿?.如果输入为NULL,则不验证服务器端证书的有效濿??.
	* @return 返回是否Post成功
	*/
	CURLcode Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath = NULL);

    CURLcode Gets(const std::string & strUrl, const std::string & strData, std::string & strResponse,
        const map_req_headers& mapHeaders, const char * pCaPath = NULL);

    CURLcode Put(const std::string & strUrl, const std::string & strData,std::string & strResponse, 
        const map_req_headers& mapHeaders,const char * pCaPath = NULL);
    CURLcode Delete(const std::string & strUrl, const std::string & strData, std::string & strResponse, 
        const map_req_headers& mapHeaders,const char * pCaPath = NULL);

	/**
	* HTTP2 Post请求
	* curl编译支持nghttp2
	*/
	CURLcode Post2(const std::string & strUrl, const std::string & strPost, std::string & strResponse,
        const map_req_headers& mapHeaders, const char * pCaPath = NULL);

public:
	void SetDebug(bool bDebug);

private:
	bool m_bDebug;
};

#endif
