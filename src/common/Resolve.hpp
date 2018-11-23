/*
 * Resolve.hpp
 *
 *  Created on: 2014Äê7ÔÂ29ÈÕ
 *      Author: haoy
 */

#ifndef RESOLVE_HPP_
#define RESOLVE_HPP_

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <netdb.h>
#endif
#include <string>
#include <vector>

#include "endpoint.h"
#include "common/util.h"

namespace ib
{
namespace common
{
namespace net{
namespace ip{

class Resolver{
public:	
	Resolver(){};
	virtual ~Resolver(){};
	
	void cleanupAddrInfo(addrinfo* entry){
		addrinfo* ai = entry;
		
		while(ai){
			addrinfo* next = ai->ai_next;
			freeaddrinfo(ai);
			ai = next;
		}
	};
	
	void parseAddrInfo(addrinfo* entry, std::vector<endpoint>& out){
		addrinfo* ai = entry;
		
		while(ai){
			if(ai->ai_addr && ai->ai_addrlen && ai->ai_family == PF_INET){
				sockaddr_in* sa = (sockaddr_in*)ai->ai_addr;
				out.push_back(endpoint(sa->sin_addr.s_addr, sa->sin_port));
			}
			ai = ai->ai_next;
		}
	};
	
	void resolve(const std::string host_name, std::vector<endpoint>& ends){
		addrinfo hints;
		hints.ai_flags = 0;
		hints.ai_family = PF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;
		hints.ai_addrlen = 0;
		hints.ai_canonname = NULL;
		hints.ai_addr = NULL;
		hints.ai_next = NULL;
		addrinfo* result = NULL;
		
		int error = getaddrinfo(host_name.c_str(), NULL, &hints, &result);
		if(error){
			cleanupAddrInfo(result);
		}
				
		parseAddrInfo(result, ends);
		
		//todo important
		//cleanupAddrInfo(result);
				
	};
};

}
}
}
}

namespace ib_common = ib::common;



//		if(error){
//			switch(error){
//			case EAI_ADDRFAMILY:
//				break;
//			case EAI_AGAIN:
//				break;
//			case EAI_BADFLAGS:
//				break;
//			case EAI_FAIL:
//				break;
//			case EAI_FAMILY:
//				break;
//			case EAI_NODATA:
//				break;
//			case EAI_NONAME:
//				break;
//			case EAI_SERVICE:
//				break;
//			case EAI_SOCKTYPE:
//				break;
//			case EAI_SYSTEM:
//				break;
//			case EAI_BADHINTS:
//				break;
//			case EAI_PROTOCOL:
//				break;
//			case EAI_OVERFLOW:
//				break;
//			default:
//				
//			}
//		}

#endif /* RESOLVE_HPP_ */
