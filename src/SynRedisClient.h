#pragma once

#include <vector>

#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include "hiredis/hiredis.h"

using namespace std;

typedef boost::shared_ptr<redisContext> RedisContextPtr;
typedef boost::shared_ptr<redisReply> RedisReplyPtr;
typedef boost::function<void(string)> MakeLogFun;

class CRedisCmdParam
{
public:
	CRedisCmdParam();
	~CRedisCmdParam(){};
	void PushKey(void* buf, size_t buflen);
	void PushKey(string key);
	void PushParam(void* buf, size_t buflen);
	void PushParam(string param);
	boost::shared_array<char*> GetParams();

	char* operator [] (size_t i)
	{
		if (i > m_paramBuf.size())
			return NULL;
		return &m_paramBuf[i][0];
	};

	int GetKeyCount() { return m_keyCount; };
	int GetParamCount() { return m_paramCount; };
private:
	int m_keyCount;
	int m_paramCount;
	vector<vector<char> > m_paramBuf;
};

class CRedisCmdArgv
{
public:
	CRedisCmdArgv(){};
	~CRedisCmdArgv(){};

	void Push(void* buf, size_t buflen)
	{
		m_ArgvBuf.push_back(vector<char>(buflen));
		vector<char> &item = *m_ArgvBuf.rbegin();
		memcpy(&item[0], buf, buflen);
	};

	void Push(string arg)
	{
		m_ArgvBuf.push_back(vector<char>(arg.size() + 1));
		vector<char> &item = *m_ArgvBuf.rbegin();
		memcpy(&item[0], arg.c_str(), arg.size() + 1);
	};

	boost::shared_array<char*> GetArgv()
	{
		boost::shared_array<char*> args(new char*[m_ArgvBuf.size()]);
		for (size_t i = 0; i < m_ArgvBuf.size(); i++)
		{
			args[i] = &m_ArgvBuf[i][0];
		}
		return args;
	};

	boost::shared_array<size_t> GetArgvLens()
	{
		boost::shared_array<size_t> arglens(new size_t[m_ArgvBuf.size()]);
		for (size_t i = 0; i < m_ArgvBuf.size(); i++)
		{
			arglens[i] = m_ArgvBuf[i].size();
		}
		return arglens;
	};

	int GetCount() { return m_ArgvBuf.size(); };
private:
	vector<vector<char> > m_ArgvBuf;
};

class CSynRedisClient
{
public:
	CSynRedisClient(string name, MakeLogFun mkelogfun);
	~CSynRedisClient();

    string GetName() { return m_strName; };

    bool Connect(string strip = "", int nport = 0, string strPassword = "", int dbindex = 0);
    void DisConnect();
    bool IsConnected() { return !!m_redisConnet; };

    string LoadScript(string name, string script);
	
    bool EvalSHA(string scriptname, string sha, CRedisCmdParam & param, RedisReplyPtr &reply);
	
    bool ExeCMD(string cmd, RedisReplyPtr &reply);
	
    bool ExeCMDArgv(CRedisCmdArgv & cmdArgv, RedisReplyPtr &reply);

private:
    RedisContextPtr  m_redisConnet;

	string m_strName;
	MakeLogFun m_makeLogFun;
};

typedef boost::shared_ptr<CSynRedisClient> CSynRedisClientPtr;

