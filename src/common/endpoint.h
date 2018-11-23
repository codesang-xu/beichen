#ifndef IPCC_SERVICE_ENDPOINT_H
#define IPCC_SERVICE_ENDPOINT_H

#include <string>

#ifdef _WIN32
#include <WinSock2.h>
#else
#include <arpa/inet.h>
#endif

using std::string;

namespace ib
{
namespace common
{
	class endpoint
	{
	public:
		endpoint()
			:ip(0), port(0)
		{};

		endpoint(unsigned int ip, unsigned short port)
		{
			this->ip = ip;
			this->port = port;
		};

		endpoint(string ip, string port)
		{
			this->ip = inet_addr(ip.c_str());
			this->port = htons(atoi(port.c_str()));
		};

	public:
		unsigned int   ip;
		unsigned short port;
	};

	inline bool operator == (const endpoint &s1, const endpoint &s2)
	{
		return (s1.ip == s2.ip && s1.port == s2.port);
	}

}
}

namespace ib_common = ib::common;

#endif
