#ifdef _WIN32
#include "./public/StdAfx.h"
#endif
#include "RedisClient.h"
#include <sstream>
#include "./public/MemLog.h"
extern CMemLog g_Logger;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRedisClient::CRedisClient(string name)
{
	m_sname = name;
	m_redisConnet=NULL;
	m_strredisip="";
	m_nredisport=0;
	m_nrdisidex=0;
	m_strdescriptioninfo="";

	clearBuff();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRedisClient::~CRedisClient(void)
{
	if(m_redisConnet!=NULL)
	{
	   redisFree(m_redisConnet);
	   m_redisConnet=NULL;

	}

	clearBuff();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CRedisClient::ConnectRedisServer(string strip, int nport, int nServerindex, string sRedisPwd)
{

	{
		ostringstream ostream;
		string sLog = "";
		ostream.str("");
		ostream << "ConnectRedisServer   strip="<<strip
			<< " nport="<< nport
			<< " nServerindex="<< nServerindex
		    <<" descriptioninfo ="<<m_strdescriptioninfo;
			sLog = ostream.str();
		g_Logger.MakeLog(sLog);
	}

	if(m_redisConnet!=NULL)
	{
		redisFree(m_redisConnet);
	    m_redisConnet=NULL;
	}

	if((true==strip.empty()) &&  (nport==0) && (nServerindex==0))
	{
		 //Redis ���� ���Բ���
		{
			ostringstream ostream;
			string sLog = "";
			ostream.str("");
			ostream << "ConnectRedisServer ����  m_strredisip="<<m_strredisip
				<< " ;m_nredisport="<< m_nredisport
				<< " ;m_nrdisidex="<< m_nrdisidex
				<<" descriptioninfo ="<<m_strdescriptioninfo;
			sLog = ostream.str();
			g_Logger.MakeLog(sLog);
		}
	}
	else
	{
		///��ʱ����
		m_strredisip=strip;
		m_nredisport=nport;
		m_nrdisidex=nServerindex;


	}

    struct timeval timeout = { 2, 500000 }; // 2.5 seconds
	m_redisConnet= redisConnectWithTimeout(m_strredisip.c_str(),m_nredisport,timeout);
	if(m_redisConnet->err)
	{
		{
			ostringstream ostream;
			string sLog = "";
			ostream.str("");
			ostream << "redisConnetRedis Server error code=" <<m_redisConnet->err
				<<" descriptioninfo ="<<m_strdescriptioninfo;

			sLog = ostream.str();
			g_Logger.MakeLog(sLog);
		}

		redisFree(m_redisConnet);
		m_redisConnet=NULL;
		return 1;

	}

	{
		g_Logger.MakeLog(m_sname + " connect success");
		redisReply *reply = NULL;
		if (!sRedisPwd.empty())
		{
			reply = (redisReply *)redisCommand(m_redisConnet, "AUTH %s", sRedisPwd.c_str());

			if (reply == NULL)
			{
				{
					ostringstream ostream;
					string sLog = "";
					ostream.str("");
					ostream << "ConnectRedisServer Redis Server command = AUTH " << sRedisPwd << "; Redis ������֤ʧ�� ��������replyΪNULL";
					sLog = ostream.str();
					g_Logger.MakeLog(sLog);
				}
				return 1;

			}
			else if (reply->type == REDIS_REPLY_ERROR)
			{
				{
					ostringstream ostream;
					string sLog = "";
					ostream.str("");
					ostream << "ConnectRedisServer Redis Server command = AUTH " << sRedisPwd << "; Redis ������֤ʧ�� ��������redis reply error";
					sLog = ostream.str();
					g_Logger.MakeLog(sLog);
				}

				return 1;
			}
		}
		else
		{
			ostringstream ostream;
			string  sLog = "";
			ostream << "ConnectRedisServer Redis ����Ϊ��";
			sLog = ostream.str();
			g_Logger.MakeLog(sLog);

		}
	}

	redisReply *reply = ( redisReply *) redisCommand(m_redisConnet,"select %d",m_nrdisidex);
	if(reply == NULL )
	{
		///
		//�޷�ѡ������
		string slog;
		slog.append("redis �޷�ѡ�����ݿ�  descriptioninfo ");
		slog.append(m_strdescriptioninfo);
		g_Logger.MakeLog(slog);

		return 2;

	}
	else
	{
		{
		    ostringstream ostream;
			string sLog = "";
			ostream.str("");
			ostream << "redis command = select " <<m_nrdisidex
				<<" ;type="<<reply->type
				<<";str="<<reply->str
				<<" ;descriptioninfo ="<<m_strdescriptioninfo;
			sLog = ostream.str();
			g_Logger.MakeLog(sLog);
		}

       freeReplyObject(reply);
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ִ��REDIS ����,��һʧ�ܽ��������񣬵ڶ�ʧ����ֱ�ӷ���
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int  CRedisClient::EexcRedisCommand(string strCommand, char* pOutStr, long long* pOutInt)
{
	if ((m_strredisip.empty()) || (m_nredisport == 0))
	{
		ostringstream ostream;
		string sLog = "";
		ostream.str("");
		ostream << "EexcRedisCommand Redis����������, Redis_Ip=" << m_strredisip << " ;Redis_Port=" << m_nredisport;
		sLog = ostream.str();
		g_Logger.MakeLog(sLog);

		return REPLY_ERROR;
	}

	/*{
	ostringstream ostream;
	string sLog = "";
	ostream.str("");
	ostream<<"EexcRedisCommand strCommand="<<strCommand<<" ;descriptioninfo ="<<m_strdescriptioninfo;
	sLog=ostream.str();
	g_Logger.MakeLog(sLog);
	}*/
	//////////////////////////////////////////////////////////////////////////////////////////
	redisReply *reply = (redisReply *)redisCommand(m_redisConnet, strCommand.c_str());
	if (reply == NULL)
	{
		//g_Logger.MakeLog(" redis ȡ���г�������ʧ��[reply == NULL] ");
		{
			ostringstream ostream;
			string sLog = "";
			ostream.str("");
			ostream << "redis ȡ���г�������ʧ��[reply == NULL]   ִ������=" << strCommand
				<< " ;�������= " << m_redisConnet->err
				<< " ;����ԭ��=" << m_redisConnet->errstr
				<< " ;descriptioninfo =" << m_strdescriptioninfo;
			sLog = ostream.str();
			g_Logger.MakeLog(sLog);
		}

		int nerconnredisvale = ConnectRedisServer();
		if (nerconnredisvale != 0)
		{
			string slog;
			slog.append("��EexcRedisCommand�� Redis �������ɹ�  descriptioninfo=");
			slog.append(m_strdescriptioninfo);
			g_Logger.MakeLog(slog);
			return REPLY_ERROR;
		}
		else
		{

			string slog;
			slog.append("��EexcRedisCommand�� Redis �����ɹ�  descriptioninfo=");
			slog.append(m_strdescriptioninfo);
			g_Logger.MakeLog(slog);

			reply = (redisReply *)redisCommand(m_redisConnet, strCommand.c_str());
			if (reply == NULL)
			{
				{
					ostringstream ostream;
					string sLog = "";
					ostream.str("");
					ostream << "redis�������� ȡ���г�������ʧ��[reply == NULL]   ִ������=" << strCommand
						<< " ;�������= " << m_redisConnet->err
						<< " ;����ԭ��=" << m_redisConnet->errstr
						<< " ;descriptioninfo =" << m_strdescriptioninfo;
					sLog = ostream.str();
					g_Logger.MakeLog(sLog);
				}

				return REPLY_ERROR;
			}

		}

	}

	int iResultType = reply->type;
	switch (iResultType)
	{
	case REDIS_REPLY_ERROR:
	case REDIS_REPLY_STATUS:
	case REDIS_REPLY_STRING:
	{
		pOutStr = new char[reply->len];
		memset(pOutStr, 0, reply->len);
		memcpy(pOutStr, reply->str, reply->len);

		break;
	}
	case REDIS_REPLY_INTEGER:
	{
		*pOutInt = reply->integer;

		break;
	}
	case REDIS_REPLY_ARRAY:
	{
		break;
	}
	default:
	{
		ostringstream ostream;
		ostream << " --- Redis ִ������: "
			<< strCommand.c_str() << " ����ֵΪ��!";
		g_Logger.MakeLog(ostream.str());

		freeReplyObject(reply);
		return REPLY_NIL;
	}
	}

	ostringstream ostream;
	ostream << " --- Redis ִ������: "
		<< strCommand.c_str()
		<< " ;db=" << m_nrdisidex << " ;result type=" << iResultType << " ;value_str='" << reply->str << "' ;value_int=" << reply->integer;
	g_Logger.MakeLog(ostream.str());

	freeReplyObject(reply);
	return iResultType;

}

void  CRedisClient::EexcRedisCommand(string strCommand, RedisReply & result)
{
	if ((m_strredisip.empty()) || (m_nredisport == 0))
	{
		ostringstream ostream;
		string sLog = "";
		ostream.str("");
		ostream << "EexcRedisCommand Redis����������, Redis_Ip=" << m_strredisip << " ;Redis_Port=" << m_nredisport;
		sLog = ostream.str();
		g_Logger.MakeLog(sLog);

		result.resultType = REPLY_ERROR;
		return;
	}

	/*{
	ostringstream ostream;
	string sLog = "";
	ostream.str("");
	ostream<<"EexcRedisCommand strCommand="<<strCommand<<" ;descriptioninfo ="<<m_strdescriptioninfo;
	sLog=ostream.str();
	g_Logger.MakeLog(sLog);
	}*/
	//////////////////////////////////////////////////////////////////////////////////////////
	redisReply *reply = (redisReply *)redisCommand(m_redisConnet, strCommand.c_str());
	if (reply == NULL)
	{
		//g_Logger.MakeLog(" redis ȡ���г�������ʧ��[reply == NULL] ");
		{
			ostringstream ostream;
			string sLog = "";
			ostream.str("");
			ostream << "redis ȡ���г�������ʧ��[reply == NULL]   ִ������=" << strCommand
				<< " ;�������= " << m_redisConnet->err
				<< " ;����ԭ��=" << m_redisConnet->errstr
				<< " ;descriptioninfo =" << m_strdescriptioninfo;
			sLog = ostream.str();
			g_Logger.MakeLog(sLog);
		}

		int nerconnredisvale = ConnectRedisServer();
		if (nerconnredisvale != 0)
		{
			string slog;
			slog.append("��EexcRedisCommand�� Redis �������ɹ�  descriptioninfo=");
			slog.append(m_strdescriptioninfo);
			g_Logger.MakeLog(slog);
			result.resultType = REPLY_ERROR;
			return;
		}
		else
		{

			string slog;
			slog.append("��EexcRedisCommand�� Redis �����ɹ�  descriptioninfo=");
			slog.append(m_strdescriptioninfo);
			g_Logger.MakeLog(slog);

			reply = (redisReply *)redisCommand(m_redisConnet, strCommand.c_str());
			if (reply == NULL)
			{
				{
					ostringstream ostream;
					string sLog = "";
					ostream.str("");
					ostream << "redis�������� ȡ���г�������ʧ��[reply == NULL]   ִ������=" << strCommand
						<< " ;�������= " << m_redisConnet->err
						<< " ;����ԭ��=" << m_redisConnet->errstr
						<< " ;descriptioninfo =" << m_strdescriptioninfo;
					sLog = ostream.str();
					g_Logger.MakeLog(sLog);
				}

				result.resultType = REPLY_ERROR;
				return;
			}

		}
	}

	result = *reply;

	ostringstream ostream;
	ostream << " --- Redis ִ������: "
		<< strCommand.c_str()
		<< " ;db=" << m_nrdisidex << " ;result type=" << reply->type << " ;value_str='" << reply->str << "' ;value_int=" << reply->integer;
	g_Logger.MakeLog(ostream.str());

	freeReplyObject(reply);
}

void CRedisClient::FreeRedisReplyObject(void *reply)
{
	freeReplyObject(reply);
}

string CRedisClient::loadScript(string sCommend)
{
	string sRes = "";

	redisReply *reply = 
		( redisReply * ) redisCommand( m_redisConnet,
		"SCRIPT LOAD %s", sCommend.c_str()) ;

	if (reply == NULL)
	{
		//g_Logger.MakeLog(" redis ȡ���г�������ʧ��[reply == NULL] ");
		{
			ostringstream ostream;
			string sLog = "";
			ostream.str("");
			ostream << "redis ȡ���г�������ʧ��[reply == NULL]   ִ������="<< sCommend
				<<" ;�������= " <<m_redisConnet->err
				<<" ;����ԭ��="<<m_redisConnet->errstr
				<<" ;descriptioninfo ="<<m_strdescriptioninfo;
			sLog = ostream.str();
			g_Logger.MakeLog(sLog);
		}

		int nerconnredisvale=ConnectRedisServer();
		if(nerconnredisvale!=0)
		{
			string slog;
			slog.append("��EexcRedisCommand�� Redis �������ɹ�  descriptioninfo=");
			slog.append(m_strdescriptioninfo);
			g_Logger.MakeLog(slog);

			sRes = "Connect_No";
			return sRes;
		}
		else
		{

			string slog;
			slog.append("��EexcRedisCommand�� Redis �����ɹ�  descriptioninfo=");
			slog.append(m_strdescriptioninfo);
			g_Logger.MakeLog(slog);

			reply = (redisReply *)redisCommand(m_redisConnet, "SCRIPT LOAD %s", sCommend.c_str()) ;
			if (reply == NULL)
			{
				{
					ostringstream ostream;
					string sLog = "";
					ostream.str("");
					ostream << "redis�������� ȡ���г�������ʧ��[reply == NULL]   ִ������="<< sCommend
						<<" ;�������= " <<m_redisConnet->err
						<<" ;����ԭ��="<<m_redisConnet->errstr
						<<" ;descriptioninfo ="<<m_strdescriptioninfo;
					sLog = ostream.str();
					g_Logger.MakeLog(sLog);
				}

				sRes = "Connect_No";
				return sRes;
			}

		}

	}

	if (reply->type == REDIS_REPLY_STRING) 
	{
		sRes = reply->str;

		ostringstream ostream;
		string sLog = "";
		ostream.str("");
		ostream<<"loadScript-- Script = " << sCommend <<" ; ScriptSHA = " << sRes;
		sLog=ostream.str();
		g_Logger.MakeLog(sLog);

	}
	else if (reply->type == REPLY_ERROR) 
	{
		sRes = "Error";

		ostringstream ostream;
		string sLog = "";
		ostream.str("");
		ostream<<"loadScript-- Script = " << sCommend <<" ; ErrorCode = " << reply->str;
		sLog=ostream.str();
		g_Logger.MakeLog(sLog);

	}

	freeReplyObject(reply);
	return sRes;
}

bool CRedisClient::evalsha(string sScriptType, char** argv, int iKeyCount, int iArgvCount)
{
	if (sScriptType.empty() || iKeyCount < 0 || iArgvCount < 0)
	{
		{
			ostringstream ostream;
			string sLog = "";
			ostream.str("");
			ostream << m_sname << " evalsha Parameter Error-- ScriptType= " << sScriptType << " ; iKeyCount= " << iKeyCount << " ; iArgvCount= " << iArgvCount;
			sLog = ostream.str();
			g_Logger.MakeLog(sLog);
		}

		m_iRedisErrorCode = " Parameter Error";
		return false;
	}

	string sSHA = m_redisSHAMap[sScriptType];

	if (sSHA == "")
	{
		string sScript = m_redisScriptMap[sScriptType];

		if (sScript == "")
		{
			ostringstream ostream;
			string sLog = "";
			ostream.str("");
			ostream << m_sname << " evalsha-- ScriptType = " << sScriptType << " ; This Script do not exist! ";
			sLog = ostream.str();
			g_Logger.MakeLog(sLog);

			return false;
		}
		else
		{
			sSHA = loadScript(sScript);

			if (sSHA == "Error")
			{
				ostringstream ostream;
				string sLog = "";
				ostream.str("");
				ostream << m_sname << " evalsha-- ScriptType = " << sScriptType << " ; This Script load error! ";
				sLog = ostream.str();
				g_Logger.MakeLog(sLog);

				return false;
			}
		}
	}

	int iCount = iKeyCount + iArgvCount;

	string sContext = "evalsha %s %d";

	for (int i = 0; i<iCount; i++)
	{
		sContext += " %s";
	}

	{
		ostringstream ostream;
		ostream << m_sname << " call " << sScriptType << " " << sContext << " " << sSHA << " " << iKeyCount;
		for (int i = 0; i<iCount; i++)
		{
			ostream << " " << argv[i];
		}
		g_Logger.MakeLog(ostream.str());
	}

	redisReply *reply = NULL;
	switch (iCount)
	{
	case 1:
	{
		reply =
			(redisReply *)redisCommand(m_redisConnet,
			sContext.c_str(), sSHA.c_str(), iKeyCount, argv[0]);
	}
	break;
	case 2:
	{
		reply =
			(redisReply *)redisCommand(m_redisConnet,
			sContext.c_str(), sSHA.c_str(), iKeyCount, argv[0], argv[1]);
	}
	break;
	case 3:
	{
		reply =
			(redisReply *)redisCommand(m_redisConnet,
			sContext.c_str(), sSHA.c_str(), iKeyCount, argv[0], argv[1], argv[2]);
	}
	break;
	case 4:
	{
		reply =
			(redisReply *)redisCommand(m_redisConnet,
			sContext.c_str(), sSHA.c_str(), iKeyCount, argv[0], argv[1], argv[2], argv[3]);
	}
	break;
	case 5:
	{
		reply =
			(redisReply *)redisCommand(m_redisConnet,
			sContext.c_str(), sSHA.c_str(), iKeyCount, argv[0], argv[1], argv[2], argv[3], argv[4]);
	}
	break;
	case 6:
	{
		reply =
			(redisReply *)redisCommand(m_redisConnet,
			sContext.c_str(), sSHA.c_str(), iKeyCount, argv[0], argv[1], argv[2], argv[3], argv[4], argv[5]);
	}
	break;
	case 7:
	{
		reply =
			(redisReply *)redisCommand(m_redisConnet,
			sContext.c_str(), sSHA.c_str(), iKeyCount, argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
	}
	break;
	case 8:
	{
		reply =
			(redisReply *)redisCommand(m_redisConnet,
			sContext.c_str(), sSHA.c_str(), iKeyCount, argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
	}
	break;
	case 9:
	{
		reply =
			(redisReply *)redisCommand(m_redisConnet,
			sContext.c_str(), sSHA.c_str(), iKeyCount, argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8]);
	}
	break;
	default:
	{
		reply =
			(redisReply *)redisCommand(m_redisConnet, sContext.c_str(), sSHA.c_str(), 0);
	}
	break;
	}

	if (reply == NULL)
	{
		//g_Logger.MakeLog(" redis ȡ���г�������ʧ��[reply == NULL] ");
		{
			ostringstream ostream;
			string sLog = "";
			ostream.str("");
			ostream << m_sname << " redis ȡ���г�������ʧ��[reply == NULL]   ִ����������=" << sScriptType
				<< " ;�������= " << m_redisConnet->err
				<< " ;����ԭ��=" << m_redisConnet->errstr
				<< " ;descriptioninfo =" << m_strdescriptioninfo;
			sLog = ostream.str();
			g_Logger.MakeLog(sLog);
		}

		m_iRedisErrorCode = "Connect_No";
		return false;
	}

	ostringstream oss;
	oss << m_sname << " call " << sScriptType << " ret:";
	m_iResultType = reply->type;
	if (reply->type == REDIS_REPLY_ARRAY)
	{
		oss << "[";
		for (int j = 0; j < reply->elements; j++)
		{
			if (reply->element[j]->type == REDIS_REPLY_STRING)
			{
				m_redisStringResultMap[j] = reply->element[j]->str;
				oss << reply->element[j]->str;
			}
			else if (reply->element[j]->type == REDIS_REPLY_INTEGER || reply->type == REDIS_REPLY_STATUS)
			{
				m_redisIntResultMap[j] = reply->element[j]->integer;
				oss << reply->element[j]->integer;
			}
			oss << ",";
		}
		oss << "]";
	}
	else if (reply->type == REDIS_REPLY_INTEGER || reply->type == REDIS_REPLY_STATUS)
	{
		m_redisIntResultMap[0] = reply->integer;
		oss << reply->integer;
	}
	else if (reply->type == REDIS_REPLY_STRING)
	{
		m_redisStringResultMap[0] = reply->str;
		oss << reply->str;
	}
	else if (reply->type == REDIS_REPLY_ERROR)
	{
		m_iRedisErrorCode = reply->str;
		oss << reply->str;
	}
	freeReplyObject(reply);

	g_Logger.MakeLog(oss.str());

	return true;
}

void CRedisClient::clearBuff()
{
	m_iResultType = 0;
	m_redisStringResultMap.clear();
	m_redisIntResultMap.clear();
	m_iRedisErrorCode = "";
	//m_iExecutionTimes = 0;
}

bool CRedisClient::InitLoadSrcipt()
{
	map<string, string>::iterator pIter;
	for(pIter = m_redisScriptMap.begin(); pIter != m_redisScriptMap.end(); ++pIter)
	{
		string sScriptType = pIter->first;
		string sScript = pIter->second;

		if ( sScript.empty() )
		{
			continue;
		}

		string sSHA = loadScript( sScript );

		if ( sSHA == "Error" || sSHA == "Connect_No" )
		{
			return false;
		}
		else
		{
			m_redisSHAMap[sScriptType] = sSHA;
		}
	}

	return true;
}

void CRedisClient::EexcRedisCommandArgv(vector<vector<char> > cmd, RedisReply & result)
{
	if ((m_strredisip.empty()) || (m_nredisport == 0))
	{
		ostringstream ostream;
		string sLog = "";
		ostream.str("");
		ostream << "EexcRedisCommand Redis����������, Redis_Ip=" << m_strredisip << " ;Redis_Port=" << m_nredisport;
		sLog = ostream.str();
		g_Logger.MakeLog(sLog);

		result.resultType = REPLY_ERROR;
		return ;
	}

	vector<char*> params;
	vector<size_t> paramslen;
	for (int i = 0; i < cmd.size(); i++)
	{
		params.push_back(&cmd[i][0]);
		paramslen.push_back(cmd[i].size());
	}

	redisReply *reply = (redisReply *)redisCommandArgv(m_redisConnet, cmd.size(), (const char **)&params[0], &paramslen[0]);
	if (reply == NULL)
	{
		//g_Logger.MakeLog(" redis ȡ���г�������ʧ��[reply == NULL] ");
		{
			ostringstream ostream;
			string sLog = "";
			ostream.str("");
			ostream << "redis ȡ���г�������ʧ��[reply == NULL]   ִ������="
				<< " ;�������= " << m_redisConnet->err
				<< " ;����ԭ��=" << m_redisConnet->errstr
				<< " ;descriptioninfo =" << m_strdescriptioninfo;
			sLog = ostream.str();
			g_Logger.MakeLog(sLog);
		}

		int nerconnredisvale = ConnectRedisServer();
		if (nerconnredisvale != 0)
		{
			string slog;
			slog.append("��EexcRedisCommand�� Redis �������ɹ�  descriptioninfo=");
			slog.append(m_strdescriptioninfo);
			g_Logger.MakeLog(slog);

			result.resultType = REPLY_ERROR;
			return;
		}
		else
		{

			string slog;
			slog.append("��EexcRedisCommand�� Redis �����ɹ�  descriptioninfo=");
			slog.append(m_strdescriptioninfo);
			g_Logger.MakeLog(slog);

			reply = (redisReply *)redisCommandArgv(m_redisConnet, cmd.size(), (const char **)&params[0], &paramslen[0]);
			if (reply == NULL)
			{
				{
					ostringstream ostream;
					string sLog = "";
					ostream.str("");
					ostream << "redis�������� ȡ���г�������ʧ��[reply == NULL]   ִ������="
						<< " ;�������= " << m_redisConnet->err
						<< " ;����ԭ��=" << m_redisConnet->errstr
						<< " ;descriptioninfo =" << m_strdescriptioninfo;
					sLog = ostream.str();
					g_Logger.MakeLog(sLog);
				}

				result.resultType = REPLY_ERROR;
				return;
			}
		}
	}

	result = *reply;

	ostringstream ostream;
	ostream << " --- Redis ִ������: ;db=" << m_nrdisidex << " ;result type=" << reply->type << " ;value_str='" << reply->str << "' ;value_int=" << reply->integer;
	g_Logger.MakeLog(ostream.str());

	freeReplyObject(reply);
}
