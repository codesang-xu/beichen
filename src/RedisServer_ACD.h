#ifndef REDIS_SERVER_ACD_H
#define REDIS_SERVER_ACD_H

#include "common/server_addr_pack.h"
#include "common/timer_service.h"
#include "public/CreditUDP.h"
#include "public/MemLog.h"
#include "public/IPCCMessage.h"
#include "ServiceInfo.h"
#include "RedirectServer.h"
#include "RedisConnectionPool.h"


class RedisServer_ACD
{
public:
	RedisServer_ACD();

    void Init(CMemLog *pLogger, RedirectServer* redirectServer, int serType);
    void Init(CMemLog *pLogger, string redisIp, uint16_t redisPort, uint32_t dbIndex, string password);
    void InitRedis();

	bool IsOK();

    bool PushBackToCallQueue(string managerid, string deptname, string callid, string & callinfo);
    bool PushFrontToCallQueue(string managerid, string deptname, string callid, string & callinfo);
    bool RemoveFromCQueue(string managerid, string deptname, string callid);
    //////bool PopCallQueue(string managerid, string deptname, int count, vector<string> & queue);
    bool GetCallQueue(string managerid, string deptname, int count, vector<string> & queue);

    bool SaveAgent(string managerid, string agentid, int statues);
    pair<int, bool> GetAgentStatus(string managerid, string agentid);
    bool RemoveAgent(string managerid, string agentid);

    bool PushBackToAgentQueue(string managerid, string deptname, string agentid, int iQualification);
    bool PushFrontToAgentQueue(string managerid, string deptname, string agentid, int iQualification);
    bool RemoveFromAgentQueue(string managerid, string deptname, string agentid, int iQualification);
    bool GetAgentQueue(string managerid, string deptname, int count, vector<pair<string, int> > & queue);

private:
	void On_SIPCCMsg_Redirect_GetServiceConf(string invokeid, vector<ServiceInfo> & serviceVec, vector<RedisDBInfo> redisVec);
	void GetRedisAddr();

private:
	RedirectServer* m_redirectServer;
	CMemLog    *m_pLogger;
	ESeverType m_serviceType;

	CRedisConnectionPool m_RedisConnectionPool;

	string getCallQueueLuaSHA;
	const static string GET_CALL_QUEUE;

    map<string, string> m_redisScriptMap;
    map<string, string> m_redisSHAMap;
};

#endif
