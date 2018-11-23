#include "SynRedisClient.h"

#include <sstream>

CRedisCmdParam::CRedisCmdParam()
	:m_keyCount(0), m_paramCount(0)
{}

void CRedisCmdParam::PushKey(void* buf, size_t buflen)
{
	m_paramBuf.push_back(vector<char>(0));
	vector<char> &key = *m_paramBuf.rbegin();
	key.resize(buflen);
	memcpy(&key[0], buf, buflen);
	m_keyCount++;
}

void CRedisCmdParam::PushKey(string key)
{
	m_paramBuf.push_back(vector<char>(0));
	vector<char> &item = *m_paramBuf.rbegin();
	item.resize(key.size() + 1);
	memcpy(&item[0], key.c_str(), key.size() + 1);
	m_keyCount++;
}

void CRedisCmdParam::PushParam(void* buf, size_t buflen)
{
	m_paramBuf.push_back(vector<char>(0));
	vector<char> &item = *m_paramBuf.rbegin();
	item.resize(buflen);
	memcpy(&item[0], buf, buflen);
	m_paramCount++;
}

void CRedisCmdParam::PushParam(string param)
{
	m_paramBuf.push_back(vector<char>(0));
	vector<char> &item = *m_paramBuf.rbegin();
	item.resize(param.size()+1);
	memcpy(&item[0], param.c_str(), param.size()+1);
	m_paramCount++;
}

boost::shared_array<char*> CRedisCmdParam::GetParams()
{
	boost::shared_array<char*> params(new char*[m_paramBuf.size()]);
	for (size_t i = 0; i < m_paramBuf.size(); i++)
	{
		params[i] = &m_paramBuf[i][0];
	}
	return params;
}

CSynRedisClient::CSynRedisClient(string name, MakeLogFun mkelogfun)
	:m_redisConnet(NULL)
	, m_strName(name)
	, m_makeLogFun(mkelogfun)
{
}


CSynRedisClient::~CSynRedisClient()
{
    DisConnect();
}

bool CSynRedisClient::Connect(string strip, int nport, string strPassword, int dbindex)
{
	if (m_redisConnet)
	{
		return true;
	}

	if (strip.empty() || nport == 0 || dbindex<0)
	{
		ostringstream oss;
		oss << m_strName << " connect, param error"
			<< ", ip = " << strip
			<< ", port = " << nport
			<< ", index = " << dbindex;
		m_makeLogFun(oss.str());
		return false;
	}

	struct timeval timeout = { 2, 500000 }; // 2.5 seconds
    m_redisConnet.reset(redisConnectWithTimeout(strip.c_str(), nport, timeout), 
        [=](void *p) {
            if (p != NULL)
            {
                redisFree((redisContext*)p);

                ostringstream oss;
                oss << "conn " << m_strName << " disconnect";
                m_makeLogFun(oss.str());
            }
        });

    if (!m_redisConnet || m_redisConnet->err)
	{
		ostringstream oss;
        oss << "conn " << m_strName << " connect to " << strip << "@" << nport
            << " db " << dbindex << ", " << strPassword
            << ", error:" << (m_redisConnet ? m_redisConnet->err : -1);
		m_makeLogFun(oss.str());

		return false;
	}

    {
        ostringstream oss;
        oss << "conn " << m_strName << " connect to " << strip << "@" << nport
            << " db " << dbindex << ", " << strPassword << " success!";
        m_makeLogFun(oss.str());
    }

	boost::shared_ptr<redisReply> reply;
	if (!strPassword.empty())
	{
        reply.reset((redisReply*)redisCommand(m_redisConnet.get(), "AUTH %s", strPassword.c_str()),
			[=](void *p) { if (p != NULL) freeReplyObject(p); });

		if (!reply || reply->type == REDIS_REPLY_ERROR)
		{
			ostringstream oss;
            oss << "conn " << m_strName << " AUTH pass=" << strPassword
                << ",error=" << (reply ? reply->str : "NULL");
			m_makeLogFun(oss.str());

			return false;
		}
	}

    reply.reset((redisReply*)redisCommand(m_redisConnet.get(), "select %d", dbindex),
		[](void *p) {
			freeReplyObject(p);
		});

	if (!reply || reply->type == REDIS_REPLY_ERROR)
	{
		ostringstream oss;
        oss << "conn " << m_strName << " select db" << dbindex
            << " error " << (reply ? reply->str : "NULL");
		m_makeLogFun(oss.str());

		return false;
	}

	return true;
}

void CSynRedisClient::DisConnect()
{
    m_redisConnet.reset();
}

string CSynRedisClient::LoadScript(string name, string script)
{
    if (m_redisConnet == NULL) return "";

	boost::shared_ptr<redisReply> reply;
	reply.reset((redisReply*)redisCommand(m_redisConnet.get(), "SCRIPT LOAD %s", script.c_str()),
		[](void *p) {
			freeReplyObject(p);
		}
	);

	if (!reply)
	{
		ostringstream oss;
        oss << "conn " << m_strName << " LoadScript " << name << " unknown error";
		m_makeLogFun(oss.str());

		return "";
	}
	else if (reply->type == REDIS_REPLY_ERROR)
	{
		ostringstream oss;
        oss << "conn " << m_strName << " LoadScript " << name << " error:" << reply->str;
		m_makeLogFun(oss.str());

		return "";
	}

	{
		ostringstream oss;
        oss << "conn " << m_strName << " LoadScript " << name << " success, sha=" << reply->str;
		m_makeLogFun(oss.str());
	}

	return reply->str;
}

bool CSynRedisClient::EvalSHA(string scriptname, string sha, CRedisCmdParam & param, RedisReplyPtr &reply)
{
    if (m_redisConnet == NULL) return false;

	int iCount = param.GetKeyCount() + param.GetParamCount();

	string sContext = "evalsha %s %d";
	for (int i = 0; i<iCount; i++)
	{
		sContext += " %s";
	}

	redisReply *rep = NULL;
	switch (iCount)
	{
	case 0:
	{
		rep =
            (redisReply *)redisCommand(m_redisConnet.get(),
			sContext.c_str(), sha.c_str(), 0);
	}
	break;
	case 1:
	{
		rep =
            (redisReply *)redisCommand(m_redisConnet.get(),
			sContext.c_str(), sha.c_str(), param.GetKeyCount(), param[0]);
	}
	break;
	case 2:
	{
		rep =
            (redisReply *)redisCommand(m_redisConnet.get(),
			sContext.c_str(), sha.c_str(), param.GetKeyCount(), param[0], param[1]);
	}
	break;
	case 3:
	{
		rep =
            (redisReply *)redisCommand(m_redisConnet.get(),
			sContext.c_str(), sha.c_str(), param.GetKeyCount(), param[0], param[1], param[2]);
	}
	break;
	case 4:
	{
		rep =
            (redisReply *)redisCommand(m_redisConnet.get(),
			sContext.c_str(), sha.c_str(), param.GetKeyCount(), param[0], param[1], param[2], param[3]);
	}
	break;
	case 5:
	{
		rep =
            (redisReply *)redisCommand(m_redisConnet.get(),
			sContext.c_str(), sha.c_str(), param.GetKeyCount(), param[0], param[1], param[2], param[3], param[4]);
	}
	break;
	case 6:
	{
		rep =
            (redisReply *)redisCommand(m_redisConnet.get(),
			sContext.c_str(), sha.c_str(), param.GetKeyCount(), param[0], param[1], param[2], param[3], param[4], param[5]);
	}
	break;
	case 7:
	{
		rep =
            (redisReply *)redisCommand(m_redisConnet.get(),
			sContext.c_str(), sha.c_str(), param.GetKeyCount(), param[0], param[1], param[2], param[3], param[4], param[5], param[6]);
	}
	break;
	case 8:
	{
		rep =
            (redisReply *)redisCommand(m_redisConnet.get(),
			sContext.c_str(), sha.c_str(), param.GetKeyCount(), param[0], param[1], param[2], param[3], param[4], param[5], param[6], param[7]);
	}
	break;
	case 9:
	{
		rep =
            (redisReply *)redisCommand(m_redisConnet.get(),
			sContext.c_str(), sha.c_str(), param.GetKeyCount(), param[0], param[1], param[2], param[3], param[4], param[5], param[6], param[7], param[8]);
	}
	break;
	default:
	{
		rep =
            (redisReply *)redisCommand(m_redisConnet.get(), sContext.c_str(), sha.c_str(), 0);
	}
	break;
	}

	reply.reset(rep, [](void *p) {
        if (p != NULL) freeReplyObject(p);
	});

	ostringstream ostream;
	ostream << "evalsha " << sha << " " << param.GetKeyCount() << " ";
	for (int i = 0; i<iCount; i++)
	{
		ostream << param[i] << " ";
	}
	string cmd = ostream.str();

	ostream.str("");
    ostream << "conn " << m_strName << " call script[" << scriptname << "] ";

	bool ret = true;
	if (rep == NULL)
	{
		ostream << "unknown error, cmd=" << cmd;
		m_makeLogFun(ostream.str());
		ret = false;
	}
	else
	{
		if (rep->type == REDIS_REPLY_ERROR)
		{
			ostream << "cmd=" << cmd << ", error=" << rep->str ;
			m_makeLogFun(ostream.str());
			ret = false;
		}
		else
		{
			ostream << "success, cmd=" << cmd;
			if (scriptname != "CheckTimeout")
				m_makeLogFun(ostream.str());
		}
	}

	return ret;
}

bool CSynRedisClient::ExeCMD(string cmd, RedisReplyPtr &reply)
{
    if (m_redisConnet == NULL) return false;

    redisReply *rep = (redisReply *)redisCommand(m_redisConnet.get(), cmd.c_str());

	reply.reset(rep, [](void *p) {
		freeReplyObject(p);
	});

	bool ret = true;
	if (rep == NULL)
	{
		ostringstream ostream;
        ostream << "conn " << m_strName << " ExeCMD unknown error, cmd=" << cmd;
		m_makeLogFun(ostream.str());
		ret = false;
	}
	else
	{
		if (rep->type == REDIS_REPLY_ERROR)
		{
			ostringstream ostream;
            ostream << "conn " << m_strName << " ExeCMD error=" << rep->str << ", cmd=" << cmd;
			m_makeLogFun(ostream.str());
			ret = false;
		}
		else
		{
			ostringstream ostream;
            ostream << "conn " << m_strName << " ExeCMD success, cmd=" << cmd;
			m_makeLogFun(ostream.str());
		}
	}

	return ret;
}

bool CSynRedisClient::ExeCMDArgv(CRedisCmdArgv & cmdArgv, RedisReplyPtr &reply)
{
    if (m_redisConnet == NULL) return false;

	boost::shared_array<char*> args = cmdArgv.GetArgv();
	boost::shared_array<size_t> arglens = cmdArgv.GetArgvLens();

    redisReply *rep = (redisReply *)redisCommandArgv(m_redisConnet.get(),
		cmdArgv.GetCount(), (const char **)&args[0], &arglens[0]);

	reply.reset(rep, [](void *p) {
		freeReplyObject(p);
	});

	bool ret = true;
	if (rep == NULL)
	{
		ostringstream ostream;
        ostream << "conn " << m_strName << " unknown error, cmd=" << args[0];
		m_makeLogFun(ostream.str());
		ret = false;
	}
	else
	{
		if (rep->type == REDIS_REPLY_ERROR)
		{
			ostringstream ostream;
            ostream << "conn " << m_strName << " error=" << rep->str << ", cmd=" << args[0];
			m_makeLogFun(ostream.str());
			ret = false;
		}
		else
		{
			ostringstream ostream;
            ostream << "conn " << m_strName << " success, cmd=" << args[0];
			m_makeLogFun(ostream.str());
		}
	}

	return ret;
}
