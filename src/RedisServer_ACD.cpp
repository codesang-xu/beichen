#include "RedisServer_ACD.h"

#include <boost/lexical_cast.hpp>
using namespace boost;

#include "public/Spliter.h"
#include "common/ipccmsg_helper.h"
#include "UserInfo.h"


//
//1、呼叫队列数据结构
//每个呼叫队列保存两个结构：
//1）、呼叫队列，保存本呼叫队列的所有呼叫.
//     类型：队列
//     key：manager.部门名（技能组名）.CallQueue.
//     value：callid.
//     说明：redis队列未按照实际的优先级排队，但是相同优先级间呼叫的相对顺序是正确的
//          队列恢复时，需要按照优先级重新调整顺序
//
//2）、呼叫信息，每个呼叫的具体信息。
//     类型：哈希
//     key：manager.部门名（技能组名）名.CallInfo.
//     field：callid
//     value：呼叫信令的序列化数据.
//
//2、座席队列数据结构
//     类型：队列
//     key：manager.部门名（技能组名）.AgentQueue.
//     value：技能等级:agentid
//     说明：redis队列未按照实际的优先级排队，但是相同技能等级的座席间的相对顺序是正确的
//          队列恢复时，需要按照优先级重新调整顺序


RedisServer_ACD::RedisServer_ACD()
{
}

void RedisServer_ACD::Init(CMemLog *pLogger, RedirectServer* redirectServer, int serType)
{
	m_pLogger = pLogger;
	m_serviceType = (ESeverType)serType;

	m_redirectServer = redirectServer;

	GetRedisAddr();
}

void RedisServer_ACD::Init(CMemLog *pLogger, string redisIp, uint16_t redisPort, uint32_t dbIndex, string password)
{
    m_pLogger = pLogger;
    m_RedisConnectionPool.Initialize("ACD_Redis",
        redisIp, redisPort, dbIndex, password,
        [&](CSynRedisClientPtr& newConnPtr){},
        boost::bind(&CMemLog::MakeLog, m_pLogger, _1, LOG_LEVEL_NORMAL, "", ""));
    InitRedis();
}

void RedisServer_ACD::InitRedis()
{
    //KEYS[1] = callqueuekey(manager.部门名（技能组名）.CallQueue); KEYS[2] = callinfokey(manager.部门名（技能组名）.CallInfo)  ; ARGV[1] = callid; ARGV[2] = makecallmsg[bytebuffer], 
    m_redisScriptMap["PUSH_BACK_CALL_QUEUE"] = "do                                   "
        "    local ret = redis.call('RPUSH', KEYS[1], ARGV[1])                  "
        "    if ret then                                                        "
        "        redis.call('HSET', KEYS[2], ARGV[1], ARGV[2])            "
        "    end                                                                "
        "    return ret                                                         "
        "end                                                                    ";

    //说明：参见文件开始部分
    //KEYS[1] = callqueuekey(manager.部门名（技能组名）.CallQueue); KEYS[2] = callinfokey(manager.部门名（技能组名）.CallInfo)  ; ARGV[1] = callid; ARGV[2] = makecallmsg[bytebuffer], 
    m_redisScriptMap["PUSH_FRONT_CALL_QUEUE"] = "do                                   "
        "    local ret = redis.call('LPUSH', KEYS[1], ARGV[1])                  "
        "    if ret then                                                        "
        "        redis.call('HSET', KEYS[2], ARGV[1], ARGV[2])            "
        "    end                                                                "
        "    return ret                                                         "
        "end                                                                    ";

    ////////说明：参见文件开始部分
    ////////KEYS[1] = callqueuekey(manager.部门名（技能组名）.CallQueue); KEYS[2] = callinfokey(manager.部门名（技能组名）.CallInfo)  ; ARGV[1] = count, 
    //////m_redisScriptMap["POP_CALL_QUEUE"] = "do                                     "
    //////    "    local callQueueKey = KEYS[1]                                      "
    //////    "    local callInfoKey = KEYS[2]                                        "
    //////    "    local callQueue = redis.call('LRANGE', callQueueKey, 0, tonumber(ARGV[1] or 0))   "
    //////    "    local result = {}                                                                 "
    //////    "    if #callQueue > 0 then                                                        "
    //////    "        redis.call('LTRIM', callQueueKey, tonumber(ARGV[1] or 0), -1)                 "
    //////    "        result = redis.call('HMGET', callInfoKey, unpack(callQueue))                  "
    //////    "        redis.call('HDEL', callInfoKey, callQueue)                                    "
    //////    "    end                                                                               "
    //////    "    return result                                                                     "
    //////    "end                                                                                   ";

    //KEYS[1] = callqueuekey(manager.部门名（技能组名）.CallQueue); KEYS[2] = callinfokey(manager.部门名（技能组名）.CallInfo)  ; ARGV[1] = count, 
    m_redisScriptMap["GET_CALL_QUEUE"] = "do                                     "
        "    local callQueue = redis.call('LRANGE', KEYS[1], 0, tonumber(ARGV[1] or 0))    "
        "    if #callQueue > 0 then                                                        "
        "        return redis.call('HMGET', KEYS[2], unpack(callQueue))                    "
        "    end                                                                           "
        "    return {}                                                                     "
        "end                                                                               ";

    //KEYS[1] = callqueuekey(manager.部门名（技能组名）.CallQueue); KEYS[2] = callinfokey(manager.部门名（技能组名）.CallInfo)  ; ARGV[1] = callid
    m_redisScriptMap["REMOVE_FROM_CALL_QUEUE"] = "do                                           "
        "    if not KEYS[1] or #KEYS[1] < 1 or not KEYS[2] or #KEYS[2] < 1 or not ARGV[1] or #ARGV[1] < 1 then  "
        "       return 0                                                                       "
        "    end                                                                               "
        "    local result1 = redis.call('LREM', KEYS[1], 1, ARGV[1])                           "
        "    local result2 = redis.call('HDEL', KEYS[2], ARGV[1])                     "
        "    return result1 + result2                                                          "
        "end                                                                                   ";

    CSynRedisClientPtr redisClient = m_RedisConnectionPool.Get(__FILE__, __LINE__);
    if (!redisClient)
    {
        ostringstream ostream;
        ostream << "RedisServer_ACD::GetCallQueue Get MainRedisC_Read Error!";
        m_pLogger->MakeLog(ostream.str());
        return;
    }

    if (!m_redisSHAMap.empty())
    {
        ostringstream oss;
        oss << "SCRIPT EXISTS " << m_redisSHAMap.begin()->second;
        RedisReplyPtr replyPtr;
        if (redisClient->ExeCMD(oss.str(), replyPtr) &&
            replyPtr && replyPtr->integer == 1) {
            return;
        }
    }

    for (auto it = m_redisScriptMap.begin(); it != m_redisScriptMap.end(); it++)
    {
        string sha = redisClient->LoadScript(it->first, it->second);
        if (sha.empty())
            return;
        m_redisSHAMap[it->first] = sha;
    }
}

void RedisServer_ACD::On_SIPCCMsg_Redirect_GetServiceConf(string invokeid, vector<ServiceInfo> & serviceVec, vector<RedisDBInfo> redisVec)
{
	if (m_RedisConnectionPool.IsInited())
		return ;
	if (redisVec.size() > 0)
	{
		if (redisVec[0].serviceType == EST_MainRedisC_Write)
		{
			m_RedisConnectionPool.Initialize("RedisC_Write",
				format_ip_addr(redisVec[0].ip), ntohs(redisVec[0].port),
				redisVec[0].dbIndex, redisVec[0].password, 
                [&](CSynRedisClientPtr& newConnPtr){},
                boost::bind(&CMemLog::MakeLog, m_pLogger, _1, LOG_LEVEL_NORMAL, "", ""));
            InitRedis();
		}
	}
}

bool RedisServer_ACD::PushBackToCallQueue(string managerid, string deptname, string callid, string & callinfo)
{
    CSynRedisClientPtr redisClient = m_RedisConnectionPool.Get(__FILE__, __LINE__);
    if (!redisClient) {
        return false;
    }

    ostringstream callqueuekey;
    callqueuekey << managerid << "." << deptname << ".CallQueue";

    ostringstream callInfokey;
    callInfokey << managerid << "." << deptname << ".CallInfo";

    CRedisCmdParam params;
    params.PushKey(callqueuekey.str());
    params.PushKey(callInfokey.str());
    params.PushParam(callid);
    params.PushParam(callinfo);

    RedisReplyPtr reply;
    string scriptName = "PUSH_BACK_CALL_QUEUE";

    return (redisClient->EvalSHA(scriptName, m_redisSHAMap[scriptName], params, reply) &&
        reply &&
        reply->type == REDIS_REPLY_INTEGER &&
        reply->integer > 0);
}

bool RedisServer_ACD::PushFrontToCallQueue(string managerid, string deptname, string callid, string & callinfo)
{
    CSynRedisClientPtr redisClient = m_RedisConnectionPool.Get(__FILE__, __LINE__);
    if (!redisClient) {
        return false;
    }

    ostringstream callqueuekey;
    callqueuekey << managerid << "." << deptname << ".CallQueue";

    ostringstream callInfokey;
    callInfokey << managerid << "." << deptname << ".CallInfo";

    CRedisCmdParam params;
    params.PushKey(callqueuekey.str());
    params.PushKey(callInfokey.str());
    params.PushParam(callid);
    params.PushParam(callinfo);

    RedisReplyPtr reply;
    string scriptName = "PUSH_FRONT_CALL_QUEUE";

    return (redisClient->EvalSHA(scriptName, m_redisSHAMap[scriptName], params, reply) &&
        reply &&
        reply->type == REDIS_REPLY_INTEGER &&
        reply->integer > 0);
}

bool RedisServer_ACD::RemoveFromCQueue(string managerid, string deptname, string callid)
{
    CSynRedisClientPtr redisClient = m_RedisConnectionPool.Get(__FILE__, __LINE__);
    if (!redisClient) {
        return false;
    }

    ostringstream callqueuekey;
    callqueuekey << managerid << "." << deptname << ".CallQueue";

    ostringstream callInfokey;
    callInfokey << managerid << "." << deptname << ".CallInfo";

    CRedisCmdParam params;
    params.PushKey(callqueuekey.str());
    params.PushKey(callInfokey.str());
    params.PushParam(callid);

    RedisReplyPtr reply;
    string scriptName = "REMOVE_FROM_CALL_QUEUE";

    return (redisClient->EvalSHA(scriptName, m_redisSHAMap[scriptName], params, reply) &&
        reply &&
        reply->type == REDIS_REPLY_INTEGER &&
        reply->integer > 0);
}

//////bool RedisServer_ACD::PopCallQueue(string managerid, string deptname, int count, vector<string> & queue)
//////{
//////    CSynRedisClientPtr redisClient = m_RedisConnectionPool.Get(__FILE__, __LINE__);
//////    if (!redisClient) {
//////        return false;
//////    }
//////
//////    ostringstream callqueuekey;
//////    callqueuekey << managerid << "." << deptname << ".CallQueue";
//////
//////    ostringstream callInfokey;
//////    callInfokey << managerid << "." << deptname << ".CallInfo";
//////
//////    CRedisCmdParam params;
//////    params.PushKey(callqueuekey.str());
//////    params.PushKey(callInfokey.str());
//////    params.PushParam(boost::lexical_cast<string>(count));
//////
//////    RedisReplyPtr reply;
//////    string scriptName = "POP_CALL_QUEUE";
//////
//////    if (!(redisClient->EvalSHA(scriptName, m_redisSHAMap[scriptName], params, reply) &&
//////        reply)){
//////        return false;
//////    }
//////
//////    if (reply->type == REDIS_REPLY_ARRAY) {
//////        for (size_t i = 0; i < reply->elements; i++) {
//////            queue.push_back(reply->element[i]->str);
//////        }
//////    }
//////
//////    return true;
//////}

bool RedisServer_ACD::GetCallQueue(string managerid, string deptname, int count, vector<string> & queue)
{
    CSynRedisClientPtr redisClient = m_RedisConnectionPool.Get(__FILE__, __LINE__);
    if (!redisClient) {
        return false;
    }

    ostringstream callqueuekey;
    callqueuekey << managerid << "." << deptname << ".CallQueue";

    ostringstream callInfokey;
    callInfokey << managerid << "." << deptname << ".CallInfo";

    CRedisCmdParam params;
    params.PushKey(callqueuekey.str());
    params.PushKey(callInfokey.str());
    params.PushParam(boost::lexical_cast<string>(count));

    RedisReplyPtr reply;
    string scriptName = "GET_CALL_QUEUE";

    if (!(redisClient->EvalSHA(scriptName, m_redisSHAMap[scriptName], params, reply) &&
        reply)){
        return false;
    }

    if (reply->type == REDIS_REPLY_ARRAY) {
        for (size_t i = 0; i < reply->elements; i++) {
            queue.push_back(reply->element[i]->str);
        }
    }

    return true;
}

bool RedisServer_ACD::SaveAgent(string managerid, string agentid, int statues)
{
    return true;
    CSynRedisClientPtr redisClient = m_RedisConnectionPool.Get(__FILE__, __LINE__);
    if (!redisClient) {
        return false;
    }

    string key = managerid + ".AgentQueue";
    ostringstream oss;
    oss << "hset " << key << " " << agentid << " " << boost::lexical_cast<string>(statues);

    RedisReplyPtr reply;
    return redisClient->ExeCMD(oss.str(), reply);
}

pair<int, bool> RedisServer_ACD::GetAgentStatus(string managerid, string agentid)
{
    pair<int, bool> ret(0, false);
    return ret;
    CSynRedisClientPtr redisClient = m_RedisConnectionPool.Get(__FILE__, __LINE__);
    if (!redisClient) {
        return ret;
    }

    string key = managerid + ".AgentQueue";
    ostringstream oss;
    oss << "hget " << key << " " << agentid ;

    RedisReplyPtr reply;
    ret.second = (redisClient->ExeCMD(oss.str(), reply) && reply);
    if (reply->type == REDIS_REPLY_STRING) {
        ret.first = boost::lexical_cast<int>(reply->str);
    }

    return ret;
}

bool RedisServer_ACD::RemoveAgent(string managerid, string agentid)
{
    return true;
    CSynRedisClientPtr redisClient = m_RedisConnectionPool.Get(__FILE__, __LINE__);
    if (!redisClient) {
        return false;
    }

    string key = managerid + ".AgentQueue";
    ostringstream oss;
    oss << "HDEL " << key << " " << agentid;

    RedisReplyPtr reply;
    if (!(redisClient->ExeCMD(oss.str(), reply) && reply)) {
        return false;
    }

    return true;
}

bool RedisServer_ACD::PushBackToAgentQueue(string managerid, string deptname, string agentid, int iQualification)
{
    CSynRedisClientPtr redisClient = m_RedisConnectionPool.Get(__FILE__, __LINE__);
    if (!redisClient) {
        return false;
    }

    ostringstream waitingAgentQueueKey;
    waitingAgentQueueKey << managerid << "." << deptname << ".AgentQueue";

    ostringstream oss;
    oss << "RPUSH " << waitingAgentQueueKey.str() << " " << iQualification << "|" << agentid;

    RedisReplyPtr reply;
    return redisClient->ExeCMD(oss.str(), reply);
}

bool RedisServer_ACD::PushFrontToAgentQueue(string managerid, string deptname, string agentid, int iQualification)
{
    CSynRedisClientPtr redisClient = m_RedisConnectionPool.Get(__FILE__, __LINE__);
    if (!redisClient) {
        return false;
    }

    ostringstream waitingAgentQueueKey;
    waitingAgentQueueKey << managerid << "." << deptname << ".AgentQueue";

    ostringstream oss;
    oss << "LPUSH " << waitingAgentQueueKey.str() << " " << iQualification << "|" << agentid;

    RedisReplyPtr reply;
    return redisClient->ExeCMD(oss.str(), reply);
}

bool RedisServer_ACD::RemoveFromAgentQueue(string managerid, string deptname, string agentid, int iQualification = 3)
{
    CSynRedisClientPtr redisClient = m_RedisConnectionPool.Get(__FILE__, __LINE__);
    if (!redisClient) {
        return false;
    }

    ostringstream waitingAgentQueueKey;
    waitingAgentQueueKey << managerid << "." << deptname << ".AgentQueue";

    ostringstream oss;
    oss << "LREM " << waitingAgentQueueKey.str() << " 1 " << " " << iQualification << "|" << agentid;

    RedisReplyPtr reply;
    return redisClient->ExeCMD(oss.str(), reply);
}

bool RedisServer_ACD::GetAgentQueue(string managerid, string deptname, int count, vector<pair<string, int>> & queue)
{
    CSynRedisClientPtr redisClient = m_RedisConnectionPool.Get(__FILE__, __LINE__);
    if (!redisClient) {
        return false;
    }

    ostringstream waitingAgentQueueKey;
    waitingAgentQueueKey << managerid << "." << deptname << ".AgentQueue";

    ostringstream oss;
    oss << "LRANGE " << waitingAgentQueueKey.str() << " 0  " << (count == 0 ? 0 : count);

    RedisReplyPtr reply;
    if (!(redisClient->ExeCMD(oss.str(), reply) && reply)) {
        return false;
    }

    if (reply->type == REDIS_REPLY_ARRAY) {
        for (size_t i = 0; i < reply->elements; i++) {
            int iQualification = 3;
            string agentid = reply->element[i]->str;
            size_t idx = agentid.find('|');
            if (idx) {
                iQualification = atoi(agentid.substr(0, idx).c_str());
                agentid = agentid.substr(idx+1);
            }
            queue.push_back(std::make_pair(agentid, iQualification));
        }
    }

    return true;
}

void RedisServer_ACD::GetRedisAddr()
{
	ostringstream querylist;

	querylist << EST_MainRedisC_Write;
	m_redirectServer->GetService(querylist.str(), m_serviceType, 
		boost::bind(&RedisServer_ACD::On_SIPCCMsg_Redirect_GetServiceConf, this, _1, _2, _3),
		__FILE__, __LINE__);
}

bool RedisServer_ACD::IsOK()
{
	return m_RedisConnectionPool.IsInited();
}

