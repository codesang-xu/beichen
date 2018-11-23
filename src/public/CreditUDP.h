// CreditUDP.h: interface for the CCreditUDP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CREDITUDP_H__53CEFDEB_45B1_420B_AFA1_0313E5D80498__INCLUDED_)
#define AFX_CREDITUDP_H__53CEFDEB_45B1_420B_AFA1_0313E5D80498__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <bitset>

#include <vector>
#include <map>
#include <queue>
#if defined(_MSC_VER) || defined(__BORLANDC__)
#elif defined (__GNUC__)
	#include <sys/times.h>
#endif


#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/thread_time.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp> 
#include <boost/function.hpp>

#include <set>
using  
 boost::asio::ip::udp;


#include "UDPSocket.h"

//#include "GlobalOperation.h"

#define  BESEND  0
#define  SENDING 1
#define  AFSEND  2
#define  TIME_REPEATSEND 100
#define  CLOSECOUNT 10
#define  HARTTIME 1500
#define  CREDIT_UDP_VER ("CreditUDP0001")
#define  CREDIT_UDP_VER_LEN 13
#define  CREDIT_MAX_DATA_SIZE 1500
extern unsigned long gettickcount(void);

#pragma pack(4)
typedef struct _CreUDPStruct
{
	char CreUDPHead[CREDIT_UDP_VER_LEN];
	int nSeq;          //�����??creditudp
	int nAck;         //�Ƿ���ȷ�ϰ� 0���� ��1��ȷ�ϰ�
	int nSendCount;   //�Ѿ����ʹ���
	int nReSend;      //�ط����� 0�������ط�
	unsigned int SourceIP;	// For compatibility the old programs, the field remains the type 'int32'
	unsigned short SourcePort;
	unsigned int DestIP;
	unsigned short DestPort;

	int nDataSize;
    char* data;
	_CreUDPStruct(int nSize)
		: data(NULL)
	{
		if (nSize > CREDIT_MAX_DATA_SIZE)
		{
			throw "data size exceed the limit";
		}

		strcpy(CreUDPHead,CREDIT_UDP_VER);
		nSeq=0;
		nAck=0;
		nSendCount=0;
		nReSend=0;
		nDataSize=nSize;
		if(nSize>0)
		{
			data=new char[nSize];
		}
	}
	~_CreUDPStruct()
	{
		if (data!=NULL)
		{
			delete []data;
			data = NULL;
		}
	}
}UDPStruct;

typedef struct _RealData
{
	udp::endpoint remote_endpoint;

	string m_ip;
	string m_port;
	vector<char> m_data;
	int m_nSize;
	
	_RealData(const string ip,const string port,char* data,int nSize)
		:m_data(nSize)
	{
		m_ip=ip;
		m_port=port;
		memcpy(&m_data[0],data,nSize);
		m_nSize=nSize;
	}
	~_RealData()
	{

	}
}RealData;

typedef struct _HartPack
{
	char Head[9];
	_HartPack()
	{
		memset(Head,0,9);
		memcpy(Head,"HartPack",8);
	}
}HartPack;

struct SendBuf
{
	UDPStruct* Buf;
	bool NeedEvent;
	unsigned long ProxyIp;
	unsigned short ProxyPort;
	udp::endpoint remote_endpoint;
	SendBuf(UDPStruct* buf, bool bNeedEvent = false, unsigned long iProxyIp = 0, unsigned short iProxyPort = 0)
	{
		Buf = buf;
		NeedEvent = bNeedEvent;
		ProxyIp = iProxyIp;
		ProxyPort = iProxyPort;
	};
	~SendBuf()
	{
		delete Buf;
	};
};

struct SendStruct
{
	int nSequence;
	unsigned long nHartTime;
	bool bSendHart;
	unsigned long LastTime;
	queue<SendBuf*>  SendQueue;
	SendStruct()
	{
		bSendHart = true;
		nSequence = 0;
		nHartTime = gettickcount();
		LastTime = gettickcount();
	};
	~SendStruct()
	{
		while ( !SendQueue.empty() )
		{
			delete SendQueue.front();
			SendQueue.pop();
		}
	};

	//long gettickcount()
	//{
	//	struct tms tm;
	//	return times(&tm)*10;
	//}
};

struct ReceiveStruct
{
	int nSequence;
	unsigned long LastTime;
	ReceiveStruct()
	{
		nSequence = 0;
		LastTime = gettickcount();
	};

	~ReceiveStruct()
	{
	};

	//long gettickcount()
	//{
	//	struct tms tm;
	//	return times(&tm)*10;
	//}
};
// 
// struct ReceiveKey
// {
// 	string IP;
// 	string Port;
// 	string DestPort;
// 
// 	ReceiveKey()
// 	{
// 
// 	};
// 
// 	ReceiveKey(const string &iP,const string &port,const string &destPort)
// 	{
// 		IP = iP;
// 		Port = port;
// 		DestPort = destPort;
// 	};
// 
// 	bool operator<(const ReceiveKey& src) const
// 	{
// 		if ( IP < src.IP )
// 		{
// 			return true;
// 		}
// 		else if ( IP > src.IP )
// 		{
// 			return false;
// 		}
// 
// 		if ( Port < src.Port )
// 		{
// 			return true;
// 		}
// 		else if ( Port > src.Port )
// 		{
// 			return false;
// 		}
// 
// 		return DestPort < src.DestPort;
// 	};
// };
// 
// struct SendKey
// {
// 	string IP;
// 	string Port;
// 
// 	SendKey()
// 	{
// 
// 	};
// 
// 	SendKey(const string &iP,const string &port)
// 	{
// 		IP = iP;
// 		Port = port;
// 	};
// 
// 	SendKey(const string &iP,const unsigned long &port)
// 	{
// 		IP = iP;
// 		char p[8] = {0};
// 		int i = sprintf(p, "%d", port);
// 		Port = string(p,i);
// 	};
// 
// 	bool operator<(const SendKey& src) const
// 	{
// 		if ( IP < src.IP )
// 		{
// 			return true;
// 		}
// 		else if ( IP > src.IP )
// 		{
// 			return false;
// 		}
// 
// 		return Port < src.Port;
// 	}
// };

typedef udp::endpoint ReceiveKey;
typedef udp::endpoint SendKey;
typedef SendKey IpPortPair;


class CCreditUDP
{
public:
	typedef boost::function<void(string)> make_log_func;
private:
	bool bStopThread;
    typedef map<SendKey,SendStruct*> SendMap;
	SendMap m_SendMap;
	typedef map<ReceiveKey,ReceiveStruct*> ReceiveMap;
	ReceiveMap m_ReceiveMap;
	boost::mutex 	m_mutexSendMapLock;
	boost::mutex 	m_mutexReceiveMapLock;
	queue<boost::shared_ptr<RealData> > 	m_ReceiveQueue;
	//

    boost::mutex 	m_mutexSendVecLock;
    boost::condition_variable 	  m_condSendEvent;
    std::set<IpPortPair> 	ConnectVector;

	boost::mutex 	m_mutexRecvLock;
	boost::mutex	m_mutextRecvEvent;
	boost::condition_variable 	  m_condRecvEvent;	// Event of receive data from network
	boost::condition_variable     m_condACKEvent;	// Event of send    data to   network

	CUDPSocket 			m_UDPSocket;
public:
	CCreditUDP();
	~CCreditUDP();
private:
	void AddToConnectVec(IpPortPair pairIpPort);
	void ClearConnectVec();
	void SendDataFunc(SendKey& key, SendStruct* pSendStruct);
	//////////////////////////////////////////////////////////////////////////
	bool SendUDPData(UDPStruct*buf, udp::endpoint& ep);
	bool SendAckData(UDPStruct* pRecvBuf,  udp::endpoint& ep);
	bool InnerSendUdpData(udp::endpoint& ep , char* pData, int iLen);

	void SendCloseMsg(udp::endpoint& ep);

	int GetCurrentSeq(udp::endpoint& ep);
	//////////////////////////////////////////////////////////////////////////

	bool SendUDPData(UDPStruct*buf);
	bool SendAckData(UDPStruct* pRecvBuf);
	bool SendHartPack(SendKey& key, SendStruct* pSendStruct);
	bool ExistRecvSeq( UDPStruct* pUDPStruct ,udp::endpoint& ep);
	bool IsCreditData(const char* buf, int size);
public:
	CUDPSocket* GetUdpSock();
	const string GetPort();
	int GetRecvBufDataSize();
	void DirectSendData(const string &ip, const string &port, char* data, int len);
	void DirectSendData(unsigned long ip, unsigned short port, char* data, int len
		, unsigned long iProxyIp = 0, unsigned short iProxyPort = 0);
	bool Init(const string & UDPPort, int nSendBufSize, int RecvBufSize, string eventTag, bool SendHart, string& strErrInfo, bool bUseCredit = true, string localIp = "127.0.0.1");
	bool Init(const string & UDPPort, int nSendBufSize, int RecvBufSize, string eventTag, bool SendHart, string& strErrInfo, make_log_func log_func, bool bUseCredit = true, string localIp = "127.0.0.1");
	bool Init(int UDPPort, int nSendBufSize, int RecvBufSize, string eventTag, bool SendHart, string& strErrInfo, bool bUseCredit = true, string localIp = "0.0.0.0");
	bool Init(int UDPPort, int nSendBufSize, int RecvBufSize, string eventTag, bool SendHart, string& strErrInfo, make_log_func log_func, bool bUseCredit = true, string localIp = "0.0.0.0");
	void SetMakeLogFunc(make_log_func log_func);
	int SendData(const string &strIP, const string &strPort, char*buffer, int nSize, string& errorInfo
		,int nReSendCount=10,bool bWait=false,int iTimeOut=500, unsigned long iProxyIp = 0, unsigned short iProxyPort = 0);

	//////////////////////////////////////////////////////////////////////////
	//by guojf
	int SendData(udp::endpoint  ep, char*buffer,int nSize,string& errorInfo
		,int nReSendCount=10,bool bWait=false,int iTimeOut=500, unsigned long iProxyIp = 0, unsigned short iProxyPort = 0);


	int ReceiveData(vector<char> &sData,int nDataSize,  udp::endpoint&  ep ,string& errorInfo,int nDeferTime=5000);
	//////////////////////////////////////////////////////////////////////////

	int ReceiveData(vector<char> &sData, int nDataSize, string& ip, string& port, string& errorInfo, int nDeferTime = 5000);

	int ReceiveDataForBuffer(char* pBuffer,int nBufferSize,string& ip,string& port,string& errorInfo,int nDeferTime=5000);

	bool SetOption(bool bUSECreditUDP);
	bool Release(bool bReleaseSock = true);
	boost::uint32_t SendProcRun();
	boost::uint32_t RecvProcRun();

	int SendData(unsigned long IP, unsigned short Port, char*buffer, int nSize, string& errorInfo
		,int nReSendCount=10,bool bWait=false,int iTimeOut=500, unsigned long iProxyIp = 0, unsigned short iProxyPort = 0);
	int ReceiveData(char*&sData,int nDataSize,unsigned long& ip,unsigned short& port,string& errorInfo,int nDeferTime=5000);
	int ReceiveDataForBuffer(char*sData,int nDataSize,unsigned long& ip,unsigned short& port,string& errorInfo,int nDeferTime =5000 );
private:
	void SendCloseMsg(const string &ip, const string &port);
	bool m_bHaveReleased;
	boost::shared_ptr<boost::thread> m_spThreadSend;
	boost::shared_ptr<boost::thread> m_spThreadRecv;
	bool bCreditUDP;
	bool bSendHart;
	string sLocalIP;
	string strLocalPort;
	unsigned long localIp_Send;
	unsigned short localPort_Send;
	bool InnerSendUdpData(const string &strIp, const string &strPort, char* pData, int iLen);

private:
	inline void make_log(string logstr){};

private:
	make_log_func make_log_func_;
};

struct thrdS
{
	CCreditUDP *lpThis;
	thrdS(CCreditUDP *p):lpThis(p){}
	void operator()()
	{
		boost::uint32_t dw = lpThis->SendProcRun();
	}
};

struct thrdR
{
	CCreditUDP *lpThis;
	thrdR(CCreditUDP *p):lpThis(p){}
	void operator()()
	{
        boost::uint32_t dw = lpThis->RecvProcRun();
	}
};
#pragma pack()

#endif // !defined(AFX_CREDITUDP_H__53CEFDEB_45B1_420B_AFA1_0313E5D80498__INCLUDED_)
