/* 
 * File:   asynchttpclient.h
 * Author: Administrator
 *
 * Created on 2015年9月15日, 下午2:06
 */

#ifndef ASYNCHTTPCLIENT_H
#define	ASYNCHTTPCLIENT_H

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>
#include "./httpclient.h"

class CAsyncHttpClient : public CHttpClient {
public:
    typedef boost::function< void(bool, std::string &)> callback_handler;

    CAsyncHttpClient();
    ~CAsyncHttpClient();
    
    void init(int thread_num);
    
	//template<typename F>
    void HttpPost(std::string &strUrl, std::string &strParam, const boost::function< void ( bool, std::string &)> &handler );

    void Http2Post(std::string &strUrl, std::string &strParam, const map_req_headers& mapHeaders, const std::string& caPath,
        const callback_handler &handler);

    void HttpPut(const std::string &strUrl, const std::string& strParam,const map_req_headers& mapHeaders, const std::string& caPath,
        const callback_handler &handler);

    void HttpDelete(const std::string &strUrl, const std::string& strParam,const map_req_headers& mapHeaders, const std::string& caPath,
        const callback_handler &handler);

    void HttpGets(const std::string &strUrl, const std::string& strParam, const map_req_headers& mapHeaders, const std::string& caPath,
        const callback_handler &handler);

private:
    void AsyncPost(std::string& strUrl, std::string& strParam, const callback_handler &handler);

    void AsyncHttp2Post(std::string &strUrl, std::string &strParam, const map_req_headers& mapHeaders, const std::string& caPath,
         const callback_handler &handler);

    void AsyncHttpPut(const std::string &strUrl, const std::string &strParam, const map_req_headers& mapHeaders, const std::string& caPath,
         const callback_handler &handler);

    void AsyncHttpDelete(const std::string &strUrl, const std::string &strParam, const map_req_headers& mapHeaders, const std::string& caPath,
         const callback_handler &handler);

    void AsyncHttpGets(const std::string &strUrl, const std::string &strParam, const map_req_headers& mapHeaders, const std::string& caPath,
        const callback_handler &handler);


private:
	boost::asio::io_service ioservice;
    boost::asio::io_service::work work;
    boost::thread_group thread_pool;
};



#endif	/* ASYNCHTTPCLIENT_H */

