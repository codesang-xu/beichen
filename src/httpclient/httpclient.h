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
	* @brief HTTP POST����
	* @param strUrl �������,�����Url��ַ,��?http://www.baidu.com
	* @param strPost �������,ʹ�����¸�ʽpara1=val1&para2=val2&??
	* @param strResponse �������,���ص�����?
	* @return �����Ƿ�Post�ɹ�
	*/
	CURLcode Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse);

	/**
	* @brief HTTP GET����
	* @param strUrl �������,�����Url��ַ,��?http://www.baidu.com
	* @param strResponse �������,���ص�����?
	* @return �����Ƿ�Post�ɹ�
	*/
	CURLcode Get(const std::string & strUrl, std::string & strResponse);

	/**
	* @brief HTTPS POST����,��֤���c?
	* @param strUrl �������,�����Url��ַ,��?https://www.alipay.com
	* @param strPost �������,ʹ�����¸�ʽpara1=val1&para2=val2&??
	* @param strResponse �������,���ص�����?
	* @param pCaPath �������,ΪCA֤���·��?.�������ΪNULL,����֤��������֤�����Ч�W??.
	* @return �����Ƿ�Post�ɹ�
	*/
	CURLcode Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath = NULL);

	/**
	* @brief HTTPS GET����,��֤���c?
	* @param strUrl �������,�����Url��ַ,��?https://www.alipay.com
	* @param strResponse �������,���ص�����?
	* @param pCaPath �������,ΪCA֤���·��?.�������ΪNULL,����֤��������֤�����Ч�W??.
	* @return �����Ƿ�Post�ɹ�
	*/
	CURLcode Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath = NULL);

    CURLcode Gets(const std::string & strUrl, const std::string & strData, std::string & strResponse,
        const map_req_headers& mapHeaders, const char * pCaPath = NULL);

    CURLcode Put(const std::string & strUrl, const std::string & strData,std::string & strResponse, 
        const map_req_headers& mapHeaders,const char * pCaPath = NULL);
    CURLcode Delete(const std::string & strUrl, const std::string & strData, std::string & strResponse, 
        const map_req_headers& mapHeaders,const char * pCaPath = NULL);

	/**
	* HTTP2 Post����
	* curl����֧��nghttp2
	*/
	CURLcode Post2(const std::string & strUrl, const std::string & strPost, std::string & strResponse,
        const map_req_headers& mapHeaders, const char * pCaPath = NULL);

public:
	void SetDebug(bool bDebug);

private:
	bool m_bDebug;
};

#endif
