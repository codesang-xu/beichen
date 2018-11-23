/* 
 * File:   asynchttpclient.cpp
 * Author: Administrator
 * 
 * Created on 2015年9月15日, 下午2:06
 */

#include "asynchttpclient.h"
#include <boost/bind.hpp>
CAsyncHttpClient::CAsyncHttpClient() : work(ioservice){
}

CAsyncHttpClient::~CAsyncHttpClient() {
}

void CAsyncHttpClient::init(int thread_num){
    for(int i=0; i<thread_num; i++){
        thread_pool.create_thread(boost::bind( &boost::asio::io_service::run, &ioservice ));
    }
}

//template<typename F>
void CAsyncHttpClient::HttpPost(std::string& strUrl, std::string& strParam,  const boost::function< void ( bool,  std::string &)> &handler ){
    ioservice.post( boost::bind(&CAsyncHttpClient::AsyncPost,this, strUrl, strParam, handler) );
}


void CAsyncHttpClient::AsyncPost(std::string& strUrl, std::string& strParam, const boost::function< void ( bool,  std::string &)> &handler){
    std::string strRsp;
    int reuslt = this->Post(strUrl, strParam,strRsp);
	handler(reuslt==CURLE_OK, strRsp);
}
void CAsyncHttpClient::Http2Post(std::string &strUrl, std::string &strParam, const std::map<std::string, std::string>& mapHeaders, const std::string& caPath,
	const boost::function< void(bool, std::string &)> &handler)
{
	ioservice.post(boost::bind(&CAsyncHttpClient::AsyncHttp2Post, this, strUrl, strParam, mapHeaders, caPath, handler));
}

void CAsyncHttpClient::AsyncHttp2Post(std::string &strUrl, std::string &strParam, const std::map<std::string, std::string>& mapHeaders, const std::string& caPath,
	const boost::function< void(bool, std::string &)> &handler)
{
	std::string strRsp;
	int reuslt = this->Post2(strUrl, strParam, strRsp, mapHeaders, caPath.c_str());
	handler(reuslt == CURLE_OK, strRsp);
}

void CAsyncHttpClient::HttpPut(const std::string &strUrl, const std::string &strParam, const map_req_headers& mapHeaders, const std::string& caPath,
    const callback_handler &handler)
{
    ioservice.post(boost::bind(&CAsyncHttpClient::AsyncHttpPut, this, strUrl, strParam, mapHeaders, caPath, handler));
}

void CAsyncHttpClient::AsyncHttpPut(const std::string &strUrl, const std::string &strParam, const map_req_headers& mapHeaders, const std::string& caPath,
    const callback_handler &handler)
{
    std::string strRsp;
    int reuslt;
    if (caPath.empty()){
        reuslt = this->Put(strUrl, strParam, strRsp, mapHeaders, NULL);
    }
    else{
        reuslt = this->Put(strUrl, strParam, strRsp, mapHeaders, caPath.c_str());
    }
    handler(reuslt == CURLE_OK, strRsp);
}

void CAsyncHttpClient::HttpDelete(const std::string &strUrl, const std::string &strParam, const map_req_headers& mapHeaders, const std::string& caPath,
    const callback_handler &handler)
{
    ioservice.post(boost::bind(&CAsyncHttpClient::AsyncHttpDelete, this, strUrl, strParam, mapHeaders, caPath, handler));
}

void CAsyncHttpClient::AsyncHttpDelete(const std::string &strUrl, const std::string &strParam, const map_req_headers& mapHeaders, const std::string& caPath,
    const callback_handler &handler)
{
    std::string strRsp;
    int reuslt;
    if (caPath.empty()){
        reuslt = this->Delete(strUrl, strParam, strRsp, mapHeaders, NULL);
    }
    else{
        reuslt = this->Delete(strUrl, strParam, strRsp, mapHeaders, caPath.c_str());
    }
    handler(reuslt == CURLE_OK, strRsp);
}

void CAsyncHttpClient::HttpGets(const std::string &strUrl, const std::string &strParam, const map_req_headers& mapHeaders, const std::string& caPath,
    const callback_handler &handler)
{
    ioservice.post(boost::bind(&CAsyncHttpClient::AsyncHttpGets, this, strUrl, strParam, mapHeaders, caPath, handler));
}

void CAsyncHttpClient::AsyncHttpGets(const std::string &strUrl, const std::string &strParam, const map_req_headers& mapHeaders, const std::string& caPath,
    const callback_handler &handler)
{
    std::string strRsp;
    int reuslt;
    if (caPath.empty()){
        reuslt = this->Gets(strUrl, strParam, strRsp, mapHeaders, NULL);
    }
    else{
        reuslt = this->Gets(strUrl, strParam, strRsp, mapHeaders, caPath.c_str());
    }
    handler(reuslt == CURLE_OK, strRsp);
}