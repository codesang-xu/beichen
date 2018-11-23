#include "httpclient.h"
#include <string>

//#include <muduo/base/Logging.h>

CHttpClient::CHttpClient(void) : 
m_bDebug(false)
{

}

CHttpClient::~CHttpClient(void)
{

}

static int OnDebug(CURL *, curl_infotype itype, char * pData, size_t size, void *)
{
	if(itype == CURLINFO_TEXT)
	{
		//printf("[TEXT]%s\n", pData);
	}
	else if(itype == CURLINFO_HEADER_IN)
	{
		printf("[HEADER_IN]%s\n", pData);
	}
	else if(itype == CURLINFO_HEADER_OUT)
	{
		printf("[HEADER_OUT]%s\n", pData);
	}
	else if(itype == CURLINFO_DATA_IN)
	{
		printf("[DATA_IN]%s\n", pData);
	}
	else if(itype == CURLINFO_DATA_OUT)
	{
		printf("[DATA_OUT]%s\n", pData);
	}
	return 0;
}

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
	std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);
	if( NULL == str || NULL == buffer )
	{
		return -1;
	}

    char* pData = (char*)buffer;
    str->append(pData, size * nmemb);
	return nmemb;
}

CURLcode CHttpClient::Post( const std::string & strUrl, const std::string & strPost, std::string & strResponse )
{
	CURLcode res;
	CURL* curl = curl_easy_init(); // Get a curl handle to use for all forthcoming curl transfers
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}

	if(m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 13);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

CURLcode CHttpClient::Get( const std::string & strUrl, std::string & strResponse )
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if(m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
 	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	/**
	* 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。
	* 如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。
	*/
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 13);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1); // 设为0，则不会自动301，302跳转；

	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

CURLcode CHttpClient::Posts( const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath /*= NULL*/ )
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if(m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if(NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		//缺省情况就是PEM，所以无需设置，另外支持DER
		//curl_easy_setopt(curl,CURLOPT_SSLCERTTYPE,"PEM");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 13);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

CURLcode CHttpClient::Put(const std::string & strUrl, const std::string & strData, std::string & strResponse, 
    const map_req_headers& mapHeaders, const char * pCaPath)
{
    CURLcode res;
    CURL* curl = curl_easy_init();
    if (NULL == curl){
        return CURLE_FAILED_INIT;
    }
    if (m_bDebug){
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
    }

    struct curl_slist *chunk = NULL;
    if (mapHeaders.size() > 0)
    {

        std::map<std::string, std::string>::const_iterator it = mapHeaders.begin();
        std::string header("");
        for (; it != mapHeaders.end(); it++)
        {
            header = it->first + ":" + it->second;
            chunk = curl_slist_append(chunk, header.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    }

    //curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_PUT, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());

    curl_easy_setopt(curl, CURLOPT_READDATA, strData.c_str());
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)strData.length());

   
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    if (NULL == pCaPath){
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
    }
    else{
        //缺省情况就是PEM，所以无需设置，另外支持DER
        //curl_easy_setopt(curl,CURLOPT_SSLCERTTYPE,"PEM");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
        curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
    }
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 13);
    res = curl_easy_perform(curl);
    curl_slist_free_all(chunk);
    curl_easy_cleanup(curl);
    return res;
}

CURLcode CHttpClient::Delete(const std::string & strUrl, const std::string & strData, std::string & strResponse, 
    const map_req_headers& mapHeaders, const char * pCaPath )
{
    CURLcode res;
    CURL* curl = curl_easy_init();
    if (NULL == curl){
        return CURLE_FAILED_INIT;
    }
    if (m_bDebug){
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
    }

    struct curl_slist *chunk = NULL;
    if (mapHeaders.size() > 0)
    {

        std::map<std::string, std::string>::const_iterator it = mapHeaders.begin();
        std::string header("");
        for (; it != mapHeaders.end(); it++)
        {
            header = it->first + ":" + it->second;
            chunk = curl_slist_append(chunk, header.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    }

    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str()); 
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strData.c_str());     

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);

    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    if (NULL == pCaPath){
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
    }
    else{
        //缺省情况就是PEM，所以无需设置，另外支持DER
        //curl_easy_setopt(curl,CURLOPT_SSLCERTTYPE,"PEM");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
        curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
    }
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 13);
    res = curl_easy_perform(curl);
    curl_slist_free_all(chunk);
    curl_easy_cleanup(curl);
    return res;
}

CURLcode CHttpClient::Gets( const std::string & strUrl, std::string & strResponse, const char * pCaPath /*= NULL*/ )
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if(m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if(NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 13);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}


CURLcode CHttpClient::Gets(const std::string & strUrl, const std::string & strData, std::string & strResponse,
    const map_req_headers& mapHeaders, const char * pCaPath)
{
    CURLcode res;
    CURL* curl = curl_easy_init();
    if (NULL == curl){
        return CURLE_FAILED_INIT;
    }
    if (m_bDebug){
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
    }

    struct curl_slist *chunk = NULL;
    if (mapHeaders.size() > 0)
    {

        std::map<std::string, std::string>::const_iterator it = mapHeaders.begin();
        std::string header("");
        for (; it != mapHeaders.end(); it++)
        {
            header = it->first + ":" + it->second;
            chunk = curl_slist_append(chunk, header.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    }

    curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);

    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    if (NULL == pCaPath){
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
    }
    else{
        //缺省情况就是PEM，所以无需设置，另外支持DER
        //curl_easy_setopt(curl,CURLOPT_SSLCERTTYPE,"PEM");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
        curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
    }
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 13);
    res = curl_easy_perform(curl);
    curl_slist_free_all(chunk);
    curl_easy_cleanup(curl);
    return res;
}

CURLcode CHttpClient::Post2(const std::string & strUrl, const std::string & strPost, std::string & strResponse,
    const map_req_headers& mapHeaders, const char * pCaPath /*= NULL*/)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	struct curl_slist *chunk = NULL;
	if (mapHeaders.size() > 0)
	{

		std::map<std::string, std::string>::const_iterator it = mapHeaders.begin();
		std::string header("");
		for (; it != mapHeaders.end(); it++)
		{
			header = it->first + ":" + it->second;
			chunk = curl_slist_append(chunk, header.c_str());
		}
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
	}

	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	//curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
	//curl_easy_setopt(curl, CURLOPT_TCP_KEEPIDLE, 120L);
	//curl_easy_setopt(curl, CURLOPT_TCP_KEEPINTVL, 60L);
	curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if (NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		//curl_easy_setopt(curl,CURLOPT_SSLCERTTYPE,"PEM");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 13);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	curl_slist_free_all(chunk);
	return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void CHttpClient::SetDebug(bool bDebug)
{
	m_bDebug = bDebug;
}

