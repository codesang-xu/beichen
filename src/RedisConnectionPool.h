#pragma once

#include <string>
 
#include <boost/atomic.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "common/blocking_queue.h"
#include "SynRedisClient.h"

using namespace std;
using namespace ib_common;

typedef boost::function<void(CSynRedisClientPtr&)> OnClientConnectedCallBack;

class CRedisConnectionPool
{
public:
	CRedisConnectionPool();
	~CRedisConnectionPool();

	void Initialize(string name, string redisIp, uint16_t redisPort, uint32_t dbIndex,
        string strPassword, OnClientConnectedCallBack connectedfun, MakeLogFun logfun, int miniConnCount = 1);
	void Uninitialize();
	bool IsInited() { return m_inited; };

    CSynRedisClientPtr Get(const char * file, int line);
    CSynRedisClientPtr TimedGet(int timeoutInMillisecond, const char * file, int line);
    void Free(CSynRedisClientPtr & connPtr);

    void Clear();

private:
	inline void MakeConnection();
	CSynRedisClientPtr MakeConnection(string redisIp, uint16_t redisPort, uint32_t dbIndex, string strPassword);
	void RecycleConn(CSynRedisClient* conn);
	void MakeConnThreadFun();
	inline void IgnoreLog(string log) {};
    
private:
	boost::atomic<bool> m_inited;

	string m_strName;
	string m_redisIp;
	uint16_t m_redisPort;
	uint32_t m_dbIndex;
	string m_strPassword;
	uint32_t m_miniConnCount;
	MakeLogFun m_logfun;
    OnClientConnectedCallBack m_OnConnectedFun;

	blocking_queue<CSynRedisClientPtr> m_connList;
	blocking_queue<int> m_signalList;
	boost::thread conn_thread_;
    int counter_;

};

