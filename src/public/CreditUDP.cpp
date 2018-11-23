// CreditUDP.cpp: implementation of the CCreditUDP class.
//
//////////////////////////////////////////////////////////////////////
#include "CreditUDP.h"

#include <string>

#include <boost/asio.hpp>
#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>

#include "GlobalOperation.h"
#include "SecretKey.h"

#ifdef OPTION_CRYPT
#include "Rijndael.h"
#endif

using namespace boost;
using namespace boost::asio::ip;
using namespace boost::asio::detail::socket_ops;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
unsigned int IPPORT_MAX_TIMEOUT = 15000;
CCreditUDP::CCreditUDP() {
	m_bHaveReleased = true;
}

CCreditUDP::~CCreditUDP() {
	Release();
}

bool CCreditUDP::Init(const string &UDPPort, int nSendBufSize, int RecvBufSize,
	string eventTag, bool SendHart, string& strErrInfo, make_log_func log_func, bool bUseCredit, string localIp)
{
	bStopThread = !bUseCredit;
	m_bHaveReleased = false;
	bSendHart = SendHart;
	bCreditUDP = bUseCredit;
	sLocalIP = "";

	make_log_func_ = log_func;

	bool iFlag = m_UDPSocket.Init(UDPPort, RecvBufSize, nSendBufSize,
		strErrInfo, localIp);
    m_UDPSocket.SetMakeLogFunc(log_func);

	sLocalIP = localIp;
	strLocalPort = UDPPort;

	localIp_Send = ConvertIP(GetLocalIP());
	localPort_Send = ConvertPort(atoi(UDPPort.c_str()));

	//int res;
	if (bCreditUDP) {
		thrdS send(this);
		thrdR recv(this);
		this->m_spThreadSend = boost::shared_ptr<boost::thread>(
			new boost::thread(send));
		this->m_spThreadRecv = boost::shared_ptr<boost::thread>(
			new boost::thread(recv));
	}

	return iFlag;
}

bool CCreditUDP::Init(const string &UDPPort, int nSendBufSize, int RecvBufSize,
	string eventTag, bool SendHart, string& strErrInfo, bool bUseCredit, string localIp)
{
	return Init(UDPPort, nSendBufSize, RecvBufSize, eventTag, SendHart, strErrInfo, boost::bind(&CCreditUDP::make_log, this, _1),
		bUseCredit, localIp);
}

bool CCreditUDP::Init(int UDPPort, int nSendBufSize, int RecvBufSize, string eventTag, bool SendHart, string& strErrInfo, bool bUseCredit, string localIp)
{
	string sPort = ConvertPortToString(UDPPort);
	return Init(sPort, nSendBufSize, RecvBufSize, eventTag, SendHart, strErrInfo, bUseCredit, localIp);
}

bool CCreditUDP::Init(int UDPPort, int nSendBufSize, int RecvBufSize, string eventTag, bool SendHart, string& strErrInfo, make_log_func log_func, bool bUseCredit, string localIp)
{
	string sPort = ConvertPortToString(UDPPort);
	return Init(sPort, nSendBufSize, RecvBufSize, eventTag, SendHart, strErrInfo, log_func, bUseCredit, localIp);
}

bool CCreditUDP::SendHartPack(SendKey& key, SendStruct* pSendStruct) {
	if (!pSendStruct->bSendHart)
		return false;
	unsigned long i = gettickcount();
	if ((i - HARTTIME) < pSendStruct->nHartTime)
		return false;
	pSendStruct->nHartTime = i;
	string sErr = "";
	bool bFlag = m_UDPSocket.SendData(key , "HartPack", 8, sErr);
	return bFlag;
}

//long CCreditUDP::GetTickCount() {
//	struct tms tm;
//	return times(&tm) * 10;
//}

bool CCreditUDP::SendUDPData(UDPStruct*buf) {
	if (buf == NULL) {
		return false;
	} else {
		if (buf->nDataSize > CREDIT_MAX_DATA_SIZE) //ï¿½Ë´ï¿½ï¿½ï¿½ï¿½â·¢ï¿½ï¿½ï¿½ï¿½Æµï¿½ï¿½ï¿??
			return true;

		string errorInfo;
		int nbufSize = buf->nDataSize + sizeof(UDPStruct) - sizeof(char*);
		char* buffer = new char[nbufSize];
		memcpy(buffer, buf, sizeof(UDPStruct) - sizeof(char*));
		memcpy(buffer + sizeof(UDPStruct) - sizeof(char*), buf->data, buf->nDataSize);
		//		bool bFlag=m_UDPSocket.SendData(buf->strDestIP,buf->nDestPort,buffer,nbufSize,errorInfo);
		//		bool bFlag=m_UDPSocket.SendData(buf->DestIP,buf->DestPort,buffer,nbufSize,errorInfo);
		using namespace boost::asio::ip;
		bool bFlag = InnerSendUdpData(address_v4(ntohl(buf->DestIP)).to_string(), boost::lexical_cast<string>(ntohs(buf->DestPort)), buffer,
				nbufSize);
		delete[] buffer;
		return bFlag;
	}
}


bool CCreditUDP::SendUDPData(UDPStruct*buf, udp::endpoint& ep)
{
	if (buf == NULL) {
		return false;
	} else {
		if (buf->nDataSize > CREDIT_MAX_DATA_SIZE) //ï¿½Ë´ï¿½ï¿½ï¿½ï¿½â·¢ï¿½ï¿½ï¿½ï¿½Æµï¿½ï¿½ï¿??
			return true;

		string errorInfo;
		int nbufSize = buf->nDataSize + sizeof(UDPStruct) - sizeof(char*);
		char* buffer = new char[nbufSize];
		memcpy(buffer, buf, sizeof(UDPStruct) - sizeof(char*));
		memcpy(buffer + sizeof(UDPStruct) - sizeof(char*), buf->data, buf->nDataSize);

		using namespace boost::asio::ip;
		bool bFlag = InnerSendUdpData(ep, buffer,	nbufSize);
		delete[] buffer;
		return bFlag;
	}
}

bool CCreditUDP::SendAckData(UDPStruct* pRecvBuf,  udp::endpoint& ep)
{
	string errInfo;
	boost::shared_ptr<UDPStruct> pSendBuf(new UDPStruct(0));
	pSendBuf->DestIP = pRecvBuf->SourceIP;
	pSendBuf->DestPort = pRecvBuf->SourcePort;
	pSendBuf->SourceIP = pRecvBuf->DestIP;
	pSendBuf->SourcePort = pRecvBuf->DestPort;
	pSendBuf->nSeq = pRecvBuf->nSeq;
	pSendBuf->nAck = 1;
	InnerSendUdpData(ep,(char*) pSendBuf.get(), sizeof(UDPStruct) - sizeof(char*));
	return true;
}

bool CCreditUDP::InnerSendUdpData(udp::endpoint& ep , char* pData, int iLen)
{
	string errinfo;
	bool bRet;
#ifdef OPTION_CRYPT
	if ( g_bCryptCudp )
	{
		vector<unsigned char> pDataEnc(iLen + 2 + 16);
		short iLenEnc = CRijndael::Encrypt((unsigned char*)pData, iLen, &pDataEnc[0], g_CudpCryptKey);
		memcpy(&pDataEnc[0] + iLenEnc, &iLen, 2);
		bRet = m_UDPSocket.SendData(ep, (char*)&pDataEnc[0], iLenEnc + 2, errinfo);
	}
	else
	{
		bRet = m_UDPSocket.SendData(ep,pData,iLen,errinfo);
	}
#else
	bRet = m_UDPSocket.SendData(ep, pData, iLen, errinfo);
#endif

	return bRet;
}



int CCreditUDP::SendData(udp::endpoint  ep, char*data,int nSize,string& errorInfo
			 ,int nReSendCount/*=10*/,bool bWait/*=false*/,int iTimeOut/*=500*/, unsigned long iProxyIp/* = 0*/, unsigned short iProxyPort/* = 0*/)
{
	if (bCreditUDP) {

		UDPStruct * pSendBuf = new UDPStruct(nSize);

		using namespace boost::asio::ip;

		pSendBuf->DestIP = ep.address().to_v4().to_ulong();
 		pSendBuf->DestPort = ep.port();

		pSendBuf->nReSend = nReSendCount;
		if (nSize > CREDIT_MAX_DATA_SIZE)
			throw "data size exceed the limit";
		memcpy(pSendBuf->data, data, nSize);

		pSendBuf->SourceIP = localIp_Send;
		pSendBuf->SourcePort = localPort_Send;


		SendKey key(ep);
		bool immediately_send = true;
		{
			boost::unique_lock<boost::mutex> sendMaplock(m_mutexSendMapLock);


			SendStruct* pSendStruct = NULL;
			if (m_SendMap.find(key) != m_SendMap.end()) {
				pSendStruct = m_SendMap[key];
				pSendBuf->nSeq = ++(pSendStruct->nSequence);
			} else {
				pSendStruct = new SendStruct;
				pSendStruct->bSendHart = bSendHart;
				pSendBuf->nSeq = 0;
				m_SendMap[key] = pSendStruct;
			}

			SendBuf* pS = new SendBuf(pSendBuf, bWait, iProxyIp,	iProxyPort);
			pS->remote_endpoint = ep;
			pSendStruct->SendQueue.push(pS);
			int iSeq = pSendBuf->nSeq;

			immediately_send = (pSendStruct->SendQueue.size() == 1);
		}
		
		if (immediately_send)
		{
			AddToConnectVec( key);
		}

		if (bWait) {
			boost::unique_lock<boost::mutex> lock;
			m_condACKEvent.timed_wait(lock, boost::posix_time::milliseconds(
				iTimeOut));
		}

        return nSize;
	} else {
		string errorInfo;
		if (m_UDPSocket.SendData(ep, data, nSize, errorInfo,
			iProxyIp, iProxyPort))
			return nSize;
		else
			return -1;
	}
}


int CCreditUDP::SendData(const string &strIP, const string &strPort, char*data,
		int nSize, string& errorInfo, int nReSendCount, bool bWait,
		int iTimeOut, unsigned long iProxyIp, unsigned short iProxyPort)
{
	udp::endpoint remote_port(boost::asio::ip::address_v4::from_string( strIP ),  boost::lexical_cast<  unsigned short>(strPort));
	return SendData(remote_port, data, nSize, errorInfo, nReSendCount, bWait, iTimeOut, iProxyIp, iProxyPort);
}

bool CCreditUDP::SetOption(bool bUSECreditUDP) {
	if (bUSECreditUDP == bCreditUDP) {
		return true;
	}

    if (!bUSECreditUDP) {
		Release(false);
	} else {
		if (m_spThreadRecv == 0) {
			thrdR recv(this);
			boost::thread thrdRec(recv); // + [6/4/2008 lixg]
		}

		if (m_spThreadSend == 0) {
			thrdS send(this);
			boost::thread thrdSend(send); // + [6/4/2008 lixg]
		}

	}

	bCreditUDP = bUSECreditUDP;
	return true;
}

void CCreditUDP::ClearConnectVec() {
    boost::unique_lock<boost::mutex> lock(m_mutexSendVecLock);
	ConnectVector.clear();
}

bool CCreditUDP::SendAckData(UDPStruct* pRecvBuf) {
	string errInfo;
	boost::shared_ptr<UDPStruct> pSendBuf(new UDPStruct(0));
	pSendBuf->DestIP = pRecvBuf->SourceIP;
	pSendBuf->DestPort = pRecvBuf->SourcePort;
	pSendBuf->SourceIP = pRecvBuf->DestIP;
	pSendBuf->SourcePort = pRecvBuf->DestPort;
	pSendBuf->nSeq = pRecvBuf->nSeq;
	pSendBuf->nAck = 1;
	InnerSendUdpData(address_v4( ntohl(pSendBuf->DestIP)).to_string(), boost::lexical_cast<string>(ntohs(pSendBuf->DestPort)),
			(char*) pSendBuf.get(), sizeof(UDPStruct) - sizeof(char*));

	return true;
}

int CCreditUDP::ReceiveData(vector<char> &sData, int nDataSize, udp::endpoint&  ep, string& errorInfo, int nDeferTime/*=5000*/)
{
	boost::shared_ptr<RealData> pRealData;
	int nSize, i;

	if (!bCreditUDP)
		return m_UDPSocket.ReceiveData(sData, sData.size(), ep, errorInfo);

	while (!bStopThread)
	{
		{
			boost::unique_lock<boost::mutex> recvlock(m_mutexRecvLock);
			if (!m_ReceiveQueue.empty()) {
				pRealData = m_ReceiveQueue.front();
				if (pRealData != NULL) {
					//ip = pRealData->m_ip;
					//port = pRealData->m_port;
					nSize = pRealData->m_nSize;
					if (sData.size() < nSize)
					{//¾¯¸æ£ºbuf±»ÖØÖÃ
						sData.resize(nSize);
					}
					ep = pRealData->remote_endpoint;
					memcpy(&sData[0], &pRealData->m_data[0], nSize);

					m_ReceiveQueue.pop();
					return nSize;
				}
				m_ReceiveQueue.pop();
			}
		}
		boost::unique_lock<boost::mutex> lock(m_mutextRecvEvent);
		bool dEventFlag = m_condRecvEvent.timed_wait(lock,
			boost::posix_time::milliseconds(50));
		if (dEventFlag == false) {
			if (bStopThread) // Îªï¿½Ë½ï¿½ï¿½ï¿½Ð»ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ý¶Ñ»ï¿½
				return -1;

			nDeferTime -= 50;
			if (nDeferTime > 0)
				continue;

			nDataSize = -1;
			char ch[30];
			i = sprintf(ch, "ï¿½ï¿½Ê±ï¿½ï¿½ï¿½Ø£ï¿½");
			ch[i] = 0;
			errorInfo = ch;
			return -1;
		}
	}
	return 0;
}


int CCreditUDP::ReceiveData(vector<char> &sData, int nDataSize, string & ip,
		string& port, string& errorInfo, int nDeferTime) 
{
	udp::endpoint ep;
	int nRet = ReceiveData( sData,nDataSize, ep, errorInfo, nDeferTime);
	ip = ep.address().to_v4().to_string();
	port = boost::lexical_cast<string>(ep.port());
	return nRet;
}

int CCreditUDP::ReceiveDataForBuffer(char* pBuffer,int nBufferSize,string& ip,string& port,string& errorInfo,int nDeferTime/*=5000*/)
{
	boost::shared_ptr<RealData> pRealData;
	int nSize, i;

	if (!bCreditUDP)
	{
		vector<char> pData(nBufferSize);
		int nSize = m_UDPSocket.ReceiveData(pData, nBufferSize, ip, port, errorInfo);
		if(nSize > nBufferSize)
			return 0;
		memcpy(pBuffer, &pData[0], nSize);
		return nSize;
	}

	while (!bStopThread)
	{
		{
			boost::unique_lock<boost::mutex> recvlock(m_mutexRecvLock);
			if (!m_ReceiveQueue.empty()) {
				pRealData = m_ReceiveQueue.front();
				if (pRealData != NULL) {
					ip = pRealData->m_ip;
					port = pRealData->m_port;
					nSize = pRealData->m_nSize;
					//sData = boost::shared_ptr<char>(new char[nSize]);
					//memcpy(sData.get(), pRealData->m_data.get(), nSize);
					if(nSize > nBufferSize)
						return 0;

					memcpy(pBuffer, &pRealData->m_data[0], nSize);
					m_ReceiveQueue.pop();
					return nSize;
				}
				m_ReceiveQueue.pop();
			}
		}
		boost::unique_lock<boost::mutex> lock(m_mutextRecvEvent);
		bool dEventFlag = m_condRecvEvent.timed_wait(lock,
			boost::posix_time::milliseconds(50));
		if (dEventFlag == false) {
			if (bStopThread) // Îªï¿½Ë½ï¿½ï¿½ï¿½Ð»ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ý¶Ñ»ï¿½
				return -1;

			nDeferTime -= 50;
			if (nDeferTime > 0)
				continue;

			//nDataSize = -1;
			char ch[30];
			i = sprintf(ch, "ï¿½ï¿½Ê±ï¿½ï¿½ï¿½Ø£ï¿½");
			ch[i] = 0;
			errorInfo = ch;
			return -1;
		}
	}
	return 0;
}

bool CCreditUDP::Release(bool bReleaseSock) {
	if (m_bHaveReleased)
		return true;
	m_bHaveReleased = true;

	bStopThread = true;

	// if ( hSendThread || hRecvThread )
	if ((m_spThreadSend&&m_spThreadSend->joinable()) || (m_spThreadRecv&&m_spThreadRecv->joinable())) {
		string strErr = "";
		m_UDPSocket.SendData(sLocalIP == "0.0.0.0" ? "127.0.0.1" : sLocalIP, strLocalPort, "", 0, strErr);
        {
            lock_guard<mutex> lock(m_mutexSendVecLock);
            m_condSendEvent.notify_all();
        }
        this->bStopThread = true;
		m_spThreadSend->timed_join(boost::posix_time::milliseconds(1000));
		m_spThreadRecv->timed_join(boost::posix_time::milliseconds(1000));
	}

	if (bCreditUDP) {
		{
			boost::unique_lock<boost::mutex> recvlock(m_mutexRecvLock);
			{
				boost::unique_lock<boost::mutex>
					sendMaplock(m_mutexSendMapLock);
				for (SendMap::iterator itSend = m_SendMap.begin(); itSend
					!= m_SendMap.end(); itSend++) 
				{
					udp::endpoint ep = itSend->first;
					SendCloseMsg( ep );
					delete itSend->second;
				}
				m_SendMap.clear();
			}

			{
				boost::unique_lock<boost::mutex> recvMaplock(
					m_mutexReceiveMapLock);
				for (ReceiveMap::iterator itRecv = m_ReceiveMap.begin(); itRecv
					!= m_ReceiveMap.end(); itRecv++) 
				{
					udp::endpoint ep = itRecv->first;
					SendCloseMsg(ep);
					delete itRecv->second;
				}
				m_ReceiveMap.clear();
			}

			while (!m_ReceiveQueue.empty()) {
				m_ReceiveQueue.pop();
			}
		}
		ClearConnectVec();
	}

	if (bReleaseSock) {
		m_UDPSocket.Release();
	} else {
		m_bHaveReleased = false;
	}

	return true;
}

bool CCreditUDP::IsCreditData(const char *buf, int size) {
	//ï¿½Ð¶ï¿½ï¿½Ç·ï¿½ CreditUDP ï¿½ï¿½Ï¢
	if (size < CREDIT_UDP_VER_LEN)
		return false;
	return memcmp(buf, CREDIT_UDP_VER, CREDIT_UDP_VER_LEN) == 0;
}

void CCreditUDP::DirectSendData(const string &ip,const string &port,
		char *data, int len) {
	string errorInfo;
	if (bCreditUDP) {
		InnerSendUdpData(ip, port, data, len);
	} else {
		m_UDPSocket.SendData(ip, port, data, len, errorInfo);
	}
}

int CCreditUDP::GetRecvBufDataSize() { // It seems this method has not been called
	//return m_UDPSocket.GetRecvBufDataSize();
	return 0;
}

const string CCreditUDP::GetPort() {
	return m_UDPSocket.GetPort();
}

boost::uint32_t CCreditUDP::RecvProcRun() {
	string sErrInfo = "";
	vector<char> buf(2048);
	boost::shared_ptr<UDPStruct> pRecvBuf;
	int iRecvSize = 2048;
	int iReceivedSize = 0;
	string strSourceIp;
	string strSourcePort;

	udp::endpoint remote_endpoint;

	unsigned long iClearTime = gettickcount();
	while (!m_bHaveReleased && !bStopThread) {
		try {
				buf.resize(iRecvSize);
				iReceivedSize = m_UDPSocket.ReceiveData(buf, iRecvSize,	remote_endpoint, sErrInfo);

				if (bStopThread) {
					if (iReceivedSize >= 0)
						return 0;
				}

				if (iReceivedSize <= 0) {
					if (gettickcount() - iClearTime >= 50) {
						iClearTime = gettickcount();
						boost::unique_lock<boost::mutex> recvMaplock(
								m_mutexReceiveMapLock);
						vector<ReceiveKey> vecTemp;
						for (ReceiveMap::iterator iter = m_ReceiveMap.begin(); iter
								!= m_ReceiveMap.end(); iter++) {
				
							if (gettickcount() - iter->second->LastTime
									>= IPPORT_MAX_TIMEOUT) {
								vecTemp.push_back(iter->first);
							}
						}
						for (vector<ReceiveKey>::iterator iterVec =
								vecTemp.begin(); iterVec != vecTemp.end(); iterVec++) {
							//SendCloseMsg((*iterVec).first);
							ostringstream oss;
							oss << "WARNING, remove ReceiveMap"
								<< ", key ip=" << iterVec->address().to_string()
								<< ", port=" << iterVec->port()
								<< ", seq=" << m_ReceiveMap[*iterVec]->nSequence
								<< ", lasttime=" << m_ReceiveMap[*iterVec]->LastTime;
							make_log_func_(oss.str());

							delete m_ReceiveMap[*iterVec];
							m_ReceiveMap.erase(*iterVec);
						}

					}

					continue;
				}

#ifdef OPTION_CRYPT
				if (g_bCryptCudp) {
					short iLen = (short)(*((short*)(&buf[0] + iReceivedSize
							- 2)));
					if ((iLen > iReceivedSize - 2) || (iLen < iReceivedSize - 2
							- 16)) {
						continue;
					}
					vector<char> bufDec(2048);
					CRijndael::Decrypt((unsigned char*)&buf[0],
							iReceivedSize - 2, (unsigned char*) &bufDec[0],
							g_CudpCryptKey);

					buf = bufDec;
					iReceivedSize = iLen;
				}
#endif

				if (!IsCreditData(&buf[0], iReceivedSize)) {

					continue;
				}

				pRecvBuf = boost::shared_ptr<UDPStruct>(new UDPStruct(0));
				memcpy(pRecvBuf.get(), &buf[0], sizeof(UDPStruct) - sizeof(char*)); // - sizeof(char*) for the 32 bit os messsage

				if (pRecvBuf->nDataSize > 1500 || pRecvBuf->nDataSize < 0) {

					continue;
				}
				if (pRecvBuf->nDataSize > 0) {
					pRecvBuf->data = new char[pRecvBuf->nDataSize];
					memcpy(pRecvBuf->data, &buf[0] + sizeof(UDPStruct) - sizeof(char*), pRecvBuf->nDataSize);
				}

				pRecvBuf->SourceIP = remote_endpoint.address().to_v4().to_ulong();
				pRecvBuf->SourcePort = remote_endpoint.port();

				if (pRecvBuf->nAck == 0) // receive data-pack
				{
					ostringstream oss;
					oss << "trace, rec"
						<< ", class=" << (*(int*)pRecvBuf->data)
						<< ", type=" << (*(int*)(pRecvBuf->data + 4))
						<< ", ip=" << boost::asio::ip::address_v4(pRecvBuf->SourceIP).to_string()
						<< ", port=" << pRecvBuf->SourcePort
						<< ", seq=" << pRecvBuf->nSeq;
					make_log_func_(oss.str());

					SendAckData(pRecvBuf.get(), remote_endpoint);
					{
						boost::unique_lock<boost::mutex> recvMaplock(
								m_mutexReceiveMapLock);
						if (ExistRecvSeq(pRecvBuf.get(),remote_endpoint))
						{
							ostringstream oss;
							oss << "WARNING, discard"
								<< ", seq=" << pRecvBuf->nSeq;
							make_log_func_(oss.str());
							continue;
						}
					}

					boost::shared_ptr<RealData> pRealData = 
                        boost::shared_ptr<RealData>(new RealData(
                            strSourceIp, strSourcePort,
                            pRecvBuf->data, pRecvBuf->nDataSize));

					pRealData->remote_endpoint = remote_endpoint;

					{
						boost::unique_lock<boost::mutex> recvlock(
								m_mutexRecvLock);
						m_ReceiveQueue.push(pRealData);
					}

					m_condRecvEvent.notify_one();
				} else if (pRecvBuf->nAck == 1) // receive confirm-pack
				{
					ostringstream oss;
					oss << "trace, rec ack"
						<< ", ip=" << boost::asio::ip::address_v4(pRecvBuf->SourceIP).to_string()
						<< ", port=" << pRecvBuf->SourcePort
						<< ", seq=" << pRecvBuf->nSeq;
					make_log_func_(oss.str());

					bool bNeedSetEvent = false;
					SendKey key(remote_endpoint);
					{
						boost::unique_lock<boost::mutex> sendMaplock(
								m_mutexSendMapLock);
						if (m_SendMap.find(key) != m_SendMap.end()) {
							SendStruct* pSendStruct = m_SendMap[key];
							if (!pSendStruct->SendQueue.empty()) {
								bNeedSetEvent = true;
								SendBuf* pTemp = pSendStruct->SendQueue.front();

								if (pTemp->Buf->nSeq == pRecvBuf->nSeq) {
									if (pTemp->NeedEvent) {
										m_condACKEvent.notify_all();
									}
									delete pTemp;
									pSendStruct->SendQueue.pop();
								}
								else
								{
									make_log_func_("1!@#$%111111");
								}
							}
						}
					}

					AddToConnectVec(key);
                }
                else if (pRecvBuf->nAck == 2) // receive close-connect message
				{
					SendKey sendKey(remote_endpoint) ; //= GetSendKey(pRecvBuf.get(), true);
					ReceiveKey receiveKey = remote_endpoint; //= GetReceiveKey(pRecvBuf.get());
					{
						boost::unique_lock<boost::mutex> sendMaplock(
								m_mutexSendMapLock);
						if (m_SendMap.find(sendKey) != m_SendMap.end()) {
							delete m_SendMap[sendKey];
							m_SendMap.erase(sendKey);
						}
					}

					{
						boost::unique_lock<boost::mutex> recvMaplock(
								m_mutexReceiveMapLock);
						if (m_ReceiveMap.find(receiveKey) != m_ReceiveMap.end()) {
							ostringstream oss;
							oss << "WARNING, remove ReceiveMap"
								<< ", key ip=" << receiveKey.address().to_string()
								<< ", port=" << receiveKey.port()
								<< ", seq=" << m_ReceiveMap[receiveKey]->nSequence
								<< ", lasttime=" << m_ReceiveMap[receiveKey]->LastTime;
							make_log_func_(oss.str());

							delete m_ReceiveMap[receiveKey];
							m_ReceiveMap.erase(receiveKey);
						}
					}
					ostringstream oss;
					oss << "WARNING, close creditudp"
						<< ", ip=" << remote_endpoint.address().to_string()
						<< ", port=" << remote_endpoint.port();
					make_log_func_(oss.str());
				}
		} catch (std::exception &e) {
			string str,str1 ;
			str = e.what();
			str1 = "cc";
		} catch (...) {
			int a;
			a =9;
		}
	} // while (!m_bHaveReleased && !bStopThread)
	return 0;
}

bool CCreditUDP::ExistRecvSeq(UDPStruct* pUDPStruct,udp::endpoint& ep) {
	int iSeq = pUDPStruct->nSeq;
	int iCount = pUDPStruct->nSendCount;
	ReceiveKey key = ep;// = GetReceiveKey(pUDPStruct);
	if (m_ReceiveMap.find(key) != m_ReceiveMap.end()) {
		ReceiveStruct* pReceiveStruct = m_ReceiveMap[key];

		bool bRet = false;
		if (pReceiveStruct->nSequence == iSeq) {
			if ((iSeq == 0) && ((gettickcount() - pReceiveStruct->LastTime)
					> 2000)) {

				bRet = false;
			} else {
				ostringstream oss;
				oss << "trace, ExistRecvSeq true 1"
					<< ", ip=" << ep.address().to_string()
					<< ", port=" << ep.port()
					<< ", package seq=" << iSeq
					<< ", SendCount=" << iCount
					<< ", local info seq=" << pReceiveStruct->nSequence
					<< ", lasttime=" << pReceiveStruct->LastTime;
				make_log_func_(oss.str());

				bRet = true;
			}
		} else if ((pReceiveStruct->nSequence < iSeq) || (iSeq == 0)) {
			pReceiveStruct->nSequence = iSeq;
			bRet = false;
		} else {
			ostringstream oss;
			oss << "trace, ExistRecvSeq true 1"
				<< ", ip=" << ep.address().to_string()
				<< ", port=" << ep.port()
				<< ", package seq=" << iSeq
				<< ", SendCount=" << iCount
				<< ", local info seq=" << pReceiveStruct->nSequence
				<< ", lasttime=" << pReceiveStruct->LastTime;
			make_log_func_(oss.str());

			bRet = true;
		}
		pReceiveStruct->LastTime = gettickcount();
		return bRet;
	} else {
		ReceiveStruct* pReceiveStruct = new ReceiveStruct;
		pReceiveStruct->nSequence = iSeq;
		m_ReceiveMap[key] = pReceiveStruct;
		return false;
	}
	return false;
}

// ReceiveKey CCreditUDP::GetReceiveKey(UDPStruct *pUDPStruct) {
// 	//	unsigned long iSourceIP = ::ConvertIP( pUDPStruct->strSourceIP );
// 	//	unsigned short iSourcePort = ::ConvertPort( pUDPStruct->nSourcePort );
// 	//	unsigned short iDestPort = ::ConvertPort( pUDPStruct->nDestPort );
// 	return ReceiveKey( address_v4( ntohl(pUDPStruct->SourceIP) ).to_string(), boost::lexical_cast<string>( ntohs(pUDPStruct->SourcePort)),
// 			boost::lexical_cast<string>(ntohs(pUDPStruct->DestPort)) );
// }
// 
// SendKey CCreditUDP::GetSendKey(UDPStruct* pUDPStruct, bool bReceived) {
// 	//	string sIP = bReceived ? pUDPStruct->strSourceIP : pUDPStruct->strDestIP;
// 	//	unsigned short iPort = bReceived ? pUDPStruct->nSourcePort : pUDPStruct->nDestPort;
// 	//	unsigned long iIP = ::ConvertIP( sIP );
// 	//	iPort = ::ConvertPort( iPort );
// 	string iIP = bReceived ?  address_v4( ntohl(pUDPStruct->SourceIP) ).to_string()
// 			: address_v4( ntohl(pUDPStruct->DestIP) ).to_string();
// 	string iPort = bReceived ? boost::lexical_cast<string>( ntohs(pUDPStruct->SourcePort))
// 			: boost::lexical_cast<string>( ntohs(pUDPStruct->DestPort));
// 	return SendKey(iIP, iPort);
// }

void CCreditUDP::AddToConnectVec(IpPortPair pairIpPort) 
{
    lock_guard<mutex> lock(m_mutexSendVecLock);
	ConnectVector.insert(pairIpPort);
    m_condSendEvent.notify_all();
}

boost::uint32_t CCreditUDP::SendProcRun() 
{
	using namespace boost::posix_time;
	unsigned long nLastTime = gettickcount(), nCurrentTime = 0;

    set<IpPortPair> vec;
	while (!m_bHaveReleased && !bStopThread) {

        boost::chrono::system_clock::time_point abs_time = 
            boost::chrono::system_clock::now() + boost::chrono::milliseconds(TIME_REPEATSEND);

        {
            boost::unique_lock<boost::mutex> lock(m_mutexSendVecLock);
            m_condSendEvent.wait_until(lock, abs_time);

            if (m_bHaveReleased || bStopThread) {
                return 0;
            }

            if (!ConnectVector.empty()) {
                vec = ConnectVector;
                ConnectVector.clear();
            }
        }

        //1¡¢·¢ËÍ´ý·¢Êý¾Ý
        if (!vec.empty()) {
            boost::unique_lock<boost::mutex> sendMaplock(m_mutexSendMapLock);
            for (set<IpPortPair>::iterator iter = vec.begin(); iter
                != vec.end(); iter++) {
                if (m_SendMap.find(*iter) != m_SendMap.end()) {
                    SendDataFunc((SendKey&)(*iter), m_SendMap[*iter]);
                }
            }
            vec.clear();
        }

        //2¡¢Çå³ý³¬Ê±Êý¾Ý
        nCurrentTime = gettickcount();
        if (nCurrentTime - nLastTime > 100) {
            nLastTime = nCurrentTime;

    		boost::unique_lock<boost::mutex> sendMaplock(m_mutexSendMapLock);
			vector<SendKey> vecTemp;
			for (SendMap::iterator iter = m_SendMap.begin(); iter != m_SendMap.end(); iter++) {
				SendDataFunc((SendKey&)iter->first, iter->second);
				if ((gettickcount() - iter->second->LastTime) >= IPPORT_MAX_TIMEOUT) {
					vecTemp.push_back(iter->first);
				}
			}
			for (vector<SendKey>::iterator iterVec = vecTemp.begin(); iterVec != vecTemp.end(); iterVec++) {
				SendStruct* pSS = m_SendMap[*iterVec];
				while (!pSS->SendQueue.empty())
				{
					SendBuf* sendbuf = pSS->SendQueue.front();
					ostringstream oss;
					oss << "WARNING, discard msg"
						<< ", class=" << (*(int*)sendbuf->Buf->data)
						<< ", type=" << (*(int*)(sendbuf->Buf->data + 4))
						<< ", ip=" << sendbuf->remote_endpoint.address().to_string()
						<< ", port=" << sendbuf->remote_endpoint.port();
					make_log_func_(oss.str());
					delete pSS->SendQueue.front();
					pSS->SendQueue.pop();
				}

				delete m_SendMap[*iterVec];
				m_SendMap.erase(*iterVec);
			}
        }
	}
	return 0;
}

void CCreditUDP::SendDataFunc(SendKey& key, SendStruct* pSendStruct) {
	if (pSendStruct->SendQueue.empty()) {
		return;
	}
	SendBuf* pSendBuf = pSendStruct->SendQueue.front();
	UDPStruct* pBuffer = pSendBuf->Buf;
	SendHartPack(key, pSendStruct);

	ostringstream oss;
	oss << "TRACE, send"
		<< ", class=" << (*(int*)pSendStruct->SendQueue.front()->Buf->data)
		<< ", type=" << (*(int*)(pSendStruct->SendQueue.front()->Buf->data + 4))
		<< ", ip=" << pSendBuf->remote_endpoint.address().to_string()
		<< ", port=" << pSendBuf->remote_endpoint.port()
		<< ", seq=" << pBuffer->nSeq
		<< ", count=" << pBuffer->nSendCount;
	make_log_func_(oss.str());

	SendUDPData(pBuffer, pSendBuf->remote_endpoint);
	pBuffer->nSendCount++;
	if ((pBuffer->nSendCount >= pBuffer->nReSend) && (pBuffer->nReSend > 0)) {
		SendBuf* sendbuf = pSendStruct->SendQueue.front();
		ostringstream oss;
		oss << "WARNING, maybe send failed"
			<< ", class=" << (*(int*)sendbuf->Buf->data)
			<< ", type=" << (*(int*)(sendbuf->Buf->data + 4))
			<< ", ip=" << sendbuf->remote_endpoint.address().to_string()
			<< ", port=" << sendbuf->remote_endpoint.port()
			<< ", SendCount=" << pBuffer->nSendCount;
		make_log_func_(oss.str());
		delete pSendStruct->SendQueue.front();
		pSendStruct->SendQueue.pop();
	}
	pSendStruct->LastTime = gettickcount();
}

void CCreditUDP::SendCloseMsg(udp::endpoint& ep) {
	UDPStruct buffer(0);
	buffer.nAck = 2;

	//buffer.DestIP = *((unsigned int*)&address_v4::from_string(ip).to_bytes().elems);
	//buffer.DestPort = htons( boost::lexical_cast<unsigned short>(port) );

	buffer.DestIP = ep.address().to_v4().to_ulong();
	buffer.DestPort = ep.port();

	InnerSendUdpData(ep, (char*) &buffer, sizeof(UDPStruct));
}

void CCreditUDP::SendCloseMsg(const string &ip, const string &port) {
	UDPStruct buffer(0);
	buffer.nAck = 2;

	buffer.DestIP = ConvertIP(ip); // *((unsigned int*)&address_v4::from_string(ip).to_bytes());
	buffer.DestPort = htons( boost::lexical_cast<unsigned short>(port) );
	InnerSendUdpData(ip, port, (char*) &buffer, sizeof(UDPStruct));
}

CUDPSocket* CCreditUDP::GetUdpSock() {
	return &m_UDPSocket;
}

int CCreditUDP::GetCurrentSeq(udp::endpoint& ep) {
	SendKey key(ep);
	int iRet = 0;

	boost::unique_lock<boost::mutex> sendMaplock(m_mutexSendMapLock);
	if (m_SendMap.find(key) != m_SendMap.end()) {
		SendStruct* pSendStruct = m_SendMap[key];
		iRet = pSendStruct->nSequence;
	}
	return iRet;
}

// int CCreditUDP::GetCurrentSeq(const string &ip, const string &port) {
// 	SendKey key(ip, port);
// 	int iRet = 0;
// 
// 	boost::unique_lock<boost::mutex> sendMaplock(m_mutexSendMapLock);
// 	if (m_SendMap.find(key) != m_SendMap.end()) {
// 		SendStruct* pSendStruct = m_SendMap[key];
// 		iRet = pSendStruct->nSequence;
// 	}
// 	return iRet;
// }

bool CCreditUDP::InnerSendUdpData(const string &strIp, const string &strPort,
		char* pData, int iLen) {
	string errinfo;
	bool bRet;
#ifdef OPTION_CRYPT
	if ( g_bCryptCudp )
	{
		vector<unsigned char> pDataEnc(iLen + 2 + 16);
		short iLenEnc = CRijndael::Encrypt( (unsigned char*)pData, iLen, &pDataEnc[0], g_CudpCryptKey );
		memcpy( &pDataEnc[0] + iLenEnc, &iLen, 2 );
		bRet = m_UDPSocket.SendData(strIp,strPort,(char*)&pDataEnc[0],iLenEnc + 2,errinfo);
	}
	else
	{
		bRet = m_UDPSocket.SendData(strIp,strPort,pData,iLen,errinfo);
	}
#else
	bRet = m_UDPSocket.SendData(strIp, strPort, pData, iLen, errinfo);
#endif
	return bRet;
}

int CCreditUDP::SendData(unsigned long IP,unsigned short Port,char*buffer,int nSize,string& errorInfo
						 ,int nReSendCount,bool bWait,int iTimeOut, unsigned long iProxyIp, unsigned short iProxyPort)
{
	string sIp;
	boost::uint32_t iIp = IP;
	ConvertNumberIpAddress(iIp, sIp);
	string sPort = lexical_cast<string>(network_to_host_short(Port));
	return SendData(sIp, sPort, buffer, nSize, errorInfo, nReSendCount, bWait, iTimeOut, iProxyIp, iProxyPort);
}

void CCreditUDP::DirectSendData(unsigned long ip, unsigned short port, char* data, int len
					, unsigned long iProxyIp, unsigned short iProxyPort)
{
	string sIp;
	boost::uint32_t iIp = ip;
	ConvertNumberIpAddress(iIp, sIp);
	string sPort = lexical_cast<string>(network_to_host_short(port));
	return DirectSendData(sIp, sPort, data, len);
}

int CCreditUDP::ReceiveData(char*&sData,int nDataSize,unsigned long& ip,unsigned short& port,string& errorInfo,int nDeferTime)
{
	vector<char> m(nDataSize);
	string sIp, sPort;
	int i = ReceiveData(m, nDataSize, sIp, sPort, errorInfo, nDeferTime);
	if (i > 0)
	{
		sData = new char[i];
		memcpy(sData, &m[0], i);
		ip = ConvertIP(sIp);
		port = host_to_network_short(lexical_cast<unsigned short>(sPort));
	}
	return i;
}

int CCreditUDP::ReceiveDataForBuffer(char*sData,int nDataSize,unsigned long& ip,unsigned short& port,string& errorInfo,int nDeferTime)
{
	string sIp, sPort;
	int i = ReceiveDataForBuffer(sData, nDataSize, sIp, sPort, errorInfo, nDeferTime);
	if (i > 0)
	{
		ip = ConvertIP(sIp);
		port = host_to_network_short(lexical_cast<unsigned short>(sPort));
	}
	return i;
}

void CCreditUDP::SetMakeLogFunc(make_log_func log_func)
{
	make_log_func_ = log_func;
    m_UDPSocket.SetMakeLogFunc(log_func);
}
