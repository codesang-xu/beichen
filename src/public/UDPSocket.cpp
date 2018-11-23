// UDPSocket.cpp: implementation of the CMySocket class.
//
//////////////////////////////////////////////////////////////////////
#include "UDPSocket.h"
#include <sstream>
using namespace std;

#include "GlobalOperation.h"

int CUDPSocket::g_iRefs = 0;

CUDPSocket::CUDPSocket():m_mySocket(io_service)
{
    make_log_func_ = boost::bind(&CUDPSocket::make_log, this, _1);
}

CUDPSocket::~CUDPSocket()
{
}

bool CUDPSocket::Init(const string & localPort, int recvbufSize, int sendbufSize, string& errInfo, string localIp)
{
	this->m_iRecvFailedCount = 0;
    recvbufSize=recvbufSize < 8192 ? 8192 : recvbufSize;
    sendbufSize=sendbufSize < 8192 ? 8192 : sendbufSize;

		this->SetParams(localPort,recvbufSize,sendbufSize);
		m_sLocalIp = localIp;

		if(!this->PrepareForUse(errInfo))
		{
			//this->Release();
			return false;
		}

	return true;
}

/*void CUDPSocket::Release()
{
	try
	{
		closesocket(m_mySocket);
		g_iRefs--;
	}

	catch(...){}

	if(g_iRefs == 0)
	{
		try
		{
			WSACleanup();
		}
		catch(...){}
	}
}*/

void CUDPSocket::SetParams(const string & localPort, int recvbufSize, int sendbufSize)
{
	this->m_strLocalPort = localPort;
	this->m_iRecvBufSize = recvbufSize; // * 8;
	this->m_iSendBufSize = sendbufSize; // * 8;

	if(boost::lexical_cast<int>(m_strLocalPort)<0 || boost::lexical_cast<int>(m_strLocalPort)>65536)
		this->m_strLocalPort = "3579";
}

bool CUDPSocket::PrepareForUse(string& errInfo)
{
      m_mySocket.open(udp::v4());
      m_mySocket.bind(udp::endpoint(udp::v4(), boost::lexical_cast<unsigned short>(m_strLocalPort) ));

#ifdef _MSC_VER
      int rcvtimeo = 50;
      if (setsockopt(m_mySocket.native_handle(), SOL_SOCKET, SO_RCVTIMEO,
          (const char *)&rcvtimeo, sizeof(rcvtimeo)) == -1)
      {
          cerr << "setsockopt " << strerror(errno) << endl;
          return false;
      }
#else
      struct timeval rcvtimeo = { 0, 50000 };
      if (setsockopt(m_mySocket.native_handle(), SOL_SOCKET, SO_RCVTIMEO,
          (const char *)&rcvtimeo, sizeof(rcvtimeo)) == -1)
      {
          cerr << "setsockopt " << strerror(errno) << endl;
          return false;
      }
#endif

	  boost::asio::socket_base::receive_buffer_size option1(this->m_iRecvBufSize);
	  m_mySocket.set_option(option1);


	  boost::asio::socket_base::send_buffer_size option2(this->m_iSendBufSize);
	  m_mySocket.set_option(option2);

//	  boost::asio::socket_base::non_blocking_io command(true);
//	  m_mySocket.io_control(command);

	 return true;
}

int CUDPSocket::ReceiveData(vector<char> & msg, int size, udp::endpoint&  ep, string& errInfo)
{
	int ret = -1;
	bool need_delete = false;
	memset(&msg[0], 0, msg.size());

	//udp::endpoint remote_endpoint;
	ret = myRecvFrom(&msg[0], msg.size(), ep);

	if(ret < 0)
	{
		m_iRecvFailedCount++;
		if(m_iRecvFailedCount > 18)	// for the switch can initiate a call at 20 millisecond intervals
		{
			boost::unique_lock<boost::mutex> lock(this->m_mut);
			m_cond.timed_wait(lock, boost::posix_time::milliseconds(1));
		}
		return -1;
	}
	//ip = remote_endpoint.address().to_string();
	//port = boost::lexical_cast<string>(remote_endpoint.port());
	m_iRecvFailedCount=0;

	return ret;
}


int CUDPSocket::ReceiveData(vector<char> & msg, int size, string& ip, string& port, string& errInfo)
{
	int ret = -1;
	bool need_delete = false;
	memset(&msg[0], 0, msg.size());

	udp::endpoint remote_endpoint;
	ret = myRecvFrom(&msg[0],size,remote_endpoint);

	if(ret < 0)
	{
		m_iRecvFailedCount++;
		if(m_iRecvFailedCount > 18)	// for the switch can initiate a call at 20 millisecond intervals
		{
			boost::unique_lock<boost::mutex> lock(this->m_mut);
			m_cond.timed_wait(lock, boost::posix_time::milliseconds(1));
		}
		return -1;
	}
	ip = remote_endpoint.address().to_string();
	port = boost::lexical_cast<string>(remote_endpoint.port());
	m_iRecvFailedCount=0;

//	DirectAnalyzeCreditData(&this->m_mySocket, msg, ret, 0, ip, port, ret);

	return ret;
}

bool CUDPSocket::SendData(const string &strIp, const string &strPort, char *msg, int size, string &errInfo
						  , unsigned long iProxyIp, unsigned short iProxyPort)
{
	if(strIp.empty())
		return false;

	if(strIp=="255.255.255.255")// 
		return false;

	if(boost::lexical_cast<int>(strPort) <=0 || boost::lexical_cast<int>(strPort) >65535)
		return false;


	if(msg == NULL)
		return true;

    try
    {
        udp::resolver resolver(this->io_service);
        udp::resolver::query query(udp::v4(), strIp, strPort);
        udp::endpoint receiver_endpoint = *resolver.resolve(query);
        int ret = this->m_mySocket.send_to(boost::asio::buffer((const void*)msg, size), receiver_endpoint);
        if (ret < size)
        {
            return false;
        }
    }
    catch (const boost::system::error_code& err)
    {
        ostringstream oss;
        oss << "严重错误 CUDPSocket error_code1 " << err.value();
        make_log_func_(oss.str());
        return false;
    }
    catch (...)
    {
        ostringstream oss;
        oss << "严重错误...";
        make_log_func_(oss.str());
        return false;
    }

	return true;
}

bool CUDPSocket::SendData(udp::endpoint&  ep, char* msg, int size, string& errInfo, unsigned long iProxyIp /*= 0*/, unsigned short iProxyPort /*= 0*/)
{
	if(ep.address().to_v4().to_string().empty())
		return false;

	if(ep.address().to_v4().to_string()=="255.255.255.255")
		return false;

	if(ep.port() <=0 || ep.port() >65535)
		return false;


	if(msg == NULL)
		return true;

    try
    {
        int ret = this->m_mySocket.send_to(boost::asio::buffer((const void*)msg, size), ep);
        if (ret < size)
        {
            return false;
        }
    }
    catch (const boost::system::error_code& err)
    {
        ostringstream oss;
        oss << "严重错误 CUDPSocket error_code2 " << err.value();
        make_log_func_(oss.str());
        return false;
    }
    catch (...)
    {
        ostringstream oss;
        oss << "严重错误...";
        make_log_func_(oss.str());
        return false;
    }


	return true;
}

bool CUDPSocket::SendData(boost::uint32_t ip, unsigned short port, char *msg, int size, string &errInfo
						  , unsigned long iProxyIp, unsigned short iProxyPort)
{
	if(ip<0)
		return false;

	if(port<=0 || port>65535)
		return false;

	try
	{
		if(msg == NULL)
			return true;

		/*sockaddr_in server;
		server.sin_addr.S_un.S_addr = ip;
		server.sin_family = AF_INET;
		server.sin_port = port;
		*/

		//struct in_addr addr1;
		//memcpy(&addr1, &ip, 4);
		string ipAddress;
		ConvertNumberIpAddress(ip,ipAddress);
	    udp::resolver resolver(this->io_service);
	    udp::resolver::query query(udp::v4(),ipAddress,"daytime");
	    udp::endpoint receiver_endpoint=*resolver.resolve(query);
	    this->m_mySocket.open(udp::v4());
	    int ret=this->m_mySocket.send_to(boost::asio::buffer((const void*)msg,size),receiver_endpoint);
	}
    catch (const boost::system::error_code& err)
    {
        ostringstream oss;
        oss << "严重错误 CUDPSocket error_code3 " << err.value();
        make_log_func_(oss.str());
        return false;
    }
    catch (...)
	{
        ostringstream oss;
        oss << "严重错误...";
        make_log_func_(oss.str());
        return false;
    }

	return true;
}

bool CUDPSocket::SendData(boost::uint32_t ip, unsigned short port, char *msg, int size, int &err
						  , const string & service_name,unsigned long iProxyIp, unsigned short iProxyPort)
{
	if(ip<0)
		return false;

	if(port<=0 || port>65535)
		return false;

	try
	{
		if(msg == NULL)
			return false;

		/*sockaddr_in server;
		server.sin_addr.S_un.S_addr = ip;
		server.sin_family = AF_INET;
		server.sin_port = port;
		*/

		//int ret = sendto(this->m_mySocket,(const char*)msg, size, 0, (const sockaddr *)&server, sizeof(server));

			//udp::resolver::query query(udp::v4(),,"daytime");
			//udp::endpoint receiver_endpoint=*resolver.resolver(query);
	    string ipAddress;
		ConvertNumberIpAddress(ip,ipAddress);
	    udp::resolver resolver(this->io_service);
	    udp::resolver::query query(udp::v4(),ipAddress,service_name);
        udp::endpoint receiver_endpoint=*resolver.resolve(query);
	    this->m_mySocket.open(udp::v4());
	    int ret=this->m_mySocket.send_to(boost::asio::buffer((const void*)msg,size),receiver_endpoint);
		/*if(-1 == ret)
		{
			err = WSAGetLastError();
			return false;
		}*/
	}
    catch (const boost::system::error_code& err)
    {
        ostringstream oss;
        oss << "严重错误 CUDPSocket error_code4 " << err.value();
        make_log_func_(oss.str());
        return false;
    }
    catch (...)
	{
        ostringstream oss;
        oss << "严重错误...";
        make_log_func_(oss.str());
        return false;
    }

	return true;
}
int CUDPSocket::ReceiveData(vector<char> &msg, int size, string &errInfo)
{
	int ret = -1;

	memset(&msg[0], 0, msg.size());

	udp::endpoint remote_endpoint;
	//int dwSender = sizeof(sender);
	ret = myRecvFrom(&msg[0], msg.size(), remote_endpoint);

	if(-1 == ret)
	{

		return -1;
	}

	return ret;
}

string CUDPSocket::GetPort()
{
	return this->m_strLocalPort;
}

int CUDPSocket::myRecvFrom(char * buf, int len,udp::endpoint &remote_endpoint)
{

	boost::system::error_code error;
	std::size_t nRet = this->m_mySocket.receive_from(boost::asio::buffer(buf,len),remote_endpoint,0,error);
	if(error)
	{
        ostringstream oss;
        oss << "严重错误 CUDPSocket error_code5 " << error.value();
        make_log_func_(oss.str());
        nRet = -1;
	}

	return nRet;
}
void CUDPSocket::ConvertNumberIpAddress(boost::uint32_t &in_ipNumberAddress,std::string &out_validIpAddress)
{
    if(in_ipNumberAddress<0||in_ipNumberAddress>4294967295)
   {
       out_validIpAddress = "0.0.0.0"; //invalid IP Address
    }
    else
   {
        boost::uint32_t divisor = 256;
        boost::uint32_t ip_section1 = 0;
        boost::uint32_t ip_section2 = 0;
        boost::uint32_t ip_section3 = 0;
        boost::uint32_t ip_section4 = 0;
        std::string ip_section_str1;
        std::string ip_section_str2;
        std::string ip_section_str3;
        std::string ip_section_str4;
        std::string dot = ".";

        ip_section1 = in_ipNumberAddress%divisor;
        in_ipNumberAddress = in_ipNumberAddress/divisor;
        ip_section2 = in_ipNumberAddress%divisor;
        in_ipNumberAddress = in_ipNumberAddress/divisor;
        ip_section3 = in_ipNumberAddress%divisor;
        in_ipNumberAddress = in_ipNumberAddress/divisor;
        ip_section4 = in_ipNumberAddress%divisor;
        ip_section_str1 = boost::lexical_cast <std::string> (ip_section1);
        ip_section_str2 = boost::lexical_cast <std::string> (ip_section2);
        ip_section_str3 = boost::lexical_cast <std::string> (ip_section3);
        ip_section_str4 = boost::lexical_cast <std::string> (ip_section4);
        out_validIpAddress = ip_section_str4+dot+ip_section_str3+dot+ip_section_str2+dot+ip_section_str1;
    }
}

