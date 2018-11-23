// CUDPSocket.h: interface for the CMySocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYSOCKET_H__D6C8EC53_3FF4_452A_9232_1B97E0CC2CC8__INCLUDED_)
#define AFX_MYSOCKET_H__D6C8EC53_3FF4_452A_9232_1B97E0CC2CC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <math.h>
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <boost/thread/condition_variable.hpp>



using namespace std;
using boost::asio::ip::udp;

#pragma pack(push,1)

typedef struct tagProxySocketSend
{
	char szSendHead[8];
	unsigned long  uDestIP;
	unsigned short  uDestPort;
	// ͷ
	tagProxySocketSend()
	{
		const char* pHead = "proxysnd";
		memcpy(szSendHead, pHead, min(sizeof(szSendHead), strlen(pHead)));
		uDestIP = 0;
		uDestPort = 0;
	}
}ProxySocketSend;

typedef struct tagProxySocketRecv
{
	char szRecvHead[8];
	u_long uSourceIP;
	u_short uSourcePort;
	// ͷ
	tagProxySocketRecv()
	{
		const char* pHead = "proxyrsp";
		memcpy(szRecvHead, pHead, min(sizeof(szRecvHead), strlen(pHead)));
		uSourceIP = 0;
		uSourcePort = 0;
	}
}ProxySocketRecv;
#pragma pack(pop)

class CUDPSocket  
{
public:
    typedef boost::function<void(string)> make_log_func;

public:
    CUDPSocket();
    virtual ~CUDPSocket();
    void Release(){ m_mySocket.close(); }
    bool Init(const string & localPort, int recvbufSize, int sendbufSize, string& errInfo, string localIp = "0.0.0.0");

public:
    string GetPort();
	int ReceiveData(vector<char> & msg, int size, string& errInfo);
	bool SendData(boost::uint32_t ip, unsigned short port, char *msg, int size, string &errInfo
		, unsigned long iProxyIp = 0, unsigned short iProxyPort = 0);
	bool SendData(const string &strIp, const string &strPort, char* msg, int size, string& errInfo
		, unsigned long iProxyIp = 0, unsigned short iProxyPort = 0);
	bool SendData(boost::uint32_t ip, unsigned short port, char *msg, int size, int &err
		, const string & service_name,unsigned long iProxyIp = 0, unsigned short iProxyPort = 0);
	int ReceiveData(vector<char> & msg, int size, string& ip, string& port, string& errInfo);

	///by guojf
	int ReceiveData(vector<char> & msg, int size, udp::endpoint&  ep, string& errInfo);
	bool SendData(udp::endpoint&  ep, char* msg, int size, string& errInfo, unsigned long iProxyIp = 0, unsigned short iProxyPort = 0);

	void ConvertNumberIpAddress(boost::uint32_t &in_ipNumberAddress,std::string &out_validIpAddress);

public:
    void SetMakeLogFunc(make_log_func log_func)
    {
        make_log_func_ = log_func;
    };

private:
    inline void make_log(string logstr){};
    
private:
	void SetParams(const string & localPort, int recvbufSize, int sendbufSize);
	bool PrepareForUse(string& errInfo);
    int myRecvFrom(char * buf, int len, udp::endpoint &remote_endpoint);

private:
	boost::asio::io_service io_service;
	udp::socket m_mySocket;

    int m_iRecvFailedCount;
    static int g_iRefs;

    boost::condition_variable m_cond;
    boost::mutex m_mut;

    string m_strLocalPort;
	int m_iRecvBufSize;
	int m_iSendBufSize;
	string m_sLocalIp;

    make_log_func make_log_func_;
};

#endif // !defined(AFX_MYSOCKET_H__D6C8EC53_3FF4_452A_9232_1B97E0CC2CC8__INCLUDED_)
