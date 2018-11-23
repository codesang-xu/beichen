#include "RedisConnectionPool.h"

#include <boost/bind.hpp>

#include "common/util.h"

CRedisConnectionPool::CRedisConnectionPool()
	: m_inited(false)
	, m_redisPort(0)
	, m_dbIndex(1)
	, m_miniConnCount(1)
	, m_logfun(bind(&CRedisConnectionPool::IgnoreLog, this, _1))
	, conn_thread_(bind(&CRedisConnectionPool::MakeConnThreadFun, this))
{
}

CRedisConnectionPool::~CRedisConnectionPool()
{
	m_signalList.put(0);
	conn_thread_.join();
}

void CRedisConnectionPool::Initialize(string name, string redisIp, uint16_t redisPort, 
    uint32_t dbIndex, string strPassword, OnClientConnectedCallBack connectedfun, MakeLogFun logfun, int miniConnCount)
{
    stringstream ss;
    ss << "Redis Connection Pool " << name << " initializing ...";
    logfun(ss.str());
    
    m_strName = name;
	m_redisIp = redisIp;
	m_redisPort = redisPort;
	m_dbIndex = dbIndex;
	m_strPassword = strPassword;
	m_miniConnCount = miniConnCount;
	m_logfun = logfun;
    m_OnConnectedFun = connectedfun;
	m_inited = true;
    counter_ = 0;

	MakeConnection();
}

void CRedisConnectionPool::Uninitialize()
{
    if (!m_inited)
        return;

    stringstream ss;
    ss << "Redis Connection Pool " << m_strName << " uninitializing ...";
    m_logfun(ss.str());

	m_inited = false;
	m_strName = "";
	m_redisIp = "";
	m_redisPort = 0;
	m_dbIndex = 1;
	m_strPassword = "";
	m_miniConnCount = 1;
    counter_ = 0;
	m_connList.clear();
	m_signalList.clear();
}

CSynRedisClientPtr CRedisConnectionPool::Get(const char * file, int line)
{
    CSynRedisClientPtr connptr;

    if (!m_inited)
        return connptr;

    MakeConnection();
    connptr = m_connList.take();
    MakeConnection();

    if (!connptr) {
        ostringstream ostream;
        ostream << "连接redis失败，at=" << ib::common::get_file_name(file) << "@" << line;

        m_logfun(ostream.str());
    }

    return connptr;
}

CSynRedisClientPtr CRedisConnectionPool::TimedGet(int timeoutInMillisecond, const char * file, int line)
{
    CSynRedisClientPtr connptr;

    if (!m_inited)
        return connptr;

    if (timeoutInMillisecond < 1)
    {
        timeoutInMillisecond = 1000*5;
    }

    MakeConnection();
    connptr = m_connList.timed_take(timeoutInMillisecond);
    MakeConnection();

    if (!connptr) {
        ostringstream ostream;
        ostream << "连接redis失败，at=" << ib::common::get_file_name(file) << "@" << line;

        m_logfun(ostream.str());
    }

    return connptr;
}

void CRedisConnectionPool::Free(CSynRedisClientPtr & connPtr)
{
    if (connPtr)
    {
        connPtr->DisConnect();

        stringstream ss;
        ss << "conn " << connPtr->GetName() << " Freed !!!";
        m_logfun(ss.str());
    }
}

void CRedisConnectionPool::MakeConnection()
{
    if (!m_inited)
        return;
	if (m_connList.size() < 1)
	{
		m_signalList.put(1);
	}
}

CSynRedisClientPtr CRedisConnectionPool::MakeConnection(string redisIp, uint16_t redisPort, 
	uint32_t dbIndex, string strPassword)
{
    for (int i = 0; i < 3 && m_inited; i++)
    {
	    ostringstream oss;
        oss << m_strName << "_" << counter_++;
	    CSynRedisClientPtr newconn(new CSynRedisClient(oss.str(), m_logfun),
		    boost::bind(&CRedisConnectionPool::RecycleConn, this, _1));
	    if (newconn->Connect(redisIp, redisPort, strPassword, dbIndex))
	    {
            m_OnConnectedFun(newconn);
		    return newconn;
	    }
        else
        {
            Free(newconn);
        }
    }
    Uninitialize();
    return CSynRedisClientPtr();
}

void CRedisConnectionPool::RecycleConn(CSynRedisClient* conn)
{
    if (conn ==  NULL || !conn->IsConnected())
		return ;

	m_connList.put(CSynRedisClientPtr(conn,
		boost::bind(&CRedisConnectionPool::RecycleConn, this, _1)));
}

void CRedisConnectionPool::MakeConnThreadFun()
{
	while (m_signalList.take() == 1)
	{
		int conncount = m_miniConnCount - m_connList.size();
		
		if (conncount < 1)
			continue;
		
		for (size_t i = 0; i < conncount; i++)
		{
			CSynRedisClientPtr connptr = MakeConnection(m_redisIp, m_redisPort, m_dbIndex, m_strPassword);
			if (connptr)
			{
				m_connList.put(connptr);
			}
			else
			{
				continue;
			}
		}
	}
}

void CRedisConnectionPool::Clear()
{
    std::deque<CSynRedisClientPtr> connList;
    m_connList.swap(connList);

    for (auto it = connList.begin(); it != connList.end(); it++)
    {
        Free(*it);
    }

    stringstream ss;
    ss << "Redis Connection Pool " << m_strName << " cleared !!!";
    m_logfun(ss.str());
}
