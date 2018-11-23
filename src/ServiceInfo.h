#ifndef SERVICE_INFO_H
#define SERVICE_INFO_H

#include <string>
using namespace std;
#ifndef NOPUBLIC
#include "public/IPCCMessage.h"
#else
#include "IPCCMessage.h"
#endif
class ServiceInfo
{
public:
	ServiceInfo()
	{
		reset();
	}

	ServiceInfo(uint32_t serviceID, ESeverType serviceType, uint32_t ip, uint16_t port, string serviceName = "")
	{
		this->reset();
		this->serviceID = serviceID;
		this->serviceName = serviceName;
		this->serviceType = serviceType;
		this->ip = ip;
		this->port = port;
		this->isLive = false;
	}

	ServiceInfo(uint32_t serviceID, uint32_t ip, uint16_t port)
	{
		this->reset();
		this->serviceID = serviceID;
		this->ip = ip;
		this->port = port;
	}

	void reset()
	{
		this->serviceID = 0;
		this->serviceName = "";
		this->serviceType = EST_Unknow;
		this->ip = 0;
		this->port = 0;
		this->isLive = false;
	}

	bool isAddrAvailable()
	{
		return !(ip == 0 && port == 0);
	}

	bool equal(uint32_t serviceID)
	{
		return (this->serviceID == serviceID);
	}

	bool equal(uint32_t ip, uint16_t port)
	{
		return ((this->ip == ip) && (this->port == port));
	}

	bool operator == (const ServiceInfo & other)
	{
		return (this->serviceID == other.serviceID) && (this->ip == other.ip) && (this->port == other.port);
	}

public:
	uint32_t serviceID;
	string serviceName;
	ESeverType serviceType;
	uint32_t ip;
	uint16_t port;
	bool isLive;
};

class RedisDBInfo
{
public:
	RedisDBInfo()
	{
		reset();
	}

	RedisDBInfo(int redistype, int dbIndex, uint32_t ip, uint16_t port, string password)
	{
		this->reset();
		this->serviceType = (ESeverType)redistype;
		this->dbIndex = dbIndex;
		this->ip = ip;
		this->port = port;
		this->password = password;
	}

	void reset()
	{
		this->serviceID = 0;
		this->serviceName = "";
		this->serviceType = EST_Unknow;
		this->ip = 0;
		this->port = 0;
		this->password = "";
		this->dbIndex = 0;
	}

	bool isAddrAvailable()
	{
		return !(ip == 0 && port == 0);
	}

	bool equal(uint32_t serviceID)
	{
		return (this->serviceID == serviceID);
	}

	bool equal(uint32_t ip, uint16_t port)
	{
		return ((this->ip == ip) && (this->port == port));
	}

	bool operator == (const ServiceInfo & other)
	{
		return (this->serviceID == other.serviceID) && (this->ip == other.ip) && (this->port == other.port);
	}

public:
	uint32_t serviceID;
	string serviceName;
	ESeverType serviceType;
	uint32_t ip;
	uint16_t port;
	string password;
	int dbIndex;
};


#endif
